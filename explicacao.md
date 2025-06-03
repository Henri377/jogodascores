# Explicação Completa do Código `jogodascores.cpp`

## Visão Geral

Este código implementa um jogo simples chamado "Jogo das Cores" usando OpenGL (GLEW) e GLFW para renderização gráfica e manipulação de janelas. O objetivo do jogo é clicar em retângulos coloridos em uma grade, removendo todos os retângulos com cores semelhantes ao retângulo clicado. O jogador ganha pontos ao remover retângulos e o jogo termina quando nao exista mais retangulos
---

## Estruturas e Constantes

### Estrutura `Rect`
```cpp
struct Rect {
    float x, y, w, h;   // Posição e tamanho do retângulo
    float r, g, b;      // Cor (RGB)
    bool active = true; // Indica se o retângulo está ativo
};
```
- Cada retângulo tem posição, tamanho, cor e um estado ativo/inativo.

### Constantes
```cpp
const int ROWS = 6, COLS = 10;           // Tamanho da grade
const float RECT_W = 0.18f, RECT_H = 0.25f; // Tamanho dos retângulos
std::vector<Rect> rects;                 // Vetor de retângulos
int score = 20000, tentativas = 0;       // Pontuação e tentativas
```

---

## Shaders

### Vertex Shader (`vs_src`)
Responsável por posicionar os vértices e passar a cor para o fragment shader.
```glsl
#version 330 core
layout(location = 0) in vec2 vp;
layout(location = 1) in vec3 cor;
out vec3 fc;
void main() {
    gl_Position = vec4(vp, 0.0, 1.0);
    fc = cor;
}
```

### Fragment Shader (`fs_src`)
Responsável por colorir os pixels dos retângulos.
```glsl
#version 330 core
in vec3 fc;
out vec4 frg;
void main() {
    frg = vec4(fc, 1.0);
}
```

---

## Funções Auxiliares

### `randomColor`
Gera uma cor RGB aleatória para cada retângulo.
```cpp
void randomColor(float& r, float& g, float& b) {
    r = (rand() % 100) / 100.0f;
    g = (rand() % 100) / 100.0f;
    b = (rand() % 100) / 100.0f;
}
```

### `initRects`
Inicializa a grade de retângulos com posições e cores aleatórias.
```cpp
void initRects() {
    rects.clear();
    float startX = -0.9f, startY = 0.8f;
    for (int y = 0; y < ROWS; ++y) {
        for (int x = 0; x < COLS; ++x) {
            float r, g, b;
            randomColor(r, g, b);
            rects.push_back({
                startX + x * RECT_W, startY - y * RECT_H, RECT_W * 0.9f, RECT_H * 0.9f, r, g, b, true
            });
        }
    }
}
```

### `colorDist`
Calcula a distância euclidiana entre duas cores RGB.
```cpp
float colorDist(float r1, float g1, float b1, float r2, float g2, float b2) {
    return sqrt((r1-r2)*(r1-r2) + (g1-g2)*(g1-g2) + (b1-b2)*(b1-b2));
}
```

### `removeSimilares`
Desativa (remove) todos os retângulos cuja cor é semelhante à cor escolhida (abaixo de um limiar).
```cpp
int removeSimilares(float r, float g, float b, float limiar = 0.25f) {
    int removidos = 0;
    for (auto& rect : rects) {
        if (rect.active && colorDist(rect.r, rect.g, rect.b, r, g, b) < limiar) {
            rect.active = false;
            removidos++;
        }
    }
    return removidos;
}
```

### `screenToGL`
Converte coordenadas de tela (pixels) para coordenadas OpenGL (-1 a 1).
```cpp
void screenToGL(GLFWwindow* win, double sx, double sy, float& x, float& y) {
    int w, h;
    glfwGetFramebufferSize(win, &w, &h);
    x = (sx / w) * 2.0f - 1.0f;
    y = 1.0f - (sy / h) * 2.0f;
}
```

### `pickRect`
Retorna o índice do retângulo clicado, se houver.
```cpp
int pickRect(float mx, float my) {
    for (int i = 0; i < rects.size(); ++i) {
        auto& r = rects[i];
        if (r.active &&
            mx >= r.x && mx <= r.x + r.w &&
            my <= r.y && my >= r.y - r.h)
            return i;
    }
    return -1;
}
```

---

## Função Principal (`main`)

### Inicialização
- Inicializa o GLFW e cria uma janela 800x600.
- Inicializa o GLEW.
- Compila e linka os shaders.
- Cria VAO e VBO para desenhar os retângulos.
- Inicializa a grade de retângulos.

### Loop Principal
```cpp
while (!glfwWindowShouldClose(win)) {
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(prog);
    glBindVertexArray(vao);

    // Desenha todos os retângulos ativos
    for (auto& r : rects) {
        if (!r.active) continue;
        float vertices[] = {
            r.x, r.y, r.r, r.g, r.b,
            r.x + r.w, r.y, r.r, r.g, r.b,
            r.x + r.w, r.y - r.h, r.r, r.g, r.b,
            r.x, r.y - r.h, r.r, r.g, r.b
        };
        GLuint indices[] = {0,1,2, 2,3,0};
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, indices);
    }

    glfwSwapBuffers(win);

    // Input
    glfwPollEvents();
    if (glfwGetMouseButton(win, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        double mx, my;
        glfwGetCursorPos(win, &mx, &my);
        float gx, gy;
        screenToGL(win, mx, my, gx, gy);
        int idx = pickRect(gx, gy);
        if (idx >= 0 && rects[idx].active) {
            tentativas++;
            int removidos = removeSimilares(rects[idx].r, rects[idx].g, rects[idx].b);
            if (removidos > 0) {
                score += removidos * 100 - tentativas * 50;
                std::cout << "Removidos: " << removidos << " | Score: " << score << std::endl;
            }
            glfwWaitEventsTimeout(0.2); // evita múltiplos cliques rápidos
        }
    }
}
```
- Limpa a tela e desenha todos os retângulos ativos.
- Processa o input do mouse: ao clicar, converte a posição do mouse para coordenadas OpenGL, identifica o retângulo clicado e remove todos os retângulos com cor semelhante.
- Atualiza a pontuação: cada retângulo removido vale 100 pontos, mas cada tentativa custa 50 pontos.
- Exibe no console o número de removidos e a pontuação atual.


---



---
