#include <gtest/gtest.h>
#include "compilation/dc_parser.h"
#include "compilation/lexer.h"
#include "dcplsp/server.h"
#include "lsp/types.h"
#include "lsp/uri.h"
#include "sidbase.h"
#include <algorithm>
#include <cstring>
#include <memory>
#include <string>
#include <utility>
#include <variant>
#include <vector>

namespace dconstruct::dcplsp {
    struct server_test_access {
        static lsp::requests::TextDocument_Completion::Result complete(server& s, const lsp::Uri& uri, std::string text, lsp::uint line, lsp::uint character) {
            lsp::TextDocumentItem item;
            item.uri = uri;
            item.languageId = "dcpl";
            item.version = 1;
            item.text = std::move(text);
            s.m_documents[std::string(uri.data())] = std::move(item);

            lsp::CompletionParams params;
            params.textDocument.uri = uri;
            params.position = lsp::Position{ .line = line, .character = character };
            return s.finish_hash_completion(std::move(params));
        }

        static bool errored(const server& s) {
            return s.m_stopReason.has_value();
        }
    };
}

namespace dconstruct::testing {

    static u64 idx(const std::string& text, lsp::uint line, lsp::uint character) {
        return dcplsp::position_to_string_index(text, lsp::Position{ .line = line, .character = character });
    }

    struct semantic_token_entry {
        lsp::uint line;
        lsp::uint start;
        lsp::uint length;
        lsp::uint type;
        std::string lexeme;
    };

    static std::vector<std::string> lines_of(const std::string& text) {
        std::vector<std::string> lines{""};
        for (const char c : text) {
            if (c == '\n') {
                lines.emplace_back();
            } else if (c != '\r') {
                lines.back().push_back(c);
            }
        }
        return lines;
    }

    static std::vector<semantic_token_entry> decode_semantic_tokens(const std::string& text, const std::vector<u32>& data) {
        const std::vector<std::string> lines = lines_of(text);
        std::vector<semantic_token_entry> entries;
        lsp::uint line = 0;
        lsp::uint start = 0;

        for (std::size_t i = 0; i + 4 < data.size(); i += 5) {
            const lsp::uint delta_line = data[i];
            const lsp::uint delta_start = data[i + 1];
            line += delta_line;
            start = delta_line == 0 ? start + delta_start : delta_start;

            const lsp::uint length = data[i + 2];
            entries.push_back({
                .line = line,
                .start = start,
                .length = length,
                .type = data[i + 3],
                .lexeme = line < lines.size() ? lines[line].substr(start, length) : "",
            });
        }

        return entries;
    }

    static bool has_semantic_token(const std::vector<semantic_token_entry>& entries, const std::string& lexeme, compilation::Parser::SEMANTIC_TOKEN_TYPE type) {
        return std::ranges::any_of(entries, [&](const semantic_token_entry& entry) {
            return entry.lexeme == lexeme && entry.type == static_cast<u32>(type);
        });
    }

    static bool has_duplicate_semantic_token_position(const std::vector<semantic_token_entry>& entries) {
        for (std::size_t i = 0; i < entries.size(); ++i) {
            for (std::size_t j = i + 1; j < entries.size(); ++j) {
                if (entries[i].line == entries[j].line && entries[i].start == entries[j].start && entries[i].length == entries[j].length) {
                    return true;
                }
            }
        }
        return false;
    }

    TEST(DCPLSP, EmptyDocument) {
        EXPECT_EQ(idx("", 0, 0), 0u);
    }

    TEST(DCPLSP, StartOfDocument) {
        EXPECT_EQ(idx("hello world", 0, 0), 0u);
    }

    TEST(DCPLSP, MiddleOfFirstLine) {
        EXPECT_EQ(idx("hello world", 0, 6), 6u);
    }

    TEST(DCPLSP, EndOfFirstLine) {
        EXPECT_EQ(idx("hello world", 0, 11), 11u);
    }

    TEST(DCPLSP, SecondLineStartLF) {
        EXPECT_EQ(idx("abc\ndef\nghi", 1, 0), 4u);
    }

    TEST(DCPLSP, SecondLineMidLF) {
        EXPECT_EQ(idx("abc\ndef\nghi", 1, 3), 7u);
    }

