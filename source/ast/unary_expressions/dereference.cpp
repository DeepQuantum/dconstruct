#include "ast/unary_expressions/dereference_expression.h"


namespace dconstruct::ast {

    void dereference_expr::pseudo_racket(std::ostream& os) const {
        os << "(deref " << *m_rhs << ")";
    }

    [[nodiscard]] expr_uptr dereference_expr::simplify() const {
        return std::make_unique<ast::dereference_expr>(m_operator, m_rhs->simplify());
    }

    [[nodiscard]] full_type dereference_expr::compute_type_unchecked(const compilation::scope& env) const noexcept {
        const full_type rhs_type = m_rhs->compute_type_unchecked(env);
        if (std::holds_alternative<ast::ptr_type>(rhs_type)) {
            return *std::get<ast::ptr_type>(rhs_type).m_pointedAt;
        }
        return ast::full_type{std::monostate()};
    }

    [[nodiscard]] semantic_check_res dereference_expr::compute_type_checked(compilation::scope& env) const noexcept {
        const semantic_check_res rhs_type = m_rhs->get_type_checked(env);
        if (!rhs_type) {
            return rhs_type;
        }

        const std::optional<std::string> invalid_dereference = std::visit([](auto&& rhs_type) -> std::optional<std::string> {
            using T = std::decay_t<decltype(rhs_type)>;

            if constexpr (is_pointer<T>) {
                if (is_unknown(*rhs_type.m_pointedAt)) {
                    return "cannot dereference void pointer";
                } else{ 
                    return std::nullopt;
                }
            } else {
                return "expected pointer type for dereference but got " + type_to_declaration_string(rhs_type);
            }
        }, *rhs_type);

        if (!invalid_dereference) {
            return *std::get<ptr_type>(*rhs_type).m_pointedAt;
        }

        return std::unexpected{semantic_check_error{*invalid_dereference, this}};
    }

    [[nodiscard]] bool dereference_expr::is_l_evaluable() const noexcept {
        return m_rhs->is_l_evaluable();
    }

    [[nodiscard]] emission_res dereference_expr::emit_dc(compilation::function& fn, compilation::global_state& global, const std::optional<reg_idx> opt_destination) const noexcept {
        emission_res rhs = m_rhs->emit_dc(fn, global);
        if (!rhs) {
            return rhs;
        }
        
        assert(std::holds_alternative<ptr_type>(*m_rhs->get_type()));
        const ptr_type& ptr_t = std::get<ptr_type>(*m_rhs->get_type());

        assert(std::holds_alternative<primitive_type>(*ptr_t.m_pointedAt));

        const primitive_kind kind = std::get<primitive_type>(*ptr_t.m_pointedAt).m_type;

        std::expected<Opcode, std::string> load_opcode = get_load_opcode(*ptr_t.m_pointedAt);
        if (!load_opcode) {
            return std::unexpected{std::move(load_opcode.error())};
        }

        emission_res load_destination = fn.get_destination(opt_destination);
        if (!load_destination) {
            return load_destination;
        }

        fn.emit_instruction(*load_opcode, *load_destination, *rhs);
        fn.free_register(*rhs);
        
        return *load_destination;
    }

    [[nodiscard]] lvalue_emission_res dereference_expr::emit_dc_lvalue(compilation::function& fn, compilation::global_state& global) const noexcept {
        lvalue_emission_res rhs = m_rhs->emit_dc_lvalue(fn, global);
        if (!rhs) {
            return rhs;
        }

        assert(std::holds_alternative<ptr_type>(*m_rhs->get_type()));
        const ptr_type& ptr_t = std::get<ptr_type>(*m_rhs->get_type());
        
        std::expected<Opcode, std::string> store_opcode = get_store_opcode(*ptr_t.m_pointedAt);
        if (!store_opcode) {
            return std::unexpected{std::move(store_opcode.error())};
        }
        
        return std::pair{rhs->first, *store_opcode};
    }
}