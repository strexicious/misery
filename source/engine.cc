#include <stdexcept>
#include <glad/glad.h>
#include "engine.hh"

bool Engine::engine_started = false;

Engine::Engine(Chassis chassis)
    : chassis{chassis},
      shader{read_shader("res/shaders/shader1.vert"), read_shader("res/shaders/shader1.frag")} { }

void Engine::run_loop() const {
    glUseProgram(shader.id());
    GLuint vbo;
    glGenBuffers(1, &vbo);
    float vertices[] = {
        0.0f,  0.5f, 1.0f, 0.0f, 0.0f, // Vertex 1: Red
        0.5f, -0.5f, 0.0f, 1.0f, 0.0f, // Vertex 2: Green
        -0.5f, -0.5f, 0.0f, 0.0f, 1.0f  // Vertex 3: Blue
    };
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5, (const void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5, (const void*)2);
    glEnableVertexAttribArray(1);

    while (!glfwWindowShouldClose(chassis.get_window())) {
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(chassis.get_window());
        glfwPollEvents();
    }
}
