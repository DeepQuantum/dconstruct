#include "ast/primary_expressions/sid_identifier.h"
#include "ast/assign.h"


namespace dconstruct::ast {

    
void sid_identifier::pseudo_c(std::ostream& os) const {
    os << m_name.m_lexeme;
}

void sid_identifier::pseudo_py(std::ostream& os) const {
    os << m_name.m_lexeme;
}

void sid_identifier::pseudo_racket(std::ostream& os) const {
    os << m_name.m_lexeme;
}

[[nodiscard]] bool sid_identifier::equals(const expression &rhs) const noexcept {
    const sid_identifier* rhs_id = dynamic_cast<const sid_identifier*>(&rhs);
    if (rhs_id == nullptr) {
        return false;
    }
    return m_name == rhs_id->m_name;
}

[[nodiscard]] expr_uptr sid_identifier::simplify() const {
    return std::make_unique<sid_identifier>(*this);
}

[[nodiscard]] expr_uptr sid_identifier::clone() const {
    auto expr = std::make_unique<sid_identifier>(m_name);
    if (m_type) expr->set_type(*m_type);
    return expr;
}


[[nodiscard]] u16 sid_identifier::calc_complexity() const noexcept {
    return 1;
}

[[nodiscard]] std::unique_ptr<sid_identifier> sid_identifier::copy() const noexcept {
    return std::unique_ptr<ast::sid_identifier>{ static_cast<ast::sid_identifier*>(this->clone().release()) };
}

[[nodiscard]] bool sid_identifier::identifier_name_equals(const std::string& name) const noexcept {
    return name == m_name.m_lexeme;
}

[[nodiscard]] semantic_check_res sid_identifier::compute_type_checked(compilation::scope& scope) const noexcept {
    const full_type *type = scope.lookup(m_name.m_lexeme);
    if (!type) {
        return std::unexpected{semantic_check_error{"undeclared identifier: " + m_name.m_lexeme, this}};
    }
    return *type;
}

[[nodiscard]] lvalue_emission_res sid_identifier::emit_dc_lvalue(compilation::function& fn, compilation::global_state& global) const noexcept {
    const reg_idx* var_location = fn.m_varsToRegs.lookup(m_name.m_lexeme);

    if (!var_location) {
        return std::unexpected{"variable " + m_name.m_lexeme + "doesn't have a register."};
    }

    return std::tuple{*var_location, Opcode::Move};
}

[[nodiscard]] emission_res sid_identifier::emit_dc(compilation::function& fn, compilation::global_state& global, const std::optional<reg_idx> destination) const noexcept {
    const auto function_sid = global.m_sidAliases.find(m_name.m_lexeme);
    if (function_sid != global.m_sidAliases.end()) {
        const u8 index = fn.add_to_symbol_table(function_sid->second.second);

        emission_res true_destination = fn.get_destination(destination);
        if (!true_destination) {
            return true_destination;
        }

        if (std::holds_alternative<function_type>(function_sid->second.first)) {
            fn.emit_instruction(Opcode::LookupPointer, *true_destination, index);
        } else { 
            fn.emit_instruction(Opcode::LoadStaticU64Imm, *true_destination, index);
        }
        return *true_destination;
    }
    const reg_idx* var_location = fn.m_varsToRegs.lookup(m_name.m_lexeme);

    if (!var_location) {
        return std::unexpected{"variable " + m_name.m_lexeme + "doesn't have a register."};
    }
    return *var_location;
}


VAR_OPTIMIZATION_ACTION sid_identifier::var_optimization_pass(var_optimization_env& env) noexcept {
    return VAR_OPTIMIZATION_ACTION::NONE;
}

MATCH_OPTIMIZATION_ACTION sid_identifier::match_optimization_pass(match_optimization_env& env) noexcept {
    return MATCH_OPTIMIZATION_ACTION::LITERAL;
}

FOREACH_OPTIMIZATION_ACTION sid_identifier::foreach_optimization_pass(foreach_optimization_env&) noexcept {
    return FOREACH_OPTIMIZATION_ACTION::NONE;
}

}