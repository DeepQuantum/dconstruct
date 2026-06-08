#include <algorithm>
#include <array>
#include <cctype>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <expected>
#include <filesystem>
#include <format>
#include <fstream>
#include <memory>
#include <optional>
#include <print>
#include <sstream>
#include <span>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "base.h"
#include "binaryfile.h"
#include "sidbase.h"
#include "compilation/dc_parser.h"
#include "compilation/lexer.h"
#include "disassembly/disassembler.h"
#include "disassembly/edit_disassembler.h"
#include "disassembly/driver_functions.h"
#include "decompilation/decomp_function.h"

#include <qui.h>
#include <qui/code_window.hpp>
#include <imgui_stdlib.h>
#include <imgui_internal.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <commdlg.h>
#include <shellapi.h>
#include <ole2.h>
#include <shlobj.h>
#ifndef GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#endif
#include <GLFW/glfw3native.h>

#include "debugger/debugger.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "buildinfo.h"

namespace dconstruct::ui {

    constexpr f32 BAR_VERTICAL_PADDING = 8.0F;
    constexpr f32 WINDOW_BUTTON_WIDTH = 46.0F;
    constexpr f32 SPLITTER_WIDTH = 6.0F;
    constexpr f32 MIN_PANEL_WIDTH = 120.0F;
    constexpr f32 ENTRY_CELL_LEFT_PADDING = 4.0F;
    constexpr f32 FUNCTION_INSTRUCTION_COLUMN_WIDTH = 560.0F;
    constexpr i32 MAX_FORCE_OPEN_NODES = 10000;
    constexpr i32 EXPAND_SHALLOW_DEPTH = 2;
    constexpr i32 EXPAND_RECURSIVE_DEPTH = 1 << 30;
    constexpr char DONATE_URL[] = "https://ko-fi.com/deepquantum";
    constexpr char GITHUB_URL[] = "https://github.com/DeepQuantum";
    constexpr wchar_t PREVIOUS_WNDPROC_PROP[] = L"dconstruct.previous_wndproc";

    enum class edit_kind {
        Int,
        Int64,
        Float,
        Sid
    };

    struct pending_edit {
        u32 m_offset = 0;
        edit_kind m_kind = edit_kind::Int;
        std::string m_text;
    };

    using mapped_var_value = std::pair<ast::full_type, std::optional<std::string>>;

    struct type_map_record {
        sid64 m_function = 0;
        u64 m_varIndex = 0;
        std::optional<mapped_var_value> m_old;
        std::optional<mapped_var_value> m_new;
    };

    struct edit_record {
        u32 m_offset = 0;
        edit_kind m_kind = edit_kind::Int;
        std::vector<std::byte> m_oldBytes;
        std::vector<std::byte> m_newBytes;
        std::string m_oldText;
        std::string m_newText;
        std::optional<type_map_record> m_typeMap;
    };

    struct local_var_type_edit {
        sid64 m_function = 0;
        u64 m_varIndex = 0;
        std::string m_oldName;
        std::string m_name;
        std::string m_typeText;
        std::string m_error;
        ImVec2 m_pos = ImVec2(0.0F, 0.0F);
        int m_typeSuggestionIndex = 0;
        std::string m_typeSuggestionFilter;
        bool m_focusType = true;
        bool m_focusCursorEnd = false;
    };

    struct struct_pointer_edit {
        u64 m_pointerOffset = 0;
        sid64 m_currentTypeId = 0;
        std::string m_text;
        bool m_focus = true;
        ImVec2 m_pos = ImVec2(0.0F, 0.0F);
    };

    enum class tree_op {
        none,
        expand,
        close
    };

    struct document {
        std::unique_ptr<BinaryFile> m_file;
        std::unique_ptr<Disassembler> m_disassembler;
        std::unique_ptr<EditDisassembler> m_editor;
        const std::vector<disassembled_entry>* m_entries = nullptr;
        std::string m_path;
        std::string m_name;
        i32 m_selectedEntry = -1;
        f32 m_listWidth = 0.0F;
        std::string m_entrySearch;
        std::string m_typeSearch;
        std::vector<qui::fuzzy_match> m_currentMatches;
        std::vector<qui::fuzzy_match> m_typeMatches;
        i32 m_entrySortColumn = -1;
        bool m_entrySortDescending = false;
        ImGuiID m_menuTarget = 0;
        bool m_openMenu = false;
        ImVec2 m_menuMousePos = ImVec2(0.0F, 0.0F);
        std::optional<u64> m_menuStructPtrOffset;
        sid64 m_menuStructTypeId = 0;
        ImGuiID m_opTarget = 0;
        ImGuiID m_opPendingTarget = 0;
        tree_op m_opMode = tree_op::none;
        tree_op m_opPendingMode = tree_op::none;
        i32 m_opMaxDepth = 0;
        i32 m_opPendingMaxDepth = 0;
        i32 m_opDepth = -1;
        i32 m_forceOpenCount = 0;
        std::vector<i32> m_depthStack;
        std::vector<ImGuiID> m_closeIds;
        std::unordered_map<const function_disassembly*, ast::code_color_buffer> m_decompiled;
        std::unordered_map<const function_disassembly*, std::string> m_decompErrors;
        std::unordered_map<const function_disassembly*, bool> m_lambdaViewDcpl;
        bool m_dirty = false;
        ImGuiID m_editingValue = 0;
        bool m_editFocus = false;
        std::string m_editBuffer;
        std::vector<pending_edit> m_pendingEdits;
        std::vector<edit_record> m_undoStack;
        std::vector<edit_record> m_redoStack;
        std::size_t m_savedDepth = 0;
        std::size_t m_savedTypeDepth = 0;
        ast::function_to_mapped_vars m_functionScopes;
        std::optional<local_var_type_edit> m_localVarTypeEdit;
        std::optional<struct_pointer_edit> m_structPointerEdit;
        std::unordered_map<const void*, i32> m_mapSortStates;
    };

    template <typename... Args>
    void log_event(std::format_string<Args...> fmt, Args&&... args) {
        static std::ofstream log_file("dconstruct.log", std::ios::app);
        const auto now = std::chrono::floor<std::chrono::seconds>(std::chrono::system_clock::now());
        std::println(log_file, "[{:%Y-%m-%d %H:%M:%S}] {}", now, std::format(fmt, std::forward<Args>(args)...));
        log_file.flush();
    }

    struct drag_state {
        bool m_active = false;
        i32 m_startWindowX = 0;
        i32 m_startWindowY = 0;
        f64 m_startCursorX = 0.0;
        f64 m_startCursorY = 0.0;
    };

    constexpr u32 VM_REGISTER_COUNT = 49;
    constexpr i32 VM_REGISTER_COLUMNS = 5;
    constexpr i32 VM_NORMAL_DEFAULT_ROWS = 2;
    constexpr i32 VM_ARGUMENT_DEFAULT_ROWS = 1;
    constexpr f32 DEBUGGER_OUTPUT_HEIGHT = 150.0F;

    struct debugger_state {
        bool m_active = false;
        bool m_expandNormal = false;
        bool m_expandArgs = false;
        f32 m_leftWidth = 0.0F;
        f32 m_middleWidth = 0.0F;
        std::array<u64, VM_REGISTER_COUNT> m_registers{};
        std::array<u64, VM_REGISTER_COUNT> m_argRegisters{};
        std::shared_ptr<dconstruct::debugger::debugger_snapshot> m_snapshot;
        ast::code_color_buffer m_instructionsAndSymbols;
        dconstruct::debugger::debugger::STATE m_lastPollState = dconstruct::debugger::debugger::STATE::DETACHED;
        bool m_hasPollState = false;
        bool m_sidsSeeded = false;
        dconstruct::debugger::debugger m_debugger;

        void render(const dconstruct::debugger::debugger_snapshot& snapshot);
    };

    struct app_state {
        std::unique_ptr<SIDBase> m_sidbase;
        std::vector<document> m_documents;
        i32 m_activeDocument = -1;
        i32 m_pendingSelect = -1;
        std::string m_loadError;
        drag_state m_drag;
        GLFWwindow* m_window = nullptr;
        unsigned int m_iconTexture = 0;
        std::string m_nameArt;
        std::string m_wordmarkArt;
        qui::message_box m_errorBox;
        qui::message_box m_closeBox;
        bool m_closeRequested = false;
        bool m_defaultViewDcpl = true;
        bool m_ssoVarOptimization = true;
        bool m_foreachOptimization = true;
        bool m_matchOptimization = true;
        bool m_secondVarOptimization = true;
        bool m_memberAccessOptimization = true;
        bool m_regexOptimization = true;
        bool m_dragHover = false;
        std::vector<std::string> m_pendingDropPaths;
        std::unordered_map<sid64, ast::full_type> m_typeMap;
        std::unordered_map<std::string, ast::function_to_mapped_vars> m_pendingTypeMaps;
        std::string m_colorScheme = "qntm";
        std::string m_previewScheme;
        debugger_state m_debugger;
    };

    void glfw_error_callback(int error, const char* description) {
        std::fprintf(stderr, "GLFW error %d: %s\n", error, description);
    }

