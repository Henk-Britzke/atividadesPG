#include <iostream>
#include <vector>
#include <random>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Janela
const GLint WIDTH = 800, HEIGHT = 600;

// Estrutura Triangle
struct Triangle {
    glm::vec2 position;
    glm::vec3 color;
};

// Lista de triângulos criados
std::vector<Triangle> triangles;

// Função de utilitário para gerar cor aleatória
glm::vec3 randomColor() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    return glm::vec3(dist(gen), dist(gen), dist(gen));
}

// Função de callback do mouse
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double mx, my;
        glfwGetCursorPos(window, &mx, &my);

        float normX = (float(mx) / WIDTH) * 2.0f - 1.0f;
        float normY = 1.0f - (float(my) / HEIGHT) * 2.0f;

        Triangle tri;
        tri.position = glm::vec2(normX, normY);
        tri.color = randomColor();

        triangles.push_back(tri);
    }
}

// Cria um único VAO para um triângulo padrão
GLuint createTriangleVAO() {
    float vertices[] = {
        -0.1f, -0.1f, 0.0f,
         0.1f, -0.1f, 0.0f,
         0.0f,  0.1f, 0.0f
    };

    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Atributo de posição
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    return VAO;
}

int main() {
    // Inicialização da janela
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Triângulos com GLM", NULL, NULL);
    if (!window) {
        std::cerr << "Erro ao criar janela GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Erro ao inicializar GLAD" << std::endl;
        return -1;
    }

    // Shader simples
    const char* vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        uniform vec2 uPosition;
        void main() {
            gl_Position = vec4(aPos.xy + uPosition, aPos.z, 1.0);
        }
    )";

    const char* fragmentShaderSource = R"(
        #version 330 core
        uniform vec3 uColor;
        out vec4 FragColor;
        void main() {
            FragColor = vec4(uColor, 1.0);
        }
    )";

    // Compila shaders
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // Programa de shader
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Libera shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // VAO padrão
    GLuint triangleVAO = createTriangleVAO();

    // Loop principal
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Limpa tela
        glClearColor(0.2f, 0.25f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Usa o programa de shader
        glUseProgram(shaderProgram);

        // Desenha cada triângulo na lista
        glBindVertexArray(triangleVAO);
        for (const Triangle& tri : triangles) {
            glUniform2fv(glGetUniformLocation(shaderProgram, "uPosition"), 1, glm::value_ptr(tri.position));
            glUniform3fv(glGetUniformLocation(shaderProgram, "uColor"), 1, glm::value_ptr(tri.color));
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }
        glBindVertexArray(0);

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}
