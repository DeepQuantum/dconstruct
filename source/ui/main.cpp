#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <filesystem>
#include <memory>
#include <span>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "base.h"
#include "binaryfile.h"
#include "sidbase.h"
#include "disassembly/disassembler.h"
#include "decompilation/decomp_function.h"

#include <qui.h>
#include <qui/code_window_ctre.hpp>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <commdlg.h>
#include <shellapi.h>
#ifndef GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#endif
#include <GLFW/glfw3native.h>

namespace {

using namespace dconstruct;

constexpr f32 BAR_VERTICAL_PADDING = 8.0F;
constexpr f32 WINDOW_BUTTON_WIDTH = 46.0F;
constexpr f32 SPLITTER_WIDTH = 6.0F;
constexpr f32 MIN_PANEL_WIDTH = 120.0F;
constexpr f32 ENTRY_CELL_LEFT_PADDING = 4.0F;
constexpr char DONATE_URL[] = "https://ko-fi.com/deepquantum";
constexpr wchar_t PREVIOUS_WNDPROC_PROP[] = L"dconstruct.previous_wndproc";

struct document {
    std::unique_ptr<BinaryFile> m_file;
    std::unique_ptr<Disassembler> m_disassembler;
    const std::vector<disassembled_entry>* m_entries = nullptr;
    std::string m_path;
    std::string m_name;
    i32 m_selectedEntry = -1;
    f32 m_listWidth = 0.0F;
    const void *m_expandRequest = nullptr;
    const void *m_pendingExpand = nullptr;
    i32 m_forceOpenDepth = 0;
    std::vector<char> m_forceStack;
    std::unordered_map<const function_disassembly *, std::string> m_decompiled;
    std::unordered_map<const function_disassembly *, bool> m_lambdaViewDcpl;
};

struct drag_state {
    bool m_active = false;
    i32 m_startWindowX = 0;
    i32 m_startWindowY = 0;
    f64 m_startCursorX = 0.0;
    f64 m_startCursorY = 0.0;
};

struct app_state {
    std::unique_ptr<SIDBase> m_sidbase;
    std::vector<document> m_documents;
    i32 m_activeDocument = -1;
    i32 m_pendingSelect = -1;
    std::string m_loadError;
    drag_state m_drag;
};

std::string lookup_sid(const SIDBase &sidbase, const BinaryFile &file, const sid64 hash) {
    const auto cached = file.m_sidCache.find(hash);
    if (cached != file.m_sidCache.end()) {
        return cached->second;
    }
    if (const char *resolved = sidbase.search(hash)) {
        return resolved;
    }
    return int_to_string_id<sid64>(hash);
}

void glfw_error_callback(int error, const char *description) {
    std::fprintf(stderr, "GLFW error %d: %s\n", error, description);
}

std::string filename_from_path(const std::string &path) {
    const std::size_t slash = path.find_last_of("/\\");
    return slash == std::string::npos ? path : path.substr(slash + 1);
}

void open_url(const char *url) {
    ShellExecuteA(nullptr, "open", url, nullptr, nullptr, SW_SHOWNORMAL);
}

LRESULT CALLBACK dconstruct_window_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
    if (message == WM_MOUSEACTIVATE) {
        return MA_ACTIVATE;
    }

    const auto previous = reinterpret_cast<WNDPROC>(GetPropW(hwnd, PREVIOUS_WNDPROC_PROP));
    if (previous != nullptr) {
        return CallWindowProcW(previous, hwnd, message, wparam, lparam);
    }
    return DefWindowProcW(hwnd, message, wparam, lparam);
}

void install_window_proc(GLFWwindow *window) {
    HWND hwnd = glfwGetWin32Window(window);
    if (hwnd == nullptr) {
        return;
    }

    const auto current = reinterpret_cast<WNDPROC>(GetWindowLongPtrW(hwnd, GWLP_WNDPROC));
    if (current == dconstruct_window_proc) {
        return;
    }

    SetPropW(hwnd, PREVIOUS_WNDPROC_PROP, reinterpret_cast<HANDLE>(current));
    SetWindowLongPtrW(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(dconstruct_window_proc));
}

void uninstall_window_proc(GLFWwindow *window) {
    HWND hwnd = glfwGetWin32Window(window);
    if (hwnd == nullptr) {
        return;
    }

    const auto previous = reinterpret_cast<WNDPROC>(GetPropW(hwnd, PREVIOUS_WNDPROC_PROP));
    if (previous != nullptr) {
        SetWindowLongPtrW(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(previous));
        RemovePropW(hwnd, PREVIOUS_WNDPROC_PROP);
    }
}

std::string prompt_open_file(GLFWwindow *window, const char *filter, const char *title) {
    char buffer[MAX_PATH] = {};

    OPENFILENAMEA ofn = {};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = glfwGetWin32Window(window);
    ofn.lpstrFilter = filter;
    ofn.lpstrFile = buffer;
    ofn.nMaxFile = sizeof(buffer);
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
    ofn.lpstrTitle = title;

    if (GetOpenFileNameA(&ofn) == TRUE) {
        return std::string(buffer);
    }
    return {};
}

f32 measure_text(ImFont *font, const char *text) {
    if (font != nullptr) {
        ImGui::PushFont(font);
    }
    const f32 width = ImGui::CalcTextSize(text).x;
    if (font != nullptr) {
        ImGui::PopFont();
    }
    return width;
}

f32 measure_entry_list_width(const app_state &state, const document &doc) {
    f32 max_width = 0.0F;
    if (doc.m_entries == nullptr || state.m_sidbase == nullptr || doc.m_file == nullptr) {
        return max_width;
    }
    const ImGuiStyle &style = ImGui::GetStyle();
    for (const disassembled_entry &entry : *doc.m_entries) {
        const f32 width =
            measure_text(qui::font_bold(), lookup_sid(*state.m_sidbase, *doc.m_file, entry.m_nameId).c_str()) +
            measure_text(qui::font_medium(), lookup_sid(*state.m_sidbase, *doc.m_file, entry.m_typeId).c_str()) +
            style.CellPadding.x * 4.0F +
            ENTRY_CELL_LEFT_PADDING * 2.0F;
        max_width = std::max(max_width, width);
    }
    return max_width;
}

void decompile_document(document &doc) {
    doc.m_decompiled.clear();
    doc.m_lambdaViewDcpl.clear();
    if (doc.m_disassembler == nullptr || doc.m_file == nullptr) {
        return;
    }

    for (const function_disassembly *func : doc.m_disassembler->get_all_functions()) {
        try {
            ast::function_definition def =
                dcompiler::decomp_function{*func, *doc.m_file, ControlFlowGraph::build(*func)}.decompile(true);
            std::ostringstream oss;
            oss << def;
            doc.m_decompiled.emplace(func, std::move(oss).str());
        } catch (const std::exception &) {
        } catch (...) {
        }
    }
}

