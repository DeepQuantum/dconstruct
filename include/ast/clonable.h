#include "base.h"

namespace dconstruct::ast {
    template<typename base_t, typename impl_t>
    struct Iclonable : public base_t {
        [[nodiscard]] virtual std::unique_ptr<expression> clone() const = 0;
    };
}