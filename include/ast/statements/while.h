#include "ast/statement.h"

namespace dconstruct::ast {
    struct while_stmt : public statement {
        explicit while_stmt(std::unique_ptr<expression>&& condition, std::unique_ptr<statement>&& body) noexcept :
        m_condition(std::move(condition)), m_body(std::move(body)) {};

        std::unique_ptr<expression> m_condition;
        std::unique_ptr<statement> m_body;
    };
}