void disassemble_document(app_state &state, document &doc) {
    doc.m_entries = nullptr;
    doc.m_selectedEntry = -1;
    doc.m_disassembler.reset();
    doc.m_decompiled.clear();
    doc.m_lambdaViewDcpl.clear();

    if (state.m_sidbase == nullptr || doc.m_file == nullptr) {
        return;
    }

    doc.m_disassembler = std::make_unique<Disassembler>(doc.m_file.get(), state.m_sidbase.get());
    doc.m_disassembler->disassemble();
    doc.m_entries = &doc.m_disassembler->get_disassembled_entries();
    decompile_document(doc);

    const ImGuiStyle &style = ImGui::GetStyle();
    doc.m_listWidth = measure_entry_list_width(state, doc) + style.WindowPadding.x * 2.0F + style.ScrollbarSize + 8.0F;
}

void close_document(app_state &state, const i32 index) {
    if (index < 0 || index >= static_cast<i32>(state.m_documents.size())) {
        return;
    }
    state.m_documents.erase(state.m_documents.begin() + index);
    if (state.m_documents.empty()) {
        state.m_activeDocument = -1;
    } else if (state.m_activeDocument >= static_cast<i32>(state.m_documents.size())) {
        state.m_activeDocument = static_cast<i32>(state.m_documents.size()) - 1;
    }
}

document *active_document(app_state &state) {
    if (state.m_activeDocument < 0 || state.m_activeDocument >= static_cast<i32>(state.m_documents.size())) {
        return nullptr;
    }
    return &state.m_documents[static_cast<u32>(state.m_activeDocument)];
}

void load_bin_file(app_state &state, const std::string &path) {
    state.m_loadError.clear();

    auto file_res = BinaryFile::from_path(path);
    if (!file_res) {
        state.m_loadError = file_res.error();
        return;
    }

    document doc;
    doc.m_file = std::make_unique<BinaryFile>(std::move(*file_res));
    doc.m_path = path;
    doc.m_name = filename_from_path(path);
    state.m_documents.push_back(std::move(doc));
    state.m_activeDocument = static_cast<i32>(state.m_documents.size()) - 1;
    state.m_pendingSelect = state.m_activeDocument;
    disassemble_document(state, state.m_documents.back());
}

void load_sidbase(app_state &state, const std::string &path) {
    auto sidbase_res = SIDBase::from_binary(path);
    if (!sidbase_res) {
        state.m_loadError = sidbase_res.error();
        return;
    }

    state.m_sidbase = std::make_unique<SIDBase>(std::move(*sidbase_res));
    for (document &doc : state.m_documents) {
        disassemble_document(state, doc);
    }
}

f32 draw_title_menu_bar(app_state &state, GLFWwindow *window) {
    const ImGuiViewport *viewport = ImGui::GetMainViewport();
    const ImVec2 pos = viewport->Pos;

    qui::WindowBarState &bar_state = qui::default_window_bar_state();

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(ImGui::GetStyle().FramePadding.x, BAR_VERTICAL_PADDING));
    const f32 height = ImGui::GetFrameHeight();

    ImGui::SetNextWindowPos(pos);
    ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, height));

    constexpr ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoScrollWithMouse |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_MenuBar;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0F, 0.0F));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0F);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0F);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, qui::color::retina_dark::MenuBarBackground);
    ImGui::PushStyleColor(ImGuiCol_MenuBarBg, qui::color::retina_dark::MenuBarBackground);
    ImGui::Begin("##dconstruct_title_menu_bar", nullptr, flags);

    ImDrawList *draw_list = ImGui::GetWindowDrawList();
    const ImVec2 bar_min = ImGui::GetWindowPos();
    const ImVec2 bar_max(bar_min.x + ImGui::GetWindowWidth(), bar_min.y + height);

    draw_list->AddLine(
        ImVec2(bar_min.x, bar_max.y - 1.0F),
        ImVec2(bar_max.x, bar_max.y - 1.0F),
        ImGui::ColorConvertFloat4ToU32(qui::color::retina_dark::Border)
    );

    constexpr f32 logo_size = 13.0F;
    const ImVec2 logo_min(bar_min.x + 10.0F, bar_min.y + (height - logo_size) * 0.5F);
    const ImVec2 logo_max(logo_min.x + logo_size, logo_min.y + logo_size);
    draw_list->AddRectFilled(logo_min, logo_max, ImGui::ColorConvertFloat4ToU32(qui::color::retina_dark::AccentBlue), 2.0F);
    draw_list->AddRectFilled(ImVec2(logo_min.x + 4.0F, logo_min.y + 4.0F), logo_max, ImGui::ColorConvertFloat4ToU32(qui::color::retina_dark::Highlight), 2.0F);

    const f32 buttons_total = WINDOW_BUTTON_WIDTH * 3.0F;
    const f32 minimize_x = bar_max.x - buttons_total;

    if (ImGui::BeginMenuBar()) {
        ImGui::SetCursorPosX(34.0F);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12.0F, 10.0F));
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.0F, 6.0F));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10.0F, 6.0F));
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Load File...", "Ctrl+O")) {
                const std::string path = prompt_open_file(window, "Binary files (*.bin)\0*.bin\0All files (*.*)\0*.*\0", "Open .bin file");
                if (!path.empty()) {
                    load_bin_file(state, path);
                }
            }
            if (ImGui::MenuItem("Load Sidbase...")) {
                const std::string path = prompt_open_file(window, "Binary files (*.bin)\0*.bin\0All files (*.*)\0*.*\0", "Open sidbase");
                if (!path.empty()) {
                    load_sidbase(state, path);
                }
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit")) {
                glfwSetWindowShouldClose(window, GLFW_TRUE);
            }
            ImGui::EndMenu();
        }
        ImGui::PopStyleVar(3);

        const char *donate_label = "Donate";
        const f32 donate_width = ImGui::CalcTextSize(donate_label).x + ImGui::GetStyle().ItemSpacing.x * 2.0F;
        ImGui::SetCursorPosX((minimize_x - bar_min.x) - donate_width);
        ImGui::PushStyleColor(ImGuiCol_Text, qui::color::retina_dark::Highlight);
        if (ImGui::MenuItem(donate_label)) {
            open_url(DONATE_URL);
        }
        ImGui::PopStyleColor();

        ImGui::EndMenuBar();
    }

    if (const document *doc = active_document(state); doc != nullptr && !doc->m_name.empty()) {
        ImFont *title_font = qui::font_medium();
        if (title_font != nullptr) {
            ImGui::PushFont(title_font);
        }
        const ImVec2 title_size = ImGui::CalcTextSize(doc->m_name.c_str());
        const f32 title_font_size = ImGui::GetFontSize();
        if (title_font != nullptr) {
            ImGui::PopFont();
        }
        const ImVec2 title_pos(
            bar_min.x + (bar_max.x - bar_min.x - title_size.x) * 0.5F,
            bar_min.y + (height - title_size.y) * 0.5F
        );
        draw_list->AddText(title_font, title_font_size, title_pos,
            ImGui::ColorConvertFloat4ToU32(qui::color::retina_dark::Text), doc->m_name.c_str());
    }

    const ImVec2 button_size(WINDOW_BUTTON_WIDTH, height);
    const f32 close_x = bar_max.x - WINDOW_BUTTON_WIDTH;
    const f32 maximize_x = close_x - WINDOW_BUTTON_WIDTH;

    if (qui::title_bar_button("minimize", "-", ImVec2(minimize_x, bar_min.y), button_size)) {
        glfwIconifyWindow(window);
    }

    const bool maximized = glfwGetWindowAttrib(window, GLFW_MAXIMIZED) == GLFW_TRUE;
    if (qui::title_bar_button("maximize", maximized ? "[]" : "[ ]", ImVec2(maximize_x, bar_min.y), button_size)) {
        if (maximized) {
            glfwRestoreWindow(window);
        } else {
            glfwMaximizeWindow(window);
        }
    }

    if (qui::title_bar_button("close", "X", ImVec2(close_x, bar_min.y), button_size, true)) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    qui::update_title_bar_drag(window, ImVec2(0.0F, 8.0F), ImVec2(minimize_x - bar_min.x, height), bar_state);

    ImGui::End();

    ImGui::PopStyleColor(2);
    ImGui::PopStyleVar(3);
    ImGui::PopStyleVar();

    return height;
}

