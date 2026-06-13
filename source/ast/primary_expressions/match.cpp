#include "ast/primary_expressions/match.h"
#include "ast/primary_expressions/literal.h"
#include "ast/primary_expressions/struct_access.h"
#include <algorithm>
#include <numeric>
#include <string_view>

using namespace std::literals::string_view_literals;

namespace dconstruct::ast {

    using grouped_patterns_t = std::vector<std::pair<std::vector<const expr_uptr*>, const expr_uptr*>>;

    [[nodiscard]] static std::string pattern_list_string(const std::vector<const expr_uptr*>& patterns) {
        ast_serialization_buffer buffer;
        for (u64 i = 0; i < patterns.size(); ++i) {
            if (i != 0) {
                buffer.append(", "sv);
            }
            buffer.append(**patterns[i]);
        }
        return buffer.take();
    }

    [[nodiscard]] static u64 max_pattern_width(const grouped_patterns_t& grouped) {
        u64 max_size = sizeof("else") - 1;
        for (const auto& [patterns, _] : grouped) {
            max_size = std::max<u64>(max_size, pattern_list_string(patterns).length());
        }
        return max_size;
    }

    void match_expr::pseudo_c(ast_serialization_buffer& buffer) const {
        buffer.append("match ("sv);
        bool first = true;
        for (const auto& condition : m_conditions) {
            if (!first) {
                buffer.append("; "sv);
            }
            first = false;
            buffer.append(*condition);
        }
        buffer.append(") {\n"sv);
        buffer.indent_more();

        const auto grouped = group_patterns();
        const u64 max_size = max_pattern_width(grouped);

        for (const auto& [patterns, expression] : grouped) {
            buffer.append_indent();
            buffer.append_padded(pattern_list_string(patterns), max_size);
            buffer.append(" -> "sv, **expression, ",\n"sv);
        }
        buffer.append_indent();
        buffer.append_padded("else", max_size);
        buffer.append(" -> "sv, *m_default, '\n');
        buffer.indent_less();
        buffer.append_indent();
        buffer.append('}');
    }

    void match_expr::to_pseudo_c_colored_string(code_color_serialization_buffer& buffer) const noexcept {
        buffer.append(AST_COLOR::KEYWORD, "match "sv);
        buffer.append(AST_COLOR::PUNCTUATION, '(');
        bool first = true;
        for (const auto& condition : m_conditions) {
            if (!first) {
                buffer.append(AST_COLOR::PUNCTUATION, "; "sv);
            }
            first = false;
            buffer.append(*condition);
        }
        buffer.append(AST_COLOR::PUNCTUATION, ") {\n"sv);
        buffer.indent_more();

        const auto grouped = group_patterns();
        const u64 max_size = max_pattern_width(grouped);

        for (const auto& [patterns, expression] : grouped) {
            buffer.append_indent();
            const u64 plain_width = pattern_list_string(patterns).length();
            for (u64 i = 0; i < patterns.size(); ++i) {
                if (i != 0) {
                    buffer.append(AST_COLOR::PUNCTUATION, ", "sv);
                }
                buffer.append(**patterns[i]);
            }
            if (plain_width < max_size) {
                buffer.append(AST_COLOR::BLANK, std::string(max_size - plain_width, ' '));
            }
            buffer.append(AST_COLOR::OPERATOR, " -> "sv);
            buffer.append(**expression);
            buffer.append(AST_COLOR::PUNCTUATION, ",\n"sv);
        }
        buffer.append_indent();
        buffer.append_padded(AST_COLOR::KEYWORD, "else"sv, max_size);
        buffer.append(AST_COLOR::OPERATOR, " -> "sv);
        buffer.append(*m_default);
        buffer.append(AST_COLOR::BLANK, '\n');
        buffer.indent_less();
        buffer.append_indent();
        buffer.append(AST_COLOR::PUNCTUATION, '}');
    }

