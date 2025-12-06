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

const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

Game* g_Game = nullptr;

// Callbacks
void error_callback(int error, const char* description) {
    std::cout << "Error: " << description << "\n";
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (g_Game && key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) g_Game->Keys[key] = true;
        else if (action == GLFW_RELEASE) g_Game->Keys[key] = false;
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

int main(void)
{
    std::cout << "=== INICIANDO APLICACAO ===" << std::endl;

    // 1. Setup GLFW/GLAD
    std::cout << "[1] Configurando callback de erro GLFW..." << std::endl;
    glfwSetErrorCallback(error_callback);

    std::cout << "[2] Inicializando GLFW..." << std::endl;
    if (!glfwInit()) {
        std::cout << "ERRO CRITICO: GLFW nao inicializou!" << std::endl;
        std::cin.get();
        exit(EXIT_FAILURE);
    }
    std::cout << "  -> GLFW OK" << std::endl;

    std::cout << "[3] Configurando window hints..." << std::endl;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif
    std::cout << "  -> Window hints OK" << std::endl;

    std::cout << "[4] Criando janela..." << std::endl;
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "PERSPECT - Debug Mode", NULL, NULL);
    if (!window) {
        std::cout << "ERRO CRITICO: Janela nao foi criada!" << std::endl;
        std::cin.get();
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    std::cout << "  -> Janela criada OK" << std::endl;

    std::cout << "[5] Configurando contexto OpenGL..." << std::endl;
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    std::cout << "  -> Contexto OK" << std::endl;

    std::cout << "[6] Carregando GLAD..." << std::endl;
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "ERRO CRITICO: GLAD nao carregou!" << std::endl;
        std::cin.get();
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    std::cout << "  -> GLAD OK" << std::endl;

    std::cout << "[7] Habilitando error reporting..." << std::endl;
    enableReportGlErrors();
    std::cout << "  -> Error reporting OK" << std::endl;

    // Configurações OpenGL
    std::cout << "[8] Configurando OpenGL..." << std::endl;
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::cout << "  -> OpenGL configurado OK" << std::endl;

    // 2. SETUP IMGUI
    std::cout << "[9] Inicializando ImGui..." << std::endl;
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    std::cout << "  -> ImGui context OK" << std::endl;

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    std::cout << "  -> ImGui IO OK" << std::endl;

    std::cout << "[10] Inicializando backends ImGui..." << std::endl;
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    std::cout << "  -> ImGui backends OK" << std::endl;

    // ============================================
    // 3. AGORA CRIA O GAME (DEPOIS DO OPENGL!)
    // ============================================
    std::cout << "[11] Criando instancia do Game..." << std::endl;
    g_Game = new Game(SCR_WIDTH, SCR_HEIGHT);
    std::cout << "  -> Game criado OK" << std::endl;

    std::cout << "[12] Inicializando GAME..." << std::endl;
    g_Game->Init();
    std::cout << "  -> GAME.Init() completo!" << std::endl;

    std::cout << "[13] Inicializando GL2D..." << std::endl;
    gl2d::init();
    gl2d::Renderer2D renderer;
    renderer.create();
    std::cout << "  -> GL2D OK" << std::endl;

    std::cout << "[14] Carregando fonte..." << std::endl;
    gl2d::Font font("resources/font.ttf");
    std::cout << "  -> Fonte carregada" << std::endl;

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    std::cout << "=== ENTRANDO NO LOOP PRINCIPAL ===" << std::endl;

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

        if (!io.WantCaptureMouse) {
            g_Game->ProcessInput(deltaTime);
        }
        g_Game->Update(deltaTime);

        // --- RENDER JOGO ---
        g_Game->Render();

        // --- RENDER HUD (GL2D) ---
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        renderer.updateWindowMetrics(width, height);

        std::string coinText = "Moedas: " + std::to_string(g_Game->Coins);
        renderer.renderText({ 150, 50 }, coinText.c_str(), font, glm::vec4(1.0f, 0.8f, 0.0f, 1.0f), 0.5f);

        std::string levelText = "Nivel: " + std::to_string(g_Game->CurrentLevel + 1);
        renderer.renderText({ (float)width - 150, 50 }, levelText.c_str(), font, glm::vec4(1.0f), 0.f);
        renderer.flush();

        // --- JANELA DE DEBUG (IMGUI) ---
        ImGui::Begin("Debug Tool (Física)");

        ImGui::Text("FPS: %.1f", io.Framerate);
        ImGui::Separator();

        ImGui::Text("Modo de Visão:");
        ImGui::Checkbox("3D (Perspectiva)", &g_Game->Is3DMode);

        ImGui::Separator();

        if (g_Game->MyPlayer) {
            ImGui::Text("Propriedades do Player:");
            ImGui::SliderFloat("Velocidade", &g_Game->MyPlayer->MoveSpeed, 1.0f, 20.0f);
            ImGui::SliderFloat("Força do Pulo", &g_Game->MyPlayer->JumpForce, 1.0f, 30.0f);
            ImGui::Text("Velocidade Y (Atual): %.2f", g_Game->MyPlayer->Velocity.y);

            if (ImGui::Button("Teleportar p/ Inicio")) {
                g_Game->MyPlayer->Position = glm::vec3(2.0f, 0.0f, 0.0f);
                g_Game->MyPlayer->Velocity = glm::vec3(0.0f);
            }
        }

        ImGui::Separator();

        if (ImGui::Button("Recarregar Fase (Reset)")) {
            g_Game->Init();
        }

        if (ImGui::Button("Pular Fase (Next Level)")) {
            g_Game->NextLevel();
        }

        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    std::cout << "=== ENCERRANDO APLICACAO ===" << std::endl;

    // Limpeza
    delete g_Game;

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    std::cout << "=== APLICACAO ENCERRADA ===" << std::endl;
    return 0;
}