void draw_status_text(const app_state &state) {
    if (state.m_sidbase == nullptr) {
        qui::status_label("No sidbase loaded \xE2\x80\x94 load one via File > Load Sidbase.", qui::color::retina_dark::AccentRed);
    } else if (state.m_documents.empty()) {
        qui::status_label("No file loaded \xE2\x80\x94 load a .bin via File > Load File.", qui::color::retina_dark::AccentRed);
    }

    if (!state.m_loadError.empty()) {
        qui::status_label(state.m_loadError.c_str(), qui::color::retina_dark::AccentRed);
    }
}

bool begin_labeled_table_frame(const char *label, ImVec2 &table_size) {
    const ImGuiStyle &style = ImGui::GetStyle();
    ImDrawList *draw_list = ImGui::GetWindowDrawList();
    const ImVec2 start_pos = ImGui::GetCursorScreenPos();
    const ImVec2 available_size = ImGui::GetContentRegionAvail();

    ImFont *font = qui::font_bold();
    if (font != nullptr) {
        ImGui::PushFont(font);
    }
    const ImVec2 text_size = ImGui::CalcTextSize(label);
    if (font != nullptr) {
        ImGui::PopFont();
    }

    const f32 top_clearance = text_size.y * 0.55F + 3.0F;
    const ImVec2 frame_min(start_pos.x, start_pos.y + top_clearance);
    const ImVec2 frame_size(available_size.x, available_size.y - top_clearance);
    if (frame_size.x <= 1.0F || frame_size.y <= 1.0F) {
        return false;
    }
    const ImVec2 frame_max(frame_min.x + frame_size.x, frame_min.y + frame_size.y);

    const ImU32 border = ImGui::ColorConvertFloat4ToU32(qui::color::retina_dark::TableBorderStrong);
    const ImU32 background = ImGui::ColorConvertFloat4ToU32(qui::color::retina_dark::WindowBackground);
    draw_list->AddRect(frame_min, frame_max, border, style.ChildRounding, 0, 1.0F);

    const f32 label_x = frame_min.x + style.WindowPadding.x + 8.0F;
    const f32 label_y = frame_min.y - text_size.y * 0.45F;
    const ImVec2 bg_min(label_x - 5.0F, label_y - 2.0F);
    const ImVec2 bg_max(label_x + text_size.x + 7.0F, label_y + text_size.y + 2.0F);
    draw_list->AddRectFilled(bg_min, bg_max, background);

    if (font != nullptr) {
        ImGui::PushFont(font);
    }
    draw_list->AddText(ImVec2(label_x, label_y), ImGui::ColorConvertFloat4ToU32(qui::color::retina_dark::Highlight), label);
    if (font != nullptr) {
        ImGui::PopFont();
    }

    const f32 inner_pad = 8.0F;
    const f32 table_y = frame_min.y + text_size.y + inner_pad;
    ImGui::SetCursorScreenPos(ImVec2(frame_min.x + inner_pad, table_y));
    table_size = ImVec2(
        std::max(1.0F, frame_size.x - inner_pad * 2.0F),
        std::max(1.0F, frame_max.y - table_y - inner_pad)
    );
    return true;
}

void draw_entry_list(app_state &state, document &doc) {
    ImVec2 table_size;
    if (!begin_labeled_table_frame("Entries", table_size)) {
        return;
    }

    if (doc.m_entries == nullptr || state.m_sidbase == nullptr || doc.m_file == nullptr) {
        qui::text_label("Not disassembled.");
        return;
    }

    const std::vector<disassembled_entry> &entries = *doc.m_entries;

    constexpr ImGuiTableFlags table_flags =
        ImGuiTableFlags_BordersInnerV |
        ImGuiTableFlags_RowBg |
        ImGuiTableFlags_Resizable |
        ImGuiTableFlags_ScrollY |
        ImGuiTableFlags_SizingStretchProp;

    if (ImGui::BeginTable("##dconstruct_entries", 2, table_flags, table_size)) {
        ImGui::TableSetupColumn("Entry Name");
        ImGui::TableSetupColumn("Type Name");
        ImGui::TableHeadersRow();

        ImGuiListClipper clipper;
        clipper.Begin(static_cast<int>(entries.size()));
        while (clipper.Step()) {
            for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i) {
                const disassembled_entry &entry = entries[static_cast<u32>(i)];
                const std::string name = lookup_sid(*state.m_sidbase, *doc.m_file, entry.m_nameId);
                const std::string type = lookup_sid(*state.m_sidbase, *doc.m_file, entry.m_typeId);

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ENTRY_CELL_LEFT_PADDING);

                ImGui::PushID(i);
                if (ImFont *font = qui::font_bold()) {
                    ImGui::PushFont(font);
                }
                if (ImGui::Selectable(name.c_str(), doc.m_selectedEntry == i, ImGuiSelectableFlags_SpanAllColumns)) {
                    doc.m_selectedEntry = i;
                }
                if (qui::font_bold() != nullptr) {
                    ImGui::PopFont();
                }
                ImGui::PopID();

                ImGui::TableSetColumnIndex(1);
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ENTRY_CELL_LEFT_PADDING);
                if (ImFont *font = qui::font_medium()) {
                    ImGui::PushFont(font);
                }
                ImGui::PushStyleColor(ImGuiCol_Text, qui::color::retina_dark::TextDisabled);
                ImGui::TextUnformatted(type.c_str());
                ImGui::PopStyleColor();
                if (qui::font_medium() != nullptr) {
                    ImGui::PopFont();
                }
            }
        }
        ImGui::EndTable();
    }
}

