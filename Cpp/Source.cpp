#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <iostream>
#include <stdio.h>
#include <functional>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <Windows.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>
#include "Address.h"
#include "AppProcess.h"
#include "Timer.h"
#include "VertextDemo.h"

//#define DEBUG_GRAPHICS

#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

glm::fmat4 createCamera(glm::fvec3 eye, glm::fvec3 forward, glm::fvec3 left, glm::fvec3 up) {
    glm::fmat4 matrixOut = glm::fmat4(1.0f);
    float* matrix = (float*)&matrixOut;
    matrix[0] = left.x;
    matrix[4] = left.y;
    matrix[8] = left.z;
    matrix[1] = up.x;
    matrix[5] = up.y;
    matrix[9] = up.z;
    matrix[2] = forward.x;
    matrix[6] = forward.y;
    matrix[10] = forward.z;
    matrix[12] = -left.x * eye.x - left.y * eye.y - left.z * eye.z;
    matrix[13] = -up.x * eye.x - up.y * eye.y - up.z * eye.z;
    matrix[14] = -forward.x * eye.x - forward.y * eye.y - forward.z * eye.z;
    return matrixOut;
}

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

    // fn
    demoGenerationColor();

#ifdef DEBUG_GRAPHICS
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

    address::SDataCamera dataCamera;
    Timer tickShowCameraVector;

    float rotate = 0;
    const float aspect = (float)gameWindowSize.x / (float)gameWindowSize.y;
    const float fnear = 0.1f;
    const float ffar = 100000.0f;
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspect, fnear, ffar);

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

        glm::mat4 view;
        if (p->readProcessMemory(gameBaseAddress + address::DataCamera, dataCamera)) {
            //view = createCamera(dataCamera.eye, dataCamera.forward, dataCamera.left, dataCamera.up);
            view = glm::lookAt(dataCamera.eye, dataCamera.eye + dataCamera.forward, glm::fvec3(0, 0, 1));
        }

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();

        //-5691.930664, 4043.582275, 1364.362915
        glm::mat4 model =
            glm::translate(glm::fmat4(1.0f), glm::fvec3(-5594.130371, 3809.487549, 1369.621216)) *
            glm::rotate(glm::mat4(1.0f), 12.0f, glm::fvec3(1.0f, 0.5f, 0.25f));

        glLineWidth(1.0f);
        glBegin(GL_TRIANGLES);
        for (int i = 0; i < sizeof(demoVertices) / sizeof(float) / 3; i++) {
            const glm::vec3 vertex = *(glm::fvec3*)&demoVertices[i * 3];
            glColor3ubv(&demoColors[i * 3]);

            const glm::vec4 pModel = model * glm::vec4(vertex, 1.0f);
            const glm::vec4 pView =  view * pModel;
            const glm::vec4 p1 = projection * pView;
            if (p1.w < 0.1f) {
                continue;
            }

            // ndc
            const glm::vec3 pNdc = glm::fvec3(p1) / p1.w;
            glVertex3fv((float*)&pNdc);
        }
        glEnd();


        glBegin(GL_LINES);
        int coutEntity = 0;

        if (tickShowCameraVector.elapsed() > 1000) {
            std::cout << "----" << std::endl;
            std::cout << "position: " << glm::to_string(*(glm::fvec3*)&dataCamera.eye) << std::endl;
            std::cout << "forward: " << glm::to_string(*(glm::fvec3*)&dataCamera.forward) << std::endl;
            std::cout << "left: " << glm::to_string(*(glm::fvec3*)&dataCamera.left) << std::endl;
            std::cout << "up: " << glm::to_string(*(glm::fvec3*)&dataCamera.up) << std::endl;
            std::cout << "matrix: " << glm::to_string(view) << std::endl;
        }

        for (int i = 0; i < 100; i++) {
            DWORD address;
            if (p->readProcessMemory(gameBaseAddress + address::Entity + address::Next * i, address)) {
                int heal;
                if (!p->readProcessMemory(address + address::Heal, heal) || heal <= 0 || heal > 50000) {
                    continue;
                }
                coutEntity++;
                
                glm::fvec3 pos;
                if (p->readProcessMemory(address + address::EntityVec3, pos)) {
                    glColor3ub(255, 255, 0);

                    const glm::vec4 p1 = projection * view * glm::vec4(pos, 1.0f);
                    if (p1.w < 0.1f) {
                        continue;
                    }

                    // ndc
                    const glm::vec3 pNdc = glm::fvec3(p1) / p1.w;
                    if (tickShowCameraVector.elapsed() > 1000) {
                        std::cout << "=>> 0x" << std::hex << address::Next * i << std::endl;
                        std::cout << "pWord: " << glm::to_string(pos) << std::endl;
                        std::cout << "pView: " << glm::to_string(p1) << std::endl;
                        std::cout << "pNdc: " << glm::to_string(pNdc) << std::endl;
                        std::cout << "heal: " << heal << std::endl;
                    }


                    glVertex3f(0, -1.0f, 0);
                    glVertex3fv((float*)&pNdc);
                }
            }
        }
        glEnd();


        if (tickShowCameraVector.elapsed() > 1000) {
            std::cout << "num: " << coutEntity << std::endl;
            tickShowCameraVector.reset();
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
