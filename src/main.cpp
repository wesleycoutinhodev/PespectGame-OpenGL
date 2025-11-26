// ... (Includes iguais ao anterior) ...
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <gl2d/gl2d.h>
#include <openglErrorReporting.h>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imguiThemes.h"

#include "game.h" 
#include "resourceManager.h"

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

Game PERSPECT(SCR_WIDTH, SCR_HEIGHT);

void error_callback(int error, const char* description) { std::cout << "Error: " << description << "\n"; }

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) PERSPECT.Keys[key] = true;
        else if (action == GLFW_RELEASE) PERSPECT.Keys[key] = false;
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) { glViewport(0, 0, width, height); }

int main(void)
{
    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "PERSPECT", NULL, NULL);
    if (!window) { glfwTerminate(); exit(EXIT_FAILURE); }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) { glfwTerminate(); exit(EXIT_FAILURE); }
    enableReportGlErrors();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

#pragma region imgui_setup
#if REMOVE_IMGUI == 0
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
#endif
#pragma endregion

    // Inicializa Jogo
    PERSPECT.Init();

    // Inicializa GL2D
    gl2d::init();
    gl2d::Renderer2D renderer;
    renderer.create();

    // --- CARREGAR FONTE (NOVO!) ---
    // Certifique-se de ter "resources/font.ttf"
    gl2d::Font font("resources/font.ttf");
    // ------------------------------

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#pragma region imgui_start
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
#pragma endregion

        // Lógica
        glfwPollEvents();
        PERSPECT.ProcessInput(deltaTime);
        PERSPECT.Update(deltaTime);

        // Render 3D
        PERSPECT.Render();

        // --- RENDERIZAÇÃO 2D (HUD) ---
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        renderer.updateWindowMetrics(width, height);

        std::string coinText = "Moedas: " + std::to_string(PERSPECT.Coins);
        renderer.renderText({ 150, 50 }, coinText.c_str(), font, glm::vec4(1.0f, 0.8f, 0.0f, 1.0f), 0.5f);

        std::string levelText = "Nivel: " + std::to_string(PERSPECT.CurrentLevel + 1);
        renderer.renderText({ (float)width - 150, 50 }, levelText.c_str(), font, glm::vec4(1.0f), 0.5f);

        renderer.flush();
        // -----------------------------

        // ImGui Debug (Se quiser manter)
        ImGui::Begin("Debug");
        ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
        ImGui::End();

#pragma region imgui_render
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#pragma endregion

        glfwSwapBuffers(window);
    }

#if REMOVE_IMGUI == 0
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
#endif

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}