namespace val_color {
inline const ImVec4 IntZero = qui::color::retina_dark::TextDisabled;
inline const ImVec4 Int = qui::color::rgba(0x6F, 0xB8, 0xE8);
inline const ImVec4 Float = qui::color::rgba(0xE5, 0xC0, 0x7B);
inline const ImVec4 Sid = qui::color::rgba(0xC2, 0x9E, 0xF0);
inline const ImVec4 String = qui::color::rgba(0xE2, 0x8C, 0x6E);
inline const ImVec4 Array = qui::color::rgba(0x9C, 0xD6, 0x8E);
inline const ImVec4 Map = qui::color::rgba(0x5C, 0xB8, 0xD6);
inline const ImVec4 Struct = qui::color::rgba(0xCF, 0xCF, 0xD4);
inline const ImVec4 Function = qui::color::retina_dark::Highlight;
inline const ImVec4 StateScript = qui::color::retina_dark::AccentYellow;
inline const ImVec4 EntryName = qui::color::retina_dark::AccentYellow;
inline const ImVec4 Group = qui::color::rgba(0xB0, 0xB0, 0xB8);
} // namespace val_color

// dcpl syntax-highlighting rules for the code window. Pattern groups and their
// order are copied from dcpl-lint/syntaxes/dcpl.tmLanguage.json; the widget
// itself is language-agnostic and only consumes this collection.
std::span<const qui::code::rule> dcpl_rules() {
    using qui::code::ctre_rule;
    using qui::code::to_u32;
    using qui::color::rgba;
    static const std::vector<qui::code::rule> rules = {
        ctre_rule<R"(//.*)">(to_u32(rgba(0x6A, 0x73, 0x80))),
        // ctre_rule<R"RE(\b(if|else|while|for|return|foreach|match|state|block|event|track|statescript|options|declarations|using|as|far|near|not|and|or|in|lambda|start|end|update|null|struct|enum)\b)RE">(to_u32(rgba(0xC6, 0x78, 0xDD))),
        // ctre_rule<R"(\b((var|arg)_\d+)\b)">(to_u32(rgba(0xE0, 0x6C, 0x75))),
        // ctre_rule<R"(\b(i|j|k|l)\b)">(to_u32(rgba(0xE0, 0x6C, 0x75))),
        // ctre_rule<R"RE("(\\.|[^"\\])*")RE">(to_u32(rgba(0x98, 0xC3, 0x79))),
        // ctre_rule<R"RE((?:--|\b)[A-Za-z0-9_/@>#\-]+\??(?=\s*\())RE">(to_u32(rgba(0x61, 0xAF, 0xEF))),
        // ctre_rule<R"RE(#[A-Z0-9]{16}(?=\())RE">(to_u32(rgba(0x61, 0xAF, 0xEF))),
        // ctre_rule<R"(>>|=>|\$)">(to_u32(rgba(0x61, 0xAF, 0xEF))),
        // ctre_rule<R"(\b(\d+)\b)">(to_u32(rgba(0xD1, 0x9A, 0x66))),
        // ctre_rule<R"RE(\b(u0|u8|i8|u16|i16|u32|i32|u64|i64|f32|f64|bool|string|timer|quaternion|bound-frame|vector|actor|cache|ir-pack|level-id|level-name|lut-table|package|particle-module|render-settings|sound-bank|symbol|vox-character)\b\??)RE">(to_u32(rgba(0xE5, 0xC0, 0x7B))),
        // ctre_rule<R"RE((\*(?!var)|\b)([\w/*\-]{3,}\??(?![\w\-])))RE">(to_u32(rgba(0x56, 0xB6, 0xC2))),
        // ctre_rule<R"(#[A-Z0-9]{16})">(to_u32(rgba(0x56, 0xB6, 0xC2))),
    };
    return rules;
}

struct value_view {
    app_state *state;
    document *doc;
};

u32 file_offset(const document &doc, const void *ptr) {
    return static_cast<u32>(reinterpret_cast<p64>(ptr) - reinterpret_cast<p64>(doc.m_file->m_dcheader));
}

bool dv_node(value_view v, const void *id, const ImVec4 &color, const char *label, const char *suffix, bool leaf) {
    document *doc = v.doc;
    const bool force = !leaf && (doc->m_forceOpenDepth > 0 || id == doc->m_expandRequest);
    if (force) {
        ImGui::SetNextItemOpen(true, ImGuiCond_Always);
    }

    ImGuiTreeNodeFlags flags = 0;
    if (leaf) {
        flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet;
    }
    ImGui::PushStyleColor(ImGuiCol_Text, color);
    const bool open = ImGui::TreeNodeEx(id, flags, "%s", label);
    ImGui::PopStyleColor();
    if (!leaf && ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
        doc->m_pendingExpand = id;
    }
    if (suffix != nullptr && suffix[0] != '\0') {
        ImGui::SameLine(0.0F, 0.0F);
        ImGui::TextColored(qui::color::retina_dark::TextDisabled, "  %s", suffix);
    }
    if (!leaf && open) {
        doc->m_forceStack.push_back(force ? 1 : 0);
        if (force) {
            doc->m_forceOpenDepth++;
        }
    }
    return open;
}

void dv_tree_pop(value_view v) {
    document *doc = v.doc;
    if (!doc->m_forceStack.empty()) {
        if (doc->m_forceStack.back() != 0) {
            doc->m_forceOpenDepth--;
        }
        doc->m_forceStack.pop_back();
    }
    ImGui::TreePop();
}

void dv_draw_values(value_view v, const disassembled_values_t &values);
void dv_draw_value(value_view v, const disassembled_values_t::value_type &value, i32 index);
void dv_draw_function(value_view v, const function_disassembly &func, i32 index);
void dv_draw_function_body(value_view v, const function_disassembly &func);
void dv_function_switch_and_body(value_view v, const function_disassembly &func, const void *id, bool open);
void dv_draw_state_script(value_view v, const ast::state_script &script, i32 index);
void dv_draw_map(value_view v, const disassembled_value &entry, const void *id, i32 index);
void dv_draw_script_lambda(value_view v, const disassembled_value &entry, const void *id, i32 index);

void dv_index_prefix(char *buffer, std::size_t size, i32 index) {
    if (index >= 0) {
        std::snprintf(buffer, size, "[%d] ", index);
    } else if (size > 0) {
        buffer[0] = '\0';
    }
}

