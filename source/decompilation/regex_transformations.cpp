#include "decompilation/regex_transformations.h"


#include <format>
#include <print>

namespace dconstruct {

template <ctll::fixed_string Pattern, typename Replacer>
[[nodiscard]] std::string regex_replace(const std::string& in, Replacer&& replace) {
    std::string out;
    out.reserve(in.size());
    auto last = in.begin();
    for (auto m : ctre::range<Pattern>(in)) {
        auto whole = m.get<0>();
        out.append(last, whole.begin());
        replace(out, m);
        last = whole.end();
    }
    out.append(last, in.end());
    return out;
}

[[nodiscard]] std::string replace_boxed_values(const std::string& in) {
    return regex_replace<R"((new-boxed-value\()(?<boxed_value_num>\d+), (?<boxed_val>.*?)(\)))">(
        in, [](std::string& out, const auto& m) {
            std::format_to(std::back_inserter(out), "boxed_{}({})",
                BOXED_VALUE_IDS[m.get<R"(boxed_value_num)">().to_number()],
                m.get<R"(boxed_val)">().to_view());
        });
}

[[nodiscard]] std::string replace_array_dereference(const std::string& in) {
    return regex_replace<R"(\(\*(?<array_var>var_\d+) \+ \((?<index>\d+) \* \d+\)\))">(
        in, [](std::string& out, const auto& m) {
            std::format_to(std::back_inserter(out), "{}[{}]", m.get<R"(array_var)">().to_view(), m.get<R"(index)">().to_view());
        });
}

[[nodiscard]] std::string replace_foreach_check(const std::string& in) {
    static constexpr ctll::fixed_string head_pattern =
        R"(foreach \([a-zA-Z\d_]*\? var_(?<foreach_var>\d+)\ : .*?\) \{)";
    static constexpr ctll::fixed_string body_pattern =
        R"(\s*(?<full_body>u16 var_(?<decl_var>\d+);)"
        R"(\s*if \(var_(\d+) && \*\(u16\*\)\(var_(\d+) \+ 12\) == 7\) \{)"
        R"(\s*var_(\d+) = \*\(u64\*\)var_(\d+);\s*\})"
        R"(\s*else if \(var_(\d+) && \*\(u16\*\)\(var_(\d+) \+ 12\) == 5\) \{)"
        R"(\s*var_(\d+) = \*\(u64\*\)var_(\d+);\s*\})"
        R"(\s*else if \(var_(\d+) && \*\(u16\*\)\(var_(\d+) \+ 12\) == 4\) \{)"
        R"(\s*var_(\d+) = \*\(u64\*\)var_(\d+);\s*\})"
        R"(\s*else \{\s*var_(\d+) = 0;\s*\}))";

    std::string out;
    out.reserve(in.size());
    const char* const end = in.data() + in.size();
    const char* last = in.data();
    const char* cur = in.data();
    while (auto head = ctre::search<head_pattern>(cur, end)) {
        const auto whole = head.get<0>().to_view();
        const char* const whole_begin = whole.data();
        const char* const whole_end = whole.data() + whole.size();
        const auto body = ctre::starts_with<body_pattern>(whole_end, end);
        if (!body) {
            cur = whole_end;
            continue;
        }
        const auto foreach_var = head.get<R"(foreach_var)">().to_view();
        const auto decl_var = body.get<R"(decl_var)">().to_view();
        const auto body_match = body.get<0>().to_view();
        out.append(last, whole_begin);
        out.append(whole_begin, foreach_var.data());
        out.append(decl_var);
        out.append(foreach_var.data() + foreach_var.size(), whole_end);
        last = body_match.data() + body_match.size();
        cur = last;
    }
    out.append(last, end);
    return out;
}

[[nodiscard]] std::string apply_decomp_regex_transformations(std::string&& input) {
    static constexpr std::array replace_funcs = {
        replace_boxed_values,
        replace_array_dereference,
        replace_foreach_check
    };
    
    std::string out = std::move(input);
    for (const auto& replace_func : replace_funcs) {
        out = replace_func(out);
    }
    return out;

}


}