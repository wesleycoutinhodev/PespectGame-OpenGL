#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <gl2d/gl2d.h>
#include <openglErrorReporting.h>

// --- INCLUDES DO IMGUI ---
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imguiThemes.h"

// --- INCLUDES DO JOGO ---
#include "game.h" 
#include "resourceManager.h"

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

// Instância Global do Jogo
Game PERSPECT(SCR_WIDTH, SCR_HEIGHT);

// Callbacks
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
    // 1. Setup GLFW/GLAD
    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "PERSPECT - Debug Mode", NULL, NULL);
    if (!window) { glfwTerminate(); exit(EXIT_FAILURE); }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) { glfwTerminate(); exit(EXIT_FAILURE); }
    enableReportGlErrors();

    // Configurações OpenGL
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // 2. SETUP IMGUI (INTERFACE DE DEBUG)
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark(); // Tema Escuro

    // Configuração para permitir teclado no ImGui
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Inicializa os backends do ImGui
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // 3. INICIALIZA JOGO E HUD
    PERSPECT.Init();

    gl2d::init();
    gl2d::Renderer2D renderer;
    renderer.create();

    // Tenta carregar a fonte (se falhar, o gl2d usa um quadrado branco, mas não crasha se tratado)
    gl2d::Font font("resources/font.ttf");

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    // --- LOOP PRINCIPAL ---
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Limpa Tela
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Inicia Frame do ImGui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // --- LÓGICA DO JOGO ---
        glfwPollEvents();

        // Se o mouse não estiver sobre uma janela do ImGui, processa o jogo
        if (!io.WantCaptureMouse) {
            PERSPECT.ProcessInput(deltaTime);
        }
        PERSPECT.Update(deltaTime);

        // --- RENDER JOGO ---
        PERSPECT.Render();

        // --- RENDER HUD (GL2D) ---
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        renderer.updateWindowMetrics(width, height);

        std::string coinText = "Moedas: " + std::to_string(PERSPECT.Coins);
        renderer.renderText({ 150, 50 }, coinText.c_str(), font, glm::vec4(1.0f, 0.8f, 0.0f, 1.0f), 0.5f);

        std::string levelText = "Nivel: " + std::to_string(PERSPECT.CurrentLevel + 1);
        renderer.renderText({ (float)width - 150, 50 }, levelText.c_str(), font, glm::vec4(1.0f), 0.f);
        renderer.flush();

        // --- JANELA DE DEBUG (IMGUI) ---
        ImGui::Begin("Debug Tool (Física)"); // Cria a janela

        ImGui::Text("FPS: %.1f", io.Framerate);
        ImGui::Separator();

        // Controle de Câmera
        ImGui::Text("Modo de Visão:");
        ImGui::Checkbox("3D (Perspectiva)", &PERSPECT.Is3DMode);

        ImGui::Separator();

        // Controles do Player (Só se o player existir)
        if (PERSPECT.MyPlayer) {
            ImGui::Text("Propriedades do Player:");

            // Slider para Velocidade (1.0 a 20.0)
            ImGui::SliderFloat("Velocidade", &PERSPECT.MyPlayer->MoveSpeed, 1.0f, 20.0f);

            // Slider para Pulo (1.0 a 30.0) - Aumente aqui para testar super pulos
            ImGui::SliderFloat("Força do Pulo", &PERSPECT.MyPlayer->JumpForce, 1.0f, 30.0f);

            // Visualizar Velocidade Atual
            ImGui::Text("Velocidade Y (Atual): %.2f", PERSPECT.MyPlayer->Velocity.y);

            // Resetar Posição (Teleporte)
            if (ImGui::Button("Teleportar p/ Inicio")) {
                PERSPECT.MyPlayer->Position = glm::vec3(2.0f, 0.0f, 0.0f);
                PERSPECT.MyPlayer->Velocity = glm::vec3(0.0f);
            }
        }

        ImGui::Separator();

        // Controles de Fase
        if (ImGui::Button("Recarregar Fase (Reset)")) {
            PERSPECT.Init();
        }

        if (ImGui::Button("Pular Fase (Next Level)")) {
            PERSPECT.NextLevel();
        }

        ImGui::End();

        // Renderiza o ImGui na frente de tudo
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Limpeza
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}