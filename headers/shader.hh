#pragma once

#include <string>
#include <glad/glad.h>

class Program {
public:

    Program(std::string const& vsource, std::string const& fsource);
    ~Program();

    GLuint id() const;

private:
    GLuint program_id;

    static void confirm_compilation(GLuint shader);
};

std::string read_shader(std::string const& path);