    [[nodiscard]] std::vector<std::pair<std::vector<const expr_uptr*>, const expr_uptr*>> match_expr::group_patterns() const noexcept {
        std::vector<std::pair<std::vector<const expr_uptr*>, const expr_uptr*>> res;

        for (const auto& [pattern, match] : m_matchPairs) {
            auto match_exists = [&match](const auto& pair) -> bool { return **pair.second == *match; };

            if (auto match_group = std::find_if(res.begin(), res.end(), match_exists); match_group != res.end()) {
                match_group->first.push_back(&pattern);
            } else {
                res.emplace_back(std::vector<const expr_uptr*>{&pattern}, &match);
            }
        }

        return res;
    }

    void match_expr::pseudo_py(ast_serialization_buffer& buffer) const {
        buffer.append("match "sv);
        bool first = true;
        for (const auto& condition : m_conditions) {
            if (!first) {
                buffer.append("; "sv);
            }
            first = false;
            buffer.append(*condition);
        }
        buffer.append(":\n"sv);
        buffer.indent_more();

        const auto grouped = group_patterns();
        const u64 max_size = max_pattern_width(grouped);

        for (const auto& [patterns, expression] : grouped) {
            buffer.append_indent();
            buffer.append_padded(pattern_list_string(patterns), max_size);
            buffer.append(" -> "sv, **expression, ",\n"sv);
        }
        buffer.append_indent();
        buffer.append_padded("else", max_size);
        buffer.append(" -> "sv, *m_default, '\n');
        buffer.indent_less();
    }

    void match_expr::pseudo_racket(ast_serialization_buffer& buffer) const {
        buffer.append("(match (list"sv);
        for (const auto& c : m_conditions) {
            buffer.append(' ', *c);
        }
        buffer.append(")\n"sv);
        buffer.indent_more();

        const auto grouped = group_patterns();
        for (const auto& [patterns, expression] : grouped) {
            for (const auto& pattern : patterns) {
                buffer.append_indent();
                buffer.append('[', **pattern, ' ', **expression, "]\n"sv);
            }
        }
        buffer.append_indent();
        buffer.append("[_ "sv, *m_default, "]\n"sv);
        buffer.indent_less();
        buffer.append_indent();
        buffer.append(')');
    }

    [[nodiscard]] expr_uptr match_expr::simplify() const {
        return clone();
    }

    [[nodiscard]] bool match_expr::equals(const expression& rhs) const noexcept {
        const match_expr* rhs_ptr = dynamic_cast<const match_expr*>(&rhs);
        if (!rhs_ptr) {
            return false;
        }
        return m_conditions == rhs_ptr->m_conditions && m_matchPairs == rhs_ptr->m_matchPairs && m_default == rhs_ptr->m_default;
    }

    [[nodiscard]] u16 match_expr::calc_complexity() const noexcept {
        const u16 conditions_complexity =
            std::accumulate(
                m_conditions.begin(),
                m_conditions.end(),
                u16{0},
                [](u16 acc, const auto& cond) {
                    return static_cast<u16>(acc + cond->get_complexity());
                }
            );

        const u16 match_pairs_complexity =
            std::accumulate(
                m_matchPairs.begin(),
                m_matchPairs.end(),
                u16{0},
                [](u16 acc, const auto& pair) {
                    return static_cast<u16>(acc + pair.first->get_complexity() + pair.second->get_complexity());
                }
            );

        return 1 + conditions_complexity + match_pairs_complexity;
    }

    [[nodiscard]] expr_uptr match_expr::clone() const {
        std::vector<expr_uptr> new_conditions;
        std::vector<matches_t> new_matchpairs;
        new_conditions.reserve(m_conditions.size());
        new_matchpairs.reserve(m_matchPairs.size());
        for (const auto& condition : m_conditions) {
            new_conditions.emplace_back(condition->clone());
        }
        for (const auto& [pattern, expression] : m_matchPairs) {
            new_matchpairs.emplace_back(pattern->clone(), expression->clone());
        }
        return std::make_unique<match_expr>(std::move(new_conditions), std::move(new_matchpairs), m_default->clone());
    }

    [[nodiscard]] full_type match_expr::compute_type_unchecked(const compilation::scope& env) const noexcept {
        for (const auto& [pattern, expression] : m_matchPairs) {
            if (const auto type_res = expression->get_type_unchecked(env); !is_unknown(type_res)) {
                return type_res;
            }
        }
        return std::monostate();
    }

