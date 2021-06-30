#pragma once

#include "opengl.hpp"
#include <iostream>

class Display {
public:
    std::string error;
    GLFWwindow* glfwWindow;

    bool Make(std::string title, int width, int height) {
        if (! glfwInit()) {
            error = "Failed to Initialize GLFW";
            return false;
        }

        this->glfwWindow = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);

        if (! this->glfwWindow) {
            error = "Failed to Make GLFW window";
            return false;
        }

        glfwMakeContextCurrent(this->glfwWindow);
        
        glfwWindowHint(GLFW_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_VERSION_MINOR, 3);

        if (! glewInit() == GLEW_OK) {
            error = "Failed to Initialize GLEW";
            return false;
        }

        return true;
    }

    void Clear() {
        glClear(GL_COLOR_BUFFER_BIT);
    }

    void Clear_Depth() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    bool shouldClose() {
        return glfwWindowShouldClose(this->glfwWindow);
    }

    void SwapBuffers() {
        glfwSwapBuffers(this->glfwWindow);
        glfwPollEvents();
    }

    bool DepthEnabled = false;
    bool BlendEnabled = false;
    bool CullFaceEnabled = false;
    int CullFace = GL_BACK;

    void EnableDepth() {
        DepthEnabled = true;
        glEnable(GL_DEPTH_TEST);
    }

    void DisableDepth() {
        DepthEnabled = false;
        glDisable(GL_DEPTH_TEST);
    }

    void EnableBlend() {
        BlendEnabled = true;
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    void DisableBlend() {
        BlendEnabled = false;
        glDisable(GL_BLEND);
    }

    void EnableCullFace(int face) {
        CullFaceEnabled = true;
        CullFace = face;
        glEnable(GL_CULL_FACE);
        glCullFace(face);
    }

    void DisableCullFace() {
        CullFaceEnabled = false;
        glDisable(GL_CULL_FACE);
    }

    void setBackgroundColor(float r, float g, float b, float a) {
        glClearColor(r, g, b, a);
    }

    char* getGL_string(GLenum name) {
        return (char*) glGetString(name);
    }

    char* graphicsVendor() {
        return getGL_string(GL_VENDOR);
    }

    char* graphicsRenderer() {
        return getGL_string(GL_RENDERER);
    }

    char* opengl_Version() {
        return getGL_string(GL_VERSION);
    }

    char* ShadingLanguageVersion() {
        return getGL_string(GL_SHADING_LANGUAGE_VERSION);
    }

    float delta_time = 0, last_time = 0;

    void calculateDeltaTime() {
	    delta_time = glfwGetTime() - last_time;
	    last_time = glfwGetTime();
    }

    void setTitle(char* title) {
        glfwSetWindowTitle(glfwWindow, title);
    }

    void Terminate() {
        glfwTerminate();
    }
};

