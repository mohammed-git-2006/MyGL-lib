#pragma once
#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#ifdef __WINDOWS__
#include "stb_image.h"
#endif
#ifdef __LINUX__
#include <stb/stb_image.h>
#endif

#define FLOAT_SIZE sizeof(float)
#define DOUBLE_SIZE sizeof(double)
#define INT_SIZE sizeof(int)
#define UINT_SIZE sizeof(unsigned int)
#define VEC_SIZE sizeof(glm::vec3)

std::string SHADRING_LANGUAGE_VERSION = "130";

void setShaderingLanguageVersion(std::string version) {
    SHADRING_LANGUAGE_VERSION = version;
}