#pragma once

#include "opengl.hpp"
#include "Display.hpp"

namespace Keyboard {
    Display* display;

    void setDisplay(Display* display) {
        Keyboard::display = display;
    }

    int isPressed(int key) {
        return glfwGetKey(display->glfwWindow, key);
    }

    int horizontal() {
        int result = 0;
        if (isPressed(GLFW_KEY_A)) result --;
        if (isPressed(GLFW_KEY_D)) result ++;

        return result;
    }

    int vertical() {
        int result = 0;
        if (isPressed(GLFW_KEY_S)) result --;
        if (isPressed(GLFW_KEY_W)) result ++;

        return result;
    }
};