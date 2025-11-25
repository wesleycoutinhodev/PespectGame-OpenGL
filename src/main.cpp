#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <gl2d/gl2d.h>
#include <openglErrorReporting.h>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imguiThemes.h"

// --- SEUS INCLUDES DO JOGO ---
#include "game.h" 
#include "resourceManager.h"

// --- VARIÁVEIS GLOBAIS ---
// Definimos o jogo aqui para poder acessar no callback de teclado
Game DodoGame(1280, 720);

// --- FUNÇÕES DE CALLBACK ---
void error_callback(int error, const char* description)
{
    std::cout << "Error: " << description << "\n";
}

// Essa função avisa a classe Game quando uma tecla é apertada ou solta
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    // Fecha com ESC
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Registra input para o jogo
    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
            DodoGame.Keys[key] = true;
        else if (action == GLFW_RELEASE)
            DodoGame.Keys[key] = false;
    }
}

// Ajusta o tamanho da tela do jogo se a janela for redimensionada
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    // (Opcional) Avisar o Game que mudou o tamanho se precisar recalcular projeção
}

int main(void)
{
    // 1. SETUP INICIAL DO GLFW
    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Dodo Duckie 3D Clone", NULL, NULL);
    if (!window) { glfwTerminate(); exit(EXIT_FAILURE); }

    glfwMakeContextCurrent(window);

    // Configurar callbacks
    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) { glfwTerminate(); exit(EXIT_FAILURE); }

    enableReportGlErrors();

    // 2. CONFIGURAÇÕES OPENGL GLOBAIS
    // CRUCIAL PARA 3D: Habilita o teste de profundidade
    glEnable(GL_DEPTH_TEST);

    // Opcional: Blending para transparência (se precisar depois)
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

#pragma region imgui_setup
#if REMOVE_IMGUI == 0
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.FontGlobalScale = 2.0f;

    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        style.Colors[ImGuiCol_WindowBg].w = 0.f;
        style.Colors[ImGuiCol_DockingEmptyBg].w = 0.f;
    }
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
#endif
#pragma endregion

    // 3. INICIALIZAÇÃO DO SEU JOGO
    // Aqui carregamos shaders, texturas e criamos os objetos
    DodoGame.Init();

    // Inicializa renderizador 2D (caso queira usar gl2d para UI depois)
    gl2d::init();
    gl2d::Renderer2D renderer;
    renderer.create();

    // Variáveis de Tempo
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    // --- LOOP PRINCIPAL ---
    while (!glfwWindowShouldClose(window))
    {
        // Cálculo do DeltaTime (Tempo entre frames)
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Limpeza
        // Importante: Limpar GL_DEPTH_BUFFER_BIT também!
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#pragma region imgui_start
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
#pragma endregion

        // --- LÓGICA DO JOGO ---
        // Input do usuário (Capturado no key_callback ou polled aqui)
        glfwPollEvents();
        DodoGame.ProcessInput(deltaTime);

        // Atualização de física/lógica
        DodoGame.Update(deltaTime);

        // --- RENDERIZAÇÃO DO JOGO 3D ---
        DodoGame.Render();

        // --- RENDERIZAÇÃO 2D (UI/DEBUG) ---
        // Se quiser usar gl2d para texto ou UI sobre o jogo
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        renderer.updateWindowMetrics(width, height);
        // Ex: renderer.renderText({10,10}, "Score: 0", ...);
        renderer.flush();

        // Debug Window do ImGui (Opcional, bom para controlar variáveis em tempo real)
        ImGui::Begin("Debug");
        ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
        ImGui::Checkbox("Modo 3D", &DodoGame.Is3DMode); // Controle direto!
        ImGui::End();

#pragma region imgui_render
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
#pragma endregion

        glfwSwapBuffers(window);
    }

    // Limpeza Final
#if REMOVE_IMGUI == 0
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
#endif

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}