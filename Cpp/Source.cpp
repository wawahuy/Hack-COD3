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
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only

    // get monitor
    //GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    //if (!monitor) {
    //    return 0;
    //}
    //const int width = glfwGetVideoMode(monitor)->width;
    //const int height = glfwGetVideoMode(monitor)->height;

    // get process game
    AppProcess* p = AppProcess::Create(_T("Call of Duty®: Modern Warfare® 3"));
    uintptr_t gameBaseAddress = p->getBaseAddress();
    if (!p->isOpen()) {
        std::cout << "Game don't open!" << std::endl;
        return 0;
    }
    glm::u32vec2 gameWindowSize = p->getWindowSize();
    glm::u32vec2 gameWindowPosition = p->getWindowPosition();

    // hint
    glfwWindowHint(GLFW_FLOATING, true);
    glfwWindowHint(GLFW_RESIZABLE, false);
    //glfwWindowHint(GLFW_MAXIMIZED, true);
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, true);

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(gameWindowSize.x, gameWindowSize.y, "Dear ImGui GLFW+OpenGL3 example", NULL, NULL);
    if (window == NULL)
        return 1;

    // Attr
    glfwSetWindowPos(window, gameWindowPosition.x, gameWindowPosition.y);
    glfwSetWindowAttrib(window, GLFW_DECORATED, false);
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
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // goooooo!
    bool visibleMenu = true;
    glfwSetWindowAttrib(window, GLFW_MOUSE_PASSTHROUGH, !visibleMenu);

    struct {
        glm::fvec3 p, v1, v2, v3;
    } data;
    union {
        struct
        {
            glm::fvec4 a, b, c, d;
        };
        glm::fmat4 mat;
    } matrixView;
    glm::mat4 mProjection = glm::perspective(45.0f, (GLfloat)gameWindowSize.x / (GLfloat)gameWindowSize.y, 1.0f, 15000.0f);
    uintptr_t addressPlayer;
    if (!p->readProcessMemory(gameBaseAddress + address::EBase::Player, addressPlayer)) {
        return 0;
    }

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

        // read data
        if (!p->readProcessMemory(gameBaseAddress + address::ViewMatrix, data)) {
            std::cout << "Dont find view matrix!" << std::endl;
            return 0;
        }

        //
        glm::vec3 cameraPos = data.p;
        glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
        matrixView.mat = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        if (visibleMenu) {
            ImGui::Text(glm::to_string(data.p).c_str());
            ImGui::Text(glm::to_string(matrixView.a).c_str());
            ImGui::Text(glm::to_string(matrixView.b).c_str());
            ImGui::Text(glm::to_string(matrixView.c).c_str());
            ImGui::Text(glm::to_string(matrixView.d).c_str());

            if (ImGui::Button("Test")) {
                return 0;
            }
        }        

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();

        glColor3f(1.0f, 1.0f, 0);

        glm::fvec4 positionPlayer(0, -1.0f, 0, 0);
        for (int i = 0; i < 14; i++) {
            uintptr_t entityAddress;
            if (p->readProcessMemory(gameBaseAddress + address::Entity + address::Next * i, entityAddress)) {
                glm::vec3 positionEntity;
                if (p->readProcessMemory(entityAddress + address::EntityVec3, positionEntity)) {
                    glm::vec4 screenReal = mProjection * matrixView.mat * glm::vec4(positionEntity, 1.0f);
                    std::cout << glm::to_string(positionEntity) << " vs " << glm::to_string(screenReal) << std::endl;
                    glBegin(GL_LINES);
                    glVertex3fv((float*)&positionPlayer);
                    glVertex3fv((float*)&screenReal);
                    glEnd();
                }
            }
        }



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