    u32 create_icon_texture() {
        HMODULE module = GetModuleHandleW(nullptr);
        HRSRC resource = FindResourceW(module, L"DCONSTRUCT_LOGO", MAKEINTRESOURCEW(10));
        if (resource == nullptr) {
            return 0;
        }
        HGLOBAL loaded = LoadResource(module, resource);
        const void* data = loaded != nullptr ? LockResource(loaded) : nullptr;
        const DWORD size = SizeofResource(module, resource);
        if (data == nullptr || size == 0) {
            return 0;
        }

        i32 width = 0;
        i32 height = 0;
        i32 channels = 0;
        stbi_uc* pixels = stbi_load_from_memory(
            static_cast<const stbi_uc*>(data),
            static_cast<int>(size),
            &width,
            &height,
            &channels,
            4
        );
        if (pixels == nullptr) {
            return 0;
        }

        unsigned int texture = 0;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGBA,
            width,
            height,
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            pixels
        );
        stbi_image_free(pixels);
        return texture;
    }

    std::string load_text_resource(const wchar_t* name) {
        HMODULE module = GetModuleHandleW(nullptr);
        HRSRC resource = FindResourceW(module, name, MAKEINTRESOURCEW(10));
        if (resource == nullptr) {
            return {};
        }
        HGLOBAL loaded = LoadResource(module, resource);
        const void* data = loaded != nullptr ? LockResource(loaded) : nullptr;
        const DWORD size = SizeofResource(module, resource);
        if (data == nullptr || size == 0) {
            return {};
        }

        std::string art(static_cast<const char*>(data), static_cast<std::size_t>(size));
        if (art.size() >= 2 && art.front() == '"' && art.back() == '"') {
            art = art.substr(1, art.size() - 2);
        }
        return art;
    }

    std::string filename_from_path(const std::string& path) {
        const std::size_t slash = path.find_last_of("/\\");
        return slash == std::string::npos ? path : path.substr(slash + 1);
    }

    void open_url(const char* url) {
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

    void install_window_proc(GLFWwindow* window) {
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

    void uninstall_window_proc(GLFWwindow* window) {
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

    std::string prompt_open_file(GLFWwindow* window, const char* filter, const char* title) {
        char buffer[MAX_PATH] = {};

        OPENFILENAMEA ofn = {};
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = glfwGetWin32Window(window);
        ofn.lpstrFilter = filter;
        ofn.lpstrFile = buffer;
        ofn.nMaxFile = sizeof(buffer);
        ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
        ofn.lpstrTitle = title;

        if (GetOpenFileNameA(&ofn) != 0) {
            return std::string(buffer);
        }
        return {};
    }

    f32 measure_text(ImFont* font, const char* text) {
        if (font != nullptr) {
            ImGui::PushFont(font);
        }
        const f32 width = ImGui::CalcTextSize(text).x;
        if (font != nullptr) {
            ImGui::PopFont();
        }
        return width;
    }

    f32 measure_entry_list_width(const app_state& state, const document& doc) {
        f32 max_width = 0.0F;
        if (doc.m_entries == nullptr || state.m_sidbase == nullptr || doc.m_file == nullptr) {
            return max_width;
        }
        const ImGuiStyle& style = ImGui::GetStyle();
        for (const disassembled_entry& entry : *doc.m_entries) {
            const f32 width =
                measure_text(qui::font_bold(), state.m_sidbase->lookup(entry.m_nameId, doc.m_file->m_sidCache)) +
                measure_text(qui::font_medium(), state.m_sidbase->lookup(entry.m_typeId, doc.m_file->m_sidCache)) +
                style.CellPadding.x * 4.0F +
                ENTRY_CELL_LEFT_PADDING * 2.0F;
            max_width = std::max(max_width, width);
        }
        return max_width;
    }

    dcompiler::OPTIMIZATION_KIND decompilation_optimizations(const app_state& state) {
        using dcompiler::OPTIMIZATION_KIND;

        auto optimizations = OPTIMIZATION_KIND::NONE;
        if (state.m_ssoVarOptimization) {
            optimizations = optimizations | OPTIMIZATION_KIND::SSO_VAR;
        }
        if (state.m_foreachOptimization) {
            optimizations = optimizations | OPTIMIZATION_KIND::FOREACH;
        }
        if (state.m_matchOptimization) {
            optimizations = optimizations | OPTIMIZATION_KIND::MATCH;
        }
        if (state.m_secondVarOptimization) {
            optimizations = optimizations | OPTIMIZATION_KIND::SECOND_VAR;
        }
        if (state.m_memberAccessOptimization) {
            optimizations = optimizations | OPTIMIZATION_KIND::MEMBER_ACCESS;
        }
        if (state.m_regexOptimization) {
            optimizations = optimizations | OPTIMIZATION_KIND::REGEX;
        }
        return optimizations;
    }

    bool all_decompilation_optimizations_enabled(const app_state& state) {
        return state.m_ssoVarOptimization &&
               state.m_foreachOptimization &&
               state.m_matchOptimization &&
               state.m_secondVarOptimization &&
               state.m_memberAccessOptimization &&
               state.m_regexOptimization;
    }

    void set_all_decompilation_optimizations(app_state& state, const bool enabled) {
        state.m_ssoVarOptimization = enabled;
        state.m_foreachOptimization = enabled;
        state.m_matchOptimization = enabled;
        state.m_secondVarOptimization = enabled;
        state.m_memberAccessOptimization = enabled;
        state.m_regexOptimization = enabled;
    }

    void decompile_document(app_state& state, document& doc) {
        doc.m_decompiled.clear();
        doc.m_decompErrors.clear();
        doc.m_lambdaViewDcpl.clear();
        if (doc.m_disassembler == nullptr || doc.m_file == nullptr || state.m_sidbase == nullptr) {
            return;
        }

        const dcompiler::OPTIMIZATION_KIND optimizations = decompilation_optimizations(state);

        // Reused across functions: take() moves the segment vector out into the
        // document, then reserve() pre-grows the next one to avoid reallocations.
        ast::code_color_serialization_buffer color_buffer;
        for (const function_disassembly* func : doc.m_disassembler->get_all_functions()) {
            const ast::mapped_var_scope* function_scope = nullptr;
            if (!doc.m_functionScopes.empty()) {
                if (auto entry = doc.m_functionScopes.find(SID(func->get_id().c_str())); entry != doc.m_functionScopes.end()) {
                    function_scope = &entry->second;
                }
            }
            auto decomp_func = dcompiler::decomp_function{*func, *doc.m_file, *state.m_sidbase, ControlFlowGraph::build(*func), std::nullopt, nullptr, function_scope};
            ast::function_definition def = decomp_func.decompile(optimizations);
            color_buffer.m_currentIndent = 0;
            color_buffer.reserve(256);
            def.to_pseudo_c_colored_string(color_buffer);
            doc.m_decompiled.emplace(func, color_buffer.take());
            if (decomp_func.m_error) {
                doc.m_decompErrors.emplace(func, *decomp_func.m_error);
            }
        }
    }

    void disassemble_document(app_state& state, document& doc) {
        doc.m_entries = nullptr;
        doc.m_selectedEntry = -1;
        doc.m_disassembler.reset();
        doc.m_editor.reset();
        doc.m_editingValue = 0;
        doc.m_decompiled.clear();
        doc.m_decompErrors.clear();
        doc.m_lambdaViewDcpl.clear();

        if (state.m_sidbase == nullptr || doc.m_file == nullptr) {
            return;
        }

        doc.m_disassembler = std::make_unique<Disassembler>(doc.m_file.get(), state.m_sidbase.get(), &state.m_typeMap);
        doc.m_disassembler->disassemble();
        doc.m_entries = &doc.m_disassembler->get_disassembled_entries();
        doc.m_editor = std::make_unique<EditDisassembler>(doc.m_file.get(), state.m_sidbase.get(), std::vector<std::string>{});
        decompile_document(state, doc);

        const ImGuiStyle& style = ImGui::GetStyle();
        doc.m_listWidth = measure_entry_list_width(state, doc) + style.WindowPadding.x * 2.0F + style.ScrollbarSize + 8.0F;
    }

    void close_document(app_state& state, const i32 index) {
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

    document* active_document(app_state& state) {
        if (state.m_activeDocument < 0 || state.m_activeDocument >= static_cast<i32>(state.m_documents.size())) {
            return nullptr;
        }
        return &state.m_documents[static_cast<u32>(state.m_activeDocument)];
    }

    void load_bin_file(app_state& state, const std::string& path) {
        state.m_loadError.clear();

        for (i32 i = 0; i < static_cast<i32>(state.m_documents.size()); ++i) {
            if (state.m_documents[static_cast<u32>(i)].m_path == path) {
                state.m_activeDocument = i;
                state.m_pendingSelect = i;
                log_event("File already open, switching to it: {}", path);
                return;
            }
        }

        auto file_res = BinaryFile::from_path(path);
        if (!file_res) {
            state.m_loadError = file_res.error();
            return;
        }

        document doc;
        doc.m_file = std::make_unique<BinaryFile>(std::move(*file_res));
        doc.m_path = path;
        doc.m_name = filename_from_path(path);
        if (auto it = state.m_pendingTypeMaps.find(std::filesystem::path(doc.m_name).stem().string()); it != state.m_pendingTypeMaps.end()) {
            doc.m_functionScopes = it->second;
        }
        state.m_documents.push_back(std::move(doc));
        state.m_activeDocument = static_cast<i32>(state.m_documents.size()) - 1;
        state.m_pendingSelect = state.m_activeDocument;
        disassemble_document(state, state.m_documents.back());
        log_event("Loaded file: {}", path);
    }

    void load_sidbase(app_state& state, const std::string& path) {
        auto sidbase_res = SIDBase::from_binary(path);
        if (!sidbase_res) {
            state.m_loadError = sidbase_res.error();
            return;
        }

        state.m_sidbase = std::make_unique<SIDBase>(std::move(*sidbase_res));
        std::error_code ec;
        const std::uintmax_t size = std::filesystem::file_size(path, ec);
        log_event("Loaded sidbase: {} ({} bytes)", path, ec ? 0 : size);
        for (document& doc : state.m_documents) {
            disassemble_document(state, doc);
        }
    }

    void show_alert(app_state& state, std::string title, std::string message) {
        qui::open_alert(state.m_errorBox, std::move(title), std::move(message));
    }

    void load_type_defs(app_state& state, const std::string& path) {
        auto type_def_res = disassembly::parse_type_defs_file(path);
        if (!type_def_res) {
            show_alert(state, "Type definition error", "Couldn't parse type definition file " + path + ":\n\n" + type_def_res.error());
            return;
        }
        state.m_typeMap = std::move(*type_def_res);
        log_event("Loaded type definitions: {} ({} types)", path, state.m_typeMap.size());
        for (document& doc : state.m_documents) {
            disassemble_document(state, doc);
        }
    }

    void load_type_map(app_state& state, const std::string& path) {
        auto scopes_res = disassembly::parse_var_type_map_file(path, &state.m_typeMap);
        if (!scopes_res) {
            show_alert(state, "Type map error", "Couldn't parse type map " + path + ":\n\n" + scopes_res.error() +
                "\nThe document will be decompiled without these types. Load a matching type definition file first if the type is unknown.");
            return;
        }

        const std::string stem = std::filesystem::path(path).stem().string();
        state.m_pendingTypeMaps[stem] = *scopes_res;

        document* match = nullptr;
        for (document& doc : state.m_documents) {
            if (std::filesystem::path(doc.m_name).stem().string() == stem) {
                match = &doc;
                break;
            }
        }
        if (match == nullptr) {
            show_alert(state, "Type map loaded", "No open document matches '" + std::filesystem::path(path).filename().string() +
                "'. It will be applied automatically when " + stem + ".bin is opened.");
            return;
        }
        match->m_functionScopes = std::move(*scopes_res);
        decompile_document(state, *match);
        log_event("Loaded type map {} for {}", path, match->m_name);
    }

    void load_var_maps_directory(app_state& state) {
        const std::filesystem::path dir = "var_maps";
        if (!std::filesystem::is_directory(dir)) {
            return;
        }

        std::string errors;

        const std::filesystem::path type_defs_path = dir / "type_defines.dcpl";
        if (std::filesystem::exists(type_defs_path)) {
            auto type_def_res = disassembly::parse_type_defs_file(type_defs_path);
            if (type_def_res) {
                state.m_typeMap = std::move(*type_def_res);
                log_event("Loaded type definitions: {} ({} types)", type_defs_path.string(), state.m_typeMap.size());
            } else {
                errors += "Couldn't parse " + type_defs_path.string() + ":\n" + type_def_res.error() + "\n";
            }
        }

        for (const auto& entry : std::filesystem::directory_iterator(dir)) {
            if (!entry.is_regular_file() || entry.path().extension() != ".dcplmap") {
                continue;
            }
            auto scopes_res = disassembly::parse_var_type_map_file(entry.path(), &state.m_typeMap);
            if (!scopes_res) {
                errors += "Couldn't parse " + entry.path().filename().string() + ":\n" + scopes_res.error() + "\n";
                continue;
            }
            state.m_pendingTypeMaps[entry.path().stem().string()] = std::move(*scopes_res);
            log_event("Parsed type map: {}", entry.path().string());
        }

        if (!errors.empty()) {
            show_alert(state, "Type map errors",
                "Some type files in var_maps could not be parsed. Affected files will be decompiled without their types.\n\n" + errors);
        }
    }

    std::string trim_copy(std::string_view value) {
        while (!value.empty() && std::isspace(static_cast<unsigned char>(value.front())) != 0) {
            value.remove_prefix(1);
        }
        while (!value.empty() && std::isspace(static_cast<unsigned char>(value.back())) != 0) {
            value.remove_suffix(1);
        }
        return std::string(value);
    }

    bool is_identifier_text(std::string_view text) {
        if (text.empty()) {
            return false;
        }
        const unsigned char first = static_cast<unsigned char>(text.front());
        if (std::isalpha(first) == 0 && text.front() != '_') {
            return false;
        }
        for (char c : text.substr(1)) {
            const unsigned char ch = static_cast<unsigned char>(c);
            if (std::isalnum(ch) == 0 && c != '_') {
                return false;
            }
        }
        return true;
    }

    bool parse_generated_var_name(std::string_view name, u64& index) {
        constexpr std::string_view prefix = "var_";
        if (!name.starts_with(prefix) || name.size() == prefix.size()) {
            return false;
        }
        u64 value = 0;
        for (char c : name.substr(prefix.size())) {
            if (std::isdigit(static_cast<unsigned char>(c)) == 0) {
                return false;
            }
            value = value * 10 + static_cast<u64>(c - '0');
        }
        index = value;
        return true;
    }

    bool token_is_member_access(const qui::code::hovered_token& token) {
        if (token.column == 0 || token.column > token.line_text.size()) {
            return false;
        }
        std::size_t prev = token.column;
        while (prev > 0 && std::isspace(static_cast<unsigned char>(token.line_text[prev - 1])) != 0) {
            --prev;
        }
        if (prev == 0) {
            return false;
        }
        if (token.line_text[prev - 1] == '.') {
            return true;
        }
        return token.line_text[prev - 1] == '>' && prev >= 2 && token.line_text[prev - 2] == '-';
    }

    bool token_is_call_name(const qui::code::hovered_token& token) {
        const std::size_t end = token.column + token.text.size();
        if (end >= token.line_text.size()) {
            return false;
        }
        std::size_t next = end;
        while (next < token.line_text.size() && std::isspace(static_cast<unsigned char>(token.line_text[next])) != 0) {
            ++next;
        }
        return next < token.line_text.size() && token.line_text[next] == '(';
    }

    std::string plain_code_string(const ast::code_color_buffer& code) {
        std::string result;
        for (const auto& [_, text] : code) {
            result += text;
        }
        return result;
    }

    std::string infer_declaration_type_text(const ast::code_color_buffer& code, const std::string& var_name) {
        const std::string plain = plain_code_string(code);
        std::size_t line_start = 0;
        while (line_start <= plain.size()) {
            std::size_t line_end = plain.find('\n', line_start);
            if (line_end == std::string::npos) {
                line_end = plain.size();
            }
            std::string_view line(plain.data() + line_start, line_end - line_start);
            const std::size_t pos = line.find(var_name);
            if (pos != std::string_view::npos) {
                const bool left_ok = pos == 0 ||
                    (!std::isalnum(static_cast<unsigned char>(line[pos - 1])) && line[pos - 1] != '_');
                const std::size_t after = pos + var_name.size();
                const bool right_ok = after >= line.size() ||
                    (!std::isalnum(static_cast<unsigned char>(line[after])) && line[after] != '_');
                if (left_ok && right_ok) {
                    std::string prefix = trim_copy(line.substr(0, pos));
                    if (!prefix.empty() &&
                        prefix.find('=') == std::string::npos &&
                        prefix.find(';') == std::string::npos &&
                        prefix.find('+') == std::string::npos &&
                        prefix.find('!') == std::string::npos &&
                        prefix.find('.') == std::string::npos &&
                        prefix != "return") {
                        if (prefix == ast::UNKNOWN_TYPE_NAME) {
                            return "u64";
                        }
                        return prefix;
                    }
                }
            }
            if (line_end == plain.size()) {
                break;
            }
            line_start = line_end + 1;
        }
        return "u64";
    }

    std::expected<ast::full_type, std::string> parse_type_text(const app_state& state, const std::string& type_text) {
        const std::string source = "typemap { #__dconstruct_type_probe { 0 " + type_text + "; } }";
        compilation::Lexer lexer{source};
        const auto [tokens, lex_errors] = lexer.get_results();
        if (!lex_errors.empty()) {
            std::ostringstream oss;
            for (const auto& err : lex_errors) {
                oss << "[syntax error] " << err << '\n';
            }
            return std::unexpected{oss.str()};
        }

        compilation::Parser parser{tokens};
        parser.add_mapped_types(state.m_typeMap);
        auto typemap = parser.make_typemap();
        if (!typemap || !parser.get_errors().empty()) {
            std::ostringstream oss;
            for (const auto& err : parser.get_errors()) {
                oss << "[parsing error] " << err.m_message << '\n';
            }
            return std::unexpected{oss.str()};
        }

        const auto func_it = typemap->find(SID("__dconstruct_type_probe"));
        if (func_it == typemap->end()) {
            return std::unexpected{"internal error: type probe was not parsed"};
        }
        const auto var_it = func_it->second.find(0);
        if (var_it == func_it->second.end()) {
            return std::unexpected{"internal error: type probe variable was not parsed"};
        }
        return var_it->second.first;
    }

    std::string document_stem(const document& doc) {
        return std::filesystem::path(doc.m_name).stem().string();
    }

    std::string serialize_document_typemap(const document& doc) {
        std::ostringstream oss;
        oss << "typemap {\n";
        if (doc.m_disassembler != nullptr) {
            for (const function_disassembly* func : doc.m_disassembler->get_all_functions()) {
                const auto scope_it = doc.m_functionScopes.find(SID(func->get_id().c_str()));
                if (scope_it == doc.m_functionScopes.end() || scope_it->second.empty()) {
                    continue;
                }
                oss << "    #" << func->get_id() << " {\n";
                for (const auto& [var_index, mapped] : scope_it->second) {
                    const auto& [type, alias] = mapped;
                    oss << "        " << var_index << ' ' << ast::type_to_declaration_string(type);
                    const std::string generated_name = "var_" + std::to_string(var_index);
                    if (alias && *alias != generated_name) {
                        oss << " -> " << *alias;
                    }
                    oss << ";\n";
                }
                oss << "    }\n";
            }
        }
        oss << "}\n";
        return oss.str();
    }

    bool write_document_typemap(app_state& state, document& doc, std::string& error) {
        const std::filesystem::path dir = "var_maps";
        std::error_code ec;
        std::filesystem::create_directories(dir, ec);
        if (ec) {
            error = "Couldn't create var_maps directory: " + ec.message();
            return false;
        }

        const std::filesystem::path path = dir / (document_stem(doc) + ".dcplmap");
        const bool existed = std::filesystem::exists(path);
        std::ofstream out(path, std::ios::trunc);
        if (!out) {
            error = "Couldn't open " + path.string() + " for writing.";
            return false;
        }
        out << serialize_document_typemap(doc);
        if (!out) {
            error = "Couldn't write " + path.string() + ".";
            return false;
        }

        state.m_pendingTypeMaps[document_stem(doc)] = doc.m_functionScopes;
        log_event("{} type map: {}", existed ? "Updated" : "Created", path.string());
        return true;
    }

    bool path_is_bin(const char* path) {
        const std::filesystem::path parsed(path);
        std::string ext = parsed.extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c) {
            return static_cast<char>(std::tolower(c));
        });
        return ext == ".bin";
    }

    std::vector<std::string> collect_bin_paths(IDataObject* data) {
        std::vector<std::string> result;
        if (data == nullptr) {
            return result;
        }

        FORMATETC format = {CF_HDROP, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
        STGMEDIUM medium = {};
        if (data->GetData(&format, &medium) != S_OK) {
            return result;
        }

        if (auto drop = static_cast<HDROP>(GlobalLock(medium.hGlobal)); drop != nullptr) {
            const UINT count = DragQueryFileA(drop, 0xFFFFFFFFU, nullptr, 0);
            char buffer[MAX_PATH];
            for (UINT i = 0; i < count; ++i) {
                if (DragQueryFileA(drop, i, buffer, MAX_PATH) > 0 && path_is_bin(buffer)) {
                    result.emplace_back(buffer);
                }
            }
            GlobalUnlock(medium.hGlobal);
        }
        ReleaseStgMedium(&medium);
        return result;
    }

    class file_drop_target : public IDropTarget {
    public:
        explicit file_drop_target(app_state* state) : m_state(state) {}

        HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** out) override {
            if (riid == IID_IUnknown || riid == IID_IDropTarget) {
                *out = static_cast<IDropTarget*>(this);
                AddRef();
                return S_OK;
            }
            *out = nullptr;
            return E_NOINTERFACE;
        }

        ULONG STDMETHODCALLTYPE AddRef() override {
            return static_cast<ULONG>(InterlockedIncrement(&m_refCount));
        }

        ULONG STDMETHODCALLTYPE Release() override {
            const LONG count = InterlockedDecrement(&m_refCount);
            if (count == 0) {
                delete this;
            }
            return static_cast<ULONG>(count);
        }

        HRESULT STDMETHODCALLTYPE DragEnter(IDataObject* data, DWORD, POINTL, DWORD* effect) override {
            m_acceptable = !collect_bin_paths(data).empty();
            m_state->m_dragHover = m_acceptable;
            *effect = m_acceptable ? DROPEFFECT_COPY : DROPEFFECT_NONE;
            return S_OK;
        }

        HRESULT STDMETHODCALLTYPE DragOver(DWORD, POINTL, DWORD* effect) override {
            *effect = m_acceptable ? DROPEFFECT_COPY : DROPEFFECT_NONE;
            return S_OK;
        }

        HRESULT STDMETHODCALLTYPE DragLeave() override {
            m_state->m_dragHover = false;
            m_acceptable = false;
            return S_OK;
        }

        HRESULT STDMETHODCALLTYPE Drop(IDataObject* data, DWORD, POINTL, DWORD* effect) override {
            std::vector<std::string> paths = collect_bin_paths(data);
            for (std::string& path : paths) {
                m_state->m_pendingDropPaths.push_back(std::move(path));
            }
            m_state->m_dragHover = false;
            m_acceptable = false;
            *effect = m_state->m_pendingDropPaths.empty() ? DROPEFFECT_NONE : DROPEFFECT_COPY;
            return S_OK;
        }

    private:
        app_state* m_state;
        LONG m_refCount = 1;
        bool m_acceptable = false;
    };

    void save_active_document(app_state& state);
    void undo_active_document(app_state& state);
    void redo_active_document(app_state& state);
    void update_dirty(document& doc);
    void request_app_close(app_state& state);

    void window_close_callback(GLFWwindow* window) {
        glfwSetWindowShouldClose(window, GLFW_FALSE);
        if (auto* state = static_cast<app_state*>(glfwGetWindowUserPointer(window))) {
            request_app_close(*state);
        }
    }

    void draw_about_popup(bool open_requested) {
        if (open_requested) {
            ImGui::OpenPopup("##dconstruct_about");
        }

        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5F, 0.5F));

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(28.0F, 24.0F));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.0F);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0F, 8.0F));
        ImGui::PushStyleColor(ImGuiCol_PopupBg, qui::color::active_palette().WindowBackground);

        if (ImGui::BeginPopupModal(
            "##dconstruct_about",
            nullptr,
            ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove
        )) {
            if (ImFont* title_font = qui::font_bold(); title_font != nullptr) {
                ImGui::PushFont(title_font, title_font->LegacySize * 1.4F);
                ImGui::PushStyleColor(ImGuiCol_Text, qui::color::active_palette().Highlight);
                ImGui::TextUnformatted("dconstruct");
                ImGui::PopStyleColor();
                ImGui::PopFont();
            }

            ImGui::PushStyleColor(ImGuiCol_Text, qui::color::active_palette().TextDisabled);
            ImGui::Text("Version %s", VERSION);
            ImGui::Text("Built %s", BUILD_DATE);
            ImGui::PopStyleColor();

            ImGui::Dummy(ImVec2(0.0F, 6.0F));
            ImGui::Separator();
            ImGui::Dummy(ImVec2(0.0F, 6.0F));

            ImGui::TextUnformatted("Created by");
            ImGui::SameLine();
            if (ImGui::TextLink("qntm")) {
                open_url(GITHUB_URL);
            }

            ImGui::Dummy(ImVec2(0.0F, 6.0F));
            ImGui::Separator();
            ImGui::Dummy(ImVec2(0.0F, 6.0F));

            if (ImFont* heading_font = qui::font_semi_bold(); heading_font != nullptr) {
                ImGui::PushFont(heading_font);
                ImGui::TextUnformatted("Special thanks");
                ImGui::PopFont();
            } else {
                ImGui::TextUnformatted("Special thanks");
            }
            ImGui::Dummy(ImVec2(0.0F, 2.0F));

            struct credit {
                const char* name;
                const char* role;
            };
            constexpr credit credits[] = {
                {"Chandler Threepwood", "beta testing"},
                {"Speclizer", "the DC-Tool"},
                {"uxh", "DC-file knowledge"},
                {"icemesh", "DC structs"},
                {"BigDragon", "beta testing"},
            };

            ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(16.0F, 5.0F));
            if (ImGui::BeginTable("##about_thanks", 2, ImGuiTableFlags_SizingFixedFit)) {
                for (const credit& entry : credits) {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::PushStyleColor(ImGuiCol_Text, qui::color::active_palette().Highlight);
                    ImGui::TextUnformatted(entry.name);
                    ImGui::PopStyleColor();
                    ImGui::TableSetColumnIndex(1);
                    ImGui::PushStyleColor(ImGuiCol_Text, qui::color::active_palette().TextDisabled);
                    ImGui::TextUnformatted(entry.role);
                    ImGui::PopStyleColor();
                }
                ImGui::EndTable();
            }
            ImGui::PopStyleVar();

            ImGui::Dummy(ImVec2(0.0F, 12.0F));
            constexpr f32 button_width = 110.0F;
            ImGui::SetCursorPosX((ImGui::GetWindowSize().x - button_width) * 0.5F);
            if (ImGui::Button("Close", ImVec2(button_width, 0.0F))) {
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

        ImGui::PopStyleColor();
        ImGui::PopStyleVar(3);
    }

    ImVec2 view_switch_size();
    bool draw_view_switch(const char* str_id, bool* dcpl, const ImVec2& size);
    ImVec2 toggle_switch_size();
    bool draw_toggle_switch(const char* str_id, bool* value);

    struct scheme_entry {
        const char* key;
        const char* label;
    };
    const std::vector<scheme_entry>& scheme_list();
    void set_active_scheme(const std::string& name);

    void draw_setting_description(const char* text) {
        ImGui::PushStyleColor(ImGuiCol_Text, qui::color::active_palette().TextDisabled);
        ImGui::PushTextWrapPos(ImGui::GetContentRegionMax().x);
        ImGui::TextUnformatted(text);
        ImGui::PopTextWrapPos();
        ImGui::PopStyleColor();
    }

    void draw_settings_popup(app_state& state, bool open_requested) {
        if (open_requested) {
            ImGui::OpenPopup("##dconstruct_settings");
        }

        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->GetCenter(), ImGuiCond_Always, ImVec2(0.5F, 0.5F));

        constexpr f32 settings_width = 900.0F;
        ImGui::SetNextWindowSizeConstraints(ImVec2(settings_width, 0.0F), ImVec2(settings_width, FLT_MAX));

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(28.0F, 24.0F));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.0F);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0F, 8.0F));
        ImGui::PushStyleColor(ImGuiCol_PopupBg, qui::color::active_palette().WindowBackground);

        if (ImGui::BeginPopupModal(
            "##dconstruct_settings",
            nullptr,
            ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove
        )) {
            if (ImFont* title_font = qui::font_bold(); title_font != nullptr) {
                ImGui::PushFont(title_font, title_font->LegacySize * 1.4F);
                ImGui::PushStyleColor(ImGuiCol_Text, qui::color::active_palette().Highlight);
                ImGui::TextUnformatted("Settings");
                ImGui::PopStyleColor();
                ImGui::PopFont();
            }

            ImGui::Dummy(ImVec2(0.0F, 6.0F));
            ImGui::Separator();
            ImGui::Dummy(ImVec2(0.0F, 6.0F));

            const ImVec2 sw_size = view_switch_size();
            const ImVec2 toggle_size = toggle_switch_size();
            bool optimizations_changed = false;

            const auto section_header = [](const char* text) {
                if (ImFont* section_font = qui::font_semi_bold(); section_font != nullptr) {
                    ImGui::PushFont(section_font);
                    ImGui::TextUnformatted(text);
                    ImGui::PopFont();
                } else {
                    ImGui::TextUnformatted(text);
                }
                ImGui::Dummy(ImVec2(0.0F, 6.0F));
            };

            const auto draw_optimization_toggle = [&](const char* label, const char* id, bool& value, const char* summary) {
                ImGui::TextUnformatted(label);
                ImGui::SameLine();
                ImGui::SetCursorPosX(ImGui::GetContentRegionMax().x - toggle_size.x);
                if (draw_toggle_switch(id, &value)) {
                    ImGui::MarkIniSettingsDirty();
                    optimizations_changed = true;
                }
                draw_setting_description(summary);
                ImGui::Dummy(ImVec2(0.0F, 6.0F));
            };

            const f32 column_gap = 28.0F;
            const f32 column_width = (ImGui::GetContentRegionAvail().x - column_gap) * 0.5F;

            ImGui::BeginChild("##settings_col_left", ImVec2(column_width, 0.0F), ImGuiChildFlags_AutoResizeY);

            section_header("General");
            ImGui::TextUnformatted("Default function view");
            ImGui::SameLine();
            ImGui::SetCursorPosX(ImGui::GetContentRegionMax().x - sw_size.x);
            if (draw_view_switch("##settings_default_view", &state.m_defaultViewDcpl, sw_size)) {
                ImGui::MarkIniSettingsDirty();
            }

            ImGui::Dummy(ImVec2(0.0F, 14.0F));
            section_header("Color theme");

            const auto label_for = [](const std::string& key) -> const char* {
                for (const scheme_entry& entry : scheme_list()) {
                    if (key == entry.key) {
                        return entry.label;
                    }
                }
                return key.c_str();
            };

            ImGui::SetNextItemWidth(-FLT_MIN);
            if (ImGui::BeginCombo("##settings_color_theme", label_for(state.m_colorScheme))) {
                for (const scheme_entry& entry : scheme_list()) {
                    const bool selected = state.m_colorScheme == entry.key;
                    if (ImGui::Selectable(entry.label, selected)) {
                        state.m_colorScheme = entry.key;
                        ImGui::MarkIniSettingsDirty();
                    }
                    if (ImGui::IsItemHovered()) {
                        state.m_previewScheme = entry.key;
                        set_active_scheme(entry.key);
                    }
                    if (selected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            draw_setting_description(
                "Colors used for the decompiled code, the value tree and syntax highlighting. "
                "Hover a theme to preview it instantly; the choice is saved when you click.");

            ImGui::EndChild();

            ImGui::SameLine(0.0F, column_gap);

            ImGui::BeginChild("##settings_col_right", ImVec2(column_width, 0.0F), ImGuiChildFlags_AutoResizeY);

            section_header("Decompilation optimizations");

            bool all_optimizations = all_decompilation_optimizations_enabled(state);
            ImGui::TextUnformatted("All optimizations");
            ImGui::SameLine();
            ImGui::SetCursorPosX(ImGui::GetContentRegionMax().x - toggle_size.x);
            if (draw_toggle_switch("##settings_opt_all", &all_optimizations)) {
                set_all_decompilation_optimizations(state, all_optimizations);
                ImGui::MarkIniSettingsDirty();
                optimizations_changed = true;
            }
            draw_setting_description("Enables every AST cleanup pass below.");
            ImGui::Dummy(ImVec2(0.0F, 6.0F));

            draw_optimization_toggle("SSO var", "##settings_opt_sso_var", state.m_ssoVarOptimization, "Removes short-lived temporary variables.");
            draw_optimization_toggle("Foreach", "##settings_opt_foreach", state.m_foreachOptimization, "Rewrites counted iterator loops into foreach loops.");
            draw_optimization_toggle("Match", "##settings_opt_match", state.m_matchOptimization, "Collapses repeated condition branches into match expressions.");
            draw_optimization_toggle("Second var", "##settings_opt_second_var", state.m_secondVarOptimization, "Runs temporary-variable cleanup after foreach and match rewrites.");
            draw_optimization_toggle("Member access", "##settings_opt_member_access", state.m_memberAccessOptimization, "Turns typed pointer offsets into member access.");
            draw_optimization_toggle("Regex", "##settings_opt_regex", state.m_regexOptimization, "Runs AST-hosted pattern rewrites such as boxed values and foreach cleanup.");

            ImGui::EndChild();

            if (optimizations_changed) {
                for (document& doc : state.m_documents) {
                    decompile_document(state, doc);
                }
            }

            ImGui::Dummy(ImVec2(0.0F, 12.0F));
            constexpr f32 button_width = 110.0F;
            ImGui::SetCursorPosX((ImGui::GetWindowSize().x - button_width) * 0.5F);
            if (ImGui::Button("Close", ImVec2(button_width, 0.0F))) {
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

        ImGui::PopStyleColor();
        ImGui::PopStyleVar(3);
    }

    f32 draw_title_menu_bar(app_state& state, GLFWwindow* window) {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        const ImVec2 pos = viewport->Pos;

        qui::WindowBarState& bar_state = qui::default_window_bar_state();

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
        ImGui::PushStyleColor(ImGuiCol_WindowBg, qui::color::active_palette().MenuBarBackground);
        ImGui::PushStyleColor(ImGuiCol_MenuBarBg, qui::color::active_palette().MenuBarBackground);
        ImGui::Begin("##dconstruct_title_menu_bar", nullptr, flags);

        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        const ImVec2 bar_min = ImGui::GetWindowPos();
        const ImVec2 bar_max(bar_min.x + ImGui::GetWindowWidth(), bar_min.y + height);

        draw_list->AddLine(
            ImVec2(bar_min.x, bar_max.y - 1.0F),
            ImVec2(bar_max.x, bar_max.y - 1.0F),
            ImGui::ColorConvertFloat4ToU32(qui::color::active_palette().Border)
        );

        constexpr f32 logo_size = 16.0F;
        const ImVec2 logo_min(bar_min.x + 10.0F, bar_min.y + (height - logo_size) * 0.5F);
        const ImVec2 logo_max(logo_min.x + logo_size, logo_min.y + logo_size);
        if (state.m_iconTexture != 0) {
            draw_list->AddImage(static_cast<ImTextureID>(state.m_iconTexture), logo_min, logo_max);
        }

        const f32 buttons_total = WINDOW_BUTTON_WIDTH * 3.0F;
        const f32 minimize_x = bar_max.x - buttons_total;

        bool about_clicked = false;
        bool settings_clicked = false;
        if (ImGui::BeginMenuBar()) {
            ImGui::SetCursorPosX(logo_max.x - bar_min.x + 8.0F);
            {
                ImFont* app_name_font = qui::font_medium();
                if (app_name_font != nullptr) {
                    ImGui::PushFont(app_name_font);
                }
                ImGui::PushStyleColor(ImGuiCol_Text, qui::color::active_palette().Highlight);
                ImGui::TextUnformatted("dconstruct");
                ImGui::PopStyleColor();
                if (app_name_font != nullptr) {
                    ImGui::PopFont();
                }
            }
            ImGui::SameLine(0.0F, 16.0F);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12.0F, 10.0F));
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.0F, 6.0F));
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10.0F, 6.0F));
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Load .bin File", "Ctrl+O")) {
                    const std::string path = prompt_open_file(window, "Binary files (*.bin)\0*.bin\0All files (*.*)\0*.*\0", "Open .bin file");
                    if (!path.empty()) {
                        load_bin_file(state, path);
                    }
                }
                if (ImGui::MenuItem("Load Sidbase")) {
                    const std::string path = prompt_open_file(window, "Binary files (*.bin)\0*.bin\0All files (*.*)\0*.*\0", "Open sidbase");
                    if (!path.empty()) {
                        load_sidbase(state, path);
                    }
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Parse type definition file")) {
                    const std::string path = prompt_open_file(window, "Type definition files (*.dcpl)\0*.dcpl\0All files (*.*)\0*.*\0", "Open type definition file");
                    if (!path.empty()) {
                        load_type_defs(state, path);
                    }
                }
                if (ImGui::MenuItem("Load type map")) {
                    const std::string path = prompt_open_file(window, "Type map files (*.dcplmap)\0*.dcplmap\0All files (*.*)\0*.*\0", "Open type map file");
                    if (!path.empty()) {
                        load_type_map(state, path);
                    }
                }
                ImGui::Separator();
                {
                    document* save_doc = active_document(state);
                    const std::string save_label = save_doc != nullptr ? "Save " + save_doc->m_name : std::string("Save");
                    if (ImGui::MenuItem(save_label.c_str(), "Ctrl+S", false, save_doc != nullptr && save_doc->m_dirty)) {
                        save_active_document(state);
                    }
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Exit")) {
                    request_app_close(state);
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Edit")) {
                const document* edit_doc = active_document(state);
                const bool can_undo = edit_doc != nullptr && !edit_doc->m_undoStack.empty();
                const bool can_redo = edit_doc != nullptr && !edit_doc->m_redoStack.empty();
                if (ImGui::MenuItem("Undo", "Ctrl+Z", false, can_undo)) {
                    undo_active_document(state);
                }
                if (ImGui::MenuItem("Redo", "Ctrl+Y", false, can_redo)) {
                    redo_active_document(state);
                }
                ImGui::EndMenu();
            }
            if (ImGui::MenuItem("Settings")) {
                settings_clicked = true;
            }
            {
                const bool debugger_active = state.m_debugger.m_active;
                if (debugger_active) {
                    ImGui::PushStyleColor(ImGuiCol_Text, qui::color::active_palette().Highlight);
                }
                if (ImGui::MenuItem("Debugger")) {
                    state.m_debugger.m_active = !state.m_debugger.m_active;
                }
                if (debugger_active) {
                    ImGui::PopStyleColor();
                }
            }
            ImGui::PopStyleVar(3);

            const char* donate_label = "Donate";
            const f32 donate_width = ImGui::CalcTextSize(donate_label).x + ImGui::GetStyle().ItemSpacing.x * 2.0F;
            const char* about_label = "About";
            const f32 about_width = ImGui::CalcTextSize(about_label).x + ImGui::GetStyle().ItemSpacing.x * 2.0F;

            ImGui::SetCursorPosX((minimize_x - bar_min.x) - donate_width - about_width);
            if (ImGui::MenuItem(about_label)) {
                about_clicked = true;
            }

            ImGui::SetCursorPosX((minimize_x - bar_min.x) - donate_width);
            ImGui::PushStyleColor(ImGuiCol_Text, qui::color::active_palette().Highlight);
            if (ImGui::MenuItem(donate_label)) {
                open_url(DONATE_URL);
            }
            ImGui::PopStyleColor();

            ImGui::EndMenuBar();
        }

        draw_about_popup(about_clicked);
        draw_settings_popup(state, settings_clicked);

        if (const document* doc = active_document(state); doc != nullptr && !doc->m_name.empty()) {
            const std::string title = doc->m_dirty ? doc->m_name + " \xE2\x97\x8F" : doc->m_name;
            ImFont* title_font = qui::font_medium();
            if (title_font != nullptr) {
                ImGui::PushFont(title_font);
            }
            const ImVec2 title_size = ImGui::CalcTextSize(title.c_str());
            const f32 title_font_size = ImGui::GetFontSize();
            if (title_font != nullptr) {
                ImGui::PopFont();
            }
            const ImVec2 title_pos(
                bar_min.x + (bar_max.x - bar_min.x - title_size.x) * 0.5F,
                bar_min.y + (height - title_size.y) * 0.5F
            );
            draw_list->AddText(
                title_font,
                title_font_size,
                title_pos,
                ImGui::ColorConvertFloat4ToU32(qui::color::active_palette().Text),
                title.c_str()
            );
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
            request_app_close(state);
        }

        qui::update_title_bar_drag(window, ImVec2(0.0F, 8.0F), ImVec2(minimize_x - bar_min.x, height), bar_state);

        ImGui::End();

        ImGui::PopStyleColor(2);
        ImGui::PopStyleVar(3);
        ImGui::PopStyleVar();

        return height;
    }

    void draw_status_text(const app_state& state) {
        if (state.m_sidbase == nullptr) {
            qui::status_label("No sidbase loaded \xE2\x80\x94 load one via File > Load Sidbase.", qui::color::active_palette().AccentRed);
        }

        if (!state.m_loadError.empty()) {
            qui::status_label(state.m_loadError.c_str(), qui::color::active_palette().AccentRed);
        }
    }

    void draw_empty_state(const app_state& state) {
        const ImVec2 region_min = ImGui::GetCursorScreenPos();
        const ImVec2 available = ImGui::GetContentRegionAvail();
        if (available.x <= 1.0F || available.y <= 1.0F) {
            return;
        }
        const ImVec2 center(region_min.x + available.x * 0.5F, region_min.y + available.y * 0.5F);
        ImDrawList* draw_list = ImGui::GetWindowDrawList();

        constexpr f32 art_gap = 22.0F;
        constexpr f32 line_gap = 10.0F;
        const char* primary = "Drop a .bin file here or use 'File \xE2\x86\x92 Load .bin file' to start editing";

        ImFont* art_font = qui::font_medium();
        auto measure_art = [&](const std::string& text, f32& out_font_size) -> ImVec2 {
            out_font_size = ImGui::GetFontSize();
            if (text.empty()) {
                return ImVec2(0.0F, 0.0F);
            }
            const char* begin = text.c_str();
            const char* end = begin + text.size();
            ImVec2 size = art_font != nullptr
                ? art_font->CalcTextSizeA(out_font_size, FLT_MAX, 0.0F, begin, end)
                : ImGui::CalcTextSize(begin, end);
            const f32 max_width = std::max(available.x - 48.0F, 1.0F);
            const f32 max_height = std::max(available.y * 0.75F, 1.0F);
            f32 scale = 1.0F;
            if (size.x > max_width) {
                scale = std::min(scale, max_width / size.x);
            }
            if (size.y > max_height) {
                scale = std::min(scale, max_height / size.y);
            }
            if (scale < 1.0F) {
                out_font_size = std::max(4.0F, out_font_size * scale);
                if (art_font != nullptr) {
                    size = art_font->CalcTextSizeA(out_font_size, FLT_MAX, 0.0F, begin, end);
                }
            }
            return size;
        };

        f32 art_font_size = 0.0F;
        const ImVec2 art_size = measure_art(state.m_nameArt, art_font_size);
        f32 wordmark_font_size = 0.0F;
        const ImVec2 wordmark_size = measure_art(state.m_wordmarkArt, wordmark_font_size);

        ImFont* primary_font = qui::font_semi_bold();
        if (primary_font != nullptr) {
            ImGui::PushFont(primary_font);
        }
        const ImVec2 primary_size = ImGui::CalcTextSize(primary);
        const f32 primary_font_size = ImGui::GetFontSize() * 1.5f;
        if (primary_font != nullptr) {
            ImGui::PopFont();
        }

        std::string secondary;
        if (state.m_sidbase == nullptr) {
            secondary = "No sidbase loaded \xE2\x80\x94 load one via File \xE2\x86\x92 Load Sidbase.";
        } else if (!state.m_loadError.empty()) {
            secondary = state.m_loadError;
        }
        const ImVec2 secondary_size = secondary.empty() ? ImVec2(0.0F, 0.0F) : ImGui::CalcTextSize(secondary.c_str());

        constexpr f32 logo_gap = 6.0F;
        const bool has_logo = !state.m_nameArt.empty() || !state.m_wordmarkArt.empty();

        f32 block_height = primary_size.y;
        if (!state.m_nameArt.empty()) {
            block_height += art_size.y;
        }
        if (!state.m_nameArt.empty() && !state.m_wordmarkArt.empty()) {
            block_height += logo_gap;
        }
        if (!state.m_wordmarkArt.empty()) {
            block_height += wordmark_size.y;
        }
        if (has_logo) {
            block_height += art_gap;
        }
        if (!secondary.empty()) {
            block_height += line_gap + secondary_size.y;
        }

        f32 y = center.y - block_height * 0.5F - available.y * 0.04F;
        if (!state.m_nameArt.empty()) {
            constexpr f32 stretch_x = 1.15F;
            const int vtx_start = draw_list->VtxBuffer.Size;
            draw_list->AddText(
                art_font,
                art_font_size,
                ImVec2(center.x - art_size.x * 0.5F, y),
                ImGui::ColorConvertFloat4ToU32(qui::color::active_palette().Highlight),
                state.m_nameArt.c_str(),
                state.m_nameArt.c_str() + state.m_nameArt.size()
            );
            for (int i = vtx_start; i < draw_list->VtxBuffer.Size; ++i) {
                ImDrawVert& vert = draw_list->VtxBuffer[i];
                vert.pos.x = center.x + (vert.pos.x - center.x) * stretch_x;
            }
            y += art_size.y;
            if (!state.m_wordmarkArt.empty()) {
                y += logo_gap;
            }
        }

        if (!state.m_wordmarkArt.empty()) {
            const char* wordmark_begin = state.m_wordmarkArt.c_str();
            const char* wordmark_end = wordmark_begin + state.m_wordmarkArt.size();
            draw_list->AddText(
                art_font,
                wordmark_font_size,
                ImVec2(center.x - wordmark_size.x * 0.5F, y),
                ImGui::ColorConvertFloat4ToU32(qui::color::active_palette().Highlight),
                wordmark_begin,
                wordmark_end
            );
            y += wordmark_size.y;
        }

        if (has_logo) {
            y += art_gap;
        }

        draw_list->AddText(
            primary_font,
            primary_font_size,
            ImVec2(center.x - primary_size.x * 0.75F, y),
            ImGui::ColorConvertFloat4ToU32(qui::color::active_palette().TextDisabled),
            primary
        );
        y += primary_size.y;

        if (!secondary.empty()) {
            y += line_gap;
            draw_list->AddText(
                ImVec2(center.x - secondary_size.x * 0.5F, y),
                ImGui::ColorConvertFloat4ToU32(qui::color::active_palette().AccentRed),
                secondary.c_str()
            );
            y += secondary_size.y;
        }
    }

    void draw_drag_overlay(const app_state& state, f32 top_offset) {
        if (!state.m_dragHover) {
            return;
        }
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        const ImVec2 area_min(viewport->Pos.x, viewport->Pos.y + top_offset);
        const ImVec2 area_max(viewport->Pos.x + viewport->Size.x, viewport->Pos.y + viewport->Size.y);
        ImDrawList* draw_list = ImGui::GetForegroundDrawList();

        draw_list->AddRectFilled(area_min, area_max, ImGui::ColorConvertFloat4ToU32(qui::color::rgba(0x42, 0x96, 0xF9, 0x29)));

        constexpr f32 inset = 14.0F;
        const ImVec2 border_min(area_min.x + inset, area_min.y + inset);
        const ImVec2 border_max(area_max.x - inset, area_max.y - inset);
        draw_list->AddRect(
            border_min,
            border_max,
            ImGui::ColorConvertFloat4ToU32(qui::color::rgba(0x42, 0x96, 0xF9, 0xE5)),
            10.0F,
            0,
            3.0F
        );

        const char* text = "Drop to open .bin file";
        ImFont* font = qui::font_bold();
        if (font != nullptr) {
            ImGui::PushFont(font);
        }
        const ImVec2 text_size = ImGui::CalcTextSize(text);
        const f32 font_size = ImGui::GetFontSize();
        if (font != nullptr) {
            ImGui::PopFont();
        }
        const ImVec2 center((area_min.x + area_max.x) * 0.5F, (area_min.y + area_max.y) * 0.5F);
        draw_list->AddText(
            font,
            font_size,
            ImVec2(center.x - text_size.x * 0.5F, center.y - text_size.y * 0.5F),
            ImGui::ColorConvertFloat4ToU32(qui::color::active_palette().Text),
            text
        );
    }

    bool begin_labeled_table_frame(const char* label, ImVec2& table_size) {
        const ImGuiStyle& style = ImGui::GetStyle();
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        const ImVec2 start_pos = ImGui::GetCursorScreenPos();
        const ImVec2 available_size = ImGui::GetContentRegionAvail();

        ImFont* font = qui::font_bold();
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

        const ImU32 border = ImGui::ColorConvertFloat4ToU32(qui::color::active_palette().TableBorderStrong);
        const ImU32 background = ImGui::ColorConvertFloat4ToU32(qui::color::active_palette().WindowBackground);
        draw_list->AddRect(frame_min, frame_max, border, style.ChildRounding, 0, 1.0F);

        const f32 label_x = frame_min.x + style.WindowPadding.x + 8.0F;
        const f32 label_y = frame_min.y - text_size.y * 0.45F;
        const ImVec2 bg_min(label_x - 5.0F, label_y - 2.0F);
        const ImVec2 bg_max(label_x + text_size.x + 7.0F, label_y + text_size.y + 2.0F);
        draw_list->AddRectFilled(bg_min, bg_max, background);

        if (font != nullptr) {
            ImGui::PushFont(font);
        }
        draw_list->AddText(ImVec2(label_x, label_y), ImGui::ColorConvertFloat4ToU32(qui::color::active_palette().Highlight), label);
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

    void draw_entry_list(app_state& state, document& doc) {
        ImVec2 table_size;
        if (!begin_labeled_table_frame("Entries", table_size)) {
            return;
        }

        if (doc.m_entries == nullptr || state.m_sidbase == nullptr || doc.m_file == nullptr) {
            qui::text_label("Not disassembled.");
            return;
        }

        const std::vector<disassembled_entry>& entries = *doc.m_entries;

        constexpr ImGuiTableFlags table_flags =
            ImGuiTableFlags_BordersInnerV |
            ImGuiTableFlags_RowBg |
            ImGuiTableFlags_Resizable |
            ImGuiTableFlags_ScrollY |
            ImGuiTableFlags_SizingStretchProp;

        if (ImGui::BeginTable("##dconstruct_entries", 2, table_flags, table_size)) {
            const char* entry_name_header = doc.m_entrySortColumn == 0
                                                ? (doc.m_entrySortDescending ? "Entry Name \xE2\x96\xBC" : "Entry Name \xE2\x96\xB2")
                                                : "Entry Name";
            const char* type_header = doc.m_entrySortColumn == 1
                                          ? (doc.m_entrySortDescending ? "Type \xE2\x96\xBC" : "Type \xE2\x96\xB2")
                                          : "Type";

            ImGui::TableSetupColumn(entry_name_header);
            ImGui::TableSetupColumn(type_header);
            ImGui::TableSetupScrollFreeze(0, 2);

            ImGui::TableNextRow();
            std::string new_name_search = doc.m_entrySearch;
            std::string new_type_search = doc.m_typeSearch;
            ImGui::TableSetColumnIndex(0);
            ImGui::SetNextItemWidth(-FLT_MIN);
            ImGui::InputTextWithHint("##dconstruct_entry_search", "Search entries", &new_name_search);
            ImGui::TableSetColumnIndex(1);
            ImGui::SetNextItemWidth(-FLT_MIN);
            ImGui::InputTextWithHint("##dconstruct_type_search", "Search types", &new_type_search);

            if (doc.m_entrySearch != new_name_search) {
                std::vector<std::string> choices;
                choices.reserve(entries.size());
                for (const disassembled_entry& entry : entries) {
                    choices.push_back(state.m_sidbase->lookup(entry.m_nameId, doc.m_file->m_sidCache));
                }
                // doc.m_currentMatches = qui::fuzzy_search(new_name_search, choices, 75.f);
                doc.m_currentMatches = qui::substring_search(new_name_search, choices);
                doc.m_entrySearch = std::move(new_name_search);
            }
            if (doc.m_typeSearch != new_type_search) {
                std::vector<std::string> choices;
                choices.reserve(entries.size());
                for (const disassembled_entry& entry : entries) {
                    choices.push_back(state.m_sidbase->lookup(entry.m_typeId, doc.m_file->m_sidCache));
                }
                // doc.m_typeMatches = qui::fuzzy_search(new_type_search, choices, 75.f);
                doc.m_typeMatches = qui::substring_search(new_type_search, choices);
                doc.m_typeSearch = std::move(new_type_search);
            }

            const bool name_filtering = !doc.m_entrySearch.empty();
            const bool type_filtering = !doc.m_typeSearch.empty();
            std::vector<i32> row_indices;
            if (name_filtering && type_filtering) {
                std::vector<char> in_type(entries.size(), 0);
                for (const qui::fuzzy_match& match : doc.m_typeMatches) {
                    if (match.index < in_type.size()) {
                        in_type[match.index] = 1;
                    }
                }
                row_indices.reserve(doc.m_currentMatches.size());
                for (const qui::fuzzy_match& match : doc.m_currentMatches) {
                    if (match.index < in_type.size() && in_type[match.index] != 0) {
                        row_indices.push_back(static_cast<i32>(match.index));
                    }
                }
            } else if (name_filtering) {
                row_indices.reserve(doc.m_currentMatches.size());
                for (const qui::fuzzy_match& match : doc.m_currentMatches) {
                    row_indices.push_back(static_cast<i32>(match.index));
                }
            } else if (type_filtering) {
                row_indices.reserve(doc.m_typeMatches.size());
                for (const qui::fuzzy_match& match : doc.m_typeMatches) {
                    row_indices.push_back(static_cast<i32>(match.index));
                }
            } else {
                row_indices.reserve(entries.size());
                for (i32 i = 0; i < static_cast<i32>(entries.size()); ++i) {
                    row_indices.push_back(i);
                }
            }

            ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
            for (int column = 0; column < 2; ++column) {
                ImGui::TableSetColumnIndex(column);
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ENTRY_CELL_LEFT_PADDING);
                ImGui::TableHeader(ImGui::TableGetColumnName(column));
                if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
                    if (doc.m_entrySortColumn == column) {
                        doc.m_entrySortDescending = !doc.m_entrySortDescending;
                    } else {
                        doc.m_entrySortColumn = column;
                        doc.m_entrySortDescending = false;
                    }
                }
            }

            if (doc.m_entrySortColumn == 0 || doc.m_entrySortColumn == 1) {
                const bool sort_by_type = doc.m_entrySortColumn == 1;
                std::stable_sort(
                    row_indices.begin(),
                    row_indices.end(),
                    [&](const i32 lhs, const i32 rhs) {
                        const disassembled_entry& left = entries[static_cast<u32>(lhs)];
                        const disassembled_entry& right = entries[static_cast<u32>(rhs)];
                        const std::string left_key = sort_by_type
                            ? state.m_sidbase->lookup(left.m_typeId, doc.m_file->m_sidCache)
                            : state.m_sidbase->lookup(left.m_nameId, doc.m_file->m_sidCache);
                        const std::string right_key = sort_by_type
                            ? state.m_sidbase->lookup(right.m_typeId, doc.m_file->m_sidCache)
                            : state.m_sidbase->lookup(right.m_nameId, doc.m_file->m_sidCache);
                        if (doc.m_entrySortDescending) {
                            return right_key < left_key;
                        }
                        return left_key < right_key;
                    }
                );
            }

            ImGuiListClipper clipper;
            clipper.Begin(static_cast<int>(row_indices.size()));
            while (clipper.Step()) {
                for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i) {
                    const int entry_index = row_indices[static_cast<u32>(i)];
                    const disassembled_entry& entry = entries[static_cast<u32>(entry_index)];
                    const char* name = state.m_sidbase->lookup(entry.m_nameId, doc.m_file->m_sidCache);
                    const char* type = state.m_sidbase->lookup(entry.m_typeId, doc.m_file->m_sidCache);

                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ENTRY_CELL_LEFT_PADDING);

                    ImGui::PushID(entry_index);
                    if (ImFont* font = qui::font_bold()) {
                        ImGui::PushFont(font);
                    }
                    if (ImGui::Selectable(name, doc.m_selectedEntry == entry_index, ImGuiSelectableFlags_SpanAllColumns)) {
                        doc.m_selectedEntry = entry_index;
                    }
                    if (qui::font_bold() != nullptr) {
                        ImGui::PopFont();
                    }
                    ImGui::PopID();

                    ImGui::TableSetColumnIndex(1);
                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ENTRY_CELL_LEFT_PADDING);
                    if (ImFont* font = qui::font_medium()) {
                        ImGui::PushFont(font);
                    }
                    ImGui::PushStyleColor(ImGuiCol_Text, qui::color::active_palette().TextDisabled);
                    ImGui::TextUnformatted(type);
                    ImGui::PopStyleColor();
                    if (qui::font_medium() != nullptr) {
                        ImGui::PopFont();
                    }
                }
            }
            ImGui::EndTable();
        }
    }

    namespace val_color_default {
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
    } // namespace val_color_default

    // A color scheme fuses every color the UI can theme: a flat map of the
    // value-tree / code colors (code-window colors are the AST_COLOR set
    // prefixed with "CODE_") plus the chrome palette that drives the rest of
    // the ImGui UI (window backgrounds, buttons, tabs, title bar, ...). Chrome
    // defaults to retina_dark so a scheme that leaves it untouched looks
    // identical to the original dark theme.
    struct color_scheme {
        std::unordered_map<std::string, ImVec4> colors;
        qui::color::palette chrome = qui::color::make_retina_dark_palette();
    };

    const char* code_color_key(ast::AST_COLOR color) {
        switch (color) {
            case ast::AST_COLOR::BLANK:       return "CODE_BLANK";
            case ast::AST_COLOR::NUMBER:      return "CODE_NUMBER";
            case ast::AST_COLOR::SID:         return "CODE_SID";
            case ast::AST_COLOR::IDENTIFIER:  return "CODE_IDENTIFIER";
            case ast::AST_COLOR::MEMBER:      return "CODE_MEMBER";
            case ast::AST_COLOR::TYPE:        return "CODE_TYPE";
            case ast::AST_COLOR::CALL:        return "CODE_CALL";
            case ast::AST_COLOR::KEYWORD:     return "CODE_KEYWORD";
            case ast::AST_COLOR::STRING:      return "CODE_STRING";
            case ast::AST_COLOR::COMMENT:     return "CODE_COMMENT";
            case ast::AST_COLOR::OPERATOR:    return "CODE_OPERATOR";
            case ast::AST_COLOR::PUNCTUATION: return "CODE_PUNCTUATION";
        }
        return "CODE_BLANK";
    }

    struct scheme_def {
        ImVec4 int_zero, integer, floating, sid, string, array, map, structure;
        ImVec4 function, state_script, entry_name, group;
        ImVec4 code_bg, code_gutter_bg, code_gutter_text, code_current_line;
        ImVec4 c_blank, c_number, c_sid, c_identifier, c_member, c_type;
        ImVec4 c_call, c_keyword, c_string, c_comment, c_operator, c_punctuation;
    };

    color_scheme make_scheme(const scheme_def& d) {
        color_scheme scheme;
        scheme.colors = {
            {"IntZero", d.int_zero}, {"Int", d.integer}, {"Float", d.floating}, {"Sid", d.sid},
            {"String", d.string}, {"Array", d.array}, {"Map", d.map}, {"Struct", d.structure},
            {"Function", d.function}, {"StateScript", d.state_script}, {"EntryName", d.entry_name}, {"Group", d.group},
            {"CODE_BG", d.code_bg}, {"CODE_GUTTER_BG", d.code_gutter_bg},
            {"CODE_GUTTER_TEXT", d.code_gutter_text}, {"CODE_CURRENT_LINE", d.code_current_line},
            {"CODE_BLANK", d.c_blank}, {"CODE_NUMBER", d.c_number}, {"CODE_SID", d.c_sid},
            {"CODE_IDENTIFIER", d.c_identifier}, {"CODE_MEMBER", d.c_member}, {"CODE_TYPE", d.c_type},
            {"CODE_CALL", d.c_call}, {"CODE_KEYWORD", d.c_keyword}, {"CODE_STRING", d.c_string},
            {"CODE_COMMENT", d.c_comment}, {"CODE_OPERATOR", d.c_operator}, {"CODE_PUNCTUATION", d.c_punctuation},
        };
        return scheme;
    }

    // Builds a full chrome palette from a handful of semantic base colors so a
    // theme only needs to specify its identity colors; the alpha-blended
    // interactive states are derived here, mirroring the structure of the
    // retina_dark palette.
    struct chrome_def {
        ImVec4 window_bg, panel, panel_raised, menubar, title;
        ImVec4 text, text_dim, border;
        ImVec4 accent, accent_strong, highlight, danger, warning;
    };

    qui::color::palette make_chrome(const chrome_def& d) {
        const auto a = [](ImVec4 v, int alpha) {
            v.w = static_cast<float>(alpha) / 255.0F;
            return v;
        };
        qui::color::palette p;
        p.WindowBackground = d.window_bg;
        p.Panel = d.panel;
        p.PanelRaised = d.panel_raised;
        p.PopupBackground = d.panel;
        p.MenuBarBackground = d.menubar;
        p.TitleBackground = d.title;
        p.Text = d.text;
        p.TextDisabled = d.text_dim;
        p.Border = a(d.border, 0x7F);
        p.Button = a(d.accent, 0x66);
        p.ButtonHovered = d.accent;
        p.ButtonActive = d.accent_strong;
        p.FrameBackground = a(d.accent, 0x40);
        p.FrameBackgroundHovered = a(d.accent, 0x66);
        p.FrameBackgroundActive = a(d.accent, 0xAA);
        p.Header = a(d.accent, 0x4F);
        p.HeaderHovered = a(d.accent, 0xCC);
        p.HeaderActive = d.accent;
        p.ScrollbarBackground = a(d.window_bg, 0x87);
        p.ScrollbarGrab = a(d.text_dim, 0x80);
        p.ScrollbarGrabHovered = a(d.text_dim, 0xB0);
        p.ScrollbarGrabActive = d.text_dim;
        p.SeparatorActive = d.accent;
        p.SeparatorHovered = a(d.accent, 0xC6);
        p.SliderGrab = a(d.accent, 0xCC);
        p.SliderGrabActive = d.accent;
        p.Tab = a(d.accent, 0x88);
        p.TabActive = d.accent_strong;
        p.TabHovered = a(d.accent, 0xCC);
        p.TabUnfocused = d.panel;
        p.TabUnfocusedActive = d.panel_raised;
        p.TableBorderStrong = a(d.border, 0xFF);
        p.TableBorderLight = a(d.border, 0x80);
        p.TableHeaderBackground = d.panel_raised;
        p.TableRowBackground = qui::color::rgba(0x00, 0x00, 0x00, 0x00);
        p.TableRowBackgroundAlt = a(d.text, 0x12);
        p.TextSelectedBackground = a(d.accent, 0x59);
        p.AccentBlue = d.accent_strong;
        p.AccentGreen = d.highlight;
        p.AccentPurple = d.accent;
        p.AccentRed = d.danger;
        p.AccentYellow = d.warning;
        p.Highlight = d.highlight;
        return p;
    }

    color_scheme make_scheme(const scheme_def& d, const chrome_def& c) {
        color_scheme scheme = make_scheme(d);
        scheme.chrome = make_chrome(c);
        return scheme;
    }

    const std::unordered_map<std::string, color_scheme>& color_schemes() {
        using qui::color::rgba;
        static const std::unordered_map<std::string, color_scheme> schemes = {
            {"qntm", make_scheme({
                .int_zero = rgba(0x7F, 0x7F, 0x7F), .integer = rgba(0x6F, 0xB8, 0xE8),
                .floating = rgba(0xE5, 0xC0, 0x7B), .sid = rgba(0xC2, 0x9E, 0xF0),
                .string = rgba(0xAE, 0x33, 0x44), .array = rgba(0x9C, 0xD6, 0x8E),
                .map = rgba(0x5C, 0xB8, 0xD6), .structure = rgba(0xCF, 0xCF, 0xD4),
                .function = rgba(0x4D, 0xC6, 0x9B), .state_script = rgba(0xF1, 0xC4, 0x0F),
                .entry_name = rgba(0xF1, 0xC4, 0x0F), .group = rgba(0xB0, 0xB0, 0xB8),
                .code_bg = rgba(0x18, 0x18, 0x18), .code_gutter_bg = rgba(0x0F, 0x0F, 0x0F),
                .code_gutter_text = rgba(0x7F, 0x7F, 0x7F), .code_current_line = rgba(0xFF, 0xFF, 0xFF, 0x0D),
                .c_blank = rgba(0xCC, 0xCC, 0xCC), .c_number = rgba(0xB5, 0xCE, 0xA8),
                .c_sid = rgba(0x4F, 0xC1, 0xFF), .c_identifier = rgba(0x9C, 0xDC, 0xFE),
                .c_member = rgba(0x9C, 0xDC, 0xFE), .c_type = rgba(0x4E, 0xC9, 0xB0),
                .c_call = rgba(0xDC, 0xDC, 0xAA), .c_keyword = rgba(0xC5, 0x86, 0xC0),
                .c_string = rgba(0xAE, 0x33, 0x44), .c_comment = rgba(0x6A, 0x99, 0x55),
                .c_operator = rgba(0xD4, 0xD4, 0xD4), .c_punctuation = rgba(0xCC, 0xCC, 0xCC),
            })},
            {"qntm-light", make_scheme({
                .int_zero = rgba(0x7F, 0x7F, 0x7F), .integer = rgba(0x6F, 0xB8, 0xE8),
                .floating = rgba(0xE5, 0xC0, 0x7B), .sid = rgba(0xC2, 0x9E, 0xF0),
                .string = rgba(0xE2, 0x8C, 0x6E), .array = rgba(0x9C, 0xD6, 0x8E),
                .map = rgba(0x5C, 0xB8, 0xD6), .structure = rgba(0xCF, 0xCF, 0xD4),
                .function = rgba(0x4D, 0xC6, 0x9B), .state_script = rgba(0xF1, 0xC4, 0x0F),
                .entry_name = rgba(0xF1, 0xC4, 0x0F), .group = rgba(0xB0, 0xB0, 0xB8),
                .code_bg = rgba(0xFF, 0xFF, 0xFF), .code_gutter_bg = rgba(0xF3, 0xF3, 0xF3),
                .code_gutter_text = rgba(0x23, 0x78, 0x93), .code_current_line = rgba(0x00, 0x00, 0x00, 0x0D),
                .c_blank = rgba(0x00, 0x00, 0x00), .c_number = rgba(0x09, 0x86, 0x58),
                .c_sid = rgba(0x00, 0x70, 0xC1), .c_identifier = rgba(0x00, 0x10, 0x80),
                .c_member = rgba(0x00, 0x10, 0x80), .c_type = rgba(0x26, 0x7F, 0x99),
                .c_call = rgba(0x79, 0x5E, 0x26), .c_keyword = rgba(0x00, 0x00, 0xFF),
                .c_string = rgba(0xAE, 0x33, 0x44), .c_comment = rgba(0x00, 0x80, 0x00),
                .c_operator = rgba(0x00, 0x00, 0x00), .c_punctuation = rgba(0x00, 0x00, 0x00),
            }, {
                .window_bg = rgba(0xF3, 0xF3, 0xF3), .panel = rgba(0xFF, 0xFF, 0xFF),
                .panel_raised = rgba(0xEC, 0xEC, 0xEC), .menubar = rgba(0xDD, 0xDD, 0xDD),
                .title = rgba(0xE5, 0xE5, 0xE5), .text = rgba(0x1E, 0x1E, 0x1E),
                .text_dim = rgba(0x6E, 0x6E, 0x6E), .border = rgba(0xC8, 0xC8, 0xC8),
                .accent = rgba(0x00, 0x78, 0xD4), .accent_strong = rgba(0x00, 0x5A, 0x9E),
                .highlight = rgba(0x11, 0x63, 0x29), .danger = rgba(0xD1, 0x34, 0x38),
                .warning = rgba(0xBF, 0x88, 0x03),
            })},
            {"ghidra", make_scheme({
                .int_zero = rgba(0x78, 0x78, 0x78), .integer = rgba(0x70, 0xC0, 0x70),
                .floating = rgba(0x70, 0xC0, 0x70), .sid = rgba(0x00, 0x8B, 0x8B),
                .string = rgba(0xF0, 0x80, 0x80), .array = rgba(0x93, 0x70, 0xDB),
                .map = rgba(0x87, 0xCE, 0xFA), .structure = rgba(0xAD, 0xD8, 0xE6),
                .function = rgba(0x00, 0x8B, 0x8B), .state_script = rgba(0xFF, 0xA0, 0x70),
                .entry_name = rgba(0xFF, 0xA0, 0x70), .group = rgba(0x78, 0x78, 0x78),
                .code_bg = rgba(0x2B, 0x2B, 0x2B), .code_gutter_bg = rgba(0x31, 0x33, 0x35),
                .code_gutter_text = rgba(0x60, 0x63, 0x66), .code_current_line = rgba(0xFF, 0xFF, 0xFF, 0x14),
                .c_blank = rgba(0xBB, 0xBB, 0xBB), .c_number = rgba(0x70, 0xC0, 0x70),
                .c_sid = rgba(0x00, 0x8B, 0x8B), .c_identifier = rgba(0xC0, 0xC0, 0x80),
                .c_member = rgba(0xC0, 0xC0, 0x80), .c_type = rgba(0x87, 0xCE, 0xFA),
                .c_call = rgba(0x00, 0x8B, 0x8B), .c_keyword = rgba(0x87, 0xCE, 0xFA),
                .c_string = rgba(0x70, 0xC0, 0x70), .c_comment = rgba(0xEE, 0x82, 0xEE),
                .c_operator = rgba(0xBB, 0xBB, 0xBB), .c_punctuation = rgba(0xBB, 0xBB, 0xBB),
            }, {
                .window_bg = rgba(0x3C, 0x3F, 0x41), .panel = rgba(0x3C, 0x3F, 0x41),
                .panel_raised = rgba(0x4C, 0x50, 0x52), .menubar = rgba(0x3C, 0x3F, 0x41),
                .title = rgba(0x3C, 0x3F, 0x41), .text = rgba(0xBB, 0xBB, 0xBB),
                .text_dim = rgba(0x80, 0x80, 0x80), .border = rgba(0x55, 0x55, 0x55),
                .accent = rgba(0x3D, 0x5B, 0x91), .accent_strong = rgba(0x2C, 0x44, 0x70),
                .highlight = rgba(0x70, 0xC0, 0x70), .danger = rgba(0xF0, 0x80, 0x80),
                .warning = rgba(0xFF, 0xA0, 0x70),
            })},
            {"ida", make_scheme({
                .int_zero = rgba(0x80, 0x80, 0x80), .integer = rgba(0xAB, 0x98, 0x70),
                .floating = rgba(0xAB, 0x98, 0x70), .sid = rgba(0xAB, 0x98, 0x70),
                .string = rgba(0xFF, 0x66, 0x20), .array = rgba(0xFF, 0xD2, 0x00),
                .map = rgba(0xFF, 0xD2, 0x00), .structure = rgba(0xFF, 0xD2, 0x00),
                .function = rgba(0xFF, 0xEC, 0xBB), .state_script = rgba(0xFF, 0xD2, 0x00),
                .entry_name = rgba(0xFF, 0xEC, 0xBB), .group = rgba(0x80, 0x80, 0x80),
                .code_bg = rgba(0x2D, 0x2D, 0x2D), .code_gutter_bg = rgba(0x26, 0x26, 0x26),
                .code_gutter_text = rgba(0x70, 0x70, 0x70), .code_current_line = rgba(0xFF, 0xFF, 0xFF, 0x14),
                .c_blank = rgba(0xC8, 0xC8, 0xC8), .c_number = rgba(0xC8, 0xC8, 0xC8),
                .c_sid = rgba(0xC8, 0xC8, 0xC8), .c_identifier = rgba(0x12, 0xFF, 0xFF),
                .c_member = rgba(0x12, 0xFF, 0xFF), .c_type = rgba(0xFF, 0xD2, 0x00),
                .c_call = rgba(0xFF, 0xEC, 0xBB), .c_keyword = rgba(0xC8, 0xC8, 0xC8),
                .c_string = rgba(0xFF, 0x66, 0x20), .c_comment = rgba(0x7F, 0x9F, 0x7F),
                .c_operator = rgba(0xC8, 0xC8, 0xC8), .c_punctuation = rgba(0xC8, 0xC8, 0xC8),
            }, {
                .window_bg = rgba(0x35, 0x35, 0x35), .panel = rgba(0x2D, 0x2D, 0x2D),
                .panel_raised = rgba(0x3C, 0x3C, 0x3C), .menubar = rgba(0x35, 0x35, 0x35),
                .title = rgba(0x35, 0x35, 0x35), .text = rgba(0xC8, 0xC8, 0xC8),
                .text_dim = rgba(0x80, 0x80, 0x80), .border = rgba(0x48, 0x48, 0x48),
                .accent = rgba(0x34, 0x5D, 0x8A), .accent_strong = rgba(0x26, 0x4A, 0x70),
                .highlight = rgba(0xFF, 0xD2, 0x00), .danger = rgba(0xD8, 0x6C, 0x6C),
                .warning = rgba(0xFF, 0xEC, 0xBB),
            })},
            {"monokai", make_scheme({
                .int_zero = rgba(0x75, 0x71, 0x5E), .integer = rgba(0xAE, 0x81, 0xFF),
                .floating = rgba(0xAE, 0x81, 0xFF), .sid = rgba(0x66, 0xD9, 0xEF),
                .string = rgba(0xE6, 0xDB, 0x74), .array = rgba(0xA6, 0xE2, 0x2E),
                .map = rgba(0x66, 0xD9, 0xEF), .structure = rgba(0xF8, 0xF8, 0xF2),
                .function = rgba(0xA6, 0xE2, 0x2E), .state_script = rgba(0xFD, 0x97, 0x1F),
                .entry_name = rgba(0xFD, 0x97, 0x1F), .group = rgba(0x75, 0x71, 0x5E),
                .code_bg = rgba(0x27, 0x28, 0x22), .code_gutter_bg = rgba(0x1E, 0x1F, 0x1C),
                .code_gutter_text = rgba(0x75, 0x71, 0x5E), .code_current_line = rgba(0xFF, 0xFF, 0xFF, 0x0D),
                .c_blank = rgba(0xF8, 0xF8, 0xF2), .c_number = rgba(0xAE, 0x81, 0xFF),
                .c_sid = rgba(0xAE, 0x81, 0xFF), .c_identifier = rgba(0xF8, 0xF8, 0xF2),
                .c_member = rgba(0xFD, 0x97, 0x1F), .c_type = rgba(0x66, 0xD9, 0xEF),
                .c_call = rgba(0xA6, 0xE2, 0x2E), .c_keyword = rgba(0xF9, 0x26, 0x72),
                .c_string = rgba(0xE6, 0xDB, 0x74), .c_comment = rgba(0x75, 0x71, 0x5E),
                .c_operator = rgba(0xF9, 0x26, 0x72), .c_punctuation = rgba(0xF8, 0xF8, 0xF2),
            }, {
                .window_bg = rgba(0x1E, 0x1F, 0x1C), .panel = rgba(0x27, 0x28, 0x22),
                .panel_raised = rgba(0x3E, 0x3D, 0x32), .menubar = rgba(0x1E, 0x1F, 0x1C),
                .title = rgba(0x1E, 0x1F, 0x1C), .text = rgba(0xF8, 0xF8, 0xF2),
                .text_dim = rgba(0x75, 0x71, 0x5E), .border = rgba(0x49, 0x48, 0x3E),
                .accent = rgba(0x66, 0xD9, 0xEF), .accent_strong = rgba(0x4E, 0xA8, 0xC0),
                .highlight = rgba(0xA6, 0xE2, 0x2E), .danger = rgba(0xF9, 0x26, 0x72),
                .warning = rgba(0xFD, 0x97, 0x1F),
            })},
            {"nord", make_scheme({
                .int_zero = rgba(0x4C, 0x56, 0x6A), .integer = rgba(0x88, 0xC0, 0xD0),
                .floating = rgba(0xB4, 0x8E, 0xAD), .sid = rgba(0x8F, 0xBC, 0xBB),
                .string = rgba(0xA3, 0xBE, 0x8C), .array = rgba(0xA3, 0xBE, 0x8C),
                .map = rgba(0x88, 0xC0, 0xD0), .structure = rgba(0xD8, 0xDE, 0xE9),
                .function = rgba(0x88, 0xC0, 0xD0), .state_script = rgba(0xEB, 0xCB, 0x8B),
                .entry_name = rgba(0xEB, 0xCB, 0x8B), .group = rgba(0x61, 0x6E, 0x88),
                .code_bg = rgba(0x2E, 0x34, 0x40), .code_gutter_bg = rgba(0x27, 0x2C, 0x36),
                .code_gutter_text = rgba(0x4C, 0x56, 0x6A), .code_current_line = rgba(0xFF, 0xFF, 0xFF, 0x0A),
                .c_blank = rgba(0xD8, 0xDE, 0xE9), .c_number = rgba(0xB4, 0x8E, 0xAD),
                .c_sid = rgba(0x8F, 0xBC, 0xBB), .c_identifier = rgba(0xD8, 0xDE, 0xE9),
                .c_member = rgba(0x88, 0xC0, 0xD0), .c_type = rgba(0x8F, 0xBC, 0xBB),
                .c_call = rgba(0x88, 0xC0, 0xD0), .c_keyword = rgba(0x81, 0xA1, 0xC1),
                .c_string = rgba(0xA3, 0xBE, 0x8C), .c_comment = rgba(0x61, 0x6E, 0x88),
                .c_operator = rgba(0x81, 0xA1, 0xC1), .c_punctuation = rgba(0xEC, 0xEF, 0xF4),
            }, {
                .window_bg = rgba(0x2E, 0x34, 0x40), .panel = rgba(0x3B, 0x42, 0x52),
                .panel_raised = rgba(0x43, 0x4C, 0x5E), .menubar = rgba(0x2E, 0x34, 0x40),
                .title = rgba(0x2E, 0x34, 0x40), .text = rgba(0xEC, 0xEF, 0xF4),
                .text_dim = rgba(0x6C, 0x7A, 0x96), .border = rgba(0x4C, 0x56, 0x6A),
                .accent = rgba(0x88, 0xC0, 0xD0), .accent_strong = rgba(0x5E, 0x81, 0xAC),
                .highlight = rgba(0xA3, 0xBE, 0x8C), .danger = rgba(0xBF, 0x61, 0x6A),
                .warning = rgba(0xEB, 0xCB, 0x8B),
            })},
        };
        return schemes;
    }

    const std::vector<scheme_entry>& scheme_list() {
        static const std::vector<scheme_entry> list = {
            {"qntm", "qntm (dark)"},
            {"qntm-light", "qntm light"},
            {"ghidra", "ghidra"},
            {"ida", "ida"},
            {"monokai", "monokai"},
            {"nord", "nord"},
        };
        return list;
    }

    const color_scheme& scheme_by_name(const std::string& name) {
        const auto& schemes = color_schemes();
        if (const auto it = schemes.find(name); it != schemes.end()) {
            return it->second;
        }
        return schemes.at("qntm");
    }

    // The scheme used to resolve colors for the frame currently being drawn.
    // It mirrors a single-threaded ImGui frame, so a process-wide pointer is
    // fine and lets the value tree resolve its colors without threading state
    // through every node. Hover-preview in Settings can repoint this for one
    // frame without changing the saved scheme.
    const color_scheme*& active_scheme() {
        static const color_scheme* ptr = nullptr;
        return ptr;
    }

    // Activates a scheme for the rest of the frame: repoints the resolver used
    // by the value tree / code window and pushes the scheme's chrome palette
    // into qui and the live ImGui style so the entire UI re-themes. Used both at
    // frame start (saved scheme) and on hover in Settings (preview).
    void set_active_scheme(const std::string& name) {
        const color_scheme& scheme = scheme_by_name(name);
        active_scheme() = &scheme;
        qui::color::active_palette() = scheme.chrome;
        qui::apply_palette_colors(qui::color::active_palette());
    }

    ImVec4 scheme_lookup(const char* key, const ImVec4& fallback) {
        if (const color_scheme* s = active_scheme(); s != nullptr) {
            if (const auto it = s->colors.find(key); it != s->colors.end()) {
                return it->second;
            }
        }
        return fallback;
    }

    namespace gcol {
        inline ImVec4 IntZero()     { return scheme_lookup("IntZero", val_color_default::IntZero); }
        inline ImVec4 Int()         { return scheme_lookup("Int", val_color_default::Int); }
        inline ImVec4 Float()       { return scheme_lookup("Float", val_color_default::Float); }
        inline ImVec4 Sid()         { return scheme_lookup("Sid", val_color_default::Sid); }
        inline ImVec4 String()      { return scheme_lookup("String", val_color_default::String); }
        inline ImVec4 Array()       { return scheme_lookup("Array", val_color_default::Array); }
        inline ImVec4 Map()         { return scheme_lookup("Map", val_color_default::Map); }
        inline ImVec4 Struct()      { return scheme_lookup("Struct", val_color_default::Struct); }
        inline ImVec4 Function()    { return scheme_lookup("Function", val_color_default::Function); }
        inline ImVec4 StateScript() { return scheme_lookup("StateScript", val_color_default::StateScript); }
        inline ImVec4 EntryName()   { return scheme_lookup("EntryName", val_color_default::EntryName); }
        inline ImVec4 Group()       { return scheme_lookup("Group", val_color_default::Group); }
    } // namespace gcol

    qui::code::theme scheme_code_theme() {
        qui::code::theme th = qui::code::default_theme();
        th.background = qui::code::to_u32(scheme_lookup("CODE_BG", qui::color::active_palette().Panel));
        th.gutter_background = qui::code::to_u32(scheme_lookup("CODE_GUTTER_BG", qui::color::active_palette().WindowBackground));
        th.gutter_text = qui::code::to_u32(scheme_lookup("CODE_GUTTER_TEXT", qui::color::active_palette().TextDisabled));
        th.current_line = qui::code::to_u32(scheme_lookup("CODE_CURRENT_LINE", qui::color::rgba(0xFF, 0xFF, 0xFF, 0x0D)));
        th.text = qui::code::to_u32(scheme_lookup("CODE_BLANK", qui::color::active_palette().Text));
        return th;
    }

    void render_decompiled_code(const char* id, const app_state&, const ast::code_color_buffer& code, qui::code::hovered_token* hovered = nullptr) {
        std::vector<qui::code::colored_span> spans;
        spans.reserve(code.size());
        for (const auto& [color, text] : code) {
            const ImVec4 resolved = scheme_lookup(code_color_key(color), qui::color::active_palette().Text);
            spans.push_back({qui::code::to_u32(resolved), text});
        }
        const qui::code::theme th = scheme_code_theme();
        qui::code::code_window_colored(id, spans, ImVec2(0.0F, 0.0F), th, hovered);
    }

    std::vector<std::string> known_type_names(const app_state& state) {
        std::vector<std::string> names = {
            "u8", "u16", "u32", "u64", "i8", "i16", "i32", "i64",
            "f32", "f64", "char", "bool", "string", "sid", "symbol", "u0"
        };
        for (const auto& [_, type] : state.m_typeMap) {
            std::visit(
                [&](auto&& value) {
                    using T = std::decay_t<decltype(value)>;
                    if constexpr (std::is_same_v<T, ast::struct_type> || std::is_same_v<T, ast::enum_type>) {
                        names.push_back(value.m_typeHash ? "#" + value.m_name : value.m_name);
                    }
                },
                type
            );
        }
        std::sort(names.begin(), names.end());
        names.erase(std::unique(names.begin(), names.end()), names.end());
        return names;
    }

    bool contains_case_insensitive(std::string_view haystack, std::string_view needle) {
        if (needle.empty()) {
            return true;
        }
        auto lower = [](char c) {
            return static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        };
        for (std::size_t start = 0; start + needle.size() <= haystack.size(); ++start) {
            bool match = true;
            for (std::size_t i = 0; i < needle.size(); ++i) {
                if (lower(haystack[start + i]) != lower(needle[i])) {
                    match = false;
                    break;
                }
            }
            if (match) {
                return true;
            }
        }
        return false;
    }

    bool local_var_index_from_token(const document& doc, sid64 function_id, std::string_view token, u64& var_index) {
        if (parse_generated_var_name(token, var_index)) {
            return true;
        }
        const auto scope_it = doc.m_functionScopes.find(function_id);
        if (scope_it == doc.m_functionScopes.end()) {
            return false;
        }
        for (const auto& [index, mapped] : scope_it->second) {
            const auto& [_, alias] = mapped;
            if (alias && *alias == token) {
                var_index = index;
                return true;
            }
        }
        return false;
    }

    void open_local_var_type_edit(
        app_state&,
        document& doc,
        const function_disassembly& func,
        const ast::code_color_buffer& code,
        const qui::code::hovered_token& token
    ) {
        if (!token.hovered || token.text.empty() || token_is_member_access(token) || token_is_call_name(token)) {
            return;
        }

        const sid64 function_id = SID(func.get_id().c_str());
        u64 var_index = 0;
        if (!local_var_index_from_token(doc, function_id, token.text, var_index)) {
            return;
        }

        local_var_type_edit edit;
        edit.m_function = function_id;
        edit.m_varIndex = var_index;
        edit.m_oldName = token.text;
        edit.m_name = token.text;
        edit.m_pos = ImVec2(token.screen_pos.x, token.screen_pos.y + ImGui::GetTextLineHeight() + 4.0F);

        const auto scope_it = doc.m_functionScopes.find(function_id);
        if (scope_it != doc.m_functionScopes.end()) {
            const auto var_it = scope_it->second.find(var_index);
            if (var_it != scope_it->second.end()) {
                edit.m_typeText = ast::type_to_declaration_string(var_it->second.first);
            }
        }
        if (edit.m_typeText.empty()) {
            edit.m_typeText = infer_declaration_type_text(code, token.text);
        }

        doc.m_localVarTypeEdit = std::move(edit);
    }

    std::optional<mapped_var_value> current_mapped_var(const document& doc, sid64 function, u64 var_index) {
        const auto scope_it = doc.m_functionScopes.find(function);
        if (scope_it == doc.m_functionScopes.end()) {
            return std::nullopt;
        }
        const auto var_it = scope_it->second.find(var_index);
        if (var_it == scope_it->second.end()) {
            return std::nullopt;
        }
        return var_it->second;
    }

    void set_mapped_var(document& doc, sid64 function, u64 var_index, const std::optional<mapped_var_value>& value) {
        if (value) {
            doc.m_functionScopes[function][var_index] = *value;
            return;
        }
        const auto scope_it = doc.m_functionScopes.find(function);
        if (scope_it != doc.m_functionScopes.end()) {
            scope_it->second.erase(var_index);
            if (scope_it->second.empty()) {
                doc.m_functionScopes.erase(scope_it);
            }
        }
    }

    void apply_type_map_record(app_state& state, document& doc, const type_map_record& record, bool redo) {
        set_mapped_var(doc, record.m_function, record.m_varIndex, redo ? record.m_new : record.m_old);
        decompile_document(state, doc);
    }

    void commit_type_map_change(
        app_state& state,
        document& doc,
        sid64 function,
        u64 var_index,
        std::optional<mapped_var_value> new_value
    ) {
        std::optional<mapped_var_value> old_value = current_mapped_var(doc, function, var_index);
        set_mapped_var(doc, function, var_index, new_value);
        decompile_document(state, doc);

        doc.m_redoStack.clear();
        edit_record record;
        record.m_typeMap = type_map_record{function, var_index, std::move(old_value), std::move(new_value)};
        doc.m_undoStack.push_back(std::move(record));
        update_dirty(doc);
        log_event("Type map edit [{}] var_{}", doc.m_name, var_index);
    }

    bool accept_local_var_type_edit(app_state& state, document& doc, local_var_type_edit& edit) {
        edit.m_error.clear();
        const std::string type_text = trim_copy(edit.m_typeText);
        const std::string name = trim_copy(edit.m_name);
        if (type_text.empty()) {
            edit.m_error = "Type is required.";
            return false;
        }
        if (!is_identifier_text(name)) {
            edit.m_error = "Name must be an identifier using letters, digits, or underscores.";
            return false;
        }

        auto type_res = parse_type_text(state, type_text);
        if (!type_res) {
            edit.m_error = type_res.error();
            return false;
        }

        const std::string generated_name = "var_" + std::to_string(edit.m_varIndex);
        std::optional<std::string> alias;
        if (name != generated_name) {
            alias = name;
        }

        commit_type_map_change(
            state, doc, edit.m_function, edit.m_varIndex,
            std::make_pair(std::move(*type_res), std::move(alias)));
        return true;
    }

    bool clear_local_var_type_edit(app_state& state, document& doc, local_var_type_edit& edit) {
        edit.m_error.clear();
        commit_type_map_change(state, doc, edit.m_function, edit.m_varIndex, std::nullopt);
        return true;
    }

    void draw_local_var_type_edit_popup(app_state& state, document& doc) {
        if (!doc.m_localVarTypeEdit) {
            return;
        }

        local_var_type_edit& edit = *doc.m_localVarTypeEdit;
        ImGui::SetNextWindowPos(edit.m_pos, ImGuiCond_Appearing);
        ImGui::SetNextWindowSize(ImVec2(560.0F, 0.0F), ImGuiCond_Appearing);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12.0F, 10.0F));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 6.0F);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, qui::color::active_palette().PopupBackground);
        constexpr ImGuiWindowFlags flags =
            ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_AlwaysAutoResize |
            ImGuiWindowFlags_NoCollapse;

        bool close = false;
        if (ImGui::Begin("Local variable", nullptr, flags)) {
            ImGui::Text("var_%llu", static_cast<unsigned long long>(edit.m_varIndex));

            constexpr f32 field_width = 258.0F;
            const f32 type_x = ImGui::GetCursorPosX();
            const f32 input_y = ImGui::GetCursorPosY();

            const std::vector<std::string> names = known_type_names(state);
            const std::string filter = trim_copy(edit.m_typeText);
            if (filter != edit.m_typeSuggestionFilter) {
                edit.m_typeSuggestionFilter = filter;
                edit.m_typeSuggestionIndex = 0;
            }

            std::vector<std::string> suggestions;
            for (const std::string& name : names) {
                if (!contains_case_insensitive(name, filter)) {
                    continue;
                }
                suggestions.push_back(name);
                if (suggestions.size() >= 8) {
                    break;
                }
            }
            edit.m_typeSuggestionIndex = suggestions.empty()
                ? 0
                : std::clamp(edit.m_typeSuggestionIndex, 0, static_cast<int>(suggestions.size()) - 1);

            struct suggestion_nav {
                int* index;
                int count;
                bool moved;
                bool* cursor_to_end;
            } nav{&edit.m_typeSuggestionIndex, static_cast<int>(suggestions.size()), false, &edit.m_focusCursorEnd};

            const ImGuiInputTextCallback history_cb = [](ImGuiInputTextCallbackData* data) -> int {
                auto* n = static_cast<suggestion_nav*>(data->UserData);
                if (data->EventFlag == ImGuiInputTextFlags_CallbackHistory) {
                    if (n->count > 0) {
                        if (data->EventKey == ImGuiKey_UpArrow) {
                            *n->index = (*n->index + n->count - 1) % n->count;
                            n->moved = true;
                        } else if (data->EventKey == ImGuiKey_DownArrow) {
                            *n->index = (*n->index + 1) % n->count;
                            n->moved = true;
                        }
                    }
                } else if (data->EventFlag == ImGuiInputTextFlags_CallbackAlways && *n->cursor_to_end) {
                    data->CursorPos = data->SelectionStart = data->SelectionEnd = data->BufTextLen;
                    *n->cursor_to_end = false;
                }
                return 0;
            };

            ImGui::SetNextItemWidth(field_width);
            if (edit.m_focusType) {
                ImGui::SetKeyboardFocusHere();
                edit.m_focusType = false;
            }
            const bool type_enter = ImGui::InputTextWithHint(
                "##local_var_type", "type", &edit.m_typeText,
                ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackHistory | ImGuiInputTextFlags_CallbackAlways,
                history_cb, &nav);
            const bool type_active = ImGui::IsItemActive();
            const f32 after_input_y = ImGui::GetCursorPosY();

            ImGui::SameLine();
            ImGui::SetNextItemWidth(field_width);
            const bool name_enter = ImGui::InputTextWithHint("##local_var_name", "name", &edit.m_name, ImGuiInputTextFlags_EnterReturnsTrue);

            bool accepted_suggestion = false;
            if (type_enter && !suggestions.empty()) {
                edit.m_typeText = suggestions[static_cast<std::size_t>(edit.m_typeSuggestionIndex)];
                edit.m_typeSuggestionFilter = trim_copy(edit.m_typeText);
                accepted_suggestion = true;
                edit.m_focusType = true;
                edit.m_focusCursorEnd = true;
            }

            ImGui::SetCursorPos(ImVec2(type_x, after_input_y));
            if (type_active) {
                ImGui::BeginChild("##local_var_type_suggestions", ImVec2(field_width, 126.0F), ImGuiChildFlags_Borders);
                if (suggestions.empty()) {
                    ImGui::PushStyleColor(ImGuiCol_Text, qui::color::active_palette().TextDisabled);
                    ImGui::TextUnformatted("No matches");
                    ImGui::PopStyleColor();
                } else {
                    for (int i = 0; i < static_cast<int>(suggestions.size()); ++i) {
                        const bool selected = i == edit.m_typeSuggestionIndex;
                        if (ImGui::Selectable(suggestions[static_cast<std::size_t>(i)].c_str(), selected)) {
                            edit.m_typeText = suggestions[static_cast<std::size_t>(i)];
                            edit.m_typeSuggestionIndex = i;
                            edit.m_typeSuggestionFilter = trim_copy(edit.m_typeText);
                        }
                        if (selected) {
                            ImGui::SetItemDefaultFocus();
                            if (nav.moved) {
                                ImGui::SetScrollHereY();
                            }
                        }
                    }
                }
                ImGui::EndChild();
                ImGui::SetCursorPosY(std::max(ImGui::GetCursorPosY(), input_y + 126.0F + ImGui::GetFrameHeightWithSpacing()));
            } else {
                ImGui::SetCursorPosY(after_input_y);
            }

            if (!edit.m_error.empty()) {
                ImGui::PushStyleColor(ImGuiCol_Text, qui::color::active_palette().AccentRed);
                ImGui::TextWrapped("%s", edit.m_error.c_str());
                ImGui::PopStyleColor();
            }

            if (ImGui::Button("OK", ImVec2(88.0F, 0.0F)) || (type_enter && !accepted_suggestion) || name_enter) {
                if (accept_local_var_type_edit(state, doc, edit)) {
                    close = true;
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(88.0F, 0.0F)) || ImGui::IsKeyPressed(ImGuiKey_Escape)) {
                close = true;
            }

            const bool has_assignment = current_mapped_var(doc, edit.m_function, edit.m_varIndex).has_value();
            ImGui::SameLine();
            ImGui::BeginDisabled(!has_assignment);
            if (ImGui::Button("Clear type assignment", ImVec2(168.0F, 0.0F))) {
                if (clear_local_var_type_edit(state, doc, edit)) {
                    close = true;
                }
            }
            ImGui::EndDisabled();
        }
        ImGui::End();
        ImGui::PopStyleColor();
        ImGui::PopStyleVar(2);

        if (close) {
            doc.m_localVarTypeEdit.reset();
        }
    }

    void draw_struct_pointer_edit_popup(app_state& state, document& doc) {
        if (!doc.m_structPointerEdit || doc.m_file == nullptr) {
            return;
        }

        struct_pointer_edit& edit = *doc.m_structPointerEdit;
        const BinaryFile& file = *doc.m_file;

        ImGui::SetNextWindowPos(edit.m_pos, ImGuiCond_Appearing);
        ImGui::SetNextWindowSize(ImVec2(440.0F, 0.0F), ImGuiCond_Appearing);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12.0F, 10.0F));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 6.0F);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, qui::color::active_palette().PopupBackground);
        constexpr ImGuiWindowFlags flags =
            ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_AlwaysAutoResize |
            ImGuiWindowFlags_NoCollapse;

        bool close = false;
        if (ImGui::Begin("Change struct pointer", nullptr, flags)) {
            ImGui::PushStyleColor(ImGuiCol_Text, qui::color::active_palette().TextDisabled);
            ImGui::Text("pointer at [0x%05X]", static_cast<u32>(edit.m_pointerOffset));
            ImGui::PopStyleColor();

            ImGui::TextUnformatted("New target offset");
            ImGui::SetNextItemWidth(416.0F);
            if (edit.m_focus) {
                ImGui::SetKeyboardFocusHere();
                edit.m_focus = false;
            }
            const bool enter = ImGui::InputTextWithHint(
                "##struct_ptr_target", "0x...", &edit.m_text, ImGuiInputTextFlags_EnterReturnsTrue);

            u64 target_offset = 0;
            bool parse_ok = false;
            {
                std::string text = trim_copy(edit.m_text);
                if (text.starts_with("0x") || text.starts_with("0X")) {
                    text = text.substr(2);
                }
                if (!text.empty() && text.find_first_not_of("0123456789abcdefABCDEF") == std::string::npos) {
                    try {
                        target_offset = std::stoull(text, nullptr, 16);
                        parse_ok = true;
                    } catch (const std::exception&) {
                        parse_ok = false;
                    }
                }
            }

            const u64 text_limit = file.m_dcheader->m_textSize;
            const bool in_bounds = parse_ok && target_offset >= 8 && target_offset < text_limit && target_offset % 8 == 0;
            const bool is_pointer_target = in_bounds && file.gets_pointed_at(location(file.m_bytes.get() + target_offset));

            ImGui::Dummy(ImVec2(0.0F, 2.0F));

            if (!parse_ok) {
                ImGui::PushStyleColor(ImGuiCol_Text, qui::color::active_palette().AccentRed);
                ImGui::TextWrapped("Enter a hexadecimal file offset (e.g. 0x1A2F0).");
                ImGui::PopStyleColor();
            } else if (!in_bounds) {
                ImGui::PushStyleColor(ImGuiCol_Text, qui::color::active_palette().AccentRed);
                ImGui::TextWrapped("0x%llX is not a valid 8-byte-aligned address inside this file.",
                    static_cast<unsigned long long>(target_offset));
                ImGui::PopStyleColor();
            } else if (!is_pointer_target) {
                ImGui::PushStyleColor(ImGuiCol_Text, qui::color::active_palette().AccentRed);
                ImGui::TextWrapped("0x%llX is not a struct pointer target. Pointing the member here would "
                    "reference raw data and break the file.", static_cast<unsigned long long>(target_offset));
                ImGui::PopStyleColor();
            } else {
                const structs::unmapped* target_struct =
                    reinterpret_cast<const structs::unmapped*>(file.m_bytes.get() + target_offset - 8);
                const sid64 target_type_id = target_struct->typeID;
                const char* type_text = state.m_sidbase != nullptr
                    ? state.m_sidbase->lookup(target_type_id, file.m_sidCache)
                    : nullptr;

                ImGui::TextUnformatted("Type at target:");
                ImGui::SameLine();
                ImGui::PushStyleColor(ImGuiCol_Text, qui::color::active_palette().Highlight);
                ImGui::TextUnformatted(type_text != nullptr ? type_text : "<unknown>");
                ImGui::PopStyleColor();

                if (target_type_id == edit.m_currentTypeId) {
                    ImGui::PushStyleColor(ImGuiCol_Text, qui::color::active_palette().AccentGreen);
                    ImGui::TextWrapped("Type ID matches the current struct.");
                    ImGui::PopStyleColor();
                } else {
                    const char* current_text = state.m_sidbase != nullptr
                        ? state.m_sidbase->lookup(edit.m_currentTypeId, file.m_sidCache)
                        : nullptr;
                    ImGui::PushStyleColor(ImGuiCol_Text, qui::color::active_palette().AccentYellow);
                    ImGui::TextWrapped("Warning: type ID does not match the current struct (%s). "
                        "The edit is allowed, but the data may not match this member.",
                        current_text != nullptr ? current_text : "<unknown>");
                    ImGui::PopStyleColor();
                }
            }

            ImGui::Dummy(ImVec2(0.0F, 4.0F));

            ImGui::BeginDisabled(!is_pointer_target);
            const bool ok = ImGui::Button("OK", ImVec2(88.0F, 0.0F)) || (enter && is_pointer_target);
            ImGui::EndDisabled();
            if (ok && is_pointer_target) {
                const p64 base = reinterpret_cast<p64>(file.m_bytes.get());
                doc.m_pendingEdits.emplace_back(
                    static_cast<u32>(edit.m_pointerOffset),
                    edit_kind::Int64,
                    std::to_string(base + target_offset));
                close = true;
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(88.0F, 0.0F)) || ImGui::IsKeyPressed(ImGuiKey_Escape)) {
                close = true;
            }
        }
        ImGui::End();
        ImGui::PopStyleColor();
        ImGui::PopStyleVar(2);

        if (close) {
            doc.m_structPointerEdit.reset();
        }
    }

    struct value_view {
        app_state* state;
        document* doc;
    };

    u32 file_offset(const document& doc, const void* ptr) {
        return static_cast<u32>(reinterpret_cast<p64>(ptr) - reinterpret_cast<p64>(doc.m_file->m_dcheader));
    }

    // A tree-node id that stays stable across re-disassembly (the underlying file
    // bytes are not reallocated), so ImGui keeps the expand state and scroll
    // position when an edit triggers a re-disassemble. The top bit is set so these
    // ids live in a separate space from the raw file-data pointers that primitive
    // leaves use as their ids (m_dcheader aliases m_bytes.get(), so without this a
    // node and a same-offset leaf would hash to a conflicting ImGui ID).
    const void* stable_id(const document& doc, u64 offset) {
        const std::uintptr_t base = reinterpret_cast<std::uintptr_t>(doc.m_file->m_bytes.get() + offset);
        return reinterpret_cast<const void*>(base | (std::uintptr_t{1} << 63));
    }

    void dv_draw_member_name(const char* member_name) {
        ImGui::SameLine(0.0F, 0.0F);
        ImFont* bold = qui::font_bold();
        if (bold != nullptr) {
            ImGui::PushFont(bold);
        }
        ImGui::PushStyleColor(ImGuiCol_Text, qui::color::active_palette().Text);
        ImGui::Text("%s: ", member_name);
        ImGui::PopStyleColor();
        if (bold != nullptr) {
            ImGui::PopFont();
        }
    }

    f32 dv_member_name_width(const char* member_name) {
        char buffer[160];
        std::snprintf(buffer, sizeof(buffer), "%s: ", member_name);
        return measure_text(qui::font_bold(), buffer);
    }

    bool dv_node(value_view v, const void* id, const ImVec4& color, const char* label, const char* suffix, bool leaf, const char* member_name = nullptr, const char* index_prefix = nullptr, std::optional<u64> struct_ptr_offset = std::nullopt, sid64 struct_type_id = 0) {
        document* doc = v.doc;
        const ImGuiID node_im_id = ImGui::GetID(id);
        i32 node_depth = -1;
        if (!leaf && doc->m_opMode != tree_op::none) {
            if (node_im_id == doc->m_opTarget) {
                node_depth = 0;
            } else if (doc->m_opDepth >= 0) {
                node_depth = doc->m_opDepth + 1;
            }
        }
        if (node_depth >= 0 && doc->m_opMode == tree_op::expand) {
            bool want_open = node_depth < doc->m_opMaxDepth;
            if (want_open) {
                if (doc->m_forceOpenCount >= MAX_FORCE_OPEN_NODES) {
                    want_open = false;
                } else {
                    ++doc->m_forceOpenCount;
                }
            }
            ImGui::SetNextItemOpen(want_open, ImGuiCond_Always);
        } else if (node_depth >= 1 && doc->m_opMode == tree_op::close) {
            doc->m_closeIds.push_back(node_im_id);
        }

        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanAvailWidth;
        if (leaf) {
            flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet;
        }
        const ImGuiStyle& style = ImGui::GetStyle();
        const f32 spacing_y = style.ItemSpacing.y;
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, spacing_y * 0.5F));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(style.ItemSpacing.x, 0.0F));
        const bool use_name = member_name != nullptr && member_name[0] != '\0';
        const char* node_label = label;
        const char* value_text = nullptr;
        if (use_name) {
            node_label = index_prefix != nullptr ? index_prefix : "";
            value_text = label + (index_prefix != nullptr ? std::strlen(index_prefix) : 0);
        }
        ImGui::PushStyleColor(ImGuiCol_Text, color);
        const bool open = ImGui::TreeNodeEx(id, flags, "%s", node_label);
        ImGui::PopStyleColor();
        if (!leaf && ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
            doc->m_menuTarget = node_im_id;
            doc->m_openMenu = true;
            doc->m_menuMousePos = ImGui::GetMousePos();
            doc->m_menuStructPtrOffset = struct_ptr_offset;
            doc->m_menuStructTypeId = struct_type_id;
        }
        if (use_name) {
            dv_draw_member_name(member_name);
            if (value_text != nullptr && value_text[0] != '\0') {
                ImGui::SameLine(0.0F, 0.0F);
                ImGui::PushStyleColor(ImGuiCol_Text, color);
                ImGui::TextUnformatted(value_text);
                ImGui::PopStyleColor();
            }
        }
        if (suffix != nullptr && suffix[0] != '\0') {
            const char* shown_suffix = suffix;
            if (use_name && shown_suffix[0] == ':') {
                ++shown_suffix;
                while (*shown_suffix == ' ') {
                    ++shown_suffix;
                }
            }
            ImGui::SameLine(0.0F, 0.0F);
            ImGui::TextColored(qui::color::active_palette().TextDisabled, "  %s", shown_suffix);
        }
        ImGui::PopStyleVar(2);
        if (!leaf && open) {
            doc->m_depthStack.push_back(doc->m_opDepth);
            if (node_depth >= 0) {
                doc->m_opDepth = node_depth;
            }
        }
        return open;
    }

    void dv_tree_pop(value_view v) {
        document* doc = v.doc;
        if (!doc->m_depthStack.empty()) {
            doc->m_opDepth = doc->m_depthStack.back();
            doc->m_depthStack.pop_back();
        }
        ImGui::TreePop();
    }

    void dv_draw_values(value_view v, const disassembled_values_t& values);
    void dv_draw_value(value_view v, const disassembled_values_t::value_type& value, i32 index, const char* member_name = nullptr);
    void dv_draw_function(value_view v, const function_disassembly& func, i32 index);
    void dv_draw_function_body(value_view v, const function_disassembly& func);
    void dv_function_switch_and_body(value_view v, const function_disassembly& func, const void* id, bool open);
    void dv_draw_state_script(value_view v, const ast::state_script& script, i32 index);
    void dv_draw_map(value_view v, const disassembled_value& entry, const void* id, i32 index);
    void dv_draw_script_lambda(value_view v, const disassembled_value& entry, const void* id, i32 index);

    struct typed_value_text {
        std::string text;
        ImVec4 color = gcol::Struct();
        std::optional<edit_kind> editKind;
    };

    void dv_index_prefix(char* buffer, std::size_t size, i32 index) {
        if (index >= 0) {
            std::snprintf(buffer, size, "[%d] ", index);
        } else if (size > 0) {
            buffer[0] = '\0';
        }
    }

    void dv_draw_struct_like(value_view v, const disassembled_value& entry, const void* id, i32 index, const char* member_name = nullptr) {
        char label[256];
        char suffix[128];
        char prefix[64];
        dv_index_prefix(prefix, sizeof(prefix), index);
        id = stable_id(*v.doc, entry.m_offset);

        if (entry.m_typeId == SID("map") || entry.m_typeId == SID("map-32") || entry.m_typeId == SID("render-settings-map") || entry.m_typeId == SID("hash-table")) {
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
            const bool open = dv_node(v, id, gcol::Array(), label, suffix, entry.m_values.empty(), member_name, prefix);
            if (open && !entry.m_values.empty()) {
                dv_draw_values(v, entry.m_values);
                dv_tree_pop(v);
            }
            return;
        }

        ImVec4 color = gcol::Struct();
        std::string type_storage;
        const char* type_name = "anonymous struct";
        if (entry.m_typeId != 0) {
            type_storage = v.state->m_sidbase->lookup(entry.m_typeId, v.doc->m_file->m_sidCache);
            type_name = type_storage.c_str();
            if (entry.m_typeId == SID("state-script")) {
                color = gcol::StateScript();
            } else if (entry.m_typeId == SID("script-lambda")) {
                color = gcol::Function();
            }
        }

        std::snprintf(label, sizeof(label), "%s%s", prefix, type_name);
        std::snprintf(suffix, sizeof(suffix), "[0x%05X]", static_cast<u32>(entry.m_offset));
        const bool open = dv_node(v, id, color, label, suffix, entry.m_values.empty(), member_name, prefix, entry.m_pointerOffset, entry.m_typeId);
        if (open && !entry.m_values.empty()) {
            dv_draw_values(v, entry.m_values);
            dv_tree_pop(v);
        }
    }

    bool ptr_in_file(const document& doc, const void* ptr) {
        if (doc.m_file == nullptr) {
            return false;
        }
        const p64 base = reinterpret_cast<p64>(doc.m_file->m_bytes.get());
        const p64 here = reinterpret_cast<p64>(ptr);
        return here >= base && (here - base) < doc.m_file->m_size;
    }

    std::string read_value_string(value_view v, const void* data_ptr, edit_kind kind) {
        switch (kind) {
            case edit_kind::Int:
                return std::format("{}", *reinterpret_cast<const i32*>(data_ptr));
            case edit_kind::Int64:
                return std::format("{}", *reinterpret_cast<const i64*>(data_ptr));
            case edit_kind::Float:
                return std::format("{}", *reinterpret_cast<const f32*>(data_ptr));
            case edit_kind::Sid:
                return v.state->m_sidbase->lookup(*reinterpret_cast<const sid64*>(data_ptr), v.doc->m_file->m_sidCache);
        }
        return {};
    }

    ImVec4 color_for_value_prefix(std::string_view prefix) {
        if (prefix == "int" || prefix == "int64" || prefix == "uint64") {
            return gcol::Int();
        }
        if (prefix == "float") {
            return gcol::Float();
        }
        if (prefix == "sid" || prefix == "pointer") {
            return gcol::Sid();
        }
        if (prefix == "function") {
            return gcol::Function();
        }
        if (prefix == "string") {
            return gcol::String();
        }
        return gcol::Struct();
    }

    std::optional<edit_kind> edit_kind_for_value_prefix(std::string_view prefix) {
        if (prefix == "int") {
            return edit_kind::Int;
        }
        if (prefix == "int64" || prefix == "uint64") {
            return edit_kind::Int64;
        }
        if (prefix == "float") {
            return edit_kind::Float;
        }
        if (prefix == "sid" || prefix == "function" || prefix == "pointer") {
            return edit_kind::Sid;
        }
        return std::nullopt;
    }

    typed_value_text make_typed_value_text(std::string text) {
        const std::size_t colon = text.find(':');
        const std::string prefix = text.substr(0, colon != std::string::npos ? colon : text.size());
        return {
            .text = std::move(text),
            .color = color_for_value_prefix(prefix),
            .editKind = edit_kind_for_value_prefix(prefix),
        };
    }

    std::optional<std::string> known_sid_name(value_view v, sid64 hash) {
        if (v.state == nullptr || v.state->m_sidbase == nullptr) {
            return std::nullopt;
        }
        const char* name = v.state->m_sidbase->lookup(hash);
        if (name == nullptr) {
            return std::nullopt;
        }
        return name;
    }

    void dv_editable_table_value(value_view v, const void* id, const void* data_ptr, const typed_value_text& value) {
        document* doc = v.doc;
        const ImGuiID edit_id = ImGui::GetID(id);
        if (doc->m_editingValue == edit_id && value.editKind.has_value()) {
            ImGui::PushID(id);
            const f32 text_w = ImGui::CalcTextSize(doc->m_editBuffer.c_str()).x;
            ImGui::SetNextItemWidth(std::max(text_w + ImGui::GetStyle().FramePadding.x * 2.0F + 24.0F, 80.0F));
            if (doc->m_editFocus) {
                ImGui::SetKeyboardFocusHere();
                doc->m_editFocus = false;
            }
            const bool entered = ImGui::InputText(
                "##edit",
                &doc->m_editBuffer,
                ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll
            );
            if (entered) {
                doc->m_pendingEdits.emplace_back(file_offset(*doc, data_ptr), *value.editKind, doc->m_editBuffer);
                doc->m_editingValue = 0;
            } else if (ImGui::IsKeyPressed(ImGuiKey_Escape) || ImGui::IsItemDeactivated()) {
                doc->m_editingValue = 0;
            }
            ImGui::PopID();
            return;
        }

        const ImVec2 text_pos = ImGui::GetCursorScreenPos();
        const f32 cell_width = std::max(ImGui::GetContentRegionAvail().x, ImGui::CalcTextSize(value.text.c_str()).x);
        const ImVec2 hit_size(cell_width, ImGui::GetTextLineHeightWithSpacing());
        ImGui::InvisibleButton("##value_hit", hit_size);
        const bool double_clicked = ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left);
        ImGui::GetWindowDrawList()->AddText(text_pos, ImGui::ColorConvertFloat4ToU32(value.color), value.text.c_str());
        if (double_clicked && value.editKind.has_value() && ptr_in_file(*doc, data_ptr)) {
            doc->m_editingValue = edit_id;
            doc->m_editFocus = true;
            doc->m_editBuffer = read_value_string(v, data_ptr, *value.editKind);
        }
    }

    void dv_editable_leaf(
        value_view v,
        const void* id,
        const void* data_ptr,
        edit_kind kind,
        const ImVec4& color,
        const char* label,
        const char* suffix,
        const char* prefix,
        const char* member_name = nullptr
    ) {
        document* doc = v.doc;
        const bool use_name = member_name != nullptr && member_name[0] != '\0';
        const ImGuiID edit_id = ImGui::GetID(id);
        if (doc->m_editingValue == edit_id) {
            ImGui::PushID(id);
            const f32 value_offset = ImGui::GetTreeNodeToLabelSpacing() +
                                     (prefix != nullptr ? ImGui::CalcTextSize(prefix).x : 0.0F) +
                                     (use_name ? dv_member_name_width(member_name) : 0.0F);
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + value_offset);
            const f32 text_w = ImGui::CalcTextSize(doc->m_editBuffer.c_str()).x;
            ImGui::SetNextItemWidth(std::max(text_w + ImGui::GetStyle().FramePadding.x * 2.0F + 24.0F, 60.0F));
            if (doc->m_editFocus) {
                ImGui::SetKeyboardFocusHere();
                doc->m_editFocus = false;
            }
            const bool entered = ImGui::InputText(
                "##edit",
                &doc->m_editBuffer,
                ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll
            );
            if (entered) {
                doc->m_pendingEdits.emplace_back(file_offset(*doc, data_ptr), kind, doc->m_editBuffer);
                doc->m_editingValue = 0;
            } else if (ImGui::IsKeyPressed(ImGuiKey_Escape) || ImGui::IsItemDeactivated()) {
                doc->m_editingValue = 0;
            }
            ImGui::PopID();
            return;
        }

        const ImGuiStyle& style = ImGui::GetStyle();
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, style.ItemSpacing.y * 0.5F));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(style.ItemSpacing.x, 0.0F));
        const char* node_label = label;
        const char* value_text = nullptr;
        if (use_name) {
            node_label = prefix != nullptr ? prefix : "";
            value_text = label + (prefix != nullptr ? std::strlen(prefix) : 0);
        }
        ImGui::PushStyleColor(ImGuiCol_Text, color);
        ImGui::TreeNodeEx(id, ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet, "%s", node_label);
        ImGui::PopStyleColor();
        const bool double_clicked = ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left);
        if (use_name) {
            dv_draw_member_name(member_name);
            if (value_text != nullptr && value_text[0] != '\0') {
                ImGui::SameLine(0.0F, 0.0F);
                ImGui::PushStyleColor(ImGuiCol_Text, color);
                ImGui::TextUnformatted(value_text);
                ImGui::PopStyleColor();
            }
        }
        if (suffix != nullptr && suffix[0] != '\0') {
            const char* shown_suffix = suffix;
            if (use_name && shown_suffix[0] == ':') {
                ++shown_suffix;
                while (*shown_suffix == ' ') {
                    ++shown_suffix;
                }
            }
            ImGui::SameLine(0.0F, 0.0F);
            ImGui::TextColored(qui::color::active_palette().TextDisabled, "  %s", shown_suffix);
        }
        ImGui::PopStyleVar(2);
        if (double_clicked && ptr_in_file(*doc, data_ptr)) {
            doc->m_editingValue = edit_id;
            doc->m_editFocus = true;
            doc->m_editBuffer = read_value_string(v, data_ptr, kind);
        }
    }

    void dv_draw_value(value_view v, const disassembled_values_t::value_type& value, i32 index, const char* member_name) {
        char label[512];
        char suffix[128];
        char prefix[64];
        dv_index_prefix(prefix, sizeof(prefix), index);
        std::visit([&](auto&& entry) {
            using T = std::decay_t<decltype(entry)>;
            if constexpr (std::is_same_v<T, mapped_value>) {
                dv_draw_value(v, *entry.m_value, index, entry.m_name.c_str());
            } else if constexpr (std::is_same_v<T, disassembled_value>) {
                dv_draw_struct_like(v, entry, &value, index, member_name);
            } else if constexpr (std::is_same_v<T, std::shared_ptr<function_disassembly>>) {
                dv_draw_function(v, *entry, index);
            } else if constexpr (std::is_same_v<T, const ast::state_script*>) {
                dv_draw_state_script(v, *entry, index);
            } else if constexpr (std::is_same_v<T, const u8*>) {
                std::snprintf(label, sizeof(label), "%s%u", prefix, *entry);
                dv_node(v, entry, *entry == 0 ? gcol::IntZero() : gcol::Int(), label, ": u8", true, member_name, prefix);
            } else if constexpr (std::is_same_v<T, const u16*>) {
                std::snprintf(label, sizeof(label), "%s%u", prefix, *entry);
                dv_node(v, entry, *entry == 0 ? gcol::IntZero() : gcol::Int(), label, ": u16", true, member_name, prefix);
            } else if constexpr (std::is_same_v<T, const u32*>) {
                std::snprintf(label, sizeof(label), "%s%u", prefix, *entry);
                dv_node(v, entry, *entry == 0 ? gcol::IntZero() : gcol::Int(), label, ": u32", true, member_name, prefix);
            } else if constexpr (std::is_same_v<T, const i32*>) {
                std::snprintf(label, sizeof(label), "%s%d", prefix, *entry);
                dv_editable_leaf(v, entry, entry, edit_kind::Int, *entry == 0 ? gcol::IntZero() : gcol::Int(), label, ": int", prefix, member_name);
            } else if constexpr (std::is_same_v<T, const u64*>) {
                const std::string resolved = v.state->m_sidbase->lookup(*entry, v.doc->m_file->m_sidCache);
                std::snprintf(label, sizeof(label), "%s%s", prefix, resolved.c_str());
                dv_editable_leaf(v, entry, entry, edit_kind::Sid, gcol::Sid(), label, ": sid", prefix, member_name);
            } else if constexpr (std::is_same_v<T, const f32*>) {
                std::snprintf(label, sizeof(label), "%s%.2f", prefix, *entry);
                dv_editable_leaf(v, entry, entry, edit_kind::Float, gcol::Float(), label, ": float", prefix, member_name);
            } else if constexpr (std::is_same_v<T, const char*>) {
                std::snprintf(label, sizeof(label), "%s\"%s\"", prefix, entry != nullptr ? entry : "");
                dv_node(v, entry, gcol::String(), label, ": string", true, member_name, prefix);
            } else if constexpr (std::is_same_v<T, const structs::map*>) {
                std::snprintf(label, sizeof(label), "%smap", prefix);
                std::snprintf(
                    suffix,
                    sizeof(suffix),
                    "keys: [0x%05X], values: [0x%05X]",
                    file_offset(*v.doc, entry->keys.data),
                    file_offset(*v.doc, entry->values.data)
                );
                dv_node(v, entry, gcol::Map(), label, suffix, true, member_name, prefix);
            }
        },
                   value);
    }

    void dv_draw_values(value_view v, const disassembled_values_t& values) {
        for (i32 i = 0; i < static_cast<i32>(values.size()); ++i) {
            ImGui::PushID(i);
            dv_draw_value(v, values[static_cast<u32>(i)], i);
            ImGui::PopID();
        }
    }

    void dv_map_extract(
        const disassembled_values_t& values,
        const structs::map*& header,
        const disassembled_value*& keys,
        const disassembled_value*& vals
    ) {
        header = nullptr;
        keys = nullptr;
        vals = nullptr;
        for (const auto& child : values) {
            if (const auto* m = std::get_if<const structs::map*>(&child)) {
                header = *m;
            } else if (const auto* dv = std::get_if<disassembled_value>(&child)) {
                if (keys == nullptr) {
                    keys = dv;
                } else {
                    vals = dv;
                }
            }
        }
    }

    std::string map_key_sort_string(value_view v, const disassembled_value_content& value) {
        std::string result;
        std::visit([&](auto&& entry) {
            using T = std::decay_t<decltype(entry)>;
            if constexpr (std::is_same_v<T, mapped_value>) {
                result = entry.m_name;
            } else if constexpr (std::is_same_v<T, disassembled_value>) {
                result = v.state->m_sidbase->lookup(entry.m_typeId, v.doc->m_file->m_sidCache);
            } else if constexpr (std::is_same_v<T, const u8*>) {
                result = std::format("{}", *entry);
            } else if constexpr (std::is_same_v<T, const u16*>) {
                result = std::format("{}", *entry);
            } else if constexpr (std::is_same_v<T, const u32*>) {
                result = std::format("{}", *entry);
            } else if constexpr (std::is_same_v<T, const i32*>) {
                result = std::format("{}", *entry);
            } else if constexpr (std::is_same_v<T, const u64*>) {
                result = v.state->m_sidbase->lookup(*entry, v.doc->m_file->m_sidCache);
            } else if constexpr (std::is_same_v<T, const f32*>) {
                result = std::format("{}", *entry);
            } else if constexpr (std::is_same_v<T, const char*>) {
                result = entry != nullptr ? entry : "";
            }
        }, value);
        return result;
    }

    void dv_draw_map_table(
        value_view v,
        const disassembled_value& keys,
        const disassembled_value& vals,
        const void* id
    ) {
        ImGui::PushID(id);
        constexpr ImGuiTableFlags table_flags =
            ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable;
        if (ImGui::BeginTable("##map", 2, table_flags)) {
            const i32 sort_state = v.doc->m_mapSortStates[id];
            const char* key_header = sort_state == 1   ? "Key \xE2\x96\xB2"
                                     : sort_state == 2 ? "Key \xE2\x96\xBC"
                                                       : "Key";
            ImGui::TableSetupColumn(key_header, ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

            ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
            for (int column = 0; column < 2; ++column) {
                ImGui::TableSetColumnIndex(column);
                ImGui::TableHeader(ImGui::TableGetColumnName(column));
                if (column == 0 && ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
                    v.doc->m_mapSortStates[id] = sort_state == 1 ? 2 : (sort_state == 2 ? 0 : 1);
                }
            }

            const std::size_t count = std::min(keys.m_values.size(), vals.m_values.size());
            std::vector<std::size_t> order(count);
            for (std::size_t i = 0; i < count; ++i) {
                order[i] = i;
            }
            if (sort_state != 0) {
                std::vector<std::string> sort_keys(count);
                for (std::size_t i = 0; i < count; ++i) {
                    sort_keys[i] = map_key_sort_string(v, keys.m_values[i]);
                }
                std::stable_sort(order.begin(), order.end(), [&](const std::size_t lhs, const std::size_t rhs) {
                    if (sort_state == 2) {
                        return sort_keys[rhs] < sort_keys[lhs];
                    }
                    return sort_keys[lhs] < sort_keys[rhs];
                });
            }

            for (const std::size_t i : order) {
                ImGui::TableNextRow();
                ImGui::PushID(static_cast<i32>(i));
                ImGui::TableSetColumnIndex(0);
                ImGui::PushID(0);
                dv_draw_value(v, keys.m_values[i], -1);
                ImGui::PopID();
                ImGui::TableSetColumnIndex(1);
                ImGui::PushID(1);
                dv_draw_value(v, vals.m_values[i], -1);
                ImGui::PopID();
                ImGui::PopID();
            }
            ImGui::EndTable();
        }
        ImGui::PopID();
    }

    void dv_draw_map(value_view v, const disassembled_value& entry, const void* id, i32 index) {
        char label[64];
        char suffix[160];
        char prefix[24];
        dv_index_prefix(prefix, sizeof(prefix), index);

        const structs::map* header = nullptr;
        const disassembled_value* keys = nullptr;
        const disassembled_value* vals = nullptr;
        dv_map_extract(entry.m_values, header, keys, vals);

        std::snprintf(label, sizeof(label), "%smap", prefix);
        if (header != nullptr) {
            std::snprintf(
                suffix,
                sizeof(suffix),
                "[0x%05X] keys: [0x%05X], values: [0x%05X]",
                static_cast<u32>(entry.m_offset),
                file_offset(*v.doc, header->keys.data),
                file_offset(*v.doc, header->values.data)
            );
        } else {
            std::snprintf(suffix, sizeof(suffix), "[0x%05X]", static_cast<u32>(entry.m_offset));
        }

        const bool empty = keys == nullptr || vals == nullptr || keys->m_values.empty();
        const bool open = dv_node(v, id, gcol::Map(), label, suffix, empty);
        if (!open || empty) {
            return;
        }

        dv_draw_map_table(v, *keys, *vals, id);
        dv_tree_pop(v);
    }

    typed_value_text symbol_table_value_text(value_view v, const SymbolTable& symbols, u32 index) {
        const location value_location = symbols.m_location + static_cast<u64>(index) * 8;
        std::string text;
        std::visit(
            [&](auto&& type) {
                using T = std::decay_t<decltype(type)>;
                if constexpr (std::is_same_v<T, ast::primitive_type>) {
                    switch (type.m_type) {
                        case ast::primitive_kind::I32:
                            text = std::format("int: {}", value_location.get<i32>());
                            break;
                        case ast::primitive_kind::I64:
                            text = std::format("int64: {}", value_location.get<i64>());
                            break;
                        case ast::primitive_kind::U64:
                            if (const std::optional<std::string> sid_name = known_sid_name(v, value_location.get<sid64>())) {
                                text = std::format("sid: {}", *sid_name);
                            } else {
                                text = std::format("uint64: {}", value_location.get<u64>());
                            }
                            break;
                        case ast::primitive_kind::F32:
                            text = std::format("float: {:.2f}", value_location.get<f32>());
                            break;
                        case ast::primitive_kind::STRING:
                            text = std::format("string: \"{}\"", value_location.get<const char*>() != nullptr ? value_location.get<const char*>() : "");
                            break;
                        case ast::primitive_kind::SID:
                            text = std::format("sid: {}", v.state->m_sidbase->lookup(value_location.get<sid64>(), v.doc->m_file->m_sidCache));
                            break;
                        default:
                            if (const std::optional<std::string> sid_name = known_sid_name(v, value_location.get<sid64>())) {
                                text = std::format("sid: {}", *sid_name);
                            } else {
                                text = std::format("unknown: {}", value_location.get<u64>());
                            }
                            break;
                    }
                } else if constexpr (std::is_same_v<T, ast::function_type>) {
                    text = std::format("function: {}", v.state->m_sidbase->lookup(value_location.get<sid64>(), v.doc->m_file->m_sidCache));
                } else if constexpr (std::is_same_v<T, ast::ptr_type>) {
                    text = std::format("pointer: {}", v.state->m_sidbase->lookup(value_location.get<sid64>(), v.doc->m_file->m_sidCache));
                } else {
                    if (const std::optional<std::string> sid_name = known_sid_name(v, value_location.get<sid64>())) {
                        text = std::format("sid: {}", *sid_name);
                    } else {
                        text = std::format("unknown: {}", value_location.get<u64>());
                    }
                }
            },
            static_cast<const ast::full_type::variant_type&>(symbols.m_types[index])
        );
        return make_typed_value_text(std::move(text));
    }

    void dv_draw_function_body(value_view v, const function_disassembly& func) {
        const std::vector<function_disassembly_line>& lines = func.m_lines;

        constexpr ImGuiTableFlags table_flags =
            ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingStretchProp;

        ImFont* mono = qui::font_medium();
        if (mono != nullptr) {
            ImGui::PushFont(mono);
        }

        static const char instructions_node = 0;
        static const char symbols_node = 0;

        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if (dv_node(v, &instructions_node, gcol::Group(), "Instructions", nullptr, false)) {
            if (ImGui::BeginTable("##instructions", 2, table_flags)) {
                ImGui::TableSetupColumn("Instruction", ImGuiTableColumnFlags_WidthFixed, FUNCTION_INSTRUCTION_COLUMN_WIDTH);
                ImGui::TableSetupColumn("Comment", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableHeadersRow();

                for (const function_disassembly_line& line : lines) {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::TextUnformatted(line.m_text.c_str());
                    ImGui::TableSetColumnIndex(1);
                    ImGui::TextColored(qui::color::active_palette().TextDisabled, "%s", line.m_comment.c_str());
                }
                ImGui::EndTable();
            }
            dv_tree_pop(v);
        }

        const SymbolTable& symbols = func.m_stackFrame.m_symbolTable;
        if (symbols.m_location.m_ptr != nullptr && !symbols.m_types.empty()) {
            ImGui::SetNextItemOpen(true, ImGuiCond_Once);
            if (dv_node(v, &symbols_node, gcol::Group(), "Symbol Table", nullptr, false)) {
                if (ImGui::BeginTable("##symbols", 3, table_flags)) {
                    ImGui::TableSetupColumn("#", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableSetupColumn("Offset", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableSetupColumn("Value");
                    ImGui::TableHeadersRow();

                    const ImVec4 hl = qui::color::active_palette().Highlight;
                    const ImU32 row_hl = ImGui::ColorConvertFloat4ToU32(ImVec4(hl.x, hl.y, hl.z, 0.15F));

                    for (u32 i = 0; i < symbols.m_types.size(); ++i) {
                        const location value_location = symbols.m_location + static_cast<u64>(i) * 8;
                        const typed_value_text value = symbol_table_value_text(v, symbols, i);

                        ImGui::TableNextRow();
                        if (ImGui::TableGetHoveredRow() == ImGui::TableGetRowIndex()) {
                            ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, row_hl);
                        }
                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("%04X", i);
                        ImGui::TableSetColumnIndex(1);
                        ImGui::TextColored(qui::color::active_palette().TextDisabled, "0x%06X", file_offset(*v.doc, value_location.m_ptr));
                        ImGui::TableSetColumnIndex(2);
                        ImGui::PushID(static_cast<i32>(i));
                        dv_editable_table_value(v, value_location.m_ptr, value_location.m_ptr, value);
                        ImGui::PopID();
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

    void dv_draw_function(value_view v, const function_disassembly& func, i32 index) {
        char label[256];
        std::snprintf(label, sizeof(label), "[%d] function %s", index, func.get_id().c_str());
        const void* fid = stable_id(*v.doc, func.m_originalOffset);
        ImGui::SetNextItemAllowOverlap();
        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        const bool open = dv_node(v, fid, gcol::Function(), label, nullptr, false);
        dv_function_switch_and_body(v, func, fid, open);
    }

    ImVec2 view_switch_size() {
        ImFont* font = qui::font_semi_bold();
        if (font != nullptr) {
            ImGui::PushFont(font);
        }
        const f32 wa = ImGui::CalcTextSize("ASM").x;
        const f32 wb = ImGui::CalcTextSize("DCPL").x;
        if (font != nullptr) {
            ImGui::PopFont();
        }
        const f32 seg = std::max(wa, wb) + 16.0F;
        return ImVec2(seg * 2.0F, ImGui::GetTextLineHeight());
    }

    bool draw_view_switch(const char* str_id, bool* dcpl, const ImVec2& size) {
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

        ImGuiStorage* storage = ImGui::GetStateStorage();
        const ImGuiID key = ImGui::GetID("##t");
        const f32 target = *dcpl ? 0.0F : 1.0F;
        f32 t = storage->GetFloat(key, target);
        t += (target - t) * std::clamp(ImGui::GetIO().DeltaTime * 14.0F, 0.0F, 1.0F);
        if (std::fabs(target - t) < 0.001F) {
            t = target;
        }
        storage->SetFloat(key, t);

        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        const f32 seg = size.x * 0.5F;
        const ImVec2 max(pos.x + size.x, pos.y + size.y);

        draw_list->AddRectFilled(pos, max, ImGui::ColorConvertFloat4ToU32(qui::color::active_palette().WindowBackground));
        draw_list->AddRect(pos, max, ImGui::ColorConvertFloat4ToU32(qui::color::active_palette().Border));

        const ImVec2 knob_min(pos.x + t * seg, pos.y);
        const ImVec2 knob_max(knob_min.x + seg, pos.y + size.y);
        ImVec4 accent = qui::color::active_palette().Highlight;
        if (!hovered) {
            accent.w = 0.85F;
        }
        draw_list->AddRectFilled(knob_min, knob_max, ImGui::ColorConvertFloat4ToU32(accent));

        ImFont* font = qui::font_semi_bold();
        if (font != nullptr) {
            ImGui::PushFont(font);
        }
        const ImU32 active = ImGui::ColorConvertFloat4ToU32(qui::color::active_palette().WindowBackground);
        const ImU32 inactive = ImGui::ColorConvertFloat4ToU32(qui::color::active_palette().TextDisabled);
        const char* labels[2] = {"DCPL", "ASM"};
        for (int i = 0; i < 2; ++i) {
            const ImVec2 text_size = ImGui::CalcTextSize(labels[i]);
            const f32 text_x = pos.x + seg * static_cast<f32>(i) + (seg - text_size.x) * 0.5F;
            const f32 text_y = pos.y + (size.y - text_size.y) * 0.5F;
            const bool is_active = (i == 0) == *dcpl;
            draw_list->AddText(ImVec2(text_x, text_y), is_active ? active : inactive, labels[i]);
        }
        if (font != nullptr) {
            ImGui::PopFont();
        }

        ImGui::PopID();
        return changed;
    }

    ImVec2 toggle_switch_size() {
        const f32 h = ImGui::GetTextLineHeight();
        return ImVec2(h * 1.9F, h);
    }

    bool draw_toggle_switch(const char* str_id, bool* value) {
        const ImVec2 size = toggle_switch_size();
        ImGui::PushID(str_id);
        const ImVec2 pos = ImGui::GetCursorScreenPos();
        ImGui::InvisibleButton("##toggle", size);
        bool changed = false;
        if (ImGui::IsItemClicked()) {
            *value = !*value;
            changed = true;
        }
        const bool hovered = ImGui::IsItemHovered();
        if (hovered) {
            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
        }

        ImGuiStorage* storage = ImGui::GetStateStorage();
        const ImGuiID key = ImGui::GetID("##t");
        const f32 target = *value ? 1.0F : 0.0F;
        f32 t = storage->GetFloat(key, target);
        t += (target - t) * std::clamp(ImGui::GetIO().DeltaTime * 14.0F, 0.0F, 1.0F);
        if (std::fabs(target - t) < 0.001F) {
            t = target;
        }
        storage->SetFloat(key, t);

        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        const f32 radius = size.y * 0.5F;
        const ImVec2 max(pos.x + size.x, pos.y + size.y);

        const ImVec4 off_col = qui::color::active_palette().Border;
        ImVec4 on_col = qui::color::active_palette().Highlight;
        if (!hovered) {
            on_col.w = 0.85F;
        }
        const ImVec4 track_col(
            off_col.x + (on_col.x - off_col.x) * t,
            off_col.y + (on_col.y - off_col.y) * t,
            off_col.z + (on_col.z - off_col.z) * t,
            off_col.w + (on_col.w - off_col.w) * t
        );
        draw_list->AddRectFilled(pos, max, ImGui::ColorConvertFloat4ToU32(track_col), radius);

        const f32 knob_r = radius - 2.0F;
        const f32 knob_x = pos.x + radius + t * (size.x - size.y);
        const ImVec2 knob_center(knob_x, pos.y + radius);
        draw_list->AddCircleFilled(knob_center, knob_r, ImGui::ColorConvertFloat4ToU32(qui::color::active_palette().WindowBackground));

        ImGui::PopID();
        return changed;
    }

    void draw_decomp_warning_box(const char* reason) {
        const ImVec4 warn = qui::color::active_palette().AccentYellow;
        const ImGuiStyle& style = ImGui::GetStyle();

        ImGui::PushStyleColor(ImGuiCol_Border, warn);
        ImGui::PushStyleColor(ImGuiCol_ChildBg, qui::color::rgba(0xF1, 0xC4, 0x0F, 0x1A));
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, style.FrameRounding);
        ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 1.0F);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0F, 8.0F));

        ImGui::BeginChild("##decomp_warning", ImVec2(0.0F, 0.0F),
                          ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY);

        ImFont* bold = qui::font_bold();
        if (bold != nullptr) {
            ImGui::PushFont(bold);
        }
        ImGui::PushStyleColor(ImGuiCol_Text, warn);
        ImGui::TextUnformatted("WARNING");
        ImGui::PopStyleColor();
        if (bold != nullptr) {
            ImGui::PopFont();
        }

        ImGui::TextWrapped("This function decompilation might be inaccurate: %s", reason);

        ImGui::EndChild();

        ImGui::PopStyleVar(3);
        ImGui::PopStyleColor(2);
    }

    void dv_function_switch_and_body(value_view v, const function_disassembly& func, const void* id, bool open) {
        bool& show_dcpl = v.doc->m_lambdaViewDcpl.try_emplace(&func, v.state->m_defaultViewDcpl).first->second;

        const ImVec2 sw_size = view_switch_size();
        ImGui::SameLine();
        ImGui::SetCursorPosX(std::max(ImGui::GetCursorPosX(), ImGui::GetContentRegionMax().x - sw_size.x));
        const f32 line_h = ImGui::GetTextLineHeight() + ImGui::GetStyle().ItemSpacing.y;
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (line_h - sw_size.y) * 0.5F);
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
                const auto err_it = v.doc->m_decompErrors.find(&func);
                if (err_it != v.doc->m_decompErrors.end()) {
                    draw_decomp_warning_box(err_it->second.c_str());
                    ImGui::Spacing();
                }
                qui::code::hovered_token hovered;
                render_decompiled_code("##dcpl_view", *v.state, it->second, &hovered);
                const bool trigger_edit =
                    hovered.hovered &&
                    !ImGui::GetIO().WantTextInput &&
                    (ImGui::IsKeyPressed(ImGuiKey_T) || ImGui::IsMouseClicked(ImGuiMouseButton_Right));
                if (trigger_edit) {
                    open_local_var_type_edit(*v.state, *v.doc, func, it->second, hovered);
                }
            } else {
                ImGui::PushStyleColor(ImGuiCol_Text, qui::color::active_palette().TextDisabled);
                qui::text_label("Decompilation unavailable for this function.");
                ImGui::PopStyleColor();
            }
        } else {
            dv_draw_function_body(v, func);
        }
        ImGui::PopID();
        dv_tree_pop(v);
    }

    void dv_draw_script_lambda(value_view v, const disassembled_value& entry, const void* id, i32 index) {
        char label[64];
        char suffix[64];
        char prefix[24];
        dv_index_prefix(prefix, sizeof(prefix), index);

        const function_disassembly* func = nullptr;
        for (const auto& child : entry.m_values) {
            if (const auto* f = std::get_if<std::shared_ptr<function_disassembly>>(&child); f != nullptr && *f != nullptr) {
                func = f->get();
            }
        }
        if (func == nullptr && v.doc->m_disassembler != nullptr) {
            func = v.doc->m_disassembler->get_function_at_offset(entry.m_offset);
        }

        std::snprintf(label, sizeof(label), "%sscript-lambda", prefix);
        std::snprintf(suffix, sizeof(suffix), "[0x%05X]", static_cast<u32>(entry.m_offset));
        if (func != nullptr) {
            ImGui::SetNextItemAllowOverlap();
        }
        const bool open = dv_node(v, id, gcol::Function(), label, suffix, func == nullptr);

        if (func == nullptr) {
            return;
        }
        dv_function_switch_and_body(v, *func, id, open);
    }

    void dv_draw_text_leaf(value_view v, const void* id, const ImVec4& color, const std::string& text) {
        std::string single_line = text;
        const std::size_t newline = single_line.find('\n');
        if (newline != std::string::npos) {
            single_line.resize(newline);
        }
        dv_node(v, id, color, single_line.c_str(), nullptr, true);
    }

    void dv_draw_state_script(value_view v, const ast::state_script& script, i32 index) {
        char label[256];
        std::snprintf(label, sizeof(label), "[%d] state-script %s", index, script.m_name.c_str());
        const bool open = dv_node(v, &script, gcol::StateScript(), label, nullptr, false);
        if (!open) {
            return;
        }
        ImGui::PushID(&script);

        if (!script.m_options.empty()) {
            if (dv_node(v, &script.m_options, gcol::Group(), "options", nullptr, false)) {
                for (const ast::sid_identifier& option : script.m_options) {
                    dv_draw_text_leaf(v, &option, gcol::Sid(), option.to_pseudo_c_string());
                }
                dv_tree_pop(v);
            }
        }

        if (!script.m_declarations.empty()) {
            if (dv_node(v, &script.m_declarations, gcol::Group(), "declarations", nullptr, false)) {
                for (const ast::variable_declaration& declaration : script.m_declarations) {
                    dv_draw_text_leaf(v, &declaration, gcol::Struct(), declaration.to_pseudo_c_string());
                }
                dv_tree_pop(v);
            }
        }

        for (const ast::state_script_state& ss_state : script.m_states) {
            char state_label[256];
            std::snprintf(state_label, sizeof(state_label), "state %s", ss_state.m_name.c_str());
            if (dv_node(v, &ss_state, gcol::StateScript(), state_label, nullptr, ss_state.m_blocks.empty()) && !ss_state.m_blocks.empty()) {
                for (const ast::state_script_block& block : ss_state.m_blocks) {
                    const std::string block_name = block.block_type_to_string();
                    if (dv_node(v, &block, gcol::Group(), block_name.c_str(), nullptr, block.m_tracks.empty()) && !block.m_tracks.empty()) {
                        for (const ast::state_script_track& track : block.m_tracks) {
                            char track_label[256];
                            std::snprintf(track_label, sizeof(track_label), "track %s", track.m_name.c_str());
                            if (dv_node(v, &track, gcol::Group(), track_label, nullptr, track.m_lambdas.empty()) && !track.m_lambdas.empty()) {
                                i32 lambda_index = 0;
                                for (const ast::state_script_lambda& lambda : track.m_lambdas) {
                                    std::visit(
                                        [&](auto&& fn) {
                                            using T = std::decay_t<decltype(fn)>;
                                            if constexpr (std::is_same_v<T, std::shared_ptr<function_disassembly>>) {
                                                if (fn != nullptr) {
                                                    dv_draw_function(v, *fn, lambda_index);
                                                }
                                            } else {
                                                dv_draw_text_leaf(v, &fn, gcol::Function(), fn.to_pseudo_c_string());
                                            }
                                        },
                                        lambda
                                    );
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

    void draw_entry_detail(app_state& state, document& doc) {
        if (doc.m_entries == nullptr || state.m_sidbase == nullptr || doc.m_file == nullptr) {
            qui::text_label("Not disassembled.");
            return;
        }
        if (doc.m_selectedEntry < 0 || doc.m_selectedEntry >= static_cast<i32>(doc.m_entries->size())) {
            ImGui::PushStyleColor(ImGuiCol_Text, qui::color::active_palette().TextDisabled);
            qui::text_label("Select an entry to inspect.");
            ImGui::PopStyleColor();
            return;
        }

        const disassembled_entry& entry = (*doc.m_entries)[static_cast<u32>(doc.m_selectedEntry)];
        const char* name = state.m_sidbase->lookup(entry.m_nameId, doc.m_file->m_sidCache);
        const char* type = state.m_sidbase->lookup(entry.m_typeId, doc.m_file->m_sidCache);

        char suffix[256];
        std::snprintf(suffix, sizeof(suffix), ": %s  [0x%05X]", type, static_cast<u32>(entry.m_offset));

        doc.m_opTarget = doc.m_opPendingTarget;
        doc.m_opMode = doc.m_opPendingMode;
        doc.m_opMaxDepth = doc.m_opPendingMaxDepth;
        doc.m_opPendingTarget = 0;
        doc.m_opPendingMode = tree_op::none;
        doc.m_opDepth = -1;
        doc.m_forceOpenCount = 0;
        doc.m_depthStack.clear();
        doc.m_closeIds.clear();

        value_view v{&state, &doc};
        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        ImGui::PushID(doc.m_selectedEntry);
        const bool entry_is_map = entry.m_typeId == SID("map") || entry.m_typeId == SID("map-32") || entry.m_typeId == SID("render-settings-map") || entry.m_typeId == SID("hash-table");
        const void* entry_id = stable_id(doc, entry.m_offset);
        const bool open = dv_node(v, entry_id, gcol::EntryName(), name, suffix, entry.m_values.empty());
        if (open && !entry.m_values.empty()) {
            if (entry_is_map) {
                const structs::map* header = nullptr;
                const disassembled_value* keys = nullptr;
                const disassembled_value* vals = nullptr;
                dv_map_extract(entry.m_values, header, keys, vals);
                if (keys != nullptr && vals != nullptr && !keys->m_values.empty()) {
                    dv_draw_map_table(v, *keys, *vals, entry_id);
                }
            } else {
                dv_draw_values(v, entry.m_values);
            }
            dv_tree_pop(v);
        }
        ImGui::PopID();

        if (doc.m_opMode == tree_op::close && !doc.m_closeIds.empty()) {
            ImGuiStorage* storage = ImGui::GetStateStorage();
            for (const ImGuiID close_id : doc.m_closeIds) {
                storage->SetInt(close_id, 0);
            }
        }

        if (doc.m_openMenu) {
            ImGui::OpenPopup("##dv_context_menu");
            doc.m_openMenu = false;
        }
        if (ImGui::BeginPopup("##dv_context_menu")) {
            if (doc.m_menuStructPtrOffset.has_value() && doc.m_file != nullptr) {
                if (ImGui::MenuItem("Change Struct Pointer")) {
                    const u64 ptr_offset = *doc.m_menuStructPtrOffset;
                    const p64 base = reinterpret_cast<p64>(doc.m_file->m_bytes.get());
                    const u64 current_target = *reinterpret_cast<const u64*>(doc.m_file->m_bytes.get() + ptr_offset) - base;
                    struct_pointer_edit edit;
                    edit.m_pointerOffset = ptr_offset;
                    edit.m_currentTypeId = doc.m_menuStructTypeId;
                    edit.m_text = std::format("0x{:X}", static_cast<u32>(current_target));
                    edit.m_pos = doc.m_menuMousePos;
                    edit.m_focus = true;
                    doc.m_structPointerEdit = std::move(edit);
                }
                ImGui::Separator();
            }
            if (ImGui::MenuItem("Expand all children")) {
                doc.m_opPendingTarget = doc.m_menuTarget;
                doc.m_opPendingMode = tree_op::expand;
                doc.m_opPendingMaxDepth = EXPAND_SHALLOW_DEPTH;
            }
            if (ImGui::MenuItem("Expand all children recursively")) {
                doc.m_opPendingTarget = doc.m_menuTarget;
                doc.m_opPendingMode = tree_op::expand;
                doc.m_opPendingMaxDepth = EXPAND_RECURSIVE_DEPTH;
            }
            if (ImGui::MenuItem("Collapse all children")) {
                doc.m_opPendingTarget = doc.m_menuTarget;
                doc.m_opPendingMode = tree_op::close;
                doc.m_opPendingMaxDepth = 0;
            }
            ImGui::EndPopup();
        }

        draw_local_var_type_edit_popup(state, doc);
        draw_struct_pointer_edit_popup(state, doc);
    }

    void draw_document(app_state& state, document& doc) {
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
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, qui::color::active_palette().SeparatorHovered);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, qui::color::active_palette().SeparatorActive);
        ImGui::Button("##dconstruct_splitter", ImVec2(SPLITTER_WIDTH, avail_height));
        if (ImGui::IsItemHovered() || ImGui::IsItemActive()) {
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
        }
        if (ImGui::IsItemActive()) {
            doc.m_listWidth += ImGui::GetIO().MouseDelta.x;
        }
        ImGui::PopStyleColor(3);

        ImGui::SameLine(0.0F, 0.0F);

        // ImGui::BeginChild("##dconstruct_entry_view", ImVec2(0.0F, 0.0F), ImGuiChildFlags_Borders);
        ImVec2 detail_size;
        if (begin_labeled_table_frame("Disassembly", detail_size)) {
            ImGui::BeginChild("##dconstruct_entry_detail", detail_size);
            draw_entry_detail(state, doc);
            ImGui::EndChild();
        }
        // ImGui::EndChild();
    }

    void show_error_box(app_state& state, const std::string& message) {
        qui::open_alert(state.m_errorBox, "Edit error", message);
    }

    void redisassemble_preserving(app_state& state, document& doc) {
        const i32 selected = doc.m_selectedEntry;
        disassemble_document(state, doc);
        if (doc.m_entries != nullptr && selected >= 0 && selected < static_cast<i32>(doc.m_entries->size())) {
            doc.m_selectedEntry = selected;
        }
    }

    u32 edit_value_size(edit_kind kind) {
        return kind == edit_kind::Sid || kind == edit_kind::Int64 ? 8U : 4U;
    }

    std::string value_at_string(value_view v, u32 offset, edit_kind kind) {
        return read_value_string(v, v.doc->m_file->m_bytes.get() + offset, kind);
    }

    std::size_t byte_edit_depth(const document& doc) {
        return static_cast<std::size_t>(std::count_if(
            doc.m_undoStack.begin(), doc.m_undoStack.end(),
            [](const edit_record& record) { return !record.m_typeMap.has_value(); }));
    }

    std::size_t type_edit_depth(const document& doc) {
        return static_cast<std::size_t>(std::count_if(
            doc.m_undoStack.begin(), doc.m_undoStack.end(),
            [](const edit_record& record) { return record.m_typeMap.has_value(); }));
    }

    void update_dirty(document& doc) {
        doc.m_dirty = byte_edit_depth(doc) != doc.m_savedDepth ||
                      type_edit_depth(doc) != doc.m_savedTypeDepth;
    }

    bool apply_one_edit(app_state& state, document& doc, const pending_edit& edit) {
        const value_view v{&state, &doc};
        const u32 size = edit_value_size(edit.m_kind);
        std::byte* value_ptr = doc.m_file->m_bytes.get() + edit.m_offset;

        const std::string old_text = value_at_string(v, edit.m_offset, edit.m_kind);
        std::vector<std::byte> old_bytes(value_ptr, value_ptr + size);

        try {
            const BinaryFileEdit value = [&]() -> BinaryFileEdit {
                switch (edit.m_kind) {
                    case edit_kind::Int:
                        return {.m_editType = EditType::INT32, .I32 = static_cast<i32>(std::stol(edit.m_text))};
                    case edit_kind::Int64:
                        return {.m_editType = EditType::INT64, .U64 = edit.m_text.starts_with("-") ? static_cast<u64>(std::stoll(edit.m_text)) : std::stoull(edit.m_text)};
                    case edit_kind::Float:
                        return {.m_editType = EditType::F32, .F32 = std::stof(edit.m_text)};
                    case edit_kind::Sid:
                        if (!edit.m_text.empty() && edit.m_text.front() == '#') {
                            return {.m_editType = EditType::SID_HASH, .U64 = std::stoull(edit.m_text.substr(1), nullptr, 16)};
                        }
                        BinaryFileEdit sid_edit{.m_editType = EditType::SID_STR};
                        sid_edit.string = &edit.m_text;
                        return sid_edit;
                }
                return {.m_editType = EditType::INT32, .I32 = 0};
            }();

            if (const error_msg err = doc.m_editor->apply_edit(edit.m_offset, 0, value); err.has_value()) {
                show_error_box(state, *err);
                return false;
            }
        } catch (const std::exception& e) {
            show_error_box(state, std::format("'{}' is not a valid value for this field. err: {}", edit.m_text, e.what()));
            return false;
        }

        const std::string new_text = value_at_string(v, edit.m_offset, edit.m_kind);
        std::vector<std::byte> new_bytes(value_ptr, value_ptr + size);

        doc.m_redoStack.clear();
        doc.m_undoStack.push_back({edit.m_offset, edit.m_kind, std::move(old_bytes), std::move(new_bytes), old_text, new_text});
        log_event("Edit applied [{}] 0x{:05X}: {} -> {}", doc.m_name, edit.m_offset, old_text, new_text);
        return true;
    }

    void process_pending_edits(app_state& state, document& doc) {
        if (doc.m_pendingEdits.empty()) {
            return;
        }
        if (doc.m_editor == nullptr || doc.m_file == nullptr || state.m_sidbase == nullptr) {
            doc.m_pendingEdits.clear();
            return;
        }

        bool applied_any = false;
        for (const pending_edit& edit : doc.m_pendingEdits) {
            applied_any = apply_one_edit(state, doc, edit) || applied_any;
        }
        doc.m_pendingEdits.clear();

        if (applied_any) {
            update_dirty(doc);
            redisassemble_preserving(state, doc);
        }
    }

    void apply_record_bytes(document& doc, const edit_record& record, bool redo) {
        const std::vector<std::byte>& bytes = redo ? record.m_newBytes : record.m_oldBytes;
        std::memcpy(doc.m_file->m_bytes.get() + record.m_offset, bytes.data(), bytes.size());
    }

    void undo_active_document(app_state& state) {
        document* doc = active_document(state);
        if (doc == nullptr || doc->m_file == nullptr || doc->m_undoStack.empty()) {
            return;
        }
        edit_record record = std::move(doc->m_undoStack.back());
        doc->m_undoStack.pop_back();
        if (record.m_typeMap) {
            apply_type_map_record(state, *doc, *record.m_typeMap, false);
            log_event("Undo type map [{}] var_{}", doc->m_name, record.m_typeMap->m_varIndex);
            doc->m_redoStack.push_back(std::move(record));
            update_dirty(*doc);
            return;
        }
        apply_record_bytes(*doc, record, false);
        log_event("Undo [{}] 0x{:05X}: {} -> {}", doc->m_name, record.m_offset, record.m_newText, record.m_oldText);
        doc->m_redoStack.push_back(std::move(record));
        update_dirty(*doc);
        redisassemble_preserving(state, *doc);
    }

    void redo_active_document(app_state& state) {
        document* doc = active_document(state);
        if (doc == nullptr || doc->m_file == nullptr || doc->m_redoStack.empty()) {
            return;
        }
        edit_record record = std::move(doc->m_redoStack.back());
        doc->m_redoStack.pop_back();
        if (record.m_typeMap) {
            apply_type_map_record(state, *doc, *record.m_typeMap, true);
            log_event("Redo type map [{}] var_{}", doc->m_name, record.m_typeMap->m_varIndex);
            doc->m_undoStack.push_back(std::move(record));
            update_dirty(*doc);
            return;
        }
        apply_record_bytes(*doc, record, true);
        log_event("Redo [{}] 0x{:05X}: {} -> {}", doc->m_name, record.m_offset, record.m_oldText, record.m_newText);
        doc->m_undoStack.push_back(std::move(record));
        update_dirty(*doc);
        redisassemble_preserving(state, *doc);
    }

    bool save_document(app_state& state, document& doc) {
        if (doc.m_editor == nullptr) {
            return false;
        }
        if (byte_edit_depth(doc) != doc.m_savedDepth) {
            if (const error_msg err = doc.m_editor->write_edited_file(); err.has_value()) {
                show_error_box(state, *err);
                return false;
            }
        }
        if (type_edit_depth(doc) != doc.m_savedTypeDepth) {
            std::string error;
            if (!write_document_typemap(state, doc, error)) {
                show_error_box(state, error);
                return false;
            }
        }
        doc.m_savedDepth = byte_edit_depth(doc);
        doc.m_savedTypeDepth = type_edit_depth(doc);
        update_dirty(doc);
        log_event("Saved file: {}", doc.m_path);
        return true;
    }

    void save_active_document(app_state& state) {
        document* doc = active_document(state);
        if (doc == nullptr || !doc->m_dirty) {
            return;
        }
        save_document(state, *doc);
    }

    bool any_document_dirty(const app_state& state) {
        for (const document& doc : state.m_documents) {
            if (doc.m_dirty) {
                return true;
            }
        }
        return false;
    }

    void save_all_documents(app_state& state) {
        for (document& doc : state.m_documents) {
            if (!doc.m_dirty) {
                continue;
            }
            save_document(state, doc);
        }
    }

    void request_app_close(app_state& state) {
        if (!any_document_dirty(state)) {
            glfwSetWindowShouldClose(state.m_window, GLFW_TRUE);
            return;
        }
        state.m_closeRequested = true;
        state.m_closeBox.title = "Unsaved changes";
        state.m_closeBox.message = "You have unsaved changes. Do you want to save them before exiting?";
        state.m_closeBox.buttons = {
            qui::message_box_button{"Exit without saving", qui::color::active_palette().AccentRed, true, true},
            qui::message_box_button{"Go back"},
            qui::message_box_button{"Save & Exit"},
        };
        state.m_closeBox.open();
    }

    void process_message_boxes(app_state& state) {
        qui::draw_message_box(state.m_errorBox);

        const int result = qui::draw_message_box(state.m_closeBox);
        if (!state.m_closeRequested) {
            return;
        }
        switch (result) {
            case 0: // Exit without saving
                state.m_closeRequested = false;
                glfwSetWindowShouldClose(state.m_window, GLFW_TRUE);
                break;
            case 2: // Save & Exit
                state.m_closeRequested = false;
                save_all_documents(state);
                if (!any_document_dirty(state)) {
                    glfwSetWindowShouldClose(state.m_window, GLFW_TRUE);
                }
                break;
            case 1: // Go back
            case qui::message_box_dismissed:
                state.m_closeRequested = false;
                break;
            default:
                break;
        }
    }

    bool debugger_vertical_splitter(const char* id, f32 height, f32& width) {
        ImGui::PushStyleColor(ImGuiCol_Button, qui::color::rgba(0x00, 0x00, 0x00, 0x00));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, qui::color::active_palette().SeparatorHovered);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, qui::color::active_palette().SeparatorActive);
        ImGui::Button(id, ImVec2(SPLITTER_WIDTH, height));
        const bool active = ImGui::IsItemActive();
        if (ImGui::IsItemHovered() || active) {
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
        }
        if (active) {
            width += ImGui::GetIO().MouseDelta.x;
        }
        ImGui::PopStyleColor(3);
        return active;
    }

    void debugger_section_header(const char* text) {
        if (ImFont* section_font = qui::font_semi_bold(); section_font != nullptr) {
            ImGui::PushFont(section_font);
            ImGui::PushStyleColor(ImGuiCol_Text, qui::color::active_palette().Highlight);
            ImGui::TextUnformatted(text);
            ImGui::PopStyleColor();
            ImGui::PopFont();
        } else {
            ImGui::TextUnformatted(text);
        }
        ImGui::Dummy(ImVec2(0.0F, 4.0F));
    }

    std::string format_register_float(f32 value) {
        std::string text = std::format("{}", value);
        if (text.find_first_of(".eEnN") == std::string::npos) {
            text += ".0";
        }
        return text;
    }

    void draw_vm_register_value(const ImVec4& color, const std::string& text) {
        const f32 avail = ImGui::GetContentRegionAvail().x;
        const f32 text_width = ImGui::CalcTextSize(text.c_str()).x;
        if (text_width < avail) {
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (avail - text_width));
        }
        ImGui::PushStyleColor(ImGuiCol_Text, color);
        ImGui::TextUnformatted(text.c_str());
        ImGui::PopStyleColor();
    }

    void draw_vm_register_cell(const app_state& state, const char* reg_prefix, i32 index, u64 value) {
        char name[32];
        std::snprintf(name, sizeof(name), "%s%d", reg_prefix, index);

        ImFont* bold = qui::font_bold();
        if (bold != nullptr) {
            ImGui::PushFont(bold);
        }
        const f32 name_avail = ImGui::GetContentRegionAvail().x;
        const f32 name_width = ImGui::CalcTextSize(name).x;
        if (name_width < name_avail) {
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (name_avail - name_width) * 0.5F);
        }
        ImGui::PushStyleColor(ImGuiCol_Text, qui::color::active_palette().Highlight);
        ImGui::TextUnformatted(name);
        ImGui::PopStyleColor();
        if (bold != nullptr) {
            ImGui::PopFont();
        }
        ImGui::Separator();

        ImFont* mono = qui::font_medium();
        if (mono != nullptr) {
            ImGui::PushFont(mono);
        }

        draw_vm_register_value(gcol::Int(), std::format("0x{:016X}", value));

        f32 as_float = 0.0F;
        const u32 low_bits = static_cast<u32>(value & 0xFFFFFFFFULL);
        std::memcpy(&as_float, &low_bits, sizeof(as_float));
        draw_vm_register_value(gcol::Float(), format_register_float(as_float));

        const char* sid_name = state.m_sidbase != nullptr ? state.m_sidbase->lookup(static_cast<sid64>(value)) : nullptr;
        draw_vm_register_value(gcol::Sid(), sid_name != nullptr ? std::string(sid_name) : std::format("#{:016X}", value));

        draw_vm_register_value(gcol::String(), std::format("\xE2\x86\x92 0x{:016X}", value));

        if (mono != nullptr) {
            ImGui::PopFont();
        }
    }

    void draw_vm_register_grid(
        const app_state& state,
        const char* id,
        const char* reg_prefix,
        std::span<const u64> values,
        i32 default_rows,
        bool& expanded
    ) {
        const i32 total = static_cast<i32>(values.size());
        const i32 default_count = std::min(total, VM_REGISTER_COLUMNS * default_rows);
        const i32 shown = expanded ? total : default_count;

        constexpr ImGuiTableFlags table_flags =
            ImGuiTableFlags_Borders |
            ImGuiTableFlags_SizingStretchSame;

        if (ImGui::BeginTable(id, VM_REGISTER_COLUMNS, table_flags)) {
            for (i32 i = 0; i < shown; ++i) {
                ImGui::TableNextColumn();
                draw_vm_register_cell(state, reg_prefix, i, values[static_cast<u32>(i)]);
            }
            ImGui::EndTable();
        }

        if (total > default_count) {
            ImGui::Dummy(ImVec2(0.0F, 2.0F));
            const i32 hidden = total - default_count;
            char label[64];
            if (expanded) {
                std::snprintf(label, sizeof(label), "Collapse##%s", id);
            } else {
                std::snprintf(label, sizeof(label), "Expand (+%d)##%s", hidden, id);
            }
            ImGui::PushStyleColor(ImGuiCol_Button, qui::color::rgba(0x00, 0x00, 0x00, 0x00));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, qui::color::active_palette().TableRowBackgroundAlt);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, qui::color::active_palette().TableRowBackgroundAlt);
            if (ImGui::Button(label, ImVec2(-FLT_MIN, 0.0F))) {
                expanded = !expanded;
            }
            ImGui::PopStyleColor(3);
        }
    }

    void append_debugger_span(ast::code_color_buffer& buffer, const ast::AST_COLOR color, std::string text) {
        if (!text.empty()) {
            buffer.emplace_back(color, std::move(text));
        }
    }

    bool debugger_token_is_number(std::string_view token) {
        if (token.empty()) {
            return false;
        }
        if (token.starts_with("0x") || token.starts_with("0X")) {
            return token.size() > 2 && std::all_of(token.begin() + 2, token.end(), [](const char c) {
                return std::isxdigit(static_cast<unsigned char>(c)) != 0;
            });
        }
        return std::all_of(token.begin(), token.end(), [](const char c) {
            return std::isdigit(static_cast<unsigned char>(c)) != 0;
        });
    }

    void append_debugger_instruction_text(ast::code_color_buffer& buffer, std::string_view text) {
        std::size_t token_index = 0;
        std::size_t pos = 0;
        while (pos < text.size()) {
            const std::size_t start = pos;
            if (std::isspace(static_cast<unsigned char>(text[pos])) != 0) {
                while (pos < text.size() && std::isspace(static_cast<unsigned char>(text[pos])) != 0) {
                    ++pos;
                }
                append_debugger_span(buffer, ast::AST_COLOR::BLANK, std::string(text.substr(start, pos - start)));
                continue;
            }

            while (pos < text.size() && std::isspace(static_cast<unsigned char>(text[pos])) == 0) {
                ++pos;
            }
            const std::string_view token = text.substr(start, pos - start);
            ast::AST_COLOR color = ast::AST_COLOR::IDENTIFIER;
            if (token_index <= 5 || debugger_token_is_number(token)) {
                color = ast::AST_COLOR::NUMBER;
            } else if (token_index == 6) {
                color = ast::AST_COLOR::KEYWORD;
            } else if (token.find('#') != std::string_view::npos) {
                color = ast::AST_COLOR::SID;
            }
            append_debugger_span(buffer, color, std::string(token));
            ++token_index;
        }
    }

    void append_debugger_symbol_value(ast::code_color_buffer& buffer, const SymbolTable& table, const u32 index, const ast::full_type& type) {
        const location value_location = table.m_location + static_cast<u64>(index) * sizeof(u64);
        const u64 raw = value_location.get<u64>();

        std::visit(
            [&](auto&& entry) -> void {
                using T = std::decay_t<decltype(entry)>;
                if constexpr (std::is_same_v<T, ast::primitive_type>) {
                    switch (entry.m_type) {
                        case ast::primitive_kind::I32: {
                            append_debugger_span(buffer, ast::AST_COLOR::TYPE, "int: ");
                            append_debugger_span(buffer, ast::AST_COLOR::NUMBER, std::format("{}", value_location.get<i32>()));
                            return;
                        }
                        case ast::primitive_kind::I64: {
                            append_debugger_span(buffer, ast::AST_COLOR::TYPE, "int64: ");
                            append_debugger_span(buffer, ast::AST_COLOR::NUMBER, std::format("{}", value_location.get<i64>()));
                            return;
                        }
                        case ast::primitive_kind::U64: {
                            append_debugger_span(buffer, ast::AST_COLOR::TYPE, "uint64: ");
                            append_debugger_span(buffer, ast::AST_COLOR::NUMBER, std::format("{}", raw));
                            return;
                        }
                        case ast::primitive_kind::F32: {
                            append_debugger_span(buffer, ast::AST_COLOR::TYPE, "float: ");
                            append_debugger_span(buffer, ast::AST_COLOR::NUMBER, std::format("{}", value_location.get<f32>()));
                            return;
                        }
                        case ast::primitive_kind::STRING: {
                            append_debugger_span(buffer, ast::AST_COLOR::TYPE, "string pointer: ");
                            append_debugger_span(buffer, ast::AST_COLOR::STRING, std::format("0x{:016X}", raw));
                            return;
                        }
                        case ast::primitive_kind::SID: {
                            append_debugger_span(buffer, ast::AST_COLOR::TYPE, "sid: ");
                            append_debugger_span(buffer, ast::AST_COLOR::SID, int_to_string_id(static_cast<sid64>(raw)));
                            return;
                        }
                        default: {
                            append_debugger_span(buffer, ast::AST_COLOR::TYPE, std::format("{}: ", ast::kind_to_string(entry.m_type)));
                            append_debugger_span(buffer, ast::AST_COLOR::NUMBER, std::format("0x{:016X}", raw));
                            return;
                        }
                    }
                } else if constexpr (std::is_same_v<T, ast::function_type>) {
                    append_debugger_span(buffer, ast::AST_COLOR::TYPE, "function: ");
                    append_debugger_span(buffer, ast::AST_COLOR::SID, int_to_string_id(static_cast<sid64>(raw)));
                    append_debugger_span(buffer, ast::AST_COLOR::BLANK, " ");
                    append_debugger_span(buffer, ast::AST_COLOR::TYPE, ast::type_to_declaration_string(type));
                } else if constexpr (std::is_same_v<T, ast::ptr_type>) {
                    append_debugger_span(buffer, ast::AST_COLOR::TYPE, "pointer: ");
                    append_debugger_span(buffer, ast::AST_COLOR::NUMBER, std::format("0x{:016X}", raw));
                    append_debugger_span(buffer, ast::AST_COLOR::BLANK, " (");
                    append_debugger_span(buffer, ast::AST_COLOR::TYPE, ast::type_to_declaration_string(type));
                    append_debugger_span(buffer, ast::AST_COLOR::BLANK, ")");
                } else if constexpr (std::is_same_v<T, std::monostate>) {
                    append_debugger_span(buffer, ast::AST_COLOR::TYPE, "unknown: ");
                    append_debugger_span(buffer, ast::AST_COLOR::NUMBER, std::format("0x{:016X}", raw));
                } else {
                    append_debugger_span(buffer, ast::AST_COLOR::TYPE, std::format("{}: ", ast::type_to_declaration_string(type)));
                    append_debugger_span(buffer, ast::AST_COLOR::NUMBER, std::format("0x{:016X}", raw));
                }
            },
            type
        );
    }

    void debugger_state::render(const dconstruct::debugger::debugger_snapshot& snapshot) {
        m_registers = snapshot.m_generalPurposeRegisters;
        m_argRegisters = snapshot.m_argumentRegisters;

        m_instructionsAndSymbols.clear();
        if (snapshot.m_func == nullptr) {
            append_debugger_span(m_instructionsAndSymbols, ast::AST_COLOR::COMMENT, "; no debugger snapshot\n");
            return;
        }

        const function_disassembly& func = *snapshot.m_func;
        m_instructionsAndSymbols.reserve(func.m_lines.size() * 96 + func.m_stackFrame.m_symbolTable.m_types.size() * 72 + 256);
        append_debugger_span(m_instructionsAndSymbols, ast::AST_COLOR::COMMENT, std::format("; {}\n\n", func.get_id()));
        append_debugger_span(m_instructionsAndSymbols, ast::AST_COLOR::KEYWORD, "INSTRUCTIONS");
        append_debugger_span(m_instructionsAndSymbols, ast::AST_COLOR::PUNCTUATION, ":\n");

        for (const function_disassembly_line& line : func.m_lines) {
            append_debugger_instruction_text(m_instructionsAndSymbols, line.m_text);
            if (!line.m_comment.empty()) {
                const u32 padding = static_cast<u32>(std::max(2, 64 - static_cast<i32>(line.m_text.size())));
                append_debugger_span(m_instructionsAndSymbols, ast::AST_COLOR::BLANK, std::string(padding, ' '));
                append_debugger_span(m_instructionsAndSymbols, ast::AST_COLOR::COMMENT, line.m_comment);
            }
            append_debugger_span(m_instructionsAndSymbols, ast::AST_COLOR::BLANK, "\n");
        }

        append_debugger_span(m_instructionsAndSymbols, ast::AST_COLOR::BLANK, "\n");
        append_debugger_span(m_instructionsAndSymbols, ast::AST_COLOR::KEYWORD, "SYMBOL TABLE");
        append_debugger_span(m_instructionsAndSymbols, ast::AST_COLOR::PUNCTUATION, ":\n");
        const SymbolTable& symbol_table = func.m_stackFrame.m_symbolTable;
        if (symbol_table.m_types.empty()) {
            append_debugger_span(m_instructionsAndSymbols, ast::AST_COLOR::COMMENT, "; empty\n");
            return;
        }

        for (u32 i = 0; i < symbol_table.m_types.size(); ++i) {
            append_debugger_span(m_instructionsAndSymbols, ast::AST_COLOR::NUMBER, std::format("{:04X}", i));
            append_debugger_span(m_instructionsAndSymbols, ast::AST_COLOR::BLANK, "  ");
            append_debugger_symbol_value(m_instructionsAndSymbols, symbol_table, i, symbol_table.m_types[i]);
            append_debugger_span(m_instructionsAndSymbols, ast::AST_COLOR::BLANK, "\n");
        }
    }

    void draw_debugger_vm_panel(app_state& state, const ImVec2& size) {
        ImGui::BeginChild("##dbg_vm_inner", size, ImGuiChildFlags_None, ImGuiWindowFlags_AlwaysVerticalScrollbar);

        debugger_state& dbg = state.m_debugger;

        debugger_section_header("Registers");
        draw_vm_register_grid(state, "##dbg_vm_registers", "r", dbg.m_registers, VM_NORMAL_DEFAULT_ROWS, dbg.m_expandNormal);

        ImGui::Dummy(ImVec2(0.0F, 12.0F));

        debugger_section_header("Arguments");
        draw_vm_register_grid(state, "##dbg_vm_arguments", "a", dbg.m_argRegisters, VM_ARGUMENT_DEFAULT_ROWS, dbg.m_expandArgs);

        ImGui::EndChild();
    }

    enum class debug_icon {
        continue_run,
        step_instruction,
        step_line
    };

    void debugger_draw_control_icon(ImDrawList* dl, debug_icon icon, ImVec2 center, f32 radius, ImU32 color) {
        const f32 thickness = std::max(1.6F, radius * 0.2F);
        switch (icon) {
            case debug_icon::continue_run: {
                dl->AddTriangleFilled(
                    ImVec2(center.x - radius * 0.55F, center.y - radius * 0.85F),
                    ImVec2(center.x - radius * 0.55F, center.y + radius * 0.85F),
                    ImVec2(center.x + radius * 0.9F, center.y),
                    color
                );
                break;
            }
            case debug_icon::step_instruction: {
                const f32 shaft_top = center.y - radius * 0.95F;
                const f32 shaft_bottom = center.y + radius * 0.1F;
                dl->AddLine(ImVec2(center.x, shaft_top), ImVec2(center.x, shaft_bottom), color, thickness);
                dl->AddTriangleFilled(
                    ImVec2(center.x - radius * 0.5F, shaft_bottom),
                    ImVec2(center.x + radius * 0.5F, shaft_bottom),
                    ImVec2(center.x, shaft_bottom + radius * 0.55F),
                    color
                );
                dl->AddCircleFilled(ImVec2(center.x, center.y + radius * 1.05F), radius * 0.27F, color);
                break;
            }
            case debug_icon::step_line: {
                const ImVec2 arc_center(center.x, center.y + radius * 0.2F);
                const f32 arc_radius = radius * 0.95F;
                dl->PathArcTo(arc_center, arc_radius, IM_PI, IM_PI * 2.0F, 20);
                dl->PathStroke(color, ImDrawFlags_None, thickness);
                const f32 tip_x = arc_center.x + arc_radius;
                dl->AddTriangleFilled(
                    ImVec2(tip_x - radius * 0.34F, arc_center.y - radius * 0.34F),
                    ImVec2(tip_x + radius * 0.34F, arc_center.y - radius * 0.34F),
                    ImVec2(tip_x, arc_center.y + radius * 0.3F),
                    color
                );
                dl->AddCircleFilled(ImVec2(center.x, arc_center.y + radius * 0.05F), radius * 0.27F, color);
                break;
            }
        }
    }

    bool debugger_control_button(const char* id, debug_icon icon, const ImVec4& color, const char* tooltip) {
        const f32 size = ImGui::GetFrameHeight();
        const ImVec2 origin = ImGui::GetCursorScreenPos();
        const bool pressed = ImGui::Button(id, ImVec2(size, size));
        if (tooltip != nullptr && ImGui::IsItemHovered()) {
            ImGui::SetTooltip("%s", tooltip);
        }
        ImDrawList* dl = ImGui::GetWindowDrawList();
        const ImVec2 center(origin.x + size * 0.5F, origin.y + size * 0.5F);
        debugger_draw_control_icon(dl, icon, center, size * 0.28F, ImGui::GetColorU32(color));
        return pressed;
    }

    void seed_debugger_sids(debugger_state& dbg) {
        if (dbg.m_sidsSeeded) {
            return;
        }
        constexpr std::array sids{SID("ss-main-menu-lightbar-controller player-ellie (on ((update)))")};
        dbg.m_debugger.set_sids(sids.begin(), sids.end());
        dbg.m_sidsSeeded = true;
    }

    void request_debugger_attach(debugger_state& dbg) {
        seed_debugger_sids(dbg);
        if (std::optional<std::string> attach_error = dbg.m_debugger.request_attach()) {
            dbg.m_debugger.append_output(std::format("[debugger] error: {}\n", *attach_error));
        }
    }

    void clear_debugger_snapshot_view(debugger_state& dbg) {
        dbg.m_snapshot.reset();
        dbg.m_instructionsAndSymbols.clear();
        dbg.m_registers.fill(0);
        dbg.m_argRegisters.fill(0);
    }

    void update_debugger_from_backend(debugger_state& dbg) {
        using debugger_type = dconstruct::debugger::debugger;

        const debugger_type::STATE state = dbg.m_debugger.poll_state();
        const bool state_changed = !dbg.m_hasPollState || state != dbg.m_lastPollState;
        debugger_type::STATE final_state = state;

        switch (state) {
            case debugger_type::STATE::DETACHED: {
                if (state_changed) {
                    dbg.m_debugger.append_output("[debugger] detached\n");
                }
                break;
            }
            case debugger_type::STATE::ATTACHING: {
                if (state_changed) {
                    dbg.m_debugger.append_output("[debugger] waiting for attach...\n");
                }
                break;
            }
            case debugger_type::STATE::ATTACHED: {
                if (state_changed) {
                    dbg.m_debugger.append_output("[debugger] attached, but no bp hit yet.\n");
                }
                break;
            }
            case debugger_type::STATE::SNAPSHOT_READY: {
                std::shared_ptr snapshot = dbg.m_debugger.poll_snapshot();
                final_state = dbg.m_debugger.poll_state();
                if (snapshot != nullptr) {
                    dbg.m_snapshot = std::move(snapshot);
                    dbg.render(*dbg.m_snapshot);
                    dbg.m_debugger.append_output(std::format("[debugger] got function: {}\n", dbg.m_snapshot->m_func->get_id()));
                }
                break;
            }
            case debugger_type::STATE::ERROR: {
                if (std::optional<std::string> error = dbg.m_debugger.poll_error()) {
                    dbg.m_debugger.append_output(std::format("[debugger] error: {}\n", *error));
                } else if (state_changed) {
                    dbg.m_debugger.append_output("[debugger] error\n");
                }
                dbg.m_debugger.detach();
                final_state = dbg.m_debugger.poll_state();
                break;
            }
        }

        dbg.m_lastPollState = final_state;
        dbg.m_hasPollState = true;
    }

    void draw_debugger_controls(debugger_state& dbg) {
        const f32 button_size = ImGui::GetFrameHeight();
        const f32 spacing = ImGui::GetStyle().ItemSpacing.x;
        const dconstruct::debugger::debugger::STATE state = dbg.m_debugger.poll_state();
        const bool attached = dbg.m_debugger.is_attached();
        const bool attaching = state == dconstruct::debugger::debugger::STATE::ATTACHING;
        const char* attach_label = attached ? "Attached to tlou-ii.exe" : attaching ? "Attaching..." : "Attach to tlou-ii.exe process";
        const f32 attach_width = ImGui::CalcTextSize(attach_label).x + ImGui::GetStyle().FramePadding.x * 2.0F;
        const f32 total_width = attach_width + button_size * 3.0F + spacing * 3.0F;
        const f32 avail = ImGui::GetContentRegionAvail().x;
        if (total_width < avail) {
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (avail - total_width) * 0.5F);
        }

        ImGui::BeginDisabled(attached || attaching);
        if (ImGui::Button(attach_label, ImVec2(attach_width, button_size))) {
            request_debugger_attach(dbg);
        }
        ImGui::EndDisabled();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, qui::color::rgba(0x00, 0x00, 0x00, 0x00));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, qui::color::active_palette().TableRowBackgroundAlt);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, qui::color::active_palette().TableRowBackgroundAlt);

        ImGui::BeginDisabled(!attached);
        if (debugger_control_button("##dbg_continue", debug_icon::continue_run, qui::color::active_palette().AccentGreen, "Continue")) {
            dbg.m_debugger.append_output("[debugger] continue\n");
        }
        ImGui::SameLine();
        if (debugger_control_button("##dbg_step_instruction", debug_icon::step_instruction, qui::color::active_palette().AccentYellow, "Next instruction")) {
            dbg.m_debugger.append_output("[debugger] step instruction\n");
        }
        ImGui::SameLine();
        if (debugger_control_button("##dbg_step_line", debug_icon::step_line, qui::color::active_palette().ButtonHovered, "Next line")) {
            dbg.m_debugger.append_output("[debugger] step line\n");
        }
        ImGui::EndDisabled();

        ImGui::PopStyleColor(3);
    }

    void draw_debugger_view(app_state& state) {
        const ImVec2 avail = ImGui::GetContentRegionAvail();
        if (avail.x <= 1.0F || avail.y <= 1.0F) {
            return;
        }

        debugger_state& dbg = state.m_debugger;
        update_debugger_from_backend(dbg);
        const bool attached = dbg.m_debugger.is_attached();
        const bool panels_disabled = !attached && dbg.m_snapshot == nullptr;

        const f32 panels_height = std::max(MIN_PANEL_WIDTH, avail.y - DEBUGGER_OUTPUT_HEIGHT - ImGui::GetStyle().ItemSpacing.y);

        if (dbg.m_leftWidth <= 0.0F) {
            dbg.m_leftWidth = avail.x * 0.28F;
        }
        if (dbg.m_middleWidth <= 0.0F) {
            dbg.m_middleWidth = avail.x * 0.40F;
        }

        const f32 splitters = SPLITTER_WIDTH * 2.0F;
        const f32 max_total = std::max(MIN_PANEL_WIDTH, avail.x - splitters - MIN_PANEL_WIDTH);
        dbg.m_leftWidth = std::clamp(dbg.m_leftWidth, MIN_PANEL_WIDTH, max_total - MIN_PANEL_WIDTH);
        const f32 middle_max = std::max(MIN_PANEL_WIDTH, avail.x - splitters - dbg.m_leftWidth - MIN_PANEL_WIDTH);
        dbg.m_middleWidth = std::clamp(dbg.m_middleWidth, MIN_PANEL_WIDTH, middle_max);

        const f32 left_block_width = dbg.m_leftWidth + SPLITTER_WIDTH + dbg.m_middleWidth;
        ImGui::BeginChild("##dbg_left_block", ImVec2(left_block_width, panels_height));
        {
            draw_debugger_controls(dbg);

            ImGui::BeginDisabled(panels_disabled);
            const f32 panels_row_height = std::max(1.0F, ImGui::GetContentRegionAvail().y - ImGui::GetStyle().ItemSpacing.y);

            ImGui::BeginChild("##dbg_left", ImVec2(dbg.m_leftWidth, panels_row_height));
            {
                ImVec2 inner_size;
                if (begin_labeled_table_frame("Instructions & Symbol Table", inner_size)) {
                    if (dbg.m_snapshot != nullptr && !dbg.m_instructionsAndSymbols.empty()) {
                        std::vector<qui::code::colored_span> spans;
                        spans.reserve(dbg.m_instructionsAndSymbols.size());
                        for (const auto& [color, text] : dbg.m_instructionsAndSymbols) {
                            const ImVec4 resolved = scheme_lookup(code_color_key(color), qui::color::active_palette().Text);
                            spans.push_back({qui::code::to_u32(resolved), text});
                        }
                        qui::code::code_window_colored("##dbg_instructions", spans, inner_size, scheme_code_theme());
                    } else {
                        const std::array<qui::code::colored_span, 2> spans{{
                            {qui::code::to_u32(scheme_lookup(code_color_key(ast::AST_COLOR::COMMENT), qui::color::active_palette().TextDisabled)), "; instructions and symbol table\n"},
                            {qui::code::to_u32(scheme_lookup(code_color_key(ast::AST_COLOR::COMMENT), qui::color::active_palette().TextDisabled)), "; (waiting for breakpoint snapshot)\n"}
                        }};
                        qui::code::code_window_colored("##dbg_instructions", spans, inner_size, scheme_code_theme());
                    }
                }
            }
            ImGui::EndChild();

            ImGui::SameLine(0.0F, 0.0F);
            debugger_vertical_splitter("##dbg_splitter_left", panels_row_height, dbg.m_leftWidth);
            ImGui::SameLine(0.0F, 0.0F);

            ImGui::BeginChild("##dbg_middle", ImVec2(0.0F, panels_row_height));
            {
                ImVec2 inner_size;
                if (begin_labeled_table_frame("Decompilation", inner_size)) {
                    const std::array<qui::code::colored_span, 1> spans{{
                        {qui::code::to_u32(qui::color::active_palette().TextDisabled), "// decompilation will appear here"}
                    }};
                    qui::code::code_window_colored("##dbg_decomp", spans, inner_size, scheme_code_theme());
                }
            }
            ImGui::EndChild();
            ImGui::EndDisabled();
        }
        ImGui::EndChild();

        ImGui::SameLine(0.0F, 0.0F);
        ImGui::BeginDisabled(panels_disabled);
        debugger_vertical_splitter("##dbg_splitter_middle", panels_height, dbg.m_middleWidth);
        ImGui::EndDisabled();
        ImGui::SameLine(0.0F, 0.0F);

        ImGui::BeginDisabled(panels_disabled);
        ImGui::BeginChild("##dbg_right", ImVec2(0.0F, panels_height));
        {
            ImVec2 inner_size;
            if (begin_labeled_table_frame("Virtual Machine", inner_size)) {
                draw_debugger_vm_panel(state, inner_size);
            }
        }
        ImGui::EndChild();
        ImGui::EndDisabled();

        ImVec2 output_size;
        if (begin_labeled_table_frame("Output", output_size)) {
            ImGui::BeginChild("##dbg_output", output_size, ImGuiChildFlags_None, ImGuiWindowFlags_AlwaysVerticalScrollbar);
            const f32 scroll_y = ImGui::GetScrollY();
            const f32 scroll_max_y = ImGui::GetScrollMaxY();
            const bool stick_to_bottom = scroll_max_y <= 0.0F || scroll_y >= scroll_max_y - 2.0F;
            if (ImFont* mono = qui::font_medium(); mono != nullptr) {
                ImGui::PushFont(mono);
            }
            ImGui::PushStyleColor(ImGuiCol_Text, qui::color::active_palette().TextDisabled);
            const std::string& output = dbg.m_debugger.output();
            if (output.empty()) {
                ImGui::TextUnformatted("Output will appear here.");
            } else {
                ImGui::TextUnformatted(output.c_str());
                if (stick_to_bottom) {
                    ImGui::SetScrollHereY(1.0F);
                }
            }
            ImGui::PopStyleColor();
            if (qui::font_medium() != nullptr) {
                ImGui::PopFont();
            }
            ImGui::EndChild();
        }
    }

    void draw_content_area(f32 top_offset, app_state& state) {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + top_offset));
        ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, viewport->Size.y - top_offset));

        constexpr ImGuiWindowFlags window_flags =
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoBringToFrontOnFocus;

        ImGui::Begin("##dconstruct_content", nullptr, window_flags);

        if (!state.m_pendingDropPaths.empty()) {
            std::vector<std::string> paths = std::move(state.m_pendingDropPaths);
            state.m_pendingDropPaths.clear();
            for (const std::string& path : paths) {
                load_bin_file(state, path);
            }
        }

        const ImGuiIO& io = ImGui::GetIO();
        if (!io.WantTextInput && io.KeyCtrl && !io.KeyShift && ImGui::IsKeyPressed(ImGuiKey_O, false)) {
            const std::string path = prompt_open_file(state.m_window, "Binary files (*.bin)\0*.bin\0All files (*.*)\0*.*\0", "Open .bin file");
            if (!path.empty()) {
                load_bin_file(state, path);
            }
        }
        if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_S, false)) {
            save_active_document(state);
        }
        if (!io.WantTextInput && io.KeyCtrl && !io.KeyShift && ImGui::IsKeyPressed(ImGuiKey_Z, false)) {
            undo_active_document(state);
        }
        if (!io.WantTextInput && io.KeyCtrl &&
            (ImGui::IsKeyPressed(ImGuiKey_Y, false) || (io.KeyShift && ImGui::IsKeyPressed(ImGuiKey_Z, false)))) {
            redo_active_document(state);
        }

        if (state.m_debugger.m_active) {
            draw_status_text(state);
            ImGui::Dummy(ImVec2(0.0F, 0.0F));
            draw_debugger_view(state);
            ImGui::End();
            return;
        }

        if (state.m_documents.empty()) {
            draw_empty_state(state);
            ImGui::End();
            return;
        }

        draw_status_text(state);
        ImGui::Dummy(ImVec2(0.0F, 0.0F));

        constexpr ImGuiTabBarFlags tab_bar_flags =
            ImGuiTabBarFlags_Reorderable |
            ImGuiTabBarFlags_AutoSelectNewTabs |
            ImGuiTabBarFlags_FittingPolicyScroll;

        if (ImGui::BeginTabBar("##dconstruct_tabs", tab_bar_flags)) {
            i32 to_close = -1;
            for (i32 i = 0; i < static_cast<i32>(state.m_documents.size()); ++i) {
                document& doc = state.m_documents[static_cast<u32>(i)];
                bool open = true;
                const std::string label = doc.m_name + (doc.m_dirty ? " \xE2\x97\x8F" : "") + "##doc" + std::to_string(i);
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

        if (document* doc = active_document(state)) {
            draw_document(state, *doc);
            process_pending_edits(state, *doc);
        }

        ImGui::End();
    }

    void* settings_read_open(ImGuiContext*, ImGuiSettingsHandler* handler, const char* name) {
        return std::strcmp(name, "Data") == 0 ? handler->UserData : nullptr;
    }

    void settings_read_line(ImGuiContext*, ImGuiSettingsHandler*, void* entry, const char* line) {
        auto* state = static_cast<app_state*>(entry);
        int value = 0;
        if (std::sscanf(line, "DefaultViewDcpl=%d", &value) == 1) {
            state->m_defaultViewDcpl = value != 0;
        } else if (std::sscanf(line, "AstOptimization=%d", &value) == 1) {
            const bool enabled = value != 0;
            state->m_ssoVarOptimization = enabled;
            state->m_foreachOptimization = enabled;
            state->m_matchOptimization = enabled;
            state->m_secondVarOptimization = enabled;
            state->m_memberAccessOptimization = enabled;
        } else if (std::sscanf(line, "RegexOptimization=%d", &value) == 1) {
            state->m_regexOptimization = value != 0;
        } else if (std::sscanf(line, "SsoVarOptimization=%d", &value) == 1) {
            state->m_ssoVarOptimization = value != 0;
        } else if (std::sscanf(line, "ForeachOptimization=%d", &value) == 1) {
            state->m_foreachOptimization = value != 0;
        } else if (std::sscanf(line, "MatchOptimization=%d", &value) == 1) {
            state->m_matchOptimization = value != 0;
        } else if (std::sscanf(line, "SecondVarOptimization=%d", &value) == 1) {
            state->m_secondVarOptimization = value != 0;
        } else if (std::sscanf(line, "MemberAccessOptimization=%d", &value) == 1) {
            state->m_memberAccessOptimization = value != 0;
        } else {
            char scheme[64];
            if (std::sscanf(line, "ColorScheme=%63s", scheme) == 1) {
                state->m_colorScheme = scheme;
            }
        }
    }

    void settings_write_all(ImGuiContext*, ImGuiSettingsHandler* handler, ImGuiTextBuffer* buf) {
        const auto* state = static_cast<const app_state*>(handler->UserData);
        buf->appendf("[%s][Data]\n", handler->TypeName);
        buf->appendf("DefaultViewDcpl=%d\n", state->m_defaultViewDcpl ? 1 : 0);
        buf->appendf("SsoVarOptimization=%d\n", state->m_ssoVarOptimization ? 1 : 0);
        buf->appendf("ForeachOptimization=%d\n", state->m_foreachOptimization ? 1 : 0);
        buf->appendf("MatchOptimization=%d\n", state->m_matchOptimization ? 1 : 0);
        buf->appendf("SecondVarOptimization=%d\n", state->m_secondVarOptimization ? 1 : 0);
        buf->appendf("MemberAccessOptimization=%d\n", state->m_memberAccessOptimization ? 1 : 0);
        buf->appendf("RegexOptimization=%d\n", state->m_regexOptimization ? 1 : 0);
        buf->appendf("ColorScheme=%s\n", state->m_colorScheme.c_str());
        buf->append("\n");
    }

    void register_ini_settings(app_state& state) {
        ImGuiSettingsHandler handler;
        handler.TypeName = "dconstruct";
        handler.TypeHash = ImHashStr("dconstruct");
        handler.ReadOpenFn = settings_read_open;
        handler.ReadLineFn = settings_read_line;
        handler.WriteAllFn = settings_write_all;
        handler.UserData = &state;
        ImGui::AddSettingsHandler(&handler);
    }

}