    TEST(DCPLSP, ThirdLineStartLF) {
        EXPECT_EQ(idx("abc\ndef\nghi", 2, 0), 8u);
    }

    TEST(DCPLSP, ThirdLineMidLF) {
        EXPECT_EQ(idx("abc\ndef\nghi", 2, 3), 11u);
    }

    TEST(DCPLSP, MidColumnsSecondLine) {
        EXPECT_EQ(idx("hello\nworld\nfoo", 1, 2), 8u);
        EXPECT_EQ(idx("hello\nworld\nfoo", 1, 4), 10u);
    }

    TEST(DCPLSP, CRLFSecondLine) {
        EXPECT_EQ(idx("abc\r\ndef", 1, 0), 5u);
        EXPECT_EQ(idx("abc\r\ndef", 1, 3), 8u);
    }

    TEST(DCPLSP, CROnlySecondLine) {
        EXPECT_EQ(idx("abc\rdef", 1, 0), 4u);
        EXPECT_EQ(idx("abc\rdef", 1, 3), 7u);
    }

    TEST(DCPLSP, LastLineNoTrailingNewline) {
        EXPECT_EQ(idx("a\nbb", 1, 0), 2u);
        EXPECT_EQ(idx("a\nbb", 1, 2), 4u);
    }

    TEST(DCPLSP, LeadingEmptyLine) {
        EXPECT_EQ(idx("\nabc", 0, 0), 0u);
        EXPECT_EQ(idx("\nabc", 1, 3), 4u);
    }

    TEST(DCPLSP, EmptyLineInMiddleAddressable) {
        EXPECT_EQ(idx("a\n\nb", 1, 0), 2u);
    }

    TEST(DCPLSP, TrailingEmptyLineAddressable) {
        EXPECT_EQ(idx("abc\n", 1, 0), 4u);
    }

    TEST(DCPLSP, LineBeyondDocumentClampsToSize) {
        const std::string text = "abc\ndef";
        EXPECT_EQ(idx(text, 5, 0), text.size());
    }

    TEST(DCPLSP, RangeRoundTripWholeDocument) {
        const std::string text = "abc\ndef\nghi";
        const u64 start = idx(text, 0, 0);
        const u64 stop = idx(text, 2, 3);
        EXPECT_EQ(text.substr(start, stop - start), text);
    }

    TEST(DCPLSP, RangeRoundTripMidToMid) {
        const std::string text = "abc\ndef\nghi";
        const u64 start = idx(text, 0, 1);
        const u64 stop = idx(text, 2, 2);
        EXPECT_EQ(start, 1u);
        EXPECT_EQ(stop, 10u);
        EXPECT_EQ(text.substr(start, stop - start), "bc\ndef\ngh");
    }

    static SIDBase make_sidbase(const std::vector<std::pair<sid64, std::string>>& items) {
        const u64 count = items.size();
        const std::size_t table_size = count * sizeof(SIDBaseEntry);
        std::size_t string_size = 0;
        for (const auto& [hash, name] : items) {
            string_size += name.size() + 1;
        }
        const std::size_t total_size = sizeof(u64) + table_size + string_size;

        auto buffer = std::make_unique<std::byte[]>(total_size);
        std::byte* base = buffer.get();

        std::memcpy(base, &count, sizeof(u64));

        auto* entries = reinterpret_cast<SIDBaseEntry*>(base + sizeof(u64));
        std::size_t string_off = sizeof(u64) + table_size;
        for (u64 i = 0; i < count; ++i) {
            entries[i].hash = items[i].first;
            entries[i].offset = string_off;
            std::memcpy(base + string_off, items[i].second.c_str(), items[i].second.size() + 1);
            string_off += items[i].second.size() + 1;
        }

        const sid64 lowest = count > 0 ? items.front().first : 0;
        const sid64 highest = count > 0 ? items.back().first : 0;

        SIDBase sidbase{ count, std::move(buffer), entries, lowest, highest, total_size };
        sidbase.acquire_strings();
        return sidbase;
    }

    static bool has_label(lsp::requests::TextDocument_Completion::Result& result, const std::string& label) {
        if (result.isNull() || !result.holdsAlternative<lsp::CompletionList>()) {
            return false;
        }
        for (const auto& item : result.get<lsp::CompletionList>().items) {
            if (item.label == label) {
                return true;
            }
        }
        return false;
    }

