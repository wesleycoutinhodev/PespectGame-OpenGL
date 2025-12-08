#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <gl2d/gl2d.h>
#include <openglErrorReporting.h>

// ImGui
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imguiThemes.h"

// Game
#include "game.h" 
#include "resourceManager.h"

// ====================================================
// 0. VARIÁVEIS GLOBAIS E CONSTANTES
// ====================================================

const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

Game* g_Game = nullptr;

// ====================================================
// 1. CALLBACKS GLFW
// ====================================================

void error_callback(int error, const char* description) {
    std::cerr << "[GLFW ERROR] " << error << ": " << description << "\n";
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    // Atalho rápido para fechar
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Repassa inputs para o sistema do jogo
    if (g_Game && key >= 0 && key < 1024) {
        if (action == GLFW_PRESS)
            g_Game->Keys[key] = true;
        else if (action == GLFW_RELEASE)
            g_Game->Keys[key] = false;
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// ====================================================
// 2. FUNÇÕES AUXILIARES DE RENDERIZAÇÃO (UI)
// ====================================================

void RenderDebugInterface(ImGuiIO& io) {
    ImGui::Begin("Debug Tool (Física)");

    ImGui::Text("FPS: %.1f", io.Framerate);
    ImGui::Separator();

    if (g_Game) {
        ImGui::Text("Modo de Visão:");
        ImGui::Checkbox("3D (Perspectiva)", &g_Game->Is3DMode);
        ImGui::Separator();

        if (g_Game->MyPlayer) {
            ImGui::Text("Propriedades do Player:");
            ImGui::SliderFloat("Velocidade", &g_Game->MyPlayer->MoveSpeed, 1.0f, 20.0f);
            ImGui::SliderFloat("Força do Pulo", &g_Game->MyPlayer->JumpForce, 1.0f, 30.0f);
            ImGui::Text("Velocidade Y: %.2f", g_Game->MyPlayer->Velocity.y);

            if (ImGui::Button("Teleportar p/ Inicio")) {
                g_Game->MyPlayer->Position = glm::vec3(2.0f, 0.0f, 0.0f);
                g_Game->MyPlayer->Velocity = glm::vec3(0.0f);
            }
        }
        ImGui::Separator();

        if (ImGui::Button("Recarregar Fase (Reset)")) {
            g_Game->Init();
        }
        ImGui::SameLine();
        if (ImGui::Button("Pular Fase (Next Level)")) {
            g_Game->NextLevel();
        }
    }

    ImGui::End();
}

void RenderHUD(gl2d::Renderer2D& renderer, gl2d::Font& font, int windowWidth, int windowHeight) {
    if (!g_Game) return;

    renderer.updateWindowMetrics(windowWidth, windowHeight);

    // Moedas
    std::string coinText = "Moedas: " + std::to_string(g_Game->Coins);
    renderer.renderText({ 150, 50 }, coinText.c_str(), font, glm::vec4(1.0f, 0.8f, 0.0f, 1.0f), 0.5f);

    // Nível
    //std::string levelText = "Nivel: " + std::to_string(g_Game->CurrentLevel + 1);
    //renderer.renderText({ (float)windowWidth - 100, 50 }, levelText.c_str(), font, glm::vec4(1.0f), 0.5f);

    renderer.flush();
}

// ====================================================
// 3. MAIN ENTRY POINT
// ====================================================

int main(void) {
    std::cout << "=== INICIANDO SISTEMA ===" << std::endl;

    // --- 1. Inicialização GLFW ---
    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) {
        std::cerr << "[CRITICAL] GLFW Init failed!" << std::endl;
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "MOONLIGHT - Engine", nullptr, nullptr);
    if (!window) {
        std::cerr << "[CRITICAL] Window creation failed!" << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // --- 2. Inicialização GLAD ---
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "[CRITICAL] GLAD Init failed!" << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // --- 3. Configurações OpenGL ---
    enableReportGlErrors();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // --- 4. Setup ImGui ---
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // --- 5. Inicialização do Jogo ---
    g_Game = new Game(SCR_WIDTH, SCR_HEIGHT);
    g_Game->Init();

    // --- 6. Inicialização GL2D (HUD) ---
    gl2d::init();
    gl2d::Renderer2D renderer;
    renderer.create();
    gl2d::Font font("resources/font.ttf");

    // Variáveis de Tempo
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    std::cout << "=== ENGINE PRONTA. EXECUTANDO LOOP. ===" << std::endl;

    // ====================================================
    // 4. LOOP PRINCIPAL
    // ====================================================
    while (!glfwWindowShouldClose(window)) {
        // Cálculo de DeltaTime
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Limpeza de Buffer
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Frame ImGui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Inputs e Update
        glfwPollEvents();
        if (!io.WantCaptureMouse) {
            g_Game->ProcessInput(deltaTime);
        }
        g_Game->Update(deltaTime);

        // Renderização: Jogo 3D
        g_Game->Render();

        // Renderização: HUD 2D
        int w, h;
        glfwGetFramebufferSize(window, &w, &h);
        RenderHUD(renderer, font, w, h);

        // Renderização: Ferramentas de Debug
        RenderDebugInterface(io);
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap Buffers
        glfwSwapBuffers(window);
    }

    // ====================================================
    // 5. ENCERRAMENTO E LIMPEZA
    // ====================================================
    std::cout << "=== ENCERRANDO SISTEMA ===" << std::endl;

    delete g_Game;

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}