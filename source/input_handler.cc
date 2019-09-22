#include <iostream>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include "input_handler.hh"

void InputHandler::glfw_keyboard_handler(
    GLFWwindow* window,
    int key,
    int scancode,
    int action,
    int mods
) {
    ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
    
    InputHandler* this_ih = static_cast<InputHandler*>(glfwGetWindowUserPointer(window));
    
    // handle presses
    if (action == GLFW_PRESS) {
        for (auto& el : this_ih->presses) {
            if (el.second.active) el.second.h(key);
        }
        
        for (auto& el : this_ih->long_presses) {
            if (el.second.active) el.second.h(key);
        }
    }
    
    // handle long presses
    if (action == GLFW_REPEAT) {
        for (auto& el : this_ih->long_presses) {
            if (el.second.active) el.second.h(key);
        }
    }

    // handle releases
    if (action == GLFW_RELEASE) {
        for (auto& el : this_ih->releases) {
            if (el.second.active) el.second.h(key);
        }
    }
}

void InputHandler::glfw_mouse_handler(GLFWwindow* window, double xpos, double ypos) {
    InputHandler* this_ih = static_cast<InputHandler*>(glfwGetWindowUserPointer(window));

    for (auto& el : this_ih->mouse_handlers) {
        if (el.second.active) el.second.h(xpos, ypos);
    }
}

void InputHandler::set_press_handler(std::string const& hname, std::function<void(int)> h) {
    presses[hname] = KeyboardHandler { h };
}

void InputHandler::set_long_press_handler(std::string const& hname, std::function<void(int)> h) {
    long_presses[hname] = KeyboardHandler { h };
}

void InputHandler::set_release_handler(std::string const& hname, std::function<void(int)> h) {
    releases[hname] = KeyboardHandler { h };
}

void InputHandler::set_mouse_handler(std::string const& hname, std::function<void(double, double)> h) {
    mouse_handlers[hname] = MouseHandler { h };
}

void InputHandler::press_handler_enabled(std::string const& hname, bool henabled) {
    if (presses.count(hname)) {
        presses[hname].active = henabled;
    }
}

void InputHandler::long_press_handler_enabled(std::string const& hname, bool henabled) {
    if (long_presses.count(hname)) {
        long_presses[hname].active = henabled;
    }
}

void InputHandler::release_handler_enabled(std::string const& hname, bool henabled) {
    if (releases.count(hname)) {
        releases[hname].active = henabled;
    }
}

void InputHandler::mouse_handler_enabled(std::string const& hname, bool henabled) {
    if (mouse_handlers.count(hname)) {
        mouse_handlers[hname].active = henabled;
    }
}