    static std::size_t item_count(lsp::requests::TextDocument_Completion::Result& result) {
        if (result.isNull() || !result.holdsAlternative<lsp::CompletionList>()) {
            return 0;
        }
        return result.get<lsp::CompletionList>().items.size();
    }

    static const lsp::CompletionItem* find_item(lsp::requests::TextDocument_Completion::Result& result, const std::string& label) {
        if (result.isNull() || !result.holdsAlternative<lsp::CompletionList>()) {
            return nullptr;
        }
        for (const auto& item : result.get<lsp::CompletionList>().items) {
            if (item.label == label) {
                return &item;
            }
        }
        return nullptr;
    }

    TEST(DCPLSP, ExtractsNeedleAtDocumentEnd) {
        dcplsp::server server{ make_sidbase({ { 1, "player-health" } }) };
        const lsp::Uri uri = lsp::Uri::parse("file:///a.dcpl");
        auto result = dcplsp::server_test_access::complete(server, uri, " #player-health", 0, 15);

        EXPECT_FALSE(dcplsp::server_test_access::errored(server));
        EXPECT_TRUE(has_label(result, "player-health"));
    }

    TEST(DCPLSP, NeedleExcludesTextBeforeHash) {
        dcplsp::server server{ make_sidbase({ { 1, "abc" } }) };
        const lsp::Uri uri = lsp::Uri::parse("file:///a.dcpl");
        auto result = dcplsp::server_test_access::complete(server, uri, " xyz#abc", 0, 8);

        EXPECT_FALSE(dcplsp::server_test_access::errored(server));
        EXPECT_TRUE(has_label(result, "abc"));
    }

    TEST(DCPLSP, UsesNearestHashWhenSeveralPresent) {
        dcplsp::server server{ make_sidbase({ { 1, "bar" } }) };
        const lsp::Uri uri = lsp::Uri::parse("file:///a.dcpl");
        auto result = dcplsp::server_test_access::complete(server, uri, " #foo #bar", 0, 10);

        EXPECT_FALSE(dcplsp::server_test_access::errored(server));
        EXPECT_TRUE(has_label(result, "bar"));
    }

    TEST(DCPLSP, NeedleExtractedOnSecondLine) {
        dcplsp::server server{ make_sidbase({ { 1, "jump" } }) };
        const lsp::Uri uri = lsp::Uri::parse("file:///a.dcpl");
        auto result = dcplsp::server_test_access::complete(server, uri, "line one\n #jump", 1, 6);

        EXPECT_FALSE(dcplsp::server_test_access::errored(server));
        EXPECT_TRUE(has_label(result, "jump"));
    }

    TEST(DCPLSP, CursorInMiddleDoesNotConsumeTrailingChar) {
        dcplsp::server server{ make_sidbase({ { 1, "foo" } }) };
        const lsp::Uri uri = lsp::Uri::parse("file:///a.dcpl");
        auto result = dcplsp::server_test_access::complete(server, uri, " #foo bar", 0, 5);

        EXPECT_FALSE(dcplsp::server_test_access::errored(server));
        EXPECT_TRUE(has_label(result, "foo"));
    }

    TEST(DCPLSP, NeedleAtEndOfNonFinalLineExcludesNewline) {
        dcplsp::server server{ make_sidbase({ { 1, "foo" } }) };
        const lsp::Uri uri = lsp::Uri::parse("file:///a.dcpl");
        auto result = dcplsp::server_test_access::complete(server, uri, "#foo\nbar", 0, 4);

        EXPECT_FALSE(dcplsp::server_test_access::errored(server));
        EXPECT_TRUE(has_label(result, "foo"));
    }