void dv_draw_struct_like(value_view v, const disassembled_value &entry, const void *id, i32 index) {
    char label[256];
    char suffix[128];
    char prefix[24];
    dv_index_prefix(prefix, sizeof(prefix), index);

    if (entry.m_typeId == SID("map") || entry.m_typeId == SID("map-32") || entry.m_typeId == SID("render-settings-map")) {
        dv_draw_map(v, entry, id, index);
        return;
    }

    if (entry.m_typeId == SID("script-lambda")) {
        dv_draw_script_lambda(v, entry, id, index);
        return;
    }

    if (entry.m_typeId == SID("array")) {
        std::snprintf(label, sizeof(label), "%sarray", prefix);
        std::snprintf(suffix, sizeof(suffix), "[0x%05X] {size: %zu}", static_cast<u32>(entry.m_offset), entry.m_values.size());
        const bool open = dv_node(v, id, val_color::Array, label, suffix, entry.m_values.empty());
        if (open && !entry.m_values.empty()) {
            dv_draw_values(v, entry.m_values);
            dv_tree_pop(v);
        }
        return;
    }

    ImVec4 color = val_color::Struct;
    std::string type_storage;
    const char *type_name = "anonymous struct";
    if (entry.m_typeId != 0) {
        type_storage = lookup_sid(*v.state->m_sidbase, *v.doc->m_file, entry.m_typeId);
        type_name = type_storage.c_str();
        if (entry.m_typeId == SID("state-script")) {
            color = val_color::StateScript;
        } else if (entry.m_typeId == SID("script-lambda")) {
            color = val_color::Function;
        }
    }

    std::snprintf(label, sizeof(label), "%s%s", prefix, type_name);
    std::snprintf(suffix, sizeof(suffix), "[0x%05X]", static_cast<u32>(entry.m_offset));
    const bool open = dv_node(v, id, color, label, suffix, entry.m_values.empty());
    if (open && !entry.m_values.empty()) {
        dv_draw_values(v, entry.m_values);
        dv_tree_pop(v);
    }
}

void dv_draw_value(value_view v, const disassembled_values_t::value_type &value, i32 index) {
    char label[512];
    char suffix[128];
    char prefix[24];
    dv_index_prefix(prefix, sizeof(prefix), index);
    std::visit([&](auto &&entry) {
        using T = std::decay_t<decltype(entry)>;
        if constexpr (std::is_same_v<T, disassembled_value>) {
            dv_draw_struct_like(v, entry, &value, index);
        } else if constexpr (std::is_same_v<T, std::shared_ptr<function_disassembly>>) {
            dv_draw_function(v, *entry, index);
        } else if constexpr (std::is_same_v<T, const ast::state_script *>) {
            dv_draw_state_script(v, *entry, index);
        } else if constexpr (std::is_same_v<T, const i32 *>) {
            std::snprintf(label, sizeof(label), "%s%d", prefix, *entry);
            dv_node(v, &value, *entry == 0 ? val_color::IntZero : val_color::Int, label, ": int", true);
        } else if constexpr (std::is_same_v<T, const u64 *>) {
            const std::string resolved = lookup_sid(*v.state->m_sidbase, *v.doc->m_file, *entry);
            std::snprintf(label, sizeof(label), "%s%s", prefix, resolved.c_str());
            dv_node(v, &value, val_color::Sid, label, ": sid", true);
        } else if constexpr (std::is_same_v<T, const f32 *>) {
            std::snprintf(label, sizeof(label), "%s%.2f", prefix, *entry);
            dv_node(v, &value, val_color::Float, label, ": float", true);
        } else if constexpr (std::is_same_v<T, const char *>) {
            std::snprintf(label, sizeof(label), "%s\"%s\"", prefix, entry != nullptr ? entry : "");
            dv_node(v, &value, val_color::String, label, ": string", true);
        } else if constexpr (std::is_same_v<T, const structs::map *>) {
            std::snprintf(label, sizeof(label), "%smap", prefix);
            std::snprintf(suffix, sizeof(suffix), "keys: [0x%05X], values: [0x%05X]",
                          file_offset(*v.doc, entry->keys.data), file_offset(*v.doc, entry->values.data));
            dv_node(v, &value, val_color::Map, label, suffix, true);
        }
    }, value);
}

void dv_draw_values(value_view v, const disassembled_values_t &values) {
    for (i32 i = 0; i < static_cast<i32>(values.size()); ++i) {
        dv_draw_value(v, values[static_cast<u32>(i)], i);
    }
}

void dv_map_extract(const disassembled_values_t &values, const structs::map *&header,
                    const disassembled_value *&keys, const disassembled_value *&vals) {
    header = nullptr;
    keys = nullptr;
    vals = nullptr;
    for (const auto &child : values) {
        if (const auto *m = std::get_if<const structs::map *>(&child)) {
            header = *m;
        } else if (const auto *dv = std::get_if<disassembled_value>(&child)) {
            if (keys == nullptr) {
                keys = dv;
            } else {
                vals = dv;
            }
        }
    }
}

void dv_draw_map_table(value_view v, const disassembled_value &keys, const disassembled_value &vals, const void *id) {
    ImGui::PushID(id);
    constexpr ImGuiTableFlags table_flags =
        ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable;
    if (ImGui::BeginTable("##map", 2, table_flags)) {
        ImGui::TableSetupColumn("Key", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableHeadersRow();

        const std::size_t count = std::min(keys.m_values.size(), vals.m_values.size());
        for (std::size_t i = 0; i < count; ++i) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            dv_draw_value(v, keys.m_values[i], -1);
            ImGui::TableSetColumnIndex(1);
            dv_draw_value(v, vals.m_values[i], -1);
        }
        ImGui::EndTable();
    }
    ImGui::PopID();
}

void dv_draw_map(value_view v, const disassembled_value &entry, const void *id, i32 index) {
    char label[64];
    char suffix[160];
    char prefix[24];
    dv_index_prefix(prefix, sizeof(prefix), index);

    const structs::map *header = nullptr;
    const disassembled_value *keys = nullptr;
    const disassembled_value *vals = nullptr;
    dv_map_extract(entry.m_values, header, keys, vals);

    std::snprintf(label, sizeof(label), "%smap", prefix);
    if (header != nullptr) {
        std::snprintf(suffix, sizeof(suffix), "[0x%05X] keys: [0x%05X], values: [0x%05X]",
                      static_cast<u32>(entry.m_offset),
                      file_offset(*v.doc, header->keys.data), file_offset(*v.doc, header->values.data));
    } else {
        std::snprintf(suffix, sizeof(suffix), "[0x%05X]", static_cast<u32>(entry.m_offset));
    }

    const bool empty = keys == nullptr || vals == nullptr || keys->m_values.empty();
    const bool open = dv_node(v, id, val_color::Map, label, suffix, empty);
    if (!open || empty) {
        return;
    }

    dv_draw_map_table(v, *keys, *vals, id);
    dv_tree_pop(v);
}

