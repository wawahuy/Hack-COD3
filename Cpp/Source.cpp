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
#include "VertexSample.h"
#include "Graphics.h"

#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

int main(int, char**)
{
#ifdef NDEBUG
    ShowWindow(GetConsoleWindow(), SW_HIDE);
#endif

    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

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
    uintptr_t playerBaseAddress;
    if (!p->readProcessMemory(gameBaseAddress + address::Player, playerBaseAddress)) {
        std::cout << "Player not found!" << std::endl;
        return 0;
    }
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

    glm::fvec3 verticesBox[16];
    buildQuadsBox(verticesBox, glm::vec3(20.0f, 20.0f, 50.0f));
    for (int i = 0; i < 16; i++) {
        verticesBox[i] = glm::translate(glm::fmat4(1.0f), glm::fvec3(0.0f, 0.0f, 20.0f)) * glm::fvec4(verticesBox[i], 1.0f);
    }

    bool noReloadBullet = false;
    bool unlimitedBullet = false;
    bool hasEspDraw = true;
    float espColorLine[3] = { 1, 1, 0.0f };
    float espColorBox[3] = { 1, 1, 0.0f };;
    float espColorHeal[3] = { 1, 0.0f, 0.0f };;
    Timer tickRewriteMemory;

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
            ImGui::Checkbox("No reload bullet", &noReloadBullet);
            ImGui::Checkbox("Unlimited bullet", &unlimitedBullet);
            ImGui::Checkbox("ESP", &hasEspDraw);
            if (hasEspDraw) {
                ImGui::ColorEdit3("ESP Color Line", espColorLine, 0);
                ImGui::ColorEdit3("ESP Color Box", espColorBox, 0);
                ImGui::ColorEdit3("ESP Color Heal", espColorHeal, 0);
            }
            if (ImGui::Button("Exit")) {
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

        if (hasEspDraw) {
            glm::mat4 view, pv;
            if (p->readProcessMemory(gameBaseAddress + address::DataCamera, dataCamera)) {
                view = glm::lookAt(dataCamera.eye, dataCamera.eye + dataCamera.forward, glm::fvec3(0, 0, 1));
                pv = projection * view;
            }

            glLineWidth(1.0f);
            int coutEntity = 0;

#ifndef NDEBUG
            if (tickShowCameraVector.elapsed() > 1000) {
                std::cout << "----" << std::endl;
                std::cout << "position: " << glm::to_string(*(glm::fvec3*)&dataCamera.eye) << std::endl;
                std::cout << "forward: " << glm::to_string(*(glm::fvec3*)&dataCamera.forward) << std::endl;
                std::cout << "left: " << glm::to_string(*(glm::fvec3*)&dataCamera.left) << std::endl;
                std::cout << "up: " << glm::to_string(*(glm::fvec3*)&dataCamera.up) << std::endl;
                std::cout << "matrix: " << glm::to_string(view) << std::endl;
            }
#endif

            for (int i = 0; i < 100; i++) {
                DWORD address;
                if (p->readProcessMemory(gameBaseAddress + address::Entity + address::Next * i, address)) {
                    int heal;
                    if (!p->readProcessMemory(address + address::Heal, heal) || heal <= 0 || heal > 50000) {
                        continue;
                    }
                    coutEntity++;

                    glm::fvec3 positionEntity;
                    if (p->readProcessMemory(address + address::EntityVec3, positionEntity)) {
                        // draw box
                        glColor3f(espColorBox[0], espColorBox[1], espColorBox[2]);
                        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                        glBegin(GL_QUADS);
                        for (int i = 0; i < 16; i++) {
                            glm::vec3 pNdc;
                            if (!word2Screen(verticesBox[i], pNdc, pv * glm::translate(glm::fmat4(1.0f), positionEntity))) {
                                continue;
                            }
                            glVertex3fv((float*)&pNdc);
                        }
                        glEnd();


                        // draw heal
                        glColor3f(espColorHeal[0], espColorHeal[1], espColorHeal[2]);
                        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                        glm::vec3 healPosition = positionEntity + glm::fvec3(25.0f, 0, 0);
                        glm::vec3 healNdcStart;
                        if (word2Screen(healPosition, healNdcStart, pv)) {
                            glm::vec3 healNdcEnd;
                            int maxZ = 50;
                            int maxHeal = 100;
                            int z = std::min(heal * maxZ / maxHeal, maxZ);
                            if (word2Screen(healPosition + glm::vec3(0, 0, z), healNdcEnd, pv)) {
                                glLineWidth(5.0f);
                                glBegin(GL_LINES);
                                glVertex3fv((float*)&healNdcStart);
                                glVertex3fv((float*)&healNdcEnd);
                                glEnd();
                            }
                        }

                        // draw line (0, -1) -> entity
                        glm::vec3 pNdc;
                        if (!word2Screen(positionEntity, pNdc, pv)) {
                            continue;
                        }

                        if (tickShowCameraVector.elapsed() > 1000) {
                            std::cout << "=>> 0x" << std::hex << address::Next * i << std::endl;
                            std::cout << "heal: " << std::dec << heal << std::endl;
                            std::cout << "pWord: " << glm::to_string(positionEntity) << std::endl;
                            std::cout << "pNdc: " << glm::to_string(pNdc) << std::endl;
                        }

                        glLineWidth(1.0f);
                        glColor3f(espColorLine[0], espColorLine[1], espColorLine[2]);
                        glBegin(GL_LINES);
                        glVertex3f(0, -1.0f, 0);
                        glVertex3fv((float*)&pNdc);
                        glEnd();
                    }
                }
            }

#ifndef NDEBUG
            if (tickShowCameraVector.elapsed() > 1000) {
                std::cout << "num: " << coutEntity << std::endl;
                tickShowCameraVector.reset();
            }
#endif
        }

        if (tickRewriteMemory.elapsed() > 1000) {
            if (noReloadBullet) {
                int re = 2000;
                if (!p->writeProcessMemory(playerBaseAddress + address::BulletReload1, re)) {
                    std::cout << "Dont write reload bullet!" << std::endl;
                }
            }
            if (unlimitedBullet) {
                int re = 2000;
                if (!p->writeProcessMemory(playerBaseAddress + address::Bullet1, re)) {
                    std::cout << "Dont write bullet !" << std::endl;
                }
            }
            tickRewriteMemory.reset();
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
