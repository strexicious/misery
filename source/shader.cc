#include <iterator>
#include <fstream>
#include "shader.hh"

Program::Program(std::string const& vsource, std::string const& fsource) {
    GLuint vshader = glCreateShader(GL_VERTEX_SHADER);
    const GLchar* p_vsource = vsource.c_str();
    glShaderSource(vshader, 1, &p_vsource, nullptr);
    glCompileShader(vshader);

    try {
        confirm_compilation(vshader);
    } catch (std::runtime_error& e) {
        glDeleteShader(vshader);
        throw std::runtime_error("Vertex shader error: " + std::string(e.what()));
    }
    
    GLuint fshader = glCreateShader(GL_FRAGMENT_SHADER);
    const GLchar* p_fsource = fsource.c_str();
    glShaderSource(fshader, 1, &p_fsource, nullptr);
    glCompileShader(fshader);

    try {
        confirm_compilation(fshader);
    } catch (std::runtime_error& e) {
        glDeleteShader(vshader);
        glDeleteShader(fshader);
        throw std::runtime_error("Fragment shader error: " + std::string(e.what()));
    }

    program_id = glCreateProgram();
    glAttachShader(program_id, vshader);
    glAttachShader(program_id, fshader);

    glLinkProgram(program_id);

    glDetachShader(program_id, vshader);
    glDetachShader(program_id, fshader);
    
    glDeleteShader(vshader);
    glDeleteShader(fshader);
}

GLuint Program::id() const {
    return program_id;
}

void Program::confirm_compilation(GLuint shader) {
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        GLsizei log_length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);
        
        std::string log;
        log.resize(log_length);
        glGetShaderInfoLog(shader, log_length, nullptr, &log[0]);

        throw std::runtime_error(log);
    }
}

Program::~Program() {
    glDeleteProgram(program_id);
}

std::string read_shader(std::string const& path) {
    std::ifstream file(path);
    return std::string{std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
}