void dv_draw_function_body(value_view v, const function_disassembly &func) {
    const std::vector<function_disassembly_line> &lines = func.m_lines;

    constexpr ImGuiTableFlags table_flags =
        ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingStretchProp;

    ImFont *mono = qui::font_medium();
    if (mono != nullptr) {
        ImGui::PushFont(mono);
    }

    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if (dv_node(v, &func.m_lines, val_color::Group, "Instructions", nullptr, false)) {
        if (ImGui::BeginTable("##instructions", 2, table_flags)) {
            ImGui::TableSetupColumn("Instruction");
            ImGui::TableSetupColumn("Comment");
            ImGui::TableHeadersRow();

            for (const function_disassembly_line &line : lines) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted(line.m_text.c_str());
                ImGui::TableSetColumnIndex(1);
                ImGui::TextColored(qui::color::retina_dark::TextDisabled, "%s", line.m_comment.c_str());
            }
            ImGui::EndTable();
        }
        dv_tree_pop(v);
    }

    const SymbolTable &symbols = func.m_stackFrame.m_symbolTable;
    if (symbols.m_location.m_ptr != nullptr && !symbols.m_types.empty()) {
        if (dv_node(v, &func.m_stackFrame, val_color::Group, "Symbol Table", nullptr, false)) {
            if (ImGui::BeginTable("##symbols", 3, table_flags)) {
                ImGui::TableSetupColumn("#", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Offset", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Value");
                ImGui::TableHeadersRow();

                for (u32 i = 0; i < symbols.m_types.size(); ++i) {
                    const location value_location = symbols.m_location + static_cast<u64>(i) * 8;
                    char value_buffer[256];
                    value_buffer[0] = '\0';
                    std::visit([&](auto &&type) {
                        using T = std::decay_t<decltype(type)>;
                        if constexpr (std::is_same_v<T, ast::primitive_type>) {
                            switch (type.m_type) {
                                case ast::primitive_kind::I32: std::snprintf(value_buffer, sizeof(value_buffer), "int: %d", value_location.get<i32>()); break;
                                case ast::primitive_kind::I64: std::snprintf(value_buffer, sizeof(value_buffer), "int64: %lld", static_cast<long long>(value_location.get<i64>())); break;
                                case ast::primitive_kind::U64: std::snprintf(value_buffer, sizeof(value_buffer), "uint64: %llu", static_cast<unsigned long long>(value_location.get<u64>())); break;
                                case ast::primitive_kind::F32: std::snprintf(value_buffer, sizeof(value_buffer), "float: %f", static_cast<double>(value_location.get<f32>())); break;
                                case ast::primitive_kind::STRING: std::snprintf(value_buffer, sizeof(value_buffer), "string: \"%s\"", value_location.get<const char *>()); break;
                                case ast::primitive_kind::SID: std::snprintf(value_buffer, sizeof(value_buffer), "sid: %s", lookup_sid(*v.state->m_sidbase, *v.doc->m_file, value_location.get<sid64>()).c_str()); break;
                                default: std::snprintf(value_buffer, sizeof(value_buffer), "unknown: %llu", static_cast<unsigned long long>(value_location.get<u64>())); break;
                            }
                        } else if constexpr (std::is_same_v<T, ast::function_type>) {
                            std::snprintf(value_buffer, sizeof(value_buffer), "function: %s", lookup_sid(*v.state->m_sidbase, *v.doc->m_file, value_location.get<sid64>()).c_str());
                        } else if constexpr (std::is_same_v<T, ast::ptr_type>) {
                            std::snprintf(value_buffer, sizeof(value_buffer), "pointer: %s", lookup_sid(*v.state->m_sidbase, *v.doc->m_file, value_location.get<sid64>()).c_str());
                        } else {
                            std::snprintf(value_buffer, sizeof(value_buffer), "unknown: %llu", static_cast<unsigned long long>(value_location.get<u64>()));
                        }
                    }, static_cast<const ast::full_type::variant_type &>(symbols.m_types[i]));

                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("%04X", i);
                    ImGui::TableSetColumnIndex(1);
                    ImGui::TextColored(qui::color::retina_dark::TextDisabled, "0x%06X", file_offset(*v.doc, value_location.m_ptr));
                    ImGui::TableSetColumnIndex(2);
                    ImGui::TextUnformatted(value_buffer);
                }
                ImGui::EndTable();
            }
            dv_tree_pop(v);
        }
    }

    if (mono != nullptr) {
        ImGui::PopFont();
    }
}

void dv_draw_function(value_view v, const function_disassembly &func, i32 index) {
    char label[256];
    std::snprintf(label, sizeof(label), "[%d] function %s", index, func.get_id().c_str());
    const bool open = dv_node(v, &func, val_color::Function, label, nullptr, false);
    dv_function_switch_and_body(v, func, &func, open);
}

ImVec2 view_switch_size() {
    ImFont *font = qui::font_semi_bold();
    if (font != nullptr) {
        ImGui::PushFont(font);
    }
    const f32 wa = ImGui::CalcTextSize("ASM").x;
    const f32 wb = ImGui::CalcTextSize("DCPL").x;
    if (font != nullptr) {
        ImGui::PopFont();
    }
    const f32 seg = std::max(wa, wb) + 16.0F;
    return ImVec2(seg * 2.0F, ImGui::GetFrameHeight() * 0.82F);
}

bool draw_view_switch(const char *str_id, bool *dcpl, const ImVec2 &size) {
    ImGui::PushID(str_id);
    const ImVec2 pos = ImGui::GetCursorScreenPos();
    ImGui::InvisibleButton("##switch", size);
    bool changed = false;
    if (ImGui::IsItemClicked()) {
        *dcpl = !*dcpl;
        changed = true;
    }
    const bool hovered = ImGui::IsItemHovered();
    if (hovered) {
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
    }

    ImGuiStorage *storage = ImGui::GetStateStorage();
    const ImGuiID key = ImGui::GetID("##t");
    const f32 target = *dcpl ? 1.0F : 0.0F;
    f32 t = storage->GetFloat(key, target);
    t += (target - t) * std::clamp(ImGui::GetIO().DeltaTime * 14.0F, 0.0F, 1.0F);
    if (std::fabs(target - t) < 0.001F) {
        t = target;
    }
    storage->SetFloat(key, t);

    ImDrawList *draw_list = ImGui::GetWindowDrawList();
    const f32 seg = size.x * 0.5F;
    const f32 rounding = size.y * 0.5F;
    const ImVec2 max(pos.x + size.x, pos.y + size.y);

    draw_list->AddRectFilled(pos, max, ImGui::ColorConvertFloat4ToU32(qui::color::retina_dark::WindowBackground), rounding);
    draw_list->AddRect(pos, max, ImGui::ColorConvertFloat4ToU32(qui::color::retina_dark::Border), rounding);

    const ImVec2 knob_min(pos.x + t * seg, pos.y);
    const ImVec2 knob_max(knob_min.x + seg, pos.y + size.y);
    ImVec4 accent = qui::color::retina_dark::Highlight;
    if (!hovered) {
        accent.w = 0.85F;
    }
    draw_list->AddRectFilled(knob_min, knob_max, ImGui::ColorConvertFloat4ToU32(accent), rounding);

    ImFont *font = qui::font_semi_bold();
    if (font != nullptr) {
        ImGui::PushFont(font);
    }
    const ImU32 active = ImGui::ColorConvertFloat4ToU32(qui::color::retina_dark::WindowBackground);
    const ImU32 inactive = ImGui::ColorConvertFloat4ToU32(qui::color::retina_dark::TextDisabled);
    const char *labels[2] = {"ASM", "DCPL"};
    for (int i = 0; i < 2; ++i) {
        const ImVec2 text_size = ImGui::CalcTextSize(labels[i]);
        const f32 text_x = pos.x + seg * static_cast<f32>(i) + (seg - text_size.x) * 0.5F;
        const f32 text_y = pos.y + (size.y - text_size.y) * 0.5F;
        const bool is_active = (i == 1) == *dcpl;
        draw_list->AddText(ImVec2(text_x, text_y), is_active ? active : inactive, labels[i]);
    }
    if (font != nullptr) {
        ImGui::PopFont();
    }

    ImGui::PopID();
    return changed;
}

