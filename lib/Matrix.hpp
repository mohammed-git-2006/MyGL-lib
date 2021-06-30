#pragma once

#include "opengl.hpp"

class MatrixObject {
public:
    glm::mat4 matrix = glm::mat4(1);
    float x = 0, y = 0, z = 0, rot_x, rot_y, rot_z;

    float* Matrix() {
        return glm::value_ptr(matrix);
    }

    MatrixObject* Pointer() {
        return this;
    }

    void Reset() {
        this->matrix = glm::mat4(1);
    }

    void Translate(float x, float y, float z) {
        this->x += x;
        this->y += y;
        this->z += z;
        
        this->matrix = glm::translate(this->matrix, glm::vec3(x, y, z));
    }
    
    void Rotate(float angle, float x, float y, float z) {
        rot_x += angle * x;
        rot_y += angle * y;
        rot_z += angle * z;

        this->matrix = glm::rotate(this->matrix, glm::radians(angle), glm::vec3(x, y, z));
    }
};

namespace FastMatrix {
    glm::mat4 position(float x, float y, float z) {
        return glm::translate(glm::mat4(1), glm::vec3(x, y, z));
    }

    glm::mat4 position_scaled(float x, float y, float z, float scale_x, float scale_y, float scale_z) {
        return glm::scale(glm::translate(glm::mat4(1), glm::vec3(x, y, z)), glm::vec3(
            scale_x, scale_y, scale_z
        ));
    }

    glm::mat4 position_margin(float x, float y, float z,
        float margin_right, float margin_top) 
        {
        
        return glm::translate(glm::mat4(1), glm::vec3(
            x + margin_right,
            y - margin_top,
            z
        ));
    }

    glm::mat4 position_margin(float x, float y, float z, float margin) {
        return glm::translate(glm::mat4(1), glm::vec3(x + margin, y - margin, z));
    }

    glm::mat4 position_rotated(float x, float y, float z, float angle, float rot_x, float rot_y, float rot_z) {
        return glm::rotate(glm::translate(glm::mat4(1), glm::vec3(x, y, z)), glm::radians(angle), glm::vec3(rot_x, rot_y, rot_z));
    }

    glm::mat4 rotation(float angle, float x, float y, float z) {
        return glm::rotate(glm::mat4(1), glm::radians(angle), glm::vec3(x, y, z));
    }

    glm::mat4 scale(float x, float y, float z) {
        return glm::scale(glm::mat4(1), glm::vec3(x, y, z));
    }
}