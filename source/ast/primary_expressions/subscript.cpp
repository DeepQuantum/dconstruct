#include "ast/primary_expressions/subscript.h"
#include "ast/optimization/foreach_optimization.h"
#include "ast/optimization/var_optimization.h"
#include "ast/optimization/match_optimization.h"

namespace dconstruct::ast {

void subscript_expr::pseudo_c(std::ostream& os) const {
    os << *m_lhs << "[" << *m_rhs << "]";
}

void subscript_expr::pseudo_py(std::ostream& os) const {
    os << *m_lhs << "[" << *m_rhs << "]";
}

void subscript_expr::pseudo_racket(std::ostream& os) const {
    os << *m_lhs << "[" << *m_rhs << "]";
}

[[nodiscard]] expr_uptr subscript_expr::simplify() const {
    return std::make_unique<subscript_expr>(m_lhs->clone(), m_rhs->simplify());
}

[[nodiscard]] expr_uptr subscript_expr::clone() const noexcept {
    return std::make_unique<subscript_expr>(m_lhs->clone(), m_rhs->clone());
}


[[nodiscard]] bool subscript_expr::equals(const expression& rhs) const noexcept {
    const subscript_expr* rhs_ptr = dynamic_cast<const subscript_expr*>(&rhs);
    if (!rhs_ptr) {
        return false;
    }
    return m_lhs == rhs_ptr->m_lhs && m_rhs == rhs_ptr->m_rhs;
}

[[nodiscard]] u16 subscript_expr::calc_complexity() const noexcept {
    return 1 + m_lhs->get_complexity() + m_rhs->get_complexity();
}

[[nodiscard]] full_type subscript_expr::compute_type_unchecked(const compilation::scope& env) const noexcept {
    const std::optional<full_type> lhs_type = m_lhs->compute_type_unchecked(env);
    if (!lhs_type) {
        return std::monostate();
    }
    if (std::holds_alternative<ptr_type>(*lhs_type)) {
        return *std::get<ptr_type>(*lhs_type).m_pointedAt;
    } else {
        return std::monostate();
    }
}

[[nodiscard]] semantic_check_res subscript_expr::compute_type_checked(compilation::scope& scope) const noexcept {
    const semantic_check_res lhs_type = m_lhs->get_type_checked(scope);
    if (!lhs_type) {
        return lhs_type;
    }
    const semantic_check_res rhs_type = m_rhs->get_type_checked(scope);
    if (!rhs_type) {
        return rhs_type;
    }

    if (!std::holds_alternative<ptr_type>(*lhs_type)) {
        return std::unexpected{semantic_check_error{"expected pointer type for subscript but got " + type_to_declaration_string(*lhs_type)}};
    }

    const full_type& ptr_t = *std::get<ptr_type>(*lhs_type).m_pointedAt;
    
    if (is_unknown(ptr_t)) {
        return std::unexpected{semantic_check_error{"cannot dereference void pointer"}};
    }

    if (!std::holds_alternative<primitive_type>(*rhs_type) || !is_integral(std::get<primitive_type>(*rhs_type).m_type)) {
        return std::unexpected{semantic_check_error{"expected integral type for subscript index but got " + type_to_declaration_string(*rhs_type)}};
    }

    return *std::get<ptr_type>(*lhs_type).m_pointedAt;
}

[[nodiscard]] emission_res subscript_expr::emit_dc(compilation::function& fn, compilation::global_state& global, const std::optional<reg_idx> opt_destination) const noexcept {
    const emission_res lhs_res = m_lhs->emit_dc(fn, global);
    if (!lhs_res) {
        return lhs_res;
    }

    const emission_res rhs_res = m_rhs->emit_dc(fn, global);
    if (!rhs_res) {
        return rhs_res;
    }

    const ptr_type& ptr_t = std::get<ptr_type>(*m_lhs->get_type());
    
    std::expected<Opcode, std::string> load_opcode = get_load_opcode(*ptr_t.m_pointedAt);
    if (!load_opcode) {
        return std::unexpected{std::move(load_opcode.error())};
    }

    const emission_res load_destination = fn.get_destination(opt_destination);
    if (!load_destination) {
        return load_destination;
    }

    const u64 ptr_size = get_size(*ptr_t.m_pointedAt);

    fn.emit_instruction(Opcode::IMulImm, *load_destination, *rhs_res, ptr_size);
    fn.emit_instruction(Opcode::IAdd, *load_destination, *lhs_res, *load_destination);
    fn.emit_instruction(*load_opcode, *load_destination, *load_destination);

    fn.free_register(*rhs_res);
    fn.free_register(*lhs_res);

    return *load_destination;
}

[[nodiscard]] lvalue_emission_res subscript_expr::emit_dc_lvalue(compilation::function& fn, compilation::global_state& global) const noexcept {
    lvalue_emission_res lhs_res = m_lhs->emit_dc_lvalue(fn, global);
    if (!lhs_res) {
        return std::unexpected{std::move(lhs_res.error())};
    }

    emission_res rhs_res = m_rhs->emit_dc(fn, global);
    if (!rhs_res) {
        return std::unexpected{std::move(rhs_res.error())};
    }

    const ptr_type& ptr_t = std::get<ptr_type>(*m_lhs->get_type());

    std::expected<Opcode, std::string> store_opcode = get_store_opcode(*ptr_t.m_pointedAt);
    if (!store_opcode) {
        return std::unexpected{std::move(store_opcode.error())};
    }
    

    emission_res store_destination = fn.get_next_unused_register();
    if (!store_destination) {
        return std::unexpected{std::move(store_destination.error())};
    }

    const u64 ptr_size = get_size(*ptr_t.m_pointedAt);

    fn.emit_instruction(Opcode::IMulImm, *store_destination, *rhs_res, ptr_size);
    fn.emit_instruction(Opcode::IAdd, *store_destination, lhs_res->first, *store_destination);

    fn.free_register(*rhs_res);

    return std::pair{*store_destination, *store_opcode};
}

VAR_OPTIMIZATION_ACTION subscript_expr::var_optimization_pass(var_optimization_env& env) noexcept {
    env.check_action(&m_lhs);
    env.check_action(&m_rhs);
    return VAR_OPTIMIZATION_ACTION::NONE;
}

FOREACH_OPTIMIZATION_ACTION subscript_expr::foreach_optimization_pass(foreach_optimization_env& env) noexcept {
    env.check_action(&m_lhs);
    env.check_action(&m_rhs);
    return FOREACH_OPTIMIZATION_ACTION::NONE;
}

}