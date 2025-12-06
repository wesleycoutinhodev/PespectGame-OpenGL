# PERSPECT 🧊

> Um jogo de plataforma e puzzle 2.5D construído com uma Engine C++ e OpenGL customizada.

![Status](https://img.shields.io/badge/Status-Em_Desenvolvimento-yellow)
![Language](https://img.shields.io/badge/C++-17-blue)
![OpenGL](https://img.shields.io/badge/OpenGL-3.3-green)

**PERSPECT** explora a mecânica de alteração de perspectiva. O que parece ser um simples jogo de plataforma 2D esconde caminhos secretos e soluções que só podem ser vistas ao alternar para a visão 3D.

---

## 🎮 Funcionalidades (Features)

### Mecânicas de Jogo
* **Troca de Perspectiva:** Alterne instantaneamente entre visão 2D (Ortográfica) e 3D (Perspectiva) para encontrar novos caminhos.
* **Física Customizada:** Sistema de colisão AABB (Axis-Aligned Bounding Box) implementado do zero.
* **Elementos de Puzzle:**
    * 📦 Caixas empurráveis.
    * 🔘 Placas de pressão e Links lógicos.
    * ↕️ Elevadores e Plataformas móveis (com física de momento).
* **Movimentação:** Pulo duplo, gravidade e controle aéreo.

### Tecnologia (Engine)
* **Renderizador OpenGL Moderno:** Suporte a Shaders (GLSL), Texturas e Skybox (Cubemaps).
* **Carregamento de Modelos 3D:** Suporte a arquivos `.OBJ` para o personagem.
* **Level Design Baseado em Texto:** Fases carregadas dinamicamente via arquivos ASCII (`.lvl.txt`), facilitando a criação de mapas.
* **Sistema de Áudio:** Música de fundo (Stream) e efeitos sonoros (SFX) via `raudio`.

---

## 🕹️ Controles

| Tecla | Ação |
| :---: | :--- |
| **W, A, S, D** | Movimentar o personagem |
| **ESPAÇO** | Pular (Pressione novamente para Pulo Duplo) |
| **C** | **Alternar Câmera (2D / 3D)** |
| **ESC** | Sair do jogo |

---

## 🛠️ Tecnologias Utilizadas

Este projeto não utiliza Engines prontas (como Unity ou Unreal). Tudo foi construído usando C++ e bibliotecas de baixo nível:

* **Linguagem:** C++17
* **Gráficos:** OpenGL 3.3
* **Janela & Input:** [GLFW](https://www.glfw.org/)
* **Carregamento de Funções GL:** [GLAD](https://glad.dav1d.de/)
* **Matemática:** [GLM](https://github.com/g-truc/glm)
* **Imagens:** [stb_image](https://github.com/nothings/stb)
* **Modelos 3D:** [tinyobjloader](https://github.com/tinyobjloader/tinyobjloader)
* **Áudio:** [raudio](https://github.com/raysan5/raudio) (wrapper do miniaudio)

---

## 🚀 Como Compilar e Rodar

### Pré-requisitos
* [CMake](https://cmake.org/) (Versão 3.10 ou superior)
* Compilador C++ (Visual Studio MSVC recomendado no Windows, ou GCC/Clang)

### Passo a Passo (Windows/Visual Studio)

1.  **Clone o repositório:**
    ```bash
    git clone [https://github.com/wesleycoutinhodev/PespectGame-OpenGL.git](https://github.com/wesleycoutinhodev/PespectGame-OpenGL.git)
    cd PespectGame-OpenGL
    ```

2.  **Gere os arquivos de build:**
    Abra a pasta no Visual Studio (modo Pasta) ou use o terminal:
    ```bash
    mkdir build
    cd build
    cmake ..
    ```

3.  **Compile:**
    Abra a solução `.sln` gerada ou compile via terminal:
    ```bash
    cmake --build .
    ```

4.  **⚠️ IMPORTANTE - Recursos (Assets):**
    Para o jogo rodar, a pasta `resources` (que contém texturas, sons e shaders) deve estar **na mesma pasta do executável**.
    
    * Copie a pasta `resources/` da raiz do projeto.
    * Cole dentro de `out/build/x64-Debug/` (ou onde o `mygame.exe` foi criado).

---

## 📂 Estrutura de Arquivos (Assets)

O jogo espera a seguinte estrutura de pastas junto ao executável:

```text
MeuJogo/
├── mygame.exe
└── resources/
|    ├── textures/      # (block.png, player.png, skybox/...)
|    ├── audio/         # (music.mp3, jump.wav, coin.mp3)
|
└── fonts/         # (arial.ttf)
├── shaders/ 
