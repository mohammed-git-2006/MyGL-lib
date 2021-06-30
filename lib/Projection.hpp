#pragma once

#include "opengl.hpp"
#include "Matrix.hpp"
#include "Size.hpp"

enum Projection_Type {
    PERSPICTIVE,
    ORTHO,
    NONE,
};

class Projection : public MatrixObject {
public:
    Projection_Type projection_type = Projection_Type::NONE;

    void setProjection_Perspictive(float fov, float aspect_ratio, float _Near, float _Far){
        this->matrix = glm::perspective(glm::radians(fov), aspect_ratio, _Near, _Far);
    }

    void setProjection_Ortho(float left, float right, float bottom, float top) {
        this->matrix = glm::ortho(left, right, bottom, top);
    }

    void setProjection_Ortho(Size size) {
        float x_value = size.x / 2;
        float y_value = size.y / 2;
        setProjection_Ortho(-x_value, x_value, -y_value, y_value);
    }
};