#include <algorithm>
#include <cctype>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <memory>
#include <optional>
#include <print>
#include <span>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "base.h"
#include "binaryfile.h"
#include "sidbase.h"
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

    struct edit_record {
        u32 m_offset = 0;
        edit_kind m_kind = edit_kind::Int;
        std::vector<std::byte> m_oldBytes;
        std::vector<std::byte> m_newBytes;
        std::string m_oldText;
        std::string m_newText;
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
        bool m_entrySortAlphabetically = false;
        bool m_entrySortDescending = false;
        ImGuiID m_menuTarget = 0;
        bool m_openMenu = false;
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
        ast::function_to_mapped_vars m_functionScopes;
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
        qui::message_box m_errorBox;
        qui::message_box m_closeBox;
        bool m_closeRequested = false;
        bool m_defaultViewDcpl = true;
        bool m_astOptimization = true;
        bool m_regexOptimization = true;
        bool m_dragHover = false;
        std::vector<std::string> m_pendingDropPaths;
        std::unordered_map<sid64, ast::full_type> m_typeMap;
        std::unordered_map<std::string, ast::function_to_mapped_vars> m_pendingTypeMaps;
        std::string m_colorScheme = "qntm";
    };

    void glfw_error_callback(int error, const char* description) {
        std::fprintf(stderr, "GLFW error %d: %s\n", error, description);
    }

    unsigned int create_icon_texture() {
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

    std::string load_name_art() {
        HMODULE module = GetModuleHandleW(nullptr);
        HRSRC resource = FindResourceW(module, L"DCONSTRUCT_NAME_ART", MAKEINTRESOURCEW(10));
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

        if (GetOpenFileNameA(&ofn) == TRUE) {
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

    void decompile_document(app_state& state, document& doc) {
        doc.m_decompiled.clear();
        doc.m_decompErrors.clear();
        doc.m_lambdaViewDcpl.clear();
        if (doc.m_disassembler == nullptr || doc.m_file == nullptr || state.m_sidbase == nullptr) {
            return;
        }

        const dcompiler::OPTIMIZATION_KIND optimizations =
            state.m_astOptimization ? dcompiler::OPTIMIZATION_KIND::AST : dcompiler::OPTIMIZATION_KIND::NONE;

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
            const ast::function_definition& def = decomp_func.decompile(optimizations);
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
        ImGui::PushStyleColor(ImGuiCol_PopupBg, qui::color::retina_dark::WindowBackground);

        if (ImGui::BeginPopupModal(
            "##dconstruct_about",
            nullptr,
            ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove
        )) {
            if (ImFont* title_font = qui::font_bold(); title_font != nullptr) {
                ImGui::PushFont(title_font, title_font->LegacySize * 1.4F);
                ImGui::PushStyleColor(ImGuiCol_Text, qui::color::retina_dark::Highlight);
                ImGui::TextUnformatted("dconstruct");
                ImGui::PopStyleColor();
                ImGui::PopFont();
            }

            ImGui::PushStyleColor(ImGuiCol_Text, qui::color::retina_dark::TextDisabled);
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
            };

            ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(16.0F, 5.0F));
            if (ImGui::BeginTable("##about_thanks", 2, ImGuiTableFlags_SizingFixedFit)) {
                for (const credit& entry : credits) {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::PushStyleColor(ImGuiCol_Text, qui::color::retina_dark::Highlight);
                    ImGui::TextUnformatted(entry.name);
                    ImGui::PopStyleColor();
                    ImGui::TableSetColumnIndex(1);
                    ImGui::PushStyleColor(ImGuiCol_Text, qui::color::retina_dark::TextDisabled);
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

    void draw_setting_description(const char* text) {
        ImGui::PushStyleColor(ImGuiCol_Text, qui::color::retina_dark::TextDisabled);
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
        ImGui::SetNextWindowPos(viewport->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5F, 0.5F));

        constexpr f32 settings_width = 560.0F;
        ImGui::SetNextWindowSizeConstraints(ImVec2(settings_width, 0.0F), ImVec2(settings_width, FLT_MAX));

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(28.0F, 24.0F));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.0F);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0F, 8.0F));
        ImGui::PushStyleColor(ImGuiCol_PopupBg, qui::color::retina_dark::WindowBackground);

        if (ImGui::BeginPopupModal(
            "##dconstruct_settings",
            nullptr,
            ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove
        )) {
            if (ImFont* title_font = qui::font_bold(); title_font != nullptr) {
                ImGui::PushFont(title_font, title_font->LegacySize * 1.4F);
                ImGui::PushStyleColor(ImGuiCol_Text, qui::color::retina_dark::Highlight);
                ImGui::TextUnformatted("Settings");
                ImGui::PopStyleColor();
                ImGui::PopFont();
            }

            ImGui::Dummy(ImVec2(0.0F, 6.0F));
            ImGui::Separator();
            ImGui::Dummy(ImVec2(0.0F, 6.0F));

            const ImVec2 sw_size = view_switch_size();
            ImGui::TextUnformatted("Default function view");
            ImGui::SameLine();
            ImGui::SetCursorPosX(ImGui::GetContentRegionMax().x - sw_size.x);
            if (draw_view_switch("##settings_default_view", &state.m_defaultViewDcpl, sw_size)) {
                ImGui::MarkIniSettingsDirty();
            }

            ImGui::Dummy(ImVec2(0.0F, 12.0F));
            ImGui::Separator();
            ImGui::Dummy(ImVec2(0.0F, 6.0F));

            if (ImFont* section_font = qui::font_semi_bold(); section_font != nullptr) {
                ImGui::PushFont(section_font);
                ImGui::TextUnformatted("Decompilation optimizations");
                ImGui::PopFont();
            } else {
                ImGui::TextUnformatted("Decompilation optimizations");
            }
            ImGui::Dummy(ImVec2(0.0F, 6.0F));

            const ImVec2 toggle_size = toggle_switch_size();
            bool optimizations_changed = false;

            ImGui::TextUnformatted("AST optimization");
            ImGui::SameLine();
            ImGui::SetCursorPosX(ImGui::GetContentRegionMax().x - toggle_size.x);
            if (draw_toggle_switch("##settings_ast_opt", &state.m_astOptimization)) {
                ImGui::MarkIniSettingsDirty();
                optimizations_changed = true;
            }
            draw_setting_description(
                "Cleans up the decompiled syntax tree before it is printed: removes unused temporary "
                "variables and rewrites common patterns into 'foreach' loops and 'match' expressions, "
                "so the output reads closer to hand-written code.");

            ImGui::Dummy(ImVec2(0.0F, 8.0F));

            ImGui::TextUnformatted("Regex optimization");
            ImGui::SameLine();
            ImGui::SetCursorPosX(ImGui::GetContentRegionMax().x - toggle_size.x);
            if (draw_toggle_switch("##settings_regex_opt", &state.m_regexOptimization)) {
                ImGui::MarkIniSettingsDirty();
                optimizations_changed = true;
            }
            draw_setting_description(
                "Runs text replacements over the finished output: rewrites 'new-boxed-value' calls into "
                "typed 'boxed_*' helpers, turns pointer arithmetic like '(*var + (i * n))' into 'var[i]', "
                "and collapses foreach boilerplate. Purely cosmetic touch-ups to the printed code.");

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
        ImGui::PushStyleColor(ImGuiCol_WindowBg, qui::color::retina_dark::MenuBarBackground);
        ImGui::PushStyleColor(ImGuiCol_MenuBarBg, qui::color::retina_dark::MenuBarBackground);
        ImGui::Begin("##dconstruct_title_menu_bar", nullptr, flags);

        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        const ImVec2 bar_min = ImGui::GetWindowPos();
        const ImVec2 bar_max(bar_min.x + ImGui::GetWindowWidth(), bar_min.y + height);

        draw_list->AddLine(
            ImVec2(bar_min.x, bar_max.y - 1.0F),
            ImVec2(bar_max.x, bar_max.y - 1.0F),
            ImGui::ColorConvertFloat4ToU32(qui::color::retina_dark::Border)
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
                ImGui::PushStyleColor(ImGuiCol_Text, qui::color::retina_dark::Highlight);
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
                if (ImGui::MenuItem("Parse type definition file...")) {
                    const std::string path = prompt_open_file(window, "Type definition files (*.dcpl)\0*.dcpl\0All files (*.*)\0*.*\0", "Open type definition file");
                    if (!path.empty()) {
                        load_type_defs(state, path);
                    }
                }
                if (ImGui::MenuItem("Load type map...")) {
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
            ImGui::PushStyleColor(ImGuiCol_Text, qui::color::retina_dark::Highlight);
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
                ImGui::ColorConvertFloat4ToU32(qui::color::retina_dark::Text),
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
            qui::status_label("No sidbase loaded \xE2\x80\x94 load one via File > Load Sidbase.", qui::color::retina_dark::AccentRed);
        }

        if (!state.m_loadError.empty()) {
            qui::status_label(state.m_loadError.c_str(), qui::color::retina_dark::AccentRed);
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
        const char* primary = "Drop a .bin file here or use File \xE2\x86\x92 Load... to start editing";

        ImFont* art_font = qui::font_medium();
        const char* art_begin = state.m_nameArt.c_str();
        const char* art_end = art_begin + state.m_nameArt.size();
        f32 art_font_size = ImGui::GetFontSize();
        ImVec2 art_size(0.0F, 0.0F);
        if (!state.m_nameArt.empty()) {
            if (art_font != nullptr) {
                art_size = art_font->CalcTextSizeA(art_font_size, FLT_MAX, 0.0F, art_begin, art_end);
            } else {
                art_size = ImGui::CalcTextSize(art_begin, art_end);
            }
            const f32 max_art_width = std::max(available.x - 48.0F, 1.0F);
            if (art_size.x > max_art_width) {
                const f32 scale = max_art_width / art_size.x;
                art_font_size = std::max(8.0F, art_font_size * scale);
                if (art_font != nullptr) {
                    art_size = art_font->CalcTextSizeA(art_font_size, FLT_MAX, 0.0F, art_begin, art_end);
                }
            }
        }

        ImFont* primary_font = qui::font_semi_bold();
        if (primary_font != nullptr) {
            ImGui::PushFont(primary_font);
        }
        const ImVec2 primary_size = ImGui::CalcTextSize(primary);
        const f32 primary_font_size = ImGui::GetFontSize();
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

        f32 block_height = primary_size.y;
        if (!state.m_nameArt.empty()) {
            block_height += art_size.y + art_gap;
        }
        if (!secondary.empty()) {
            block_height += line_gap + secondary_size.y;
        }

        f32 y = center.y - block_height * 0.5F;
        if (!state.m_nameArt.empty()) {
            draw_list->AddText(
                art_font,
                art_font_size,
                ImVec2(center.x - art_size.x * 0.5F, y),
                ImGui::ColorConvertFloat4ToU32(qui::color::retina_dark::Highlight),
                art_begin,
                art_end
            );
            y += art_size.y + art_gap;
        }

        draw_list->AddText(
            primary_font,
            primary_font_size,
            ImVec2(center.x - primary_size.x * 0.5F, y),
            ImGui::ColorConvertFloat4ToU32(qui::color::retina_dark::TextDisabled),
            primary
        );
        y += primary_size.y;

        if (!secondary.empty()) {
            y += line_gap;
            draw_list->AddText(
                ImVec2(center.x - secondary_size.x * 0.5F, y),
                ImGui::ColorConvertFloat4ToU32(qui::color::retina_dark::AccentRed),
                secondary.c_str()
            );
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
            ImGui::ColorConvertFloat4ToU32(qui::color::retina_dark::Text),
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

    void draw_entry_list(app_state& state, document& doc) {
        ImGui::SetNextItemWidth(-1.0F);
        ImGui::InputTextWithHint("##dconstruct_entry_search", "Search entries", &doc.m_entrySearch);

        ImVec2 table_size;
        if (!begin_labeled_table_frame("Entries", table_size)) {
            return;
        }

        if (doc.m_entries == nullptr || state.m_sidbase == nullptr || doc.m_file == nullptr) {
            qui::text_label("Not disassembled.");
            return;
        }

        const std::vector<disassembled_entry>& entries = *doc.m_entries;
        const bool searching = !doc.m_entrySearch.empty();
        std::vector<qui::fuzzy_match> matches;
        if (searching) {
            std::vector<std::string> choices;
            choices.reserve(entries.size());
            for (const disassembled_entry& entry : entries) {
                choices.push_back(state.m_sidbase->lookup(entry.m_nameId, doc.m_file->m_sidCache));
            }
            matches = qui::fuzzy_search(doc.m_entrySearch, choices);
        }
        std::vector<i32> row_indices;
        if (searching) {
            row_indices.reserve(matches.size());
            for (const qui::fuzzy_match& match : matches) {
                row_indices.push_back(static_cast<i32>(match.index));
            }
        } else {
            row_indices.reserve(entries.size());
            for (i32 i = 0; i < static_cast<i32>(entries.size()); ++i) {
                row_indices.push_back(i);
            }
        }

        constexpr ImGuiTableFlags table_flags =
            ImGuiTableFlags_BordersInnerV |
            ImGuiTableFlags_RowBg |
            ImGuiTableFlags_Resizable |
            ImGuiTableFlags_ScrollY |
            ImGuiTableFlags_SizingStretchProp;

        if (ImGui::BeginTable("##dconstruct_entries", 2, table_flags, table_size)) {
            const char* entry_name_header = doc.m_entrySortAlphabetically
                                                ? (doc.m_entrySortDescending ? "Entry Name \xE2\x96\xBC" : "Entry Name \xE2\x96\xB2")
                                                : "Entry Name";

            ImGui::TableSetupColumn(entry_name_header);
            ImGui::TableSetupColumn("Type");
            ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
            for (int column = 0; column < 2; ++column) {
                ImGui::TableSetColumnIndex(column);
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ENTRY_CELL_LEFT_PADDING);
                ImGui::TableHeader(ImGui::TableGetColumnName(column));
                if (column == 0 && ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
                    if (doc.m_entrySortAlphabetically) {
                        doc.m_entrySortDescending = !doc.m_entrySortDescending;
                    } else {
                        doc.m_entrySortAlphabetically = true;
                        doc.m_entrySortDescending = false;
                    }
                }
            }

            if (doc.m_entrySortAlphabetically) {
                std::stable_sort(
                    row_indices.begin(),
                    row_indices.end(),
                    [&](const i32 lhs, const i32 rhs) {
                        const disassembled_entry& left = entries[static_cast<u32>(lhs)];
                        const disassembled_entry& right = entries[static_cast<u32>(rhs)];
                        const std::string left_name = state.m_sidbase->lookup(left.m_nameId, doc.m_file->m_sidCache);
                        const std::string right_name = state.m_sidbase->lookup(right.m_nameId, doc.m_file->m_sidCache);
                        if (doc.m_entrySortDescending) {
                            return right_name < left_name;
                        }
                        return left_name < right_name;
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
                    ImGui::PushStyleColor(ImGuiCol_Text, qui::color::retina_dark::TextDisabled);
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

    using code_color_map = std::unordered_map<ast::AST_COLOR, ImVec4>;

    const code_color_map& qntm_color_map() {
        using qui::color::rgba;
        static const code_color_map map = {
            {ast::AST_COLOR::BLANK, rgba(0xCC, 0xCC, 0xCC)},
            {ast::AST_COLOR::NUMBER, rgba(0xB5, 0xCE, 0xA8)},
            {ast::AST_COLOR::SID, rgba(0x4F, 0xC1, 0xFF)},
            {ast::AST_COLOR::IDENTIFIER, rgba(0x9C, 0xDC, 0xFE)},
            {ast::AST_COLOR::MEMBER, rgba(0x9C, 0xDC, 0xFE)},
            {ast::AST_COLOR::TYPE, rgba(0x4E, 0xC9, 0xB0)},
            {ast::AST_COLOR::CALL, rgba(0xDC, 0xDC, 0xAA)},
            {ast::AST_COLOR::KEYWORD, rgba(0xC5, 0x86, 0xC0)},
            {ast::AST_COLOR::STRING, rgba(0xAE, 0x33, 0x44)},
            {ast::AST_COLOR::COMMENT, rgba(0x6A, 0x99, 0x55)},
            {ast::AST_COLOR::OPERATOR, rgba(0xD4, 0xD4, 0xD4)},
            {ast::AST_COLOR::PUNCTUATION, rgba(0xCC, 0xCC, 0xCC)},
        };
        return map;
    }

    const std::unordered_map<std::string, code_color_map>& color_schemes() {
        static const std::unordered_map<std::string, code_color_map> schemes = {
            {"qntm", qntm_color_map()},
        };
        return schemes;
    }


    const code_color_map& active_color_map(const app_state& state) {
        const auto& schemes = color_schemes();
        if (const auto it = schemes.find(state.m_colorScheme); it != schemes.end()) {
            return it->second;
        }
        return qntm_color_map();
    }

    void render_decompiled_code(const char* id, const app_state& state, const ast::code_color_buffer& code) {
        const code_color_map& scheme = active_color_map(state);
        std::vector<qui::code::colored_span> spans;
        spans.reserve(code.size());
        for (const auto& [color, text] : code) {
            const auto it = scheme.find(color);
            const ImVec4 resolved = it != scheme.end() ? it->second : qui::color::retina_dark::Text;
            spans.push_back({qui::code::to_u32(resolved), text});
        }
        qui::code::code_window_colored(id, spans);
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
        ImGui::PushStyleColor(ImGuiCol_Text, qui::color::retina_dark::Text);
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

    bool dv_node(value_view v, const void* id, const ImVec4& color, const char* label, const char* suffix, bool leaf, const char* member_name = nullptr, const char* index_prefix = nullptr) {
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
            ImGui::TextColored(qui::color::retina_dark::TextDisabled, "  %s", shown_suffix);
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
        ImVec4 color = val_color::Struct;
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
            const bool open = dv_node(v, id, val_color::Array, label, suffix, entry.m_values.empty(), member_name, prefix);
            if (open && !entry.m_values.empty()) {
                dv_draw_values(v, entry.m_values);
                dv_tree_pop(v);
            }
            return;
        }

        ImVec4 color = val_color::Struct;
        std::string type_storage;
        const char* type_name = "anonymous struct";
        if (entry.m_typeId != 0) {
            type_storage = v.state->m_sidbase->lookup(entry.m_typeId, v.doc->m_file->m_sidCache);
            type_name = type_storage.c_str();
            if (entry.m_typeId == SID("state-script")) {
                color = val_color::StateScript;
            } else if (entry.m_typeId == SID("script-lambda")) {
                color = val_color::Function;
            }
        }

        std::snprintf(label, sizeof(label), "%s%s", prefix, type_name);
        std::snprintf(suffix, sizeof(suffix), "[0x%05X]", static_cast<u32>(entry.m_offset));
        const bool open = dv_node(v, id, color, label, suffix, entry.m_values.empty(), member_name, prefix);
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
            return val_color::Int;
        }
        if (prefix == "float") {
            return val_color::Float;
        }
        if (prefix == "sid" || prefix == "pointer") {
            return val_color::Sid;
        }
        if (prefix == "function") {
            return val_color::Function;
        }
        if (prefix == "string") {
            return val_color::String;
        }
        return val_color::Struct;
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
            ImGui::TextColored(qui::color::retina_dark::TextDisabled, "  %s", shown_suffix);
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
                dv_node(v, entry, *entry == 0 ? val_color::IntZero : val_color::Int, label, ": u8", true, member_name, prefix);
            } else if constexpr (std::is_same_v<T, const u16*>) {
                std::snprintf(label, sizeof(label), "%s%u", prefix, *entry);
                dv_node(v, entry, *entry == 0 ? val_color::IntZero : val_color::Int, label, ": u16", true, member_name, prefix);
            } else if constexpr (std::is_same_v<T, const u32*>) {
                std::snprintf(label, sizeof(label), "%s%u", prefix, *entry);
                dv_node(v, entry, *entry == 0 ? val_color::IntZero : val_color::Int, label, ": u32", true, member_name, prefix);
            } else if constexpr (std::is_same_v<T, const i32*>) {
                std::snprintf(label, sizeof(label), "%s%d", prefix, *entry);
                dv_editable_leaf(v, entry, entry, edit_kind::Int, *entry == 0 ? val_color::IntZero : val_color::Int, label, ": int", prefix, member_name);
            } else if constexpr (std::is_same_v<T, const u64*>) {
                const std::string resolved = v.state->m_sidbase->lookup(*entry, v.doc->m_file->m_sidCache);
                std::snprintf(label, sizeof(label), "%s%s", prefix, resolved.c_str());
                dv_editable_leaf(v, entry, entry, edit_kind::Sid, val_color::Sid, label, ": sid", prefix, member_name);
            } else if constexpr (std::is_same_v<T, const f32*>) {
                std::snprintf(label, sizeof(label), "%s%.2f", prefix, *entry);
                dv_editable_leaf(v, entry, entry, edit_kind::Float, val_color::Float, label, ": float", prefix, member_name);
            } else if constexpr (std::is_same_v<T, const char*>) {
                std::snprintf(label, sizeof(label), "%s\"%s\"", prefix, entry != nullptr ? entry : "");
                dv_node(v, entry, val_color::String, label, ": string", true, member_name, prefix);
            } else if constexpr (std::is_same_v<T, const structs::map*>) {
                std::snprintf(label, sizeof(label), "%smap", prefix);
                std::snprintf(
                    suffix,
                    sizeof(suffix),
                    "keys: [0x%05X], values: [0x%05X]",
                    file_offset(*v.doc, entry->keys.data),
                    file_offset(*v.doc, entry->values.data)
                );
                dv_node(v, entry, val_color::Map, label, suffix, true, member_name, prefix);
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
            ImGui::TableSetupColumn("Key", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableHeadersRow();

            const std::size_t count = std::min(keys.m_values.size(), vals.m_values.size());
            for (std::size_t i = 0; i < count; ++i) {
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
        const bool open = dv_node(v, id, val_color::Map, label, suffix, empty);
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
        if (dv_node(v, &instructions_node, val_color::Group, "Instructions", nullptr, false)) {
            if (ImGui::BeginTable("##instructions", 2, table_flags)) {
                ImGui::TableSetupColumn("Instruction", ImGuiTableColumnFlags_WidthFixed, FUNCTION_INSTRUCTION_COLUMN_WIDTH);
                ImGui::TableSetupColumn("Comment", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableHeadersRow();

                for (const function_disassembly_line& line : lines) {
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

        const SymbolTable& symbols = func.m_stackFrame.m_symbolTable;
        if (symbols.m_location.m_ptr != nullptr && !symbols.m_types.empty()) {
            ImGui::SetNextItemOpen(true, ImGuiCond_Once);
            if (dv_node(v, &symbols_node, val_color::Group, "Symbol Table", nullptr, false)) {
                if (ImGui::BeginTable("##symbols", 3, table_flags)) {
                    ImGui::TableSetupColumn("#", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableSetupColumn("Offset", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableSetupColumn("Value");
                    ImGui::TableHeadersRow();

                    const ImVec4 hl = qui::color::retina_dark::Highlight;
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
                        ImGui::TextColored(qui::color::retina_dark::TextDisabled, "0x%06X", file_offset(*v.doc, value_location.m_ptr));
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
        const bool open = dv_node(v, fid, val_color::Function, label, nullptr, false);
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

        draw_list->AddRectFilled(pos, max, ImGui::ColorConvertFloat4ToU32(qui::color::retina_dark::WindowBackground));
        draw_list->AddRect(pos, max, ImGui::ColorConvertFloat4ToU32(qui::color::retina_dark::Border));

        const ImVec2 knob_min(pos.x + t * seg, pos.y);
        const ImVec2 knob_max(knob_min.x + seg, pos.y + size.y);
        ImVec4 accent = qui::color::retina_dark::Highlight;
        if (!hovered) {
            accent.w = 0.85F;
        }
        draw_list->AddRectFilled(knob_min, knob_max, ImGui::ColorConvertFloat4ToU32(accent));

        ImFont* font = qui::font_semi_bold();
        if (font != nullptr) {
            ImGui::PushFont(font);
        }
        const ImU32 active = ImGui::ColorConvertFloat4ToU32(qui::color::retina_dark::WindowBackground);
        const ImU32 inactive = ImGui::ColorConvertFloat4ToU32(qui::color::retina_dark::TextDisabled);
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

        const ImVec4 off_col = qui::color::retina_dark::Border;
        ImVec4 on_col = qui::color::retina_dark::Highlight;
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
        draw_list->AddCircleFilled(knob_center, knob_r, ImGui::ColorConvertFloat4ToU32(qui::color::retina_dark::WindowBackground));

        ImGui::PopID();
        return changed;
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
                    ImGui::PushStyleColor(ImGuiCol_Text, qui::color::retina_dark::AccentRed);
                    ImGui::TextUnformatted(err_it->second.c_str());
                    ImGui::PopStyleColor();
                }
                render_decompiled_code("##dcpl_view", *v.state, it->second);
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
        const bool open = dv_node(v, id, val_color::Function, label, suffix, func == nullptr);

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
        const bool open = dv_node(v, &script, val_color::StateScript, label, nullptr, false);
        if (!open) {
            return;
        }
        ImGui::PushID(&script);

        if (!script.m_options.empty()) {
            if (dv_node(v, &script.m_options, val_color::Group, "options", nullptr, false)) {
                for (const ast::sid_identifier& option : script.m_options) {
                    dv_draw_text_leaf(v, &option, val_color::Sid, option.to_pseudo_c_string());
                }
                dv_tree_pop(v);
            }
        }

        if (!script.m_declarations.empty()) {
            if (dv_node(v, &script.m_declarations, val_color::Group, "declarations", nullptr, false)) {
                for (const ast::variable_declaration& declaration : script.m_declarations) {
                    dv_draw_text_leaf(v, &declaration, val_color::Struct, declaration.to_pseudo_c_string());
                }
                dv_tree_pop(v);
            }
        }

        for (const ast::state_script_state& ss_state : script.m_states) {
            char state_label[256];
            std::snprintf(state_label, sizeof(state_label), "state %s", ss_state.m_name.c_str());
            if (dv_node(v, &ss_state, val_color::StateScript, state_label, nullptr, ss_state.m_blocks.empty()) && !ss_state.m_blocks.empty()) {
                for (const ast::state_script_block& block : ss_state.m_blocks) {
                    const std::string block_name = block.block_type_to_string();
                    if (dv_node(v, &block, val_color::Group, block_name.c_str(), nullptr, block.m_tracks.empty()) && !block.m_tracks.empty()) {
                        for (const ast::state_script_track& track : block.m_tracks) {
                            char track_label[256];
                            std::snprintf(track_label, sizeof(track_label), "track %s", track.m_name.c_str());
                            if (dv_node(v, &track, val_color::Group, track_label, nullptr, track.m_lambdas.empty()) && !track.m_lambdas.empty()) {
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
                                                dv_draw_text_leaf(v, &fn, val_color::Function, fn.to_pseudo_c_string());
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
            ImGui::PushStyleColor(ImGuiCol_Text, qui::color::retina_dark::TextDisabled);
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
        const bool open = dv_node(v, entry_id, val_color::EntryName, name, suffix, entry.m_values.empty());
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
            if (ImGui::MenuItem("Close all children")) {
                doc.m_opPendingTarget = doc.m_menuTarget;
                doc.m_opPendingMode = tree_op::close;
                doc.m_opPendingMaxDepth = 0;
            }
            ImGui::EndPopup();
        }
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

    void update_dirty(document& doc) {
        doc.m_dirty = doc.m_undoStack.size() != doc.m_savedDepth;
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
        apply_record_bytes(*doc, record, true);
        log_event("Redo [{}] 0x{:05X}: {} -> {}", doc->m_name, record.m_offset, record.m_oldText, record.m_newText);
        doc->m_undoStack.push_back(std::move(record));
        update_dirty(*doc);
        redisassemble_preserving(state, *doc);
    }

    void save_active_document(app_state& state) {
        document* doc = active_document(state);
        if (doc == nullptr || doc->m_editor == nullptr || !doc->m_dirty) {
            return;
        }
        if (const error_msg err = doc->m_editor->write_edited_file(); err.has_value()) {
            show_error_box(state, *err);
            return;
        }
        doc->m_savedDepth = doc->m_undoStack.size();
        update_dirty(*doc);
        log_event("Saved file: {}", doc->m_path);
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
            if (!doc.m_dirty || doc.m_editor == nullptr) {
                continue;
            }
            if (const error_msg err = doc.m_editor->write_edited_file(); err.has_value()) {
                show_error_box(state, *err);
                continue;
            }
            doc.m_savedDepth = doc.m_undoStack.size();
            update_dirty(doc);
            log_event("Saved file: {}", doc.m_path);
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
            qui::message_box_button{"Exit without saving", qui::color::retina_dark::AccentRed, true, true},
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
            state->m_astOptimization = value != 0;
        } else if (std::sscanf(line, "RegexOptimization=%d", &value) == 1) {
            state->m_regexOptimization = value != 0;
        }
    }

    void settings_write_all(ImGuiContext*, ImGuiSettingsHandler* handler, ImGuiTextBuffer* buf) {
        const auto* state = static_cast<const app_state*>(handler->UserData);
        buf->appendf("[%s][Data]\n", handler->TypeName);
        buf->appendf("DefaultViewDcpl=%d\n", state->m_defaultViewDcpl ? 1 : 0);
        buf->appendf("AstOptimization=%d\n", state->m_astOptimization ? 1 : 0);
        buf->appendf("RegexOptimization=%d\n", state->m_regexOptimization ? 1 : 0);
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

    GLFWwindow* window = glfwCreateWindow(1600, 900, "dconstruct", nullptr, nullptr);
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
    state.m_nameArt = dconstruct::ui::load_name_art();
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
        DragAcceptFiles(drop_hwnd, FALSE);
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