    TEST(DCPLSP, CompletionReplacesTypedNeedleRange) {
        dcplsp::server server{ make_sidbase({ { 1, "gas-mask-mat" } }) };
        const lsp::Uri uri = lsp::Uri::parse("file:///a.dcpl");
        auto result = dcplsp::server_test_access::complete(server, uri, "#gas-mask-", 0, 10);

        EXPECT_FALSE(dcplsp::server_test_access::errored(server));
        const lsp::CompletionItem* item = find_item(result, "gas-mask-mat");
        ASSERT_NE(item, nullptr);

        ASSERT_TRUE(item->textEdit.has_value());
        ASSERT_TRUE(std::holds_alternative<lsp::TextEdit>(*item->textEdit));
        const lsp::TextEdit& edit = std::get<lsp::TextEdit>(*item->textEdit);
        EXPECT_EQ(edit.range.start.line, 0u);
        EXPECT_EQ(edit.range.start.character, 1u);
        EXPECT_EQ(edit.range.end.line, 0u);
        EXPECT_EQ(edit.range.end.character, 10u);
        EXPECT_EQ(edit.newText, "gas-mask-mat");

        ASSERT_TRUE(item->filterText.has_value());
        EXPECT_EQ(*item->filterText, "gas-mask-mat");
    }

    TEST(DCPLSP, HashAtDocumentStart) {
        dcplsp::server server{ make_sidbase({ { 1, "foo" } }) };
        const lsp::Uri uri = lsp::Uri::parse("file:///a.dcpl");
        auto result = dcplsp::server_test_access::complete(server, uri, "#foo", 0, 4);

        EXPECT_FALSE(dcplsp::server_test_access::errored(server));
        EXPECT_TRUE(has_label(result, "foo"));
    }

    TEST(DCPLSP, EmptyNeedleYieldsNoItems) {
        dcplsp::server server{ make_sidbase({ { 1, "foo" } }) };
        const lsp::Uri uri = lsp::Uri::parse("file:///a.dcpl");
        auto result = dcplsp::server_test_access::complete(server, uri, " #", 0, 2);

        EXPECT_FALSE(dcplsp::server_test_access::errored(server));
        EXPECT_EQ(item_count(result), 0u);
    }

    TEST(DCPLSP, NoHashBeforeCursorErrors) {
        dcplsp::server server{ make_sidbase({ { 1, "foo" } }) };
        const lsp::Uri uri = lsp::Uri::parse("file:///a.dcpl");
        dcplsp::server_test_access::complete(server, uri, "no hash here", 0, 12);

        EXPECT_TRUE(dcplsp::server_test_access::errored(server));
    }

