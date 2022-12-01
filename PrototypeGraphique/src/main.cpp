#include <cstdio>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <glad/gl.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "demo.h"

void debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
    if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
        return;

    printf("OpenGL error: %s\n", message);
}

int main(int argc, char* argv[])
{
    int screenWidth = 1200;
    int screenHeight = 800;

    if (!glfwInit())
    {
        printf("glfwInit failed\n");
        return -1;
    }

    // Target OpenGL 3.0 API in Compatibility mode
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "GLFW test", nullptr, nullptr);
    if (window == nullptr)
    {
        printf("glfwCreateWindow failed\n");
        return -1;
    }

    glfwMakeContextCurrent(window);
    if (gladLoaderLoadGL() == 0)
    {
        printf("gladLoaderLoadGL failed\n");
        return -1;
    }

    {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(debugCallback, NULL);
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    bool showDemoWindow = false;
    Demo demo;

    while (glfwWindowShouldClose(window) == false)
    {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (showDemoWindow)
            ImGui::ShowDemoWindow(&showDemoWindow);

        ImGui::Checkbox("Show demo window", &showDemoWindow);

        demo.Display();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}