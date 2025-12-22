#pragma once

#include "ast/statement.h"

namespace dconstruct::ast {
    struct dummy : public statement {
        dummy() noexcept = default;
        void pseudo_c(std::ostream&) const final {};
        void pseudo_py(std::ostream&) const final {};
		void pseudo_racket(std::ostream&) const final {};
        [[nodiscard]] bool equals(const statement& rhs) const noexcept final {
            return dynamic_cast<const dummy*>(&rhs);
        }
        [[nodiscard]] std::unique_ptr<statement> clone() const noexcept final {
            return std::make_unique<dummy>();
        }
        VAR_FOLDING_ACTION decomp_optimization_pass(second_pass_env& env) noexcept final {
            return VAR_FOLDING_ACTION::NONE;
        }
    };
}