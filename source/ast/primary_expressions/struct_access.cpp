#include "ast/primary_expressions/struct_access.h"
#include "ast/optimization/foreach_optimization.h"
#include "ast/optimization/match_optimization.h"
#include "ast/optimization/var_optimization.h"
#include <tuple>

namespace dconstruct::ast {

    [[nodiscard]] const struct_type* access_struct_type_from_type(const full_type& type) noexcept {
        if (const auto* struct_t = std::get_if<struct_type>(&type)) {
            return struct_t;
        }

        const auto* ptr_t = std::get_if<ptr_type>(&type);
        if (!ptr_t || !ptr_t->m_pointedAt) {
            return nullptr;
        }

        return std::get_if<struct_type>(ptr_t->m_pointedAt.get());
    }

    [[nodiscard]] std::optional<std::tuple<const struct_type*, const std::pair<std::string, ref_full_type>*, u64>> find_member_access(const full_type& type, const std::string& member_name) noexcept {
        const struct_type* struct_t = access_struct_type_from_type(type);
        if (!struct_t) {
            return std::nullopt;
        }

        u64 offset = 0;
        for (const auto& member : struct_t->m_members) {
            if (member.first == member_name) {
                return std::tuple{struct_t, &member, offset};
            }
            offset += get_size(*member.second);
        }

        return std::nullopt;
    }

    struct_access::struct_access(expr_uptr&& lhs, compilation::token member_name) noexcept : m_lhs(std::move(lhs)), m_memberName(std::move(member_name)) {}

    void struct_access::pseudo_c(ast_serialization_buffer& buffer) const {
        if (std::holds_alternative<ptr_type>(m_lhs->get_type().value_or(std::monostate()))) {
            buffer.append(*m_lhs, "->"sv, m_memberName.m_lexeme);
        } else {
            buffer.append(*m_lhs, '.', m_memberName.m_lexeme);
        }
    }

    void struct_access::pseudo_py(ast_serialization_buffer& buffer) const {
        buffer.append(*m_lhs, '.', m_memberName.m_lexeme);
    }

    void struct_access::pseudo_racket(ast_serialization_buffer& buffer) const {
        buffer.append("(struct-ref "sv, *m_lhs, ' ', m_memberName.m_lexeme, ')');
    }

    void struct_access::to_pseudo_c_colored_string(code_color_serialization_buffer& buffer) const noexcept {
        buffer.append(*m_lhs);
        const bool is_ptr = std::holds_alternative<ptr_type>(m_lhs->get_type().value_or(std::monostate()));
        buffer.append(AST_COLOR::OPERATOR, is_ptr ? "->"sv : "."sv);
        buffer.append(AST_COLOR::MEMBER, m_memberName.m_lexeme);
    }

    [[nodiscard]] expr_uptr struct_access::simplify() const {
        return std::make_unique<struct_access>(m_lhs->simplify(), m_memberName);
    }

    [[nodiscard]] full_type struct_access::compute_type_unchecked(const compilation::scope& env) const noexcept {
        const full_type lhs_type = m_lhs->get_type_unchecked(env);
        const auto member = find_member_access(lhs_type, m_memberName.m_lexeme);
        if (!member || !std::get<1>(*member) || !std::get<1>(*member)->second) {
            return std::monostate();
        }
        return *std::get<1>(*member)->second;
    }

    [[nodiscard]] semantic_check_res struct_access::compute_type_checked(compilation::scope& env) const noexcept {
        const semantic_check_res lhs_type = m_lhs->get_type_checked(env);
        if (!lhs_type) {
            return lhs_type;
        }

        const struct_type* struct_t = access_struct_type_from_type(*lhs_type);
        if (!struct_t) {
            return std::unexpected{semantic_check_error{"expected struct or pointer-to-struct type for member access but got " + type_to_declaration_string(*lhs_type), this}};
        }

        const auto member = find_member_access(*lhs_type, m_memberName.m_lexeme);
        if (!member || !std::get<1>(*member) || !std::get<1>(*member)->second) {
            return std::unexpected{semantic_check_error{"unknown member '" + m_memberName.m_lexeme + "' on struct " + struct_t->m_name, this}};
        }

        return *std::get<1>(*member)->second;
    }

    [[nodiscard]] expr_uptr struct_access::clone() const noexcept {
        auto expr = std::make_unique<struct_access>(m_lhs->clone(), m_memberName);
        if (m_type) {
            expr->set_type(*m_type);
        }
        return expr;
    }

    [[nodiscard]] bool struct_access::equals(const expression& rhs) const noexcept {
        const struct_access* rhs_ptr = dynamic_cast<const struct_access*>(&rhs);
        if (!rhs_ptr) {
            return false;
        }
        return m_lhs == rhs_ptr->m_lhs && m_memberName == rhs_ptr->m_memberName;
    }

    [[nodiscard]] u16 struct_access::calc_complexity() const noexcept {
        return 1 + m_lhs->get_complexity();
    }

