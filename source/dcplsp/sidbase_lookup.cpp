#include "dcplsp/sidbase_lookup.h"
#include "lsp/types.h"
#include "sidbase.h"
#include "qui/search.hpp"

namespace dconstruct::dcplsp {
    [[nodiscard]] lsp::CompletionList get_sidbase_matches(const lsp::String& needle, const dconstruct::SIDBase& sidbase, const u64 max_sidbase_matches) {
        if (needle.empty()) {
            return {};
        }

        const std::vector<std::string>& choices = sidbase.get_acquired_strings();
        const std::vector<std::string>& normalized_choices = sidbase.get_normalized_acquired_strings();

        std::vector<qui::fuzzy_match> matches;
        lsp::Array<lsp::CompletionItem> results;

        const std::string normalized_query = qui::fuzzy_normalize(needle);
        for (u64 i = 0; i < normalized_choices.size() && matches.size() < max_sidbase_matches; ++i) {
            const u64 position = normalized_choices[i].find(normalized_query);
            if (position != std::string::npos) {
                matches.push_back({i, static_cast<double>(position)});
            }
        }

        std::stable_sort(matches.begin(), matches.end(), [](const qui::fuzzy_match &lhs, const qui::fuzzy_match &rhs) {
            return lhs.score < rhs.score;
        });

        results.reserve(max_sidbase_matches);
        for (u64 i = 0; i < matches.size(); ++i) {
            results.push_back(lsp::CompletionItem {
                .label = choices[matches[i].index],
                .kind = lsp::CompletionItemKind::Constant,
            });
        }

        return lsp::CompletionList { .isIncomplete = true, .items = std::move(results) };
    }
}
