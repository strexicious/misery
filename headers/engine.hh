#pragma once

#define GLFW_INCLUDE_NONE
#include <cstdint>
#include <GLFW/glfw3.h>
#include "chassis.hh"
#include "shader.hh"

class Engine {
public:
    Engine(Chassis&& chassis);

    void run_loop() const;

private:
    Chassis chassis;
    Program shader;

    static bool engine_started;
};