    [[nodiscard]] semantic_check_res match_expr::compute_type_checked(compilation::scope& env) const noexcept {
        for (auto cond_it = m_conditions.begin(); cond_it < m_conditions.end() - 1; ++cond_it) {
            const expr_uptr& condition = *cond_it;

            const semantic_check_res cond_type = condition->compute_type_checked(env);

            if (!cond_type) {
                return cond_type;
            }

            const errmsg invalid_type = std::visit(
                [](auto&& cond) -> errmsg {
                    using T = std::decay_t<decltype(cond)>;

                    if constexpr (is_primitive<T>) {
                        if (is_integral(cond.m_type)) {
                            return std::nullopt;
                        }
                        return "expected arithmetic type for precondition but got " + type_to_declaration_string(cond);
                    }
                    return "precondition must be of arithmetic type, but got " + type_to_declaration_string(cond);
                },
                *cond_type
            );

            if (invalid_type) {
                return std::unexpected{semantic_check_error{*invalid_type, condition.get()}};
            }
        }

        const semantic_check_res match_var_type = m_conditions.back()->get_type_checked(env);

        if (!match_var_type) {
            return match_var_type;
        }

        const errmsg invalid_match_var_type = std::visit(
            [](auto&& arg) -> errmsg {
                using match_var_t = std::decay_t<decltype(arg)>;

                if constexpr (is_primitive<match_var_t>) {
                    if (arg.m_type != primitive_kind::NULLPTR && arg.m_type != primitive_kind::NOTHING) {
                        return std::nullopt;
                    }
                    return "expected complete primitive type for match variable but got " + type_to_declaration_string(arg);
                }
                return "expected primitive type for match variable but got " + type_to_declaration_string(arg);
            },
            *match_var_type
        );

        if (invalid_match_var_type) {
            return std::unexpected{semantic_check_error{*invalid_match_var_type, m_conditions.back().get()}};
        }

        bool result_type_set = false;
        full_type result_type = std::monostate();

        for (const auto& [pattern, expression] : m_matchPairs) {
            const semantic_check_res pattern_type = pattern->get_type_checked(env);

            if (!pattern_type) {
                return pattern_type;
            }

            if (const errmsg pattern_err = not_assignable_reason(*match_var_type, *pattern_type)) {
                return std::unexpected{semantic_check_error{
                    "expected pattern type " + type_to_declaration_string(*match_var_type) + " but got " + type_to_declaration_string(*pattern_type), pattern.get()}};
            }

            const semantic_check_res expr_type = expression->get_type_checked(env);

            if (!expr_type) {
                return expr_type;
            }

            if (!result_type_set) {
                result_type = *expr_type;
                const auto load_opcode = get_static_load_opcode(result_type);
                if (!load_opcode) {
                    return std::unexpected{semantic_check_error{load_opcode.error(), expression.get()}};
                }
                m_loadOpcode = *load_opcode;
                result_type_set = true;
            } else if (*expr_type != result_type) {
                return std::unexpected{semantic_check_error{"expected result type " + type_to_declaration_string(result_type) + " but got " + type_to_declaration_string(*expr_type), expression.get()}};
            }
        }

        assert(result_type_set && !std::holds_alternative<std::monostate>(result_type));

        if (!m_default) {
            return result_type;
        }

        const semantic_check_res default_type = m_default->get_type_checked(env);

        if (!default_type) {
            return default_type;
        }

        if (auto bad_assign = not_assignable_reason(result_type, *default_type)) {
            return std::unexpected{semantic_check_error{*bad_assign, m_default.get()}};
        }

        return result_type;
    }

    void match_expr::init_match_metrics() noexcept {
        sort_matches();
        calc_density_R();
    }

