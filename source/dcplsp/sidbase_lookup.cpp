#include "dcplsp/sidbase_lookup.h"
#include "lsp/types.h"
#include "sidbase.h"
#include "qui/search.hpp"

#include <print>

namespace dconstruct::dcplsp {
    [[nodiscard]] lsp::CompletionList get_sidbase_matches(const lsp::String& needle, const dconstruct::SIDBase& sidbase, const u64 max_sidbase_matches) {
        if (needle.empty()) {
            return {};
        }

        const std::vector<std::string>& choices = sidbase.get_acquired_strings();

        std::println(stderr, "choices.size(): {}", choices.size());

        std::vector<qui::fuzzy_match> matches;
        lsp::Array<lsp::CompletionItem> results;

        const std::string normalized_query = qui::fuzzy_normalize(needle);
        for (std::size_t i = 0; i < choices.size(); ++i) {
            const std::string normalized_choice = qui::fuzzy_normalize(choices[i]);
            const std::size_t position = normalized_choice.find(normalized_query);
            if (i < 10) {
                std::println(stderr, "query: {} choice: {} pos: {}", normalized_query, normalized_choice, position);
            }
            if (position != std::string::npos) {
                std::println(stderr, "found match {}", normalized_choice);
                matches.push_back({i, static_cast<double>(position)});
            }
        }

        std::stable_sort(matches.begin(), matches.end(), [](const qui::fuzzy_match &lhs, const qui::fuzzy_match &rhs) {
            return lhs.score < rhs.score;
        });

        results.reserve(max_sidbase_matches);
        for (u64 i = 0; i < max_sidbase_matches && i < matches.size(); ++i) {
            results.push_back(lsp::CompletionItem {
                .label = choices[matches[i].index],
                .kind = lsp::CompletionItemKind::Constant,
            });
        }

        return lsp::CompletionList { .isIncomplete = true, .items = std::move(results) };
    }
}
