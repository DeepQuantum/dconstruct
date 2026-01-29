#include "ast/primary_expressions/identifier.h"
#include "ast/assign.h"


namespace dconstruct::ast {

    
void identifier::pseudo_c(std::ostream& os) const {
    os << m_name.m_lexeme;
}

void identifier::pseudo_py(std::ostream& os) const {
    os << m_name.m_lexeme;
}

void identifier::pseudo_racket(std::ostream& os) const {
    os << m_name.m_lexeme;
}

[[nodiscard]] bool identifier::equals(const expression &rhs) const noexcept {
    const identifier* rhs_id = dynamic_cast<const identifier*>(&rhs);
    if (rhs_id == nullptr) {
        return false;
    }
    return m_name == rhs_id->m_name;
}

[[nodiscard]] expr_uptr identifier::simplify() const {
    return std::make_unique<identifier>(*this);
}

[[nodiscard]] expr_uptr identifier::clone() const {
    auto expr = std::make_unique<identifier>(m_name);
    if (m_type) expr->set_type(*m_type);
    return expr;
}


[[nodiscard]] u16 identifier::calc_complexity() const noexcept {
    return 1;
}

[[nodiscard]] std::unique_ptr<identifier> identifier::copy() const noexcept {
    return std::unique_ptr<ast::identifier>{ static_cast<ast::identifier*>(this->clone().release()) };
}

[[nodiscard]] bool identifier::identifier_name_equals(const std::string& name) const noexcept {
    return name == m_name.m_lexeme;
}

[[nodiscard]] semantic_check_res identifier::compute_type_checked(compilation::scope& scope) const noexcept {
    const full_type *type = scope.lookup(m_name.m_lexeme);
    if (!type) {
        return std::unexpected{semantic_check_error{"undeclared identifier: " + m_name.m_lexeme, this}};
    }
    return *type;
}

[[nodiscard]] emission_res identifier::emit_dc_lvalue(compilation::function& fn, compilation::global_state& global) const noexcept {
    const reg_idx* var_location = fn.m_varsToRegs.lookup(m_name.m_lexeme);

    if (!var_location) {
        return std::unexpected{"variable " + m_name.m_lexeme + "doesn't have a register."};
    }

    return *var_location;
}

[[nodiscard]] emission_res identifier::emit_dc(compilation::function& fn, compilation::global_state& global, const std::optional<reg_idx> destination) const noexcept {
    const auto function_sid = global.m_sidAliases.find(m_name.m_lexeme);
    if (function_sid != global.m_sidAliases.end()) {
        const u8 index = fn.add_to_symbol_table(function_sid->second.second);

        reg_idx true_destination;
        if (!destination) {
            const emission_res next_destination = fn.get_next_unused_register();
            if (!next_destination) {
                return next_destination;
            }
            true_destination = *next_destination;
        } else {
            true_destination = *destination;
        }

        if (std::holds_alternative<function_type>(function_sid->second.first)) {
            fn.emit_instruction(Opcode::LookupPointer, true_destination, index);
        } else { 
            fn.emit_instruction(Opcode::LoadStaticU64Imm, true_destination, index);
        }
        return true_destination;
    }

    return emit_dc_lvalue(fn, global);
}


VAR_OPTIMIZATION_ACTION identifier::var_optimization_pass(var_optimization_env& env) noexcept {
    if (!m_name.m_lexeme.starts_with("var")) {
        return VAR_OPTIMIZATION_ACTION::NONE;
    }
    auto* ctx = env.m_env.lookup(m_name.m_lexeme); 
    if (!ctx) {
        return VAR_OPTIMIZATION_ACTION::NONE;
    }
    return VAR_OPTIMIZATION_ACTION::VAR_READ;
}

MATCH_OPTIMIZATION_ACTION identifier::match_optimization_pass(match_optimization_env& env) noexcept {
    if (env.m_checkingCondition) {
        if (env.m_checkIdentifier.empty()) {
            env.m_checkIdentifier = m_name.m_lexeme;
            return MATCH_OPTIMIZATION_ACTION::CHECK_VAR_SET;
        } else if (env.m_checkIdentifier == m_name.m_lexeme) {
            return MATCH_OPTIMIZATION_ACTION::CHECK_VAR_READ;
        } else {
            return MATCH_OPTIMIZATION_ACTION::NONE;
        }
    }
    if (!m_name.m_lexeme.starts_with("var")) {
        return MATCH_OPTIMIZATION_ACTION::NONE;
    }
    if (!env.m_resultDeclaration) {
        return MATCH_OPTIMIZATION_ACTION::NONE;
    }
    return MATCH_OPTIMIZATION_ACTION::RESULT_VAR_WRITE;
}

FOREACH_OPTIMIZATION_ACTION identifier::foreach_optimization_pass(foreach_optimization_env&) noexcept {
    return FOREACH_OPTIMIZATION_ACTION::NONE;
}

}