void dv_function_switch_and_body(value_view v, const function_disassembly &func, const void *id, bool open) {
    bool &show_dcpl = v.doc->m_lambdaViewDcpl[&func];

    const ImVec2 sw_size = view_switch_size();
    ImGui::SameLine();
    ImGui::SetCursorPosX(std::max(ImGui::GetCursorPosX(), ImGui::GetContentRegionMax().x - sw_size.x));
    ImGui::PushID(id);
    draw_view_switch("##view_switch", &show_dcpl, sw_size);
    ImGui::PopID();

    if (!open) {
        return;
    }

    ImGui::PushID(id);
    if (show_dcpl) {
        const auto it = v.doc->m_decompiled.find(&func);
        if (it != v.doc->m_decompiled.end()) {
            const std::string &text = it->second;
            qui::code::code_window("##dcpl_view", text, dcpl_rules());
        } else {
            ImGui::PushStyleColor(ImGuiCol_Text, qui::color::retina_dark::TextDisabled);
            qui::text_label("Decompilation unavailable for this function.");
            ImGui::PopStyleColor();
        }
    } else {
        dv_draw_function_body(v, func);
    }
    ImGui::PopID();
    dv_tree_pop(v);
}

void dv_draw_script_lambda(value_view v, const disassembled_value &entry, const void *id, i32 index) {
    char label[64];
    char suffix[64];
    char prefix[24];
    dv_index_prefix(prefix, sizeof(prefix), index);

    const function_disassembly *func = nullptr;
    for (const auto &child : entry.m_values) {
        if (const auto *f = std::get_if<std::shared_ptr<function_disassembly>>(&child); f != nullptr && *f != nullptr) {
            func = f->get();
        }
    }
    if (func == nullptr && v.doc->m_disassembler != nullptr) {
        func = v.doc->m_disassembler->get_function_at_offset(entry.m_offset);
    }

    std::snprintf(label, sizeof(label), "%sscript-lambda", prefix);
    std::snprintf(suffix, sizeof(suffix), "[0x%05X]", static_cast<u32>(entry.m_offset));
    const bool open = dv_node(v, id, val_color::Function, label, suffix, func == nullptr);

    if (func == nullptr) {
        return;
    }
    dv_function_switch_and_body(v, *func, id, open);
}

void dv_draw_text_leaf(value_view v, const void *id, const ImVec4 &color, const std::string &text) {
    std::string single_line = text;
    const std::size_t newline = single_line.find('\n');
    if (newline != std::string::npos) {
        single_line.resize(newline);
    }
    dv_node(v, id, color, single_line.c_str(), nullptr, true);
}

void dv_draw_state_script(value_view v, const ast::state_script &script, i32 index) {
    char label[256];
    std::snprintf(label, sizeof(label), "[%d] state-script %s", index, script.m_name.c_str());
    const bool open = dv_node(v, &script, val_color::StateScript, label, nullptr, false);
    if (!open) {
        return;
    }
    ImGui::PushID(&script);

    if (!script.m_options.empty()) {
        if (dv_node(v, &script.m_options, val_color::Group, "options", nullptr, false)) {
            for (const ast::sid_identifier &option : script.m_options) {
                dv_draw_text_leaf(v, &option, val_color::Sid, option.to_c_string());
            }
            dv_tree_pop(v);
        }
    }

    if (!script.m_declarations.empty()) {
        if (dv_node(v, &script.m_declarations, val_color::Group, "declarations", nullptr, false)) {
            for (const ast::variable_declaration &declaration : script.m_declarations) {
                dv_draw_text_leaf(v, &declaration, val_color::Struct, declaration.to_c_string());
            }
            dv_tree_pop(v);
        }
    }

    for (const ast::state_script_state &ss_state : script.m_states) {
        char state_label[256];
        std::snprintf(state_label, sizeof(state_label), "state %s", ss_state.m_name.c_str());
        if (dv_node(v, &ss_state, val_color::StateScript, state_label, nullptr, ss_state.m_blocks.empty()) && !ss_state.m_blocks.empty()) {
            for (const ast::state_script_block &block : ss_state.m_blocks) {
                const std::string block_name = block.block_type_to_string();
                if (dv_node(v, &block, val_color::Group, block_name.c_str(), nullptr, block.m_tracks.empty()) && !block.m_tracks.empty()) {
                    for (const ast::state_script_track &track : block.m_tracks) {
                        char track_label[256];
                        std::snprintf(track_label, sizeof(track_label), "track %s", track.m_name.c_str());
                        if (dv_node(v, &track, val_color::Group, track_label, nullptr, track.m_lambdas.empty()) && !track.m_lambdas.empty()) {
                            i32 lambda_index = 0;
                            for (const ast::state_script_lambda &lambda : track.m_lambdas) {
                                std::visit([&](auto &&fn) {
                                    using T = std::decay_t<decltype(fn)>;
                                    if constexpr (std::is_same_v<T, std::shared_ptr<function_disassembly>>) {
                                        if (fn != nullptr) {
                                            dv_draw_function(v, *fn, lambda_index);
                                        }
                                    } else {
                                        dv_draw_text_leaf(v, &fn, val_color::Function, fn.to_c_string());
                                    }
                                }, lambda);
                                ++lambda_index;
                            }
                            dv_tree_pop(v);
                        }
                    }
                    dv_tree_pop(v);
                }
            }
            dv_tree_pop(v);
        }
    }

    ImGui::PopID();
    dv_tree_pop(v);
}

