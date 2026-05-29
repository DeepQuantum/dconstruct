#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include "base.h"
#include "binaryfile.h"
#include "sidbase.h"
#include "disassembly/disassembler.h"

#include <qui.h>

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

void disassemble_document(app_state &state, document &doc) {
    doc.m_entries = nullptr;
    doc.m_selectedEntry = -1;
    doc.m_disassembler.reset();

    if (state.m_sidbase == nullptr || doc.m_file == nullptr) {
        return;
    }

    doc.m_disassembler = std::make_unique<Disassembler>(doc.m_file.get(), state.m_sidbase.get());
    doc.m_disassembler->disassemble();
    doc.m_entries = &doc.m_disassembler->get_disassembled_entries();

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