    [[nodiscard]] const ast::full_type* struct_access::member_access_struct_type() const noexcept {
        if (!m_type) {
            return nullptr;
        }

        if (std::holds_alternative<struct_type>(*m_type)) {
            return &*m_type;
        }

        const auto* ptr_t = std::get_if<ptr_type>(&*m_type);
        if (ptr_t && ptr_t->m_pointedAt && std::holds_alternative<struct_type>(*ptr_t->m_pointedAt)) {
            return ptr_t->m_pointedAt.get();
        }

        return nullptr;
    }

    [[nodiscard]] std::optional<compilation::source_location> struct_access::source_location() const noexcept {
        return compilation::source_location{m_memberName.m_file, m_memberName.m_line};
    }

    [[nodiscard]] emission_res struct_access::emit_dc(compilation::function& fn, compilation::global_state& global, const std::optional<reg_idx> opt_destination) const noexcept {
        const std::optional<full_type> lhs_cached_type = m_lhs->get_type();
        if (!lhs_cached_type) {
            return std::unexpected{"struct member access emitted before type checking"};
        }

        const auto member = find_member_access(*lhs_cached_type, m_memberName.m_lexeme);
        if (!member || !std::get<1>(*member) || !std::get<1>(*member)->second) {
            return std::unexpected{"unknown struct member " + m_memberName.m_lexeme};
        }

        const auto* member_info = std::get<1>(*member);
        const u64 member_offset = std::get<2>(*member);
        std::expected<Opcode, std::string> load_opcode = get_load_opcode(*member_info->second);
        if (!load_opcode) {
            return std::unexpected{std::move(load_opcode.error())};
        }

        emission_res base_res = m_lhs->emit_dc(fn, global);
        if (!base_res) {
            return base_res;
        }

        const emission_res load_destination = fn.fix_destination(opt_destination);
        if (!load_destination) {
            return load_destination;
        }

        emission_res address_res = load_destination;
        if (*address_res == *base_res) {
            address_res = fn.get_next_unused_register();
        }
        if (!address_res) {
            return std::unexpected{std::move(address_res.error())};
        }

        if (member_offset == 0) {
            fn.emit_instruction(Opcode::Move, *address_res, *base_res);
        } else {
            fn.emit_instruction(Opcode::IAddImm, *address_res, *base_res, member_offset);
        }
        fn.free_register(*base_res);

        fn.emit_instruction(*load_opcode, *load_destination, *address_res);
        if (*address_res != *load_destination) {
            fn.free_register(*address_res);
        }

        return *load_destination;
    }

    [[nodiscard]] lvalue_emission_res struct_access::emit_dc_lvalue(compilation::function& fn, compilation::global_state& global) const noexcept {
        const std::optional<full_type> lhs_cached_type = m_lhs->get_type();
        if (!lhs_cached_type) {
            return std::unexpected{"struct member access emitted before type checking"};
        }

        const auto member = find_member_access(*lhs_cached_type, m_memberName.m_lexeme);
        if (!member || !std::get<1>(*member) || !std::get<1>(*member)->second) {
            return std::unexpected{"unknown struct member " + m_memberName.m_lexeme};
        }

        const auto* member_info = std::get<1>(*member);
        const u64 member_offset = std::get<2>(*member);

        emission_res base_res = m_lhs->emit_dc(fn, global);
        if (!base_res) {
            return std::unexpected{std::move(base_res.error())};
        }

        emission_res address_res = fn.get_next_unused_register();
        if (!address_res) {
            return std::unexpected{std::move(address_res.error())};
        }

        if (member_offset == 0) {
            fn.emit_instruction(Opcode::Move, *address_res, *base_res);
        } else {
            fn.emit_instruction(Opcode::IAddImm, *address_res, *base_res, member_offset);
        }
        fn.free_register(*base_res);

        std::expected<Opcode, std::string> store_opcode = get_store_opcode(*member_info->second);
        if (!store_opcode) {
            fn.free_register(*address_res);
            return std::unexpected{std::move(store_opcode.error())};
        }

        return std::pair{*address_res, *store_opcode};
    }

    VAR_OPTIMIZATION_ACTION struct_access::var_optimization_pass(var_optimization_env& env) noexcept {
        env.check_action(&m_lhs);
        return VAR_OPTIMIZATION_ACTION::NONE;
    }

    FOREACH_OPTIMIZATION_ACTION struct_access::foreach_optimization_pass(foreach_optimization_env& env) noexcept {
        env.check_action(&m_lhs);
        return FOREACH_OPTIMIZATION_ACTION::NONE;
    }

    MATCH_OPTIMIZATION_ACTION struct_access::match_optimization_pass(match_optimization_env& env) noexcept {
        return MATCH_OPTIMIZATION_ACTION::NONE;
    }

    void struct_access::regex_optimization_pass() noexcept {
        m_lhs->regex_optimization_pass();
    }

    [[nodiscard]] std::unique_ptr<struct_access> struct_access::to_struct_access() noexcept {
        if (auto replacement = m_lhs->to_struct_access()) {
            m_lhs = std::move(replacement);
        }
        return nullptr;
    }

}