    TEST(DCPLSP, SemanticTokensCoverParserRoles) {
        const std::string text = R"(enum Mood {
    Happy = 1,
    Sad = 2
}
struct Actor {
    u32 health;
}
u32 heal(u32 amount) {
    Actor actor;
    actor->health = amount + 1;
    return #game:heal(actor->health);
})";

        const std::vector<u32> data = dcplsp::make_semantic_tokens(text);
        ASSERT_FALSE(data.empty());
        ASSERT_EQ(data.size() % 5, 0u);

        const std::vector<semantic_token_entry> entries = decode_semantic_tokens(text, data);
        EXPECT_FALSE(has_duplicate_semantic_token_position(entries));
        EXPECT_TRUE(has_semantic_token(entries, "enum", compilation::Parser::SEMANTIC_TOKEN_TYPE::KEYWORD));
        EXPECT_TRUE(has_semantic_token(entries, "Mood", compilation::Parser::SEMANTIC_TOKEN_TYPE::TYPE));
        EXPECT_TRUE(has_semantic_token(entries, "Happy", compilation::Parser::SEMANTIC_TOKEN_TYPE::CONSTANT));
        EXPECT_TRUE(has_semantic_token(entries, "1", compilation::Parser::SEMANTIC_TOKEN_TYPE::NUMBER));
        EXPECT_TRUE(has_semantic_token(entries, "struct", compilation::Parser::SEMANTIC_TOKEN_TYPE::KEYWORD));
        EXPECT_TRUE(has_semantic_token(entries, "Actor", compilation::Parser::SEMANTIC_TOKEN_TYPE::TYPE));
        EXPECT_TRUE(has_semantic_token(entries, "health", compilation::Parser::SEMANTIC_TOKEN_TYPE::VARIABLE));
        EXPECT_TRUE(has_semantic_token(entries, "heal", compilation::Parser::SEMANTIC_TOKEN_TYPE::FUNCTION));
        EXPECT_TRUE(has_semantic_token(entries, "amount", compilation::Parser::SEMANTIC_TOKEN_TYPE::VARIABLE));
        EXPECT_TRUE(has_semantic_token(entries, "actor", compilation::Parser::SEMANTIC_TOKEN_TYPE::VARIABLE));
        EXPECT_TRUE(has_semantic_token(entries, "#game:heal", compilation::Parser::SEMANTIC_TOKEN_TYPE::FUNCTION));
        EXPECT_TRUE(has_semantic_token(entries, "return", compilation::Parser::SEMANTIC_TOKEN_TYPE::KEYWORD));
        EXPECT_TRUE(has_semantic_token(entries, "=", compilation::Parser::SEMANTIC_TOKEN_TYPE::OPERATOR));
        EXPECT_TRUE(has_semantic_token(entries, "+", compilation::Parser::SEMANTIC_TOKEN_TYPE::OPERATOR));
    }

    TEST(DCPLSP, SemanticTokensCoverStatescriptAndTypemap) {
        const std::string statescript = R"(statescript #script {
    options {
        #opt
    }
    declarations {
        u32 #value = 1;
    }
    state #main {
        block event #run {
            track #track {
                lambda {
                    breakpoint;
                }
            }
        }
    }
})";

        const std::vector<semantic_token_entry> state_entries = decode_semantic_tokens(statescript, dcplsp::make_semantic_tokens(statescript));
        ASSERT_FALSE(state_entries.empty());
        EXPECT_FALSE(has_duplicate_semantic_token_position(state_entries));
        EXPECT_TRUE(has_semantic_token(state_entries, "statescript", compilation::Parser::SEMANTIC_TOKEN_TYPE::KEYWORD));
        EXPECT_TRUE(has_semantic_token(state_entries, "#script", compilation::Parser::SEMANTIC_TOKEN_TYPE::CONSTANT));
        EXPECT_TRUE(has_semantic_token(state_entries, "options", compilation::Parser::SEMANTIC_TOKEN_TYPE::KEYWORD));
        EXPECT_TRUE(has_semantic_token(state_entries, "#opt", compilation::Parser::SEMANTIC_TOKEN_TYPE::CONSTANT));
        EXPECT_TRUE(has_semantic_token(state_entries, "declarations", compilation::Parser::SEMANTIC_TOKEN_TYPE::KEYWORD));
        EXPECT_TRUE(has_semantic_token(state_entries, "#value", compilation::Parser::SEMANTIC_TOKEN_TYPE::CONSTANT));
        EXPECT_TRUE(has_semantic_token(state_entries, "lambda", compilation::Parser::SEMANTIC_TOKEN_TYPE::KEYWORD));
        EXPECT_TRUE(has_semantic_token(state_entries, "breakpoint", compilation::Parser::SEMANTIC_TOKEN_TYPE::KEYWORD));

        const std::string typemap = R"(typemap {
    #func {
        0 u32 -> alias;
    }
})";

        compilation::Lexer lexer{typemap};
        const auto& [tokens, lex_errors] = lexer.get_results();
        ASSERT_TRUE(lex_errors.empty());

        compilation::Parser parser{tokens};
        ASSERT_TRUE(parser.make_typemap().has_value());
        ASSERT_TRUE(parser.get_errors().empty());

        const std::vector<semantic_token_entry> typemap_entries = decode_semantic_tokens(typemap, parser.get_semantic_tokens());
        ASSERT_FALSE(typemap_entries.empty());
        EXPECT_FALSE(has_duplicate_semantic_token_position(typemap_entries));
        EXPECT_TRUE(has_semantic_token(typemap_entries, "typemap", compilation::Parser::SEMANTIC_TOKEN_TYPE::KEYWORD));
        EXPECT_TRUE(has_semantic_token(typemap_entries, "#func", compilation::Parser::SEMANTIC_TOKEN_TYPE::FUNCTION));
        EXPECT_TRUE(has_semantic_token(typemap_entries, "0", compilation::Parser::SEMANTIC_TOKEN_TYPE::NUMBER));
        EXPECT_TRUE(has_semantic_token(typemap_entries, "u32", compilation::Parser::SEMANTIC_TOKEN_TYPE::TYPE));
        EXPECT_TRUE(has_semantic_token(typemap_entries, "alias", compilation::Parser::SEMANTIC_TOKEN_TYPE::VARIABLE));
    }

}
