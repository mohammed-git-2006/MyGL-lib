#pragma once

#include "opengl.hpp"
#include "Display.hpp"

namespace Mouse {
    Display* display;

    double x = 0, y = 0;
    double last_x = 0, last_y = 0;
    double delta_x = 0, delta_y = 0;

    void Update() {
        delta_x = x - last_x;
        delta_y = y - last_y;

        last_x = x;
        last_y = y;

        glfwGetCursorPos(display->glfwWindow, &x, &y);
    }

    void setDisplay(Display* display) {
        Mouse::display = display;
    }
}

//_CURSOR_CALLBACK = void(GLFWwindow* window, double x, double y) {
//    _CURRENT_MOUSE_PTR->delta_x = _CURRENT_MOUSE_PTR->last_x - x;
//    _CURRENT_MOUSE_PTR->delta_y = _CURRENT_MOUSE_PTR->last_y - y;
//
//    _CURRENT_MOUSE_PTR->last_x = x;
//    _CURRENT_MOUSE_PTR->last_y = y;
//}