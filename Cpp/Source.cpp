#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <iostream>
#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <Windows.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>
#include "Address.h"
#include "AppProcess.h"
#include "Timer.h"

#define DEBUG_GRAPHICS

#ifdef DEBUG_GRAPHICS
#include "VertextDemo.h"
#endif

#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

int main(int, char**)
{
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

#ifdef DEBUG_GRAPHICS
    // fn
    demoGenerationColor();

    // get monitor
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    if (!monitor) {
        return 0;
    }
    const int width = glfwGetVideoMode(monitor)->width;
    const int height = glfwGetVideoMode(monitor)->height;
    glm::u32vec2 gameWindowSize(width, height);
    glm::u32vec2 gameWindowPosition(0, 0);
#else
    // get process game
    AppProcess* p = AppProcess::Create(_T("Call of Duty®: Modern Warfare® 3"));
    uintptr_t gameBaseAddress = p->getBaseAddress();
    if (!p->isOpen()) {
        std::cout << "Game don't open!" << std::endl;
        return 0;
    }
    glm::u32vec2 gameWindowSize = p->getWindowSize();
    glm::u32vec2 gameWindowPosition = p->getWindowPosition();
#endif

    // hint
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_FLOATING, true);
    glfwWindowHint(GLFW_RESIZABLE, false);
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, true);
#ifdef DEBUG_GRAPHICS
    glfwWindowHint(GLFW_MAXIMIZED, true);
#endif

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(gameWindowSize.x, gameWindowSize.y, "Dear ImGui GLFW+OpenGL3 example", NULL, NULL);
    if (window == NULL)
        return 1;

    // Attr
    glfwSetWindowPos(window, gameWindowPosition.x, gameWindowPosition.y);
    glfwSetWindowAttrib(window, GLFW_DECORATED, false);
    glfwSetWindowAttrib(window, GLFW_MOUSE_PASSTHROUGH, true);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Initialize OpenGL loader
    bool err = glewInit() != GLEW_OK;
    if (err)
    {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
        return 1;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    glEnable(GL_DEPTH_TEST);

    // Goooooo!
    bool visibleMenu = false;

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // key
        if (GetAsyncKeyState(VK_INSERT) & 1) {
            visibleMenu = !visibleMenu;
            glfwSetWindowAttrib(window, GLFW_MOUSE_PASSTHROUGH, !visibleMenu);
        }

        if (visibleMenu) {
            if (ImGui::Button("Test")) {
                return 0;
            }
        }

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();

#ifdef DEBUG_GRAPHICS
        if (GetAsyncKeyState(VK_ESCAPE) & 1) {
            return 0;
        }
        //std::cout << "-----------" << std::endl;
        static float rotate = 0;
        rotate += 0.01f;

        const float aspect = (float)gameWindowSize.x / (float)gameWindowSize.y;
        const float fnear = -2.0f;
        const float ffar = 10.0f;
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspect, fnear, ffar);
        glm::mat4 view = glm::lookAt(glm::vec3(0,0, -10.0f), glm::vec3(0, 0, 0), glm::vec3(0, -1.0f, 0));
        glm::mat4 model = glm::rotate(glm::mat4(1.0f), rotate, glm::fvec3(1.0f, 0.5f, 0.25f));
        glDepthRange(fnear, ffar);

        glLineWidth(3.0f);
        glBegin(GL_TRIANGLES);
        for (int i = 0; i < sizeof(demoVertices) / sizeof(float) / 3; i++) {
            const glm::vec3 vertex = *(glm::fvec3*)&demoVertices[i * 3];
            glColor3ubv(&demoColors[i * 3]);

            const glm::vec4 p1 = projection * view * model * glm::vec4(vertex, 1.0f);
            if (p1.w < 0.1f) {
                continue;
            }

            // ndc
            const glm::vec3 pNdc = glm::fvec3(p1) / p1.w;

            // screen cood
            glm::vec3 screenPos(1.0f);
            screenPos.x = 1 / 2.0f * pNdc.x + 0 + 1 / 2.0f;
            screenPos.y = 1 / 2.0f * pNdc.y + 0 + 1 / 2.0f;
            screenPos.z = ((fnear - ffar) / 2.0f * pNdc.z + fnear + ffar) / 2.0f;

            glVertex3fv((float*)&screenPos);

            //std::cout << glm::to_string(vertex) << " -> " << glm::to_string(p1) << " -> " << glm::to_string(pNdc) << " -> " << glm::to_string(screenPos) << std::endl;
        }
        glEnd();
#endif // DEBUG_GRAPHICS

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
