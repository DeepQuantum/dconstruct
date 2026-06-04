#include "ast/primary_expressions/sizeof_expression.h"
#include "ast/primary_expressions/struct_access.h"

namespace dconstruct::ast {

    [[nodiscard]] bool sizeof_expr::equals(const expression& rhs) const noexcept {
        const sizeof_expr* rhs_ptr = dynamic_cast<const sizeof_expr*>(&rhs);
        if (!rhs_ptr) {
            return false;
        }
        return m_operand == rhs_ptr->m_operand;
    }

    [[nodiscard]] u16 sizeof_expr::calc_complexity() const noexcept {
        if (const expr_uptr* expr_ptr = std::get_if<expr_uptr>(&m_operand)) {
            return 1 + (*expr_ptr)->get_complexity();
        }
        return 1;
    }

    [[nodiscard]] expr_uptr sizeof_expr::clone() const noexcept {
        if (const full_type* type_ptr = std::get_if<full_type>(&m_operand)) {
            return std::make_unique<sizeof_expr>(*type_ptr);
        } else {
            return std::make_unique<sizeof_expr>(std::get<expr_uptr>(m_operand)->clone());
        }
    }

    [[nodiscard]] expr_uptr sizeof_expr::simplify() const {
        return clone();
    }

    void sizeof_expr::pseudo_c(ast_serialization_buffer& buffer) const {
        buffer.append("sizeof("sv);
        if (const full_type* type_ptr = std::get_if<full_type>(&m_operand)) {
            buffer.append(type_to_declaration_string(*type_ptr));
        } else {
            buffer.append(*std::get<expr_uptr>(m_operand));
        }
        buffer.append(')');
    }

    void sizeof_expr::pseudo_py(ast_serialization_buffer& buffer) const {
        if (const full_type* type_ptr = std::get_if<full_type>(&m_operand)) {
            buffer.append("sizeof("sv, type_to_declaration_string(*type_ptr), ')');
        } else {
            buffer.append("sys.getsizeof("sv, *std::get<expr_uptr>(m_operand), ')');
        }
    }

    void sizeof_expr::pseudo_racket(ast_serialization_buffer& buffer) const {
        buffer.append("(sizeof "sv);
        if (const full_type* type_ptr = std::get_if<full_type>(&m_operand)) {
            buffer.append(type_to_declaration_string(*type_ptr));
        } else {
            buffer.append(*std::get<expr_uptr>(m_operand));
        }
        buffer.append(')');
    }

    void sizeof_expr::to_pseudo_c_colored_string(code_color_serialization_buffer& buffer) const noexcept {
        buffer.append(AST_COLOR::KEYWORD, "sizeof"sv);
        buffer.append(AST_COLOR::PUNCTUATION, '(');
        if (const full_type* type_ptr = std::get_if<full_type>(&m_operand)) {
            type_to_colored_declaration_string(*type_ptr, buffer);
        } else {
            buffer.append(*std::get<expr_uptr>(m_operand));
        }
        buffer.append(AST_COLOR::PUNCTUATION, ')');
    }

    [[nodiscard]] inline full_type sizeof_expr::compute_type_unchecked(const compilation::scope& env) const noexcept {
        return make_type_from_prim(primitive_kind::U64);
    }

    [[nodiscard]] semantic_check_res sizeof_expr::compute_type_checked(compilation::scope& env) const noexcept {
        if (const expr_uptr* expr_ptr = std::get_if<expr_uptr>(&m_operand)) {
            const semantic_check_res op_res = (*expr_ptr)->compute_type_checked(env);
            if (!op_res) {
                return op_res;
            }
        }
        return make_type_from_prim(primitive_kind::U64);
    }

    [[nodiscard]] emission_res sizeof_expr::emit_dc(
        compilation::function& fn,
        compilation::global_state& global,
        const std::optional<reg_idx> destination
    ) const noexcept {
        u64 type_size;

        if (const full_type* type_ptr = std::get_if<full_type>(&m_operand)) {
            type_size = get_size(*type_ptr);
        } else {
            std::optional<full_type> op_type = std::get<expr_uptr>(m_operand)->get_type();
            assert(op_type);
            type_size = get_size(*op_type);
        }

        if (type_size > std::numeric_limits<u16>::max()) {
            return std::unexpected{"expected type with size less than " + std::to_string(std::numeric_limits<u16>::max()) + " but got size " + std::to_string(type_size)};
        }

        const emission_res sizeof_dest = fn.fix_destination(destination);
        if (!sizeof_dest) {
            return sizeof_dest;
        }

        const u8 lo = static_cast<u16>(type_size) & 0xFF;
        const u8 hi = (static_cast<u16>(type_size) >> 8) & 0xFF;
        fn.emit_instruction(Opcode::LoadU16Imm, *sizeof_dest, lo, hi);

        return *sizeof_dest;
    }

    [[nodiscard]] VAR_OPTIMIZATION_ACTION sizeof_expr::var_optimization_pass(var_optimization_env& env) noexcept {
        if (expr_uptr* expr_ptr = std::get_if<expr_uptr>(&m_operand)) {
            env.check_action(expr_ptr);
        }
        return VAR_OPTIMIZATION_ACTION::NONE;
    }

    [[nodiscard]] FOREACH_OPTIMIZATION_ACTION sizeof_expr::foreach_optimization_pass(foreach_optimization_env& env) noexcept {
        return FOREACH_OPTIMIZATION_ACTION::NONE;
    }

    [[nodiscard]] std::unique_ptr<struct_access> sizeof_expr::to_struct_access() noexcept {
        if (expr_uptr* expr_ptr = std::get_if<expr_uptr>(&m_operand)) {
            auto& expr = *expr_ptr;
            if (auto replacement = expr->to_struct_access()) {
                expr = std::move(replacement);
            }
        }
        return nullptr;
    }

}
