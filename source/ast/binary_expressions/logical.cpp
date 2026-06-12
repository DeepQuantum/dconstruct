#include "ast/binary_expressions/logical.h"

namespace dconstruct::ast {

    namespace {
        void append_branch_locations(std::vector<u64>& destination, std::vector<u64>&& source) noexcept {
            destination.insert(destination.end(), source.begin(), source.end());
        }

        [[nodiscard]] bool is_logical_operand(const primitive_kind kind) noexcept {
            return is_integral(kind) || is_floating_point(kind) || kind == primitive_kind::NULLPTR;
        }

        [[nodiscard]] bool are_compatible_logical_operands(
            const primitive_kind lhs,
            const primitive_kind rhs
        ) noexcept {
            if (lhs == primitive_kind::NULLPTR || rhs == primitive_kind::NULLPTR) {
                return is_logical_operand(lhs) && is_logical_operand(rhs);
            }

            return (is_integral(lhs) && is_integral(rhs)) || (is_floating_point(lhs) && is_floating_point(rhs));
        }
    }

    [[nodiscard]] expr_uptr logical_expr::simplify() const {
        return nullptr;
    }

    [[nodiscard]] semantic_check_res logical_expr::compute_type_checked(compilation::scope& env) const noexcept {
        semantic_check_res lhs_type = m_lhs->get_type_checked(env);
        if (!lhs_type) {
            return lhs_type;
        }

        semantic_check_res rhs_type = m_rhs->get_type_checked(env);
        if (!rhs_type) {
            return rhs_type;
        }

        const std::optional<std::string> invalid_logical = std::visit(
            [](auto&& lhs_type, auto&& rhs_type) -> std::optional<std::string> {
                using lhs_t = std::decay_t<decltype(lhs_type)>;
                using rhs_t = std::decay_t<decltype(rhs_type)>;

                if constexpr (!is_primitive<lhs_t>) {
                    return "expected primitive type for logical op lhs but got " + type_to_declaration_string(lhs_type);
                } else if constexpr (!is_primitive<rhs_t>) {
                    return "expected primitive type for logical op rhs but got " + type_to_declaration_string(rhs_type);
                } else if (are_compatible_logical_operands(lhs_type.m_type, rhs_type.m_type)) {
                    return std::nullopt;
                } else {
                    return "expected compatible types for logical op but got " + type_to_declaration_string(lhs_type) + " and " + type_to_declaration_string(rhs_type);
                }
            },
            *lhs_type,
            *rhs_type
        );

        if (!invalid_logical) {
            return make_type_from_prim(primitive_kind::BOOL);
        }

        return std::unexpected{semantic_check_error{*invalid_logical, this}};
    }

    [[nodiscard]] emission_res logical_expr::emit_dc(
        compilation::function_context& fn,
        compilation::global_state& global,
        const std::optional<reg_idx> destination
    ) const noexcept {
        const emission_res logical_destination = fn.fix_destination(destination);
        if (!logical_destination) {
            return logical_destination;
        }

        if (m_operator.m_lexeme == "&&") {
            fn.emit_instruction(Opcode::LoadU16Imm, *logical_destination, 1, 0);

            const emission_res lhs = m_lhs->emit_dc(fn, global);
            if (!lhs) {
                return lhs;
            }

            const u16 lhs_false_branch = static_cast<u16>(fn.m_instructions.size());
            fn.emit_instruction(Opcode::BranchIfNot, compilation::function_context::BRANCH_PLACEHOLDER, *lhs, compilation::function_context::BRANCH_PLACEHOLDER);
            fn.free_register(*lhs);

            const emission_res rhs = m_rhs->emit_dc(fn, global);
            if (!rhs) {
                return rhs;
            }

            const u16 rhs_false_branch = static_cast<u16>(fn.m_instructions.size());
            fn.emit_instruction(Opcode::BranchIfNot, compilation::function_context::BRANCH_PLACEHOLDER, *rhs, compilation::function_context::BRANCH_PLACEHOLDER);
            fn.free_register(*rhs);

            const u16 done_branch = static_cast<u16>(fn.m_instructions.size());
            fn.emit_instruction(Opcode::Branch, compilation::function_context::BRANCH_PLACEHOLDER, 00, compilation::function_context::BRANCH_PLACEHOLDER);

            const u16 set_false_location = static_cast<u16>(fn.m_instructions.size());
            fn.emit_instruction(Opcode::LoadU16Imm, *logical_destination, 0, 0);

            const u16 end_location = static_cast<u16>(fn.m_instructions.size());
            fn.m_instructions[lhs_false_branch].set_lo_hi(set_false_location);
            fn.m_instructions[rhs_false_branch].set_lo_hi(set_false_location);
            fn.m_instructions[done_branch].set_lo_hi(end_location);

            return *logical_destination;
        }

        fn.emit_instruction(Opcode::LoadU16Imm, *logical_destination, 0, 0);

        const emission_res lhs = m_lhs->emit_dc(fn, global);
        if (!lhs) {
            return lhs;
        }

        const u16 lhs_true_branch = static_cast<u16>(fn.m_instructions.size());
        fn.emit_instruction(Opcode::BranchIf, compilation::function_context::BRANCH_PLACEHOLDER, *lhs, compilation::function_context::BRANCH_PLACEHOLDER);
        fn.free_register(*lhs);

        const emission_res rhs = m_rhs->emit_dc(fn, global);
        if (!rhs) {
            return rhs;
        }

        const u16 rhs_true_branch = static_cast<u16>(fn.m_instructions.size());
        fn.emit_instruction(Opcode::BranchIf, compilation::function_context::BRANCH_PLACEHOLDER, *rhs, compilation::function_context::BRANCH_PLACEHOLDER);
        fn.free_register(*rhs);

        const u16 done_branch = static_cast<u16>(fn.m_instructions.size());
        fn.emit_instruction(Opcode::Branch, compilation::function_context::BRANCH_PLACEHOLDER, 00, compilation::function_context::BRANCH_PLACEHOLDER);

        const u16 set_true_location = static_cast<u16>(fn.m_instructions.size());
        fn.emit_instruction(Opcode::LoadU16Imm, *logical_destination, 1, 0);

        const u16 end_location = static_cast<u16>(fn.m_instructions.size());
        fn.m_instructions[lhs_true_branch].set_lo_hi(set_true_location);
        fn.m_instructions[rhs_true_branch].set_lo_hi(set_true_location);
        fn.m_instructions[done_branch].set_lo_hi(end_location);

        return *logical_destination;
    }

