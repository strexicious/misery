#pragma once

#include <functional>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <GLFW/glfw3.h>

struct KeyboardHandler {
    std::function<void(int)> h;
    bool active = true;
};

struct MouseHandler {
    std::function<void(double, double)> h;
    bool active = true;
};

class InputHandler {
public:

    void set_press_handler(std::string const&, std::function<void(int)>);
    void set_long_press_handler(std::string const&, std::function<void(int)>);
    void set_release_handler(std::string const&, std::function<void(int)>);
    void set_mouse_handler(std::string const&, std::function<void(double, double)>);

    void press_handler_enabled(std::string const&, bool);
    void long_press_handler_enabled(std::string const&, bool);
    void release_handler_enabled(std::string const&, bool);
    void mouse_handler_enabled(std::string const&, bool);

    static void glfw_keyboard_handler(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void glfw_mouse_handler(GLFWwindow* window, double xpos, double ypos);

private:
    std::unordered_map<std::string, KeyboardHandler> presses;
    std::unordered_map<std::string, KeyboardHandler> long_presses;
    std::unordered_map<std::string, KeyboardHandler> releases;
    std::unordered_map<std::string, MouseHandler> mouse_handlers;

};
