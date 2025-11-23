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

static void error_callback(int error, const char* description)
{
	std::cout << "Error: " << description << "\n";
}

int main(void)
{
	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
#endif

	GLFWwindow* window = glfwCreateWindow(1280, 720, "Template OpenGL", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	enableReportGlErrors();

	//glfwSwapInterval(1); // Descomente para ativar VSync

#pragma region imgui_setup
#if REMOVE_IMGUI == 0
	ImGui::CreateContext();
	ImGui::StyleColorsDark(); // Tema padrão

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Habilita Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Habilita Multi-Janelas

	io.FontGlobalScale = 2.0f; // Escala da fonte

	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.Colors[ImGuiCol_WindowBg].w = 0.f;
		style.Colors[ImGuiCol_DockingEmptyBg].w = 0.f;
	}

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");
#endif
#pragma endregion

	// Inicializa renderizador 2D
	gl2d::init();
	gl2d::Renderer2D renderer;
	renderer.create();

	// Loop Principal
	while (!glfwWindowShouldClose(window))
	{
		// 1. Limpeza e Input
		int width = 0, height = 0;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // Cor de fundo cinza escuro suave
		glClear(GL_COLOR_BUFFER_BIT);

#pragma region imgui_start
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
#pragma endregion

		renderer.updateWindowMetrics(width, height);

		// ---------------------------------------------------------
		// SEU CÓDIGO DO JOGO COMEÇA AQUI
		// ---------------------------------------------------------

		// Exemplo: renderer.renderRectangle({10, 10, 100, 100}, Colors_Red);

		// Descomente a linha abaixo para ver os exemplos do ImGui quando precisar
		// ImGui::ShowDemoWindow(); 

		// ---------------------------------------------------------
		// FIM DO CÓDIGO DO JOGO
		// ---------------------------------------------------------

		renderer.flush(); // Renderiza tudo que foi acumulado

#pragma region imgui_render
		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Atualiza janelas externas do ImGui (Viewports)
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
#pragma endregion

		glfwSwapBuffers(window);
		glfwPollEvents();
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