    void match_expr::sort_matches() noexcept {
        const bool all_patterns_are_raw_numbers = std::all_of(
            m_matchPairs.begin(),
            m_matchPairs.end(),
            [](const matches_t& match) noexcept {
                return match.first->raw_pattern_number().has_value();
            }
        );

        if (!all_patterns_are_raw_numbers) {
            return;
        }

        std::sort(
            m_matchPairs.begin(),
            m_matchPairs.end(),
            [](const matches_t& lhs, const matches_t& rhs) noexcept {
                const auto lhs_value = lhs.first->raw_pattern_number();
                const auto rhs_value = rhs.first->raw_pattern_number();
                assert(lhs_value && rhs_value);
                return *lhs_value < *rhs_value;
            }
        );
    }

    void match_expr::calc_density_R() noexcept {
        if (m_matchPairs.empty()) {
            return;
        }

        for (const auto& [pattern, _] : m_matchPairs) {
            if (!pattern->raw_pattern_number()) {
                return;
            }
        }

        const auto first_value = m_matchPairs.front().first->raw_pattern_number();
        const auto last_value = m_matchPairs.back().first->raw_pattern_number();
        assert(first_value && last_value);

        m_min = *first_value;
        m_max = *last_value;
        m_R = m_max - m_min + 1;
        m_density = (f32)m_matchPairs.size() / (f32)m_R;
        m_hasDensity = true;
    }

    [[nodiscard]] resstr<reg_idx> match_expr::emit_dc(
        compilation::function_context& fn,
        compilation::global_state& global,
        const std::optional<reg_idx> destination
    ) const noexcept {
        const resstr<reg_idx> condition = m_conditions.back()->emit_dc(fn, global);
        if (!condition) {
            return condition;
        }

        if (!m_hasDensity) {
            return std::unexpected{"only compilation with literals is currently supported"};
        }

        constexpr f32 array_approach_min_density = .5f;
        constexpr u32 array_approach_max_R = 16;

        if (m_density >= array_approach_min_density && m_R <= array_approach_max_R) {
            return emit_dc_array_approach(fn, global, *condition, destination);
        }

        return std::unexpected{"match expression is not dense enough for array compilation"};
    }

    [[nodiscard]] resstr<reg_idx> match_expr::emit_dc_array_approach(
        compilation::function_context& fn,
        compilation::global_state& global,
        const reg_idx condition_reg,
        const std::optional<reg_idx> destination
    ) const noexcept {
        const i64 array_size = m_max - m_min + 1;
        auto match_it = m_matchPairs.begin();

        std::vector<sid64> entries;
        entries.reserve(array_size);

        const i64 start_symbol_table_size = fn.m_symbolTable.size();

        static const literal sentinel{SID("sentinel")};

        const auto emit_default_to_symbol_table = [&]() -> resstr<u16> {
            if (m_default) {
                return m_default->emit_to_symbol_table(fn, global);
            }
            return sentinel.emit_to_symbol_table(fn, global);
        };

        std::optional<u64> default_symbol_table_entry;
        resstr<u16> symbol_table_entry_res;
        for (i64 i = m_min; i <= m_max; ++i) {
            if (match_it != m_matchPairs.end() && static_cast<u16>(*match_it->first->raw_pattern_number()) == i) {
                symbol_table_entry_res = match_it->second->emit_to_symbol_table(fn, global);
                ++match_it;
            } else {
                symbol_table_entry_res = emit_default_to_symbol_table();
                default_symbol_table_entry = symbol_table_entry_res.value_or(0);
            }
            if (!symbol_table_entry_res) {
                return std::unexpected{symbol_table_entry_res.error()};
            }
            entries.push_back(*symbol_table_entry_res);
        }

        if (!default_symbol_table_entry && m_default) {
            const auto st_res = m_default->emit_to_symbol_table(fn, global);
            if (!st_res) {
                return std::unexpected{st_res.error()};
            }
            default_symbol_table_entry = *st_res;
        }

        const i64 start_idx = m_min + start_symbol_table_size;
        u64 default_branch = 0;

        // for now assume only unsigned numbers in cases
        // if we have a default: check if condition var is larger than (largest case - lowest case), in which case we just load the default case
        const u16 default_end_idx = m_max - m_min;
        if (m_default) {
            u64 min_idx = fn.add_to_symbol_table(m_min);
            u64 max_idx = fn.add_to_symbol_table(m_max);
            u64 offset = fn.add_to_symbol_table(default_end_idx);

            const resstr<reg_idx> range_dest = fn.get_next_unused_register();
            if (!range_dest) {
                return range_dest;
            }

            if (m_min != 0) {
                const Opcode arith_instruction = m_min < 0 ? Opcode::IAdd : Opcode::ISub;
                fn.emit_instruction(Opcode::LoadStaticI64Imm, *range_dest, offset, 0);
                fn.emit_instruction(arith_instruction, condition_reg, condition_reg, *range_dest);
            }

            fn.emit_lohi_instruction(Opcode::LoadU16Imm, *range_dest, default_end_idx);
            fn.emit_instruction(Opcode::IGreaterThan, *range_dest, condition_reg, *range_dest);
            default_branch = fn.m_instructions.size();
            fn.emit_instruction(Opcode::BranchIfNot, compilation::function_context::BRANCH_PLACEHOLDER, *range_dest, compilation::function_context::BRANCH_PLACEHOLDER);
            fn.free_register(*range_dest);
        }

        const resstr<reg_idx> load_dest_res = fn.fix_destination(destination);
        if (!load_dest_res) {
            return load_dest_res;
        }

        assert(m_loadOpcode.has_value());
        u64 end_branch = 0;
        if (m_default) {
            fn.emit_instruction(Opcode::LoadStaticU64Imm, *load_dest_res, (u8)*default_symbol_table_entry);

            end_branch = fn.m_instructions.size();
            fn.emit_instruction(Opcode::Branch, compilation::function_context::BRANCH_PLACEHOLDER, 0, compilation::function_context::BRANCH_PLACEHOLDER);
        }
        const u64 default_location = fn.m_instructions.size();
        if (start_idx != 0) {
            const Opcode math_opcode = start_idx < 0 ? Opcode::ISubImm : Opcode::IAddImm;
            fn.emit_instruction(math_opcode, condition_reg, condition_reg, start_idx);
        }
        fn.emit_instruction(*m_loadOpcode, *load_dest_res, condition_reg);
        const u64 end_location = fn.m_instructions.size();
        if (m_default) {
            fn.m_instructions[default_branch].set_lo_hi(static_cast<u16>(default_location));
            fn.m_instructions[end_branch].set_lo_hi(static_cast<u16>(end_location));
        }

        return *load_dest_res;
    }

    VAR_OPTIMIZATION_ACTION match_expr::var_optimization_pass(var_optimization_env& env) noexcept {
        for (auto& condition : m_conditions) {
            env.check_action(&condition);
        }
        for (auto& [pattern, expression] : m_matchPairs) {
            env.check_action(&pattern);
            env.check_action(&expression);
        }
        return VAR_OPTIMIZATION_ACTION::NONE;
    }

    FOREACH_OPTIMIZATION_ACTION match_expr::foreach_optimization_pass(foreach_optimization_env& env) noexcept {
        for (auto& condition : m_conditions) {
            env.check_action(&condition);
        }
        for (auto& [pattern, expression] : m_matchPairs) {
            env.check_action(&pattern);
            env.check_action(&expression);
        }
        return FOREACH_OPTIMIZATION_ACTION::NONE;
    }

    void match_expr::regex_optimization_pass() noexcept {
        for (auto& condition : m_conditions) {
            condition->regex_optimization_pass();
        }
        for (auto& [pattern, expression] : m_matchPairs) {
            pattern->regex_optimization_pass();
            expression->regex_optimization_pass();
        }
        if (m_default) {
            m_default->regex_optimization_pass();
        }
    }

    [[nodiscard]] std::unique_ptr<struct_access> match_expr::to_struct_access() noexcept {
        for (auto& condition : m_conditions) {
            replace_if_struct_access(condition);
        }
        for (auto& [pattern, expression] : m_matchPairs) {
            replace_if_struct_access(pattern);
            replace_if_struct_access(expression);
        }
        if (m_default) {
            replace_if_struct_access(m_default);
        }
        return nullptr;
    }

}