int main() {
    glfwSetErrorCallback(dconstruct::ui::glfw_error_callback);
    if (glfwInit() == GLFW_FALSE) {
        return 1;
    }

    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    GLFWwindow* window = glfwCreateWindow(1900, 1200, "dconstruct", nullptr, nullptr);
    if (window == nullptr) {
        glfwTerminate();
        return 1;
    }
    glfwSetWindowSizeLimits(window, 720, 450, GLFW_DONT_CARE, GLFW_DONT_CARE);
    dconstruct::ui::install_window_proc(window);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
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

    dconstruct::ui::app_state state;
    dconstruct::ui::register_ini_settings(state);
    state.m_window = window;
    state.m_iconTexture = dconstruct::ui::create_icon_texture();
    state.m_nameArt = dconstruct::ui::load_text_resource(L"DCONSTRUCT_NAME_ART");
    state.m_wordmarkArt = dconstruct::ui::load_text_resource(L"DCONSTRUCT_WORDMARK_ART");
    state.m_errorBox.popup_id = "##dconstruct_error_box";
    state.m_errorBox.selectable = true;
    state.m_errorBox.width = 600.0F * dpi_scale;
    state.m_closeBox.popup_id = "##dconstruct_close_box";
    state.m_closeBox.width = 540.0F * dpi_scale;
    glfwSetWindowUserPointer(window, &state);
    glfwSetWindowCloseCallback(window, dconstruct::ui::window_close_callback);

    const HRESULT ole_result = OleInitialize(nullptr);
    HWND drop_hwnd = glfwGetWin32Window(window);
    dconstruct::ui::file_drop_target* drop_target = nullptr;
    if (SUCCEEDED(ole_result) && drop_hwnd != nullptr) {
        DragAcceptFiles(drop_hwnd, false);
        drop_target = new dconstruct::ui::file_drop_target(&state);
        if (FAILED(RegisterDragDrop(drop_hwnd, drop_target))) {
            drop_target->Release();
            drop_target = nullptr;
        }
    }

    dconstruct::ui::log_event("Start");
    if (std::filesystem::exists("sidbase.bin")) {
        dconstruct::ui::load_sidbase(state, "sidbase.bin");
    }
    dconstruct::ui::load_var_maps_directory(state);

    while (glfwWindowShouldClose(window) == GLFW_FALSE) {
        glfwPollEvents();

        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0) {
            glfwWaitEvents();
            continue;
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        qui::update_window_resize(window, viewport->Pos, viewport->Size, qui::default_window_bar_state());

        // A hovered Settings entry requests a preview for the next frame so the
        // whole UI (including the title/menu bar drawn first) re-themes, not
        // just the content area drawn after the hover is detected.
        dconstruct::ui::set_active_scheme(state.m_previewScheme.empty() ? state.m_colorScheme : state.m_previewScheme);
        state.m_previewScheme.clear();

        const f32 bar_height = dconstruct::ui::draw_title_menu_bar(state, window);
        dconstruct::ui::draw_content_area(bar_height, state);
        dconstruct::ui::process_message_boxes(state);
        dconstruct::ui::draw_drag_overlay(state, bar_height);

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

    if (io.IniFilename != nullptr) {
        ImGui::SaveIniSettingsToDisk(io.IniFilename);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    if (drop_target != nullptr) {
        RevokeDragDrop(drop_hwnd);
        drop_target->Release();
    }
    if (SUCCEEDED(ole_result)) {
        OleUninitialize();
    }

    dconstruct::ui::uninstall_window_proc(window);
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
