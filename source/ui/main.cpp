#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <string>

#include <qui.h>

#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <commdlg.h>
#ifndef GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#endif
#include <GLFW/glfw3native.h>
#endif

namespace {

constexpr float TitleBarHeight = 34.0F;
constexpr float MenuBarHeight = 27.0F;

struct AppState {
    std::string loaded_file_path;
    std::string loaded_file_name;
};

void glfw_error_callback(int error, const char *description) {
    std::fprintf(stderr, "GLFW error %d: %s\n", error, description);
}

std::string filename_from_path(const std::string &path) {
    const std::size_t slash = path.find_last_of("/\\");
    return slash == std::string::npos ? path : path.substr(slash + 1);
}

std::string prompt_open_bin_file(GLFWwindow *window) {
#if defined(_WIN32)
    char buffer[MAX_PATH] = {};

    OPENFILENAMEA ofn = {};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = glfwGetWin32Window(window);
    ofn.lpstrFilter = "Binary files (*.bin)\0*.bin\0All files (*.*)\0*.*\0";
    ofn.lpstrFile = buffer;
    ofn.nMaxFile = sizeof(buffer);
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
    ofn.lpstrTitle = "Open .bin file";

    if (GetOpenFileNameA(&ofn) == TRUE) {
        return std::string(buffer);
    }
    return {};
#else
    (void)window;
    return {};
#endif
}

void draw_menu_bar(float top_offset, AppState &state, GLFWwindow *window) {
    const ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + top_offset));
    ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, MenuBarHeight));

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

    ImGui::Begin("##dconstruct_menu_bar", nullptr, flags);

    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Open...", "Ctrl+O")) {
                const std::string path = prompt_open_bin_file(window);
                if (!path.empty()) {
                    state.loaded_file_path = path;
                    state.loaded_file_name = filename_from_path(path);
                }
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit")) {
                glfwSetWindowShouldClose(window, GLFW_TRUE);
            }
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    ImGui::End();

    ImGui::PopStyleColor(2);
    ImGui::PopStyleVar(3);
}

void draw_content_area(float top_offset, const AppState &state) {
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

    if (state.loaded_file_path.empty()) {
        qui::section_label("No file loaded");
        ImGui::Spacing();
        qui::text_label("Use File > Open... to load a .bin file.");
    } else {
        qui::section_label(state.loaded_file_name.c_str());
        ImGui::Spacing();
        qui::text_label("Path");
        ImGui::TextUnformatted(state.loaded_file_path.c_str());
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

    GLFWwindow *window = glfwCreateWindow(1280, 720, "dconstruct", nullptr, nullptr);
    if (window == nullptr) {
        glfwTerminate();
        return 1;
    }
    glfwSetWindowSizeLimits(window, 720, 450, GLFW_DONT_CARE, GLFW_DONT_CARE);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    float x_scale = 1.0F;
    float y_scale = 1.0F;
    glfwGetWindowContentScale(window, &x_scale, &y_scale);
    const float dpi_scale = std::max(x_scale, y_scale);

    qui::apply_retina_dark_theme();
    qui::apply_dpi_scale(dpi_scale);
    qui::load_default_fonts(dpi_scale);

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    AppState state;

    while (glfwWindowShouldClose(window) == GLFW_FALSE) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        qui::draw_glfw_window_bar(window, "dconstruct", TitleBarHeight);
        draw_menu_bar(TitleBarHeight, state, window);
        draw_content_area(TitleBarHeight + MenuBarHeight, state);

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

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
