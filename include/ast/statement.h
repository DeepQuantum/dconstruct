#pragma once

#include "base.h"
#include "expression.h"
#include <vector>
#include "compilation/tokens.h"

namespace dconstruct::ast {

    struct statement : public Iprintable {
        virtual ~statement() = default;
        [[nodiscard]] virtual b8 equals(const statement &rhs) const noexcept = 0;
    };

    [[nodiscard]] inline b8 operator==(const statement& lhs, const statement& rhs) noexcept {
        return lhs.equals(rhs);
    }

    [[nodiscard]] inline b8 operator==(const std::unique_ptr<statement>& lhs, const std::unique_ptr<statement>& rhs) noexcept {
        return *lhs == *rhs;
    }
}