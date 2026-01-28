#include "base.h"
#include "function.h"


namespace dconstruct::compilation {
    struct dc_rvalue_register {

        explicit dc_rvalue_register(const reg_idx idx, compilation::function& fn) noexcept : m_idx(idx), m_fn(fn) {
            fn.m_usedRegisters.set(idx, true);
        };

        ~dc_rvalue_register() noexcept {
            if (m_idx) {
                m_fn.m_usedRegisters.set(*m_idx, false);
            }
        }

        dc_rvalue_register(const dc_rvalue_register&) = delete; 
        dc_rvalue_register& operator=(const dc_rvalue_register&) = delete;

        dc_rvalue_register(dc_rvalue_register&& rhs) noexcept : m_idx(rhs.m_idx), m_fn(rhs.m_fn) {
            rhs.m_idx = std::nullopt;
        };

        dc_rvalue_register& operator=(dc_rvalue_register&& rhs) noexcept {
            if (m_idx) {
                m_fn.free_register(*m_idx);
            }
            m_idx = rhs.m_idx;
            rhs.m_idx = std::nullopt;
            return *this;
        };

        [[nodiscard]] operator reg_idx() const noexcept {
            assert(m_idx.has_value());
            return *m_idx;
        }

        std::optional<reg_idx> m_idx;
    private:
        compilation::function& m_fn;
    };

    // struct dc_lvalue_register {
    //     explicit dc_lvalue_register(reg_idx idx, compilation::function& fn) noexcept : m_idx(idx), m_fn(fn), m_refCount(new u64(0)) {};

    //     dc_lvalue_register(const dc_lvalue_register& rhs) noexcept : m_idx(rhs.m_idx), m_refCount(rhs.m_refCount), m_fn(rhs.m_fn) {
    //         *m_refCount++;
    //     }
    //     dc_lvalue_register& operator=(const dc_lvalue_register& rhs) noexcept = delete;

    //     dc_lvalue_register(dc_lvalue_register&& rhs) noexcept : m_idx(rhs.m_idx), m_refCount(rhs.m_refCount), m_fn(rhs.m_fn) {
    //         rhs.m_idx = std::nullopt;
    //     }

    //     dc_lvalue_register& operator=(dc_lvalue_register&& rhs) noexcept {
    //         if (m_idx) {
    //             *m_refCount--;
    //         }
    //         m_idx = rhs.m_idx;
    //         rhs.m_idx = std::nullopt;

    //     }

    //     [[nodiscard]] operator reg_idx() const noexcept {
    //         return m_idx;
    //     }

    //     std::optional<reg_idx> m_idx;
    // private:
    //     u64* m_refCount;
    //     compilation::function& m_fn;
    // };

    struct dc_lvalue_register {
        explicit dc_lvalue_register(reg_idx idx) noexcept : m_idx(idx) {}

        dc_lvalue_register(const dc_lvalue_register&) = default;
        dc_lvalue_register& operator=(const dc_lvalue_register&) = default;

        [[nodiscard]] operator reg_idx() const noexcept {
            return m_idx;
        }

        reg_idx m_idx;
    };

    using dc_register = std::variant<dc_rvalue_register, dc_lvalue_register>;

    [[nodiscard]] std::optional<reg_idx> get(const dc_register& reg) noexcept {
        return std::visit([](auto&& arg) -> std::optional<reg_idx> {
            return arg.m_idx;
        }, reg);
    }
}