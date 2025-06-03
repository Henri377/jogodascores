#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <cmath>
#include <iostream>
#include <ctime>
#include <cstdlib>

struct Rect {
    float x, y, w, h;
    float r, g, b;
    bool active = true;
};

const int ROWS = 6, COLS = 10;
const float RECT_W = 0.18f, RECT_H = 0.25f;
std::vector<Rect> rects;
int score = 1000, tentativas = 0;

const char* vs_src = R"(
#version 330 core
layout(location = 0) in vec2 vp;
layout(location = 1) in vec3 cor;
out vec3 fc;
void main() {
    gl_Position = vec4(vp, 0.0, 1.0);
    fc = cor;
}
)";

const char* fs_src = R"(
#version 330 core
in vec3 fc;
out vec4 frg;
void main() {
    frg = vec4(fc, 1.0);
}
)";

// Gera cor aleatória
void randomColor(float& r, float& g, float& b) {
    r = (rand() % 100) / 100.0f;
    g = (rand() % 100) / 100.0f;
    b = (rand() % 100) / 100.0f;
}

// Inicializa a grade de retângulos
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

// Calcula distância euclidiana entre duas cores
float colorDist(float r1, float g1, float b1, float r2, float g2, float b2) {
    return sqrt((r1-r2)*(r1-r2) + (g1-g2)*(g1-g2) + (b1-b2)*(b1-b2));
}

// Remove retângulos similares à cor escolhida
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

// Converte coordenada de tela para OpenGL
void screenToGL(GLFWwindow* win, double sx, double sy, float& x, float& y) {
    int w, h;
    glfwGetFramebufferSize(win, &w, &h);
    x = (sx / w) * 2.0f - 1.0f;
    y = 1.0f - (sy / h) * 2.0f;
}

// Retorna índice do retângulo clicado
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

int main() {
    srand(time(0));
    if (!glfwInit()) return -1;
    GLFWwindow* win = glfwCreateWindow(800, 600, "Jogo das Cores", NULL, NULL);
    if (!win) return -1;
    glfwMakeContextCurrent(win);
    glewInit();

    // Shaders
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vs_src, NULL);
    glCompileShader(vs);
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fs_src, NULL);
    glCompileShader(fs);
    GLuint prog = glCreateProgram();
    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    glLinkProgram(prog);

    // VBO/VAO
    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    initRects();

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

    std::cout << "Pontuação final: " << score << std::endl;
    glfwTerminate();
    return 0;
}