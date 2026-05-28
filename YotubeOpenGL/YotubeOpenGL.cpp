// YotubeOpenGL.cpp : Este archivo contiene la función "main". La ejecución del programa comienza y termina ahí.
//

// YotubeOpenGL.cpp
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>
#include <vector>
#include <cstddef>

#include "shaderClass.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "Camera.h"

const unsigned int width = 800;
const unsigned int height = 800;

int main()
{
    // ======================= GLFW =======================

    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // ======================= WINDOW =======================

    GLFWwindow* window = glfwCreateWindow(width, height, "YouTubeOpenGL", NULL, NULL);

    if (!window)
    {
        std::cout << "Error creando ventana\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // ======================= GLAD =======================

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Error inicializando GLAD\n";
        return -1;
    }

    glViewport(0, 0, width, height);

    // ======================= DEPTH TEST =======================

    glEnable(GL_DEPTH_TEST);

    // ======================= VERTEX STRUCTURE (misma salida, diferente estructura) =======================
    // Definimos una estructura Vertex y usamos std::vector<Vertex> para almacenar
    // los datos en lugar de un arreglo plano.
    struct Vertex {
        float pos[3];
        float color[3];
        float uv[2];
    };

    std::vector<Vertex> vertices = {
        // FRONT
        {{-0.5f, -0.5f,  0.5f}, {1,1,1}, {0.0f, 0.0f}},
        {{ 0.5f, -0.5f,  0.5f}, {1,1,1}, {1.0f, 0.0f}},
        {{ 0.5f,  0.5f,  0.5f}, {1,1,1}, {1.0f, 1.0f}},
        {{-0.5f,  0.5f,  0.5f}, {1,1,1}, {0.0f, 1.0f}},

        // BACK
        {{-0.5f, -0.5f, -0.5f}, {1,1,1}, {0.0f, 0.0f}},
        {{-0.5f,  0.5f, -0.5f}, {1,1,1}, {0.0f, 1.0f}},
        {{ 0.5f,  0.5f, -0.5f}, {1,1,1}, {1.0f, 1.0f}},
        {{ 0.5f, -0.5f, -0.5f}, {1,1,1}, {1.0f, 0.0f}},

        // RIGHT
        {{ 0.5f, -0.5f,  0.5f}, {1,1,1}, {0.0f, 0.0f}},
        {{ 0.5f, -0.5f, -0.5f}, {1,1,1}, {1.0f, 0.0f}},
        {{ 0.5f,  0.5f, -0.5f}, {1,1,1}, {1.0f, 1.0f}},
        {{ 0.5f,  0.5f,  0.5f}, {1,1,1}, {0.0f, 1.0f}},

        // LEFT
        {{-0.5f, -0.5f, -0.5f}, {1,1,1}, {0.0f, 0.0f}},
        {{-0.5f, -0.5f,  0.5f}, {1,1,1}, {1.0f, 0.0f}},
        {{-0.5f,  0.5f,  0.5f}, {1,1,1}, {1.0f, 1.0f}},
        {{-0.5f,  0.5f, -0.5f}, {1,1,1}, {0.0f, 1.0f}},

        // TOP
        {{-0.5f,  0.5f,  0.5f}, {1,1,1}, {0.0f, 0.0f}},
        {{ 0.5f,  0.5f,  0.5f}, {1,1,1}, {1.0f, 0.0f}},
        {{ 0.5f,  0.5f, -0.5f}, {1,1,1}, {1.0f, 1.0f}},
        {{-0.5f,  0.5f, -0.5f}, {1,1,1}, {0.0f, 1.0f}},

        // BOTTOM
        {{-0.5f, -0.5f,  0.5f}, {1,1,1}, {0.0f, 0.0f}},
        {{-0.5f, -0.5f, -0.5f}, {1,1,1}, {1.0f, 0.0f}},
        {{ 0.5f, -0.5f, -0.5f}, {1,1,1}, {1.0f, 1.0f}},
        {{ 0.5f, -0.5f,  0.5f}, {1,1,1}, {0.0f, 1.0f}}
    };

    // ======================= INDICES (6 caras * 2 tri * 3 indices = 36) =======================
    unsigned int indices[] =
    {
        // FRONT
        0, 1, 2, 0, 2, 3,
        // BACK
        4, 5, 6, 4, 6, 7,
        // RIGHT
        8, 9,10, 8,10,11,
        // LEFT
       12,13,14,12,14,15,
        // TOP
       16,17,18,16,18,19,
        // BOTTOM
       20,21,22,20,22,23
    };

    // ======================= SHADER =======================

    Shader shaderProgram("default.vert", "default.frag");

    // ======================= VAO VBO EBO =======================

    VAO VAO1;
    VAO1.Bind();

    // VBO acepta un puntero a datos; reinterpretamos el vector de Vertex a void*
    VBO VBO1(reinterpret_cast<GLfloat*>(vertices.data()), vertices.size() * sizeof(Vertex));
    EBO EBO1(indices, sizeof(indices));

    // Position
    VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, pos));

    // Color
    VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, color));

    // Texture Coordinates
    VAO1.LinkAttrib(VBO1, 2, 2, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, uv));

    VAO1.Unbind();
    VBO1.Unbind();
    EBO1.Unbind();

    // ======================= TEXTURE =======================

    int widthImg, heightImg, numColCh;

    stbi_set_flip_vertically_on_load(true);

    unsigned char* bytes = stbi_load(
        "pajaro.png",
        &widthImg,
        &heightImg,
        &numColCh,
        0
    );

    if (bytes == nullptr)
    {
        std::cout << "NO SE CARGO LA IMAGEN\n";
    }
    else
    {
        std::cout << "IMAGEN CARGADA\n";
    }

    GLuint texture;

    glGenTextures(1, &texture);

    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_2D, texture);

    // PNG transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Filters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Wrap
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Detect image format
    GLenum format;

    if (numColCh == 4)
    {
        format = GL_RGBA;
    }
    else
    {
        format = GL_RGB;
    }

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        format,
        widthImg,
        heightImg,
        0,
        format,
        GL_UNSIGNED_BYTE,
        bytes
    );

    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(bytes);

    // ======================= TEXTURE UNIFORM =======================

    shaderProgram.Activate();

    GLuint tex0Uni = glGetUniformLocation(shaderProgram.ID, "tex0");

    glUniform1i(tex0Uni, 0);

    // ======================= CAMERA =======================

    Camera camera(width, height, glm::vec3(0.0f, 0.0f, 2.0f));

    // Parámetros para el recorrido por la curva Bézier dentro del loop
    double lastTime = glfwGetTime();
    float bezierT = 0.0f;
    const float bezierSpeed = 0.15f; // velocidad moderada en unidades t por segundo

    // ======================= LOOP =======================

    while (!glfwWindowShouldClose(window))
    {
		//  color para limpiar la pantalla cada frame (RGBA)

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shaderProgram.Activate();

        glm::mat4 model = glm::mat4(1.0f);

        model = glm::rotate(
            model,
            (float)glfwGetTime() * glm::radians(50.0f),
            glm::vec3(0.5f, 1.0f, 0.0f)
        );

        GLuint modelLoc = glGetUniformLocation(shaderProgram.ID, "model");

        glUniformMatrix4fv(
            modelLoc,
            1,
            GL_FALSE,
            glm::value_ptr(model)
        );

        // Camera

        camera.Inputs(window);

        // Avanza t en función del tiempo para un movimiento suave por la curva
        double currentTime = glfwGetTime();
        double delta = currentTime - lastTime;
        lastTime = currentTime;

        bezierT += bezierSpeed * static_cast<float>(delta);
        // envolver para loop continuo
        bezierT = std::fmod(bezierT, 1.0f);
        if (bezierT < 0.0f) bezierT += 1.0f;

        // Actualiza la posición de la cámara usando la función existente
        camera.Position = camera.evaluateBezier(bezierT);
        // Mantener la orientación mirando al centro (Matrix usa lookAt hacia el modelo)

        camera.Matrix(
            45.0f,
            0.1f,
            100.0f,
            shaderProgram,
            "camMatrix"
        );

        // Texture

        glActiveTexture(GL_TEXTURE0);

        glBindTexture(GL_TEXTURE_2D, texture);

        // Draw

        VAO1.Bind();

        glDrawElements(
            GL_TRIANGLES,
            sizeof(indices) / sizeof(unsigned int),
            GL_UNSIGNED_INT,
            0
        );

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    // ======================= CLEANUP =======================

    VAO1.Delete();
    VBO1.Delete();
    EBO1.Delete();

    glDeleteTextures(1, &texture);

    shaderProgram.Delete();

    glfwDestroyWindow(window);

    glfwTerminate();

    return 0;
}
// Ejecutar programa: Ctrl + F5 o menú Depurar > Iniciar sin depurar
// Depurar programa: F5 o menú Depurar > Iniciar depuración

// Sugerencias para primeros pasos: 1. Use la ventana del Explorador de soluciones para agregar y administrar archivos
//   2. Use la ventana de Team Explorer para conectar con el control de código fuente
//   3. Use la ventana de salida para ver la salida de compilación y otros mensajes
//   4. Use la ventana Lista de errores para ver los errores
//   5. Vaya a Proyecto > Agregar nuevo elemento para crear nuevos archivos de código, o a Proyecto > Agregar elemento existente para agregar archivos de código existentes al proyecto
//   6. En el futuro, para volver a abrir este proyecto, vaya a Archivo > Abrir > Proyecto y seleccione el archivo .sln