    [[nodiscard]] condition_branch_res logical_expr::emit_dc_branch(
        compilation::function_context& fn,
        compilation::global_state& global,
        const bool branch_when_true
    ) const noexcept {
        const bool is_and = m_operator.m_lexeme == "&&";

        if (is_and && !branch_when_true) {
            condition_branch_res lhs_false = m_lhs->emit_dc_branch(fn, global, false);
            if (!lhs_false) {
                return std::unexpected{lhs_false.error()};
            }

            condition_branch_res rhs_false = m_rhs->emit_dc_branch(fn, global, false);
            if (!rhs_false) {
                return std::unexpected{rhs_false.error()};
            }

            append_branch_locations(*lhs_false, std::move(*rhs_false));
            return lhs_false;
        }

        if (is_and) {
            condition_branch_res lhs_false = m_lhs->emit_dc_branch(fn, global, false);
            if (!lhs_false) {
                return std::unexpected{lhs_false.error()};
            }

            condition_branch_res rhs_true = m_rhs->emit_dc_branch(fn, global, true);
            if (!rhs_true) {
                return std::unexpected{rhs_true.error()};
            }

            patch_branch_targets(fn, *lhs_false, static_cast<u16>(fn.m_instructions.size()));
            return rhs_true;
        }

        if (branch_when_true) {
            condition_branch_res lhs_true = m_lhs->emit_dc_branch(fn, global, true);
            if (!lhs_true) {
                return std::unexpected{lhs_true.error()};
            }

            condition_branch_res rhs_true = m_rhs->emit_dc_branch(fn, global, true);
            if (!rhs_true) {
                return std::unexpected{rhs_true.error()};
            }

            append_branch_locations(*lhs_true, std::move(*rhs_true));
            return lhs_true;
        }

        condition_branch_res lhs_true = m_lhs->emit_dc_branch(fn, global, true);
        if (!lhs_true) {
            return std::unexpected{lhs_true.error()};
        }

        condition_branch_res rhs_false = m_rhs->emit_dc_branch(fn, global, false);
        if (!rhs_false) {
            return std::unexpected{rhs_false.error()};
        }

        patch_branch_targets(fn, *lhs_true, static_cast<u16>(fn.m_instructions.size()));
        return rhs_false;
    }

    void logical_expr::pseudo_py(ast_serialization_buffer& buffer) const {
        if (m_operator.m_lexeme == "&&") {
            buffer.append(*m_lhs, " and "sv, *m_rhs);
        } else {
            buffer.append(*m_lhs, " or "sv, *m_rhs);
        }
    }

    void logical_expr::pseudo_racket(ast_serialization_buffer& buffer) const {
        if (m_operator.m_lexeme == "&&") {
            buffer.append("(and "sv, *m_lhs, ' ', *m_rhs, ')');
        } else {
            buffer.append("(or "sv, *m_lhs, ' ', *m_rhs, ')');
        }
    }

}
