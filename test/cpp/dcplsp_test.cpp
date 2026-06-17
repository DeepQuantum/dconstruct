#include <gtest/gtest.h>
#include "dcplsp/server.h"
#include "lsp/types.h"
#include "lsp/uri.h"
#include "sidbase.h"
#include <cstring>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace dconstruct::dcplsp {
    struct server_test_access {
        static lsp::requests::TextDocument_Completion::Result complete(server& s, const lsp::Uri& uri, std::string text, lsp::uint line, lsp::uint character) {
            lsp::TextDocumentItem item;
            item.uri = uri;
            item.languageId = "dcpl";
            item.version = 1;
            item.text = std::move(text);
            s.m_documents[uri.data()] = std::move(item);

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
        EXPECT_TRUE(item_count(result) == 0);
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

}
