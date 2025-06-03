# Jogo das Cores

Um jogo simples em C++ e OpenGL onde o usuário deve clicar em retângulos de uma grade para remover todos os retângulos com cores similares. A cada tentativa, a pontuação diminui. O objetivo é remover o máximo de retângulos similares com a menor quantidade de tentativas possível.

## Como funciona

- O jogo exibe uma grade de retângulos coloridos.
- Ao clicar em um retângulo, todos os retângulos com cor similar são removidos.
- A pontuação é calculada com base na quantidade de retângulos removidos e no número de tentativas.
- O jogo termina quando a janela é fechada, mostrando a pontuação final no terminal.

## Requisitos

- [GLEW](http://glew.sourceforge.net/) (ex: `glew-2.1.0`)
- [GLFW](https://www.glfw.org/) (ex: `glfw-3.4`)
- OpenGL 3.3+
- Compilador C++ (g++, MinGW, MSYS2, etc.)

## Compilação no Windows

1. Baixe e extraia as bibliotecas GLEW e GLFW.
2. Compile com o comando (ajuste os caminhos conforme suas pastas):