void draw_entry_detail(app_state &state, document &doc) {
    if (doc.m_entries == nullptr || state.m_sidbase == nullptr || doc.m_file == nullptr) {
        qui::text_label("Not disassembled.");
        return;
    }
    if (doc.m_selectedEntry < 0 || doc.m_selectedEntry >= static_cast<i32>(doc.m_entries->size())) {
        ImGui::PushStyleColor(ImGuiCol_Text, qui::color::retina_dark::TextDisabled);
        qui::text_label("Select an entry to inspect.");
        ImGui::PopStyleColor();
        return;
    }

    const disassembled_entry &entry = (*doc.m_entries)[static_cast<u32>(doc.m_selectedEntry)];
    const std::string name = lookup_sid(*state.m_sidbase, *doc.m_file, entry.m_nameId);
    const std::string type = lookup_sid(*state.m_sidbase, *doc.m_file, entry.m_typeId);

    char suffix[256];
    std::snprintf(suffix, sizeof(suffix), ": %s  [0x%05X]", type.c_str(), static_cast<u32>(entry.m_offset));

    doc.m_expandRequest = doc.m_pendingExpand;
    doc.m_pendingExpand = nullptr;
    doc.m_forceOpenDepth = 0;
    doc.m_forceStack.clear();

    value_view v{&state, &doc};
    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    ImGui::PushID(doc.m_selectedEntry);
    const bool entry_is_map = entry.m_typeId == SID("map") || entry.m_typeId == SID("map-32") || entry.m_typeId == SID("render-settings-map");
    const bool open = dv_node(v, &entry, val_color::EntryName, name.c_str(), suffix, entry.m_values.empty());
    if (open && !entry.m_values.empty()) {
        if (entry_is_map) {
            const structs::map *header = nullptr;
            const disassembled_value *keys = nullptr;
            const disassembled_value *vals = nullptr;
            dv_map_extract(entry.m_values, header, keys, vals);
            if (keys != nullptr && vals != nullptr && !keys->m_values.empty()) {
                dv_draw_map_table(v, *keys, *vals, &entry);
            }
        } else {
            dv_draw_values(v, entry.m_values);
        }
        dv_tree_pop(v);
    }
    ImGui::PopID();
}

void draw_document(app_state &state, document &doc) {
    const f32 avail_width = ImGui::GetContentRegionAvail().x;
    const f32 avail_height = ImGui::GetContentRegionAvail().y;

    if (doc.m_listWidth <= 0.0F) {
        doc.m_listWidth = avail_width * 0.25F;
    }
    const f32 max_list_width = std::max(MIN_PANEL_WIDTH, avail_width - SPLITTER_WIDTH - MIN_PANEL_WIDTH);
    doc.m_listWidth = std::clamp(doc.m_listWidth, MIN_PANEL_WIDTH, max_list_width);

    ImGui::BeginChild("##dconstruct_entry_list", ImVec2(doc.m_listWidth, 0.0F));
    draw_entry_list(state, doc);
    ImGui::EndChild();

    ImGui::SameLine(0.0F, 0.0F);

    ImGui::PushStyleColor(ImGuiCol_Button, qui::color::rgba(0x00, 0x00, 0x00, 0x00));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, qui::color::retina_dark::SeparatorHovered);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, qui::color::retina_dark::SeparatorActive);
    ImGui::Button("##dconstruct_splitter", ImVec2(SPLITTER_WIDTH, avail_height));
    if (ImGui::IsItemHovered() || ImGui::IsItemActive()) {
        ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
    }
    if (ImGui::IsItemActive()) {
        doc.m_listWidth += ImGui::GetIO().MouseDelta.x;
    }
    ImGui::PopStyleColor(3);

    ImGui::SameLine(0.0F, 0.0F);

    ImGui::BeginChild("##dconstruct_entry_view", ImVec2(0.0F, 0.0F), ImGuiChildFlags_Borders);
    ImVec2 detail_size;
    if (begin_labeled_table_frame("Disassembly", detail_size)) {
        ImGui::BeginChild("##dconstruct_entry_detail", detail_size);
        draw_entry_detail(state, doc);
        ImGui::EndChild();
    }
    ImGui::EndChild();
}

void draw_content_area(f32 top_offset, app_state &state) {
    const ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + top_offset));
    ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, viewport->Size.y - top_offset));

    constexpr ImGuiWindowFlags window_flags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoBringToFrontOnFocus;

    ImGui::Begin("##dconstruct_content", nullptr, window_flags);

    draw_status_text(state);
    ImGui::Dummy(ImVec2(0.0F, 0.0F));

    if (state.m_documents.empty()) {
        ImGui::End();
        return;
    }

    constexpr ImGuiTabBarFlags tab_bar_flags =
        ImGuiTabBarFlags_Reorderable |
        ImGuiTabBarFlags_AutoSelectNewTabs |
        ImGuiTabBarFlags_FittingPolicyScroll;

    if (ImGui::BeginTabBar("##dconstruct_tabs", tab_bar_flags)) {
        i32 to_close = -1;
        for (i32 i = 0; i < static_cast<i32>(state.m_documents.size()); ++i) {
            document &doc = state.m_documents[static_cast<u32>(i)];
            bool open = true;
            const std::string label = doc.m_name + "##doc" + std::to_string(i);
            const ImGuiTabItemFlags flags = i == state.m_pendingSelect ? ImGuiTabItemFlags_SetSelected : 0;
            if (ImGui::BeginTabItem(label.c_str(), &open, flags)) {
                state.m_activeDocument = i;
                ImGui::EndTabItem();
            }
            if (!open) {
                to_close = i;
            }
        }
        ImGui::EndTabBar();
        state.m_pendingSelect = -1;
        if (to_close >= 0) {
            close_document(state, to_close);
        }
    }

    if (document *doc = active_document(state)) {
        draw_document(state, *doc);
    }

    ImGui::End();
}

} // namespace

int main() {
    glfwSetErrorCallback(glfw_error_callback);
    if (glfwInit() == GLFW_FALSE) {
        return 1;
    }

    const char *glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    GLFWwindow *window = glfwCreateWindow(1600, 900, "dconstruct", nullptr, nullptr);
    if (window == nullptr) {
        glfwTerminate();
        return 1;
    }
    glfwSetWindowSizeLimits(window, 720, 450, GLFW_DONT_CARE, GLFW_DONT_CARE);
    install_window_proc(window);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    f32 x_scale = 1.0F;
    f32 y_scale = 1.0F;
    glfwGetWindowContentScale(window, &x_scale, &y_scale);
    const f32 dpi_scale = std::max(x_scale, y_scale);

    qui::apply_retina_dark_theme();
    qui::apply_dpi_scale(dpi_scale);
    qui::load_default_fonts(dpi_scale, 20.0F);

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    app_state state;
    if (std::filesystem::exists("sidbase.bin")) {
        load_sidbase(state, "sidbase.bin");
    }

    while (glfwWindowShouldClose(window) == GLFW_FALSE) {
        glfwPollEvents();

        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0) {
            glfwWaitEvents();
            continue;
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        const ImGuiViewport *viewport = ImGui::GetMainViewport();
        qui::update_window_resize(window, viewport->Pos, viewport->Size, qui::default_window_bar_state());

        const f32 bar_height = draw_title_menu_bar(state, window);
        draw_content_area(bar_height, state);

        ImGui::Render();

        int display_w = 0;
        int display_h = 0;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.06F, 0.06F, 0.06F, 1.00F);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    uninstall_window_proc(window);
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
