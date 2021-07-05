#pragma once

#include "VerticesLoader.hpp"
#include "VertexArray.hpp"
#include "ShaderProgram.hpp"
#include "Matrix.hpp"
#include "Texture.hpp"

class RenderObject {
    public:
    ShaderProgram shader;
    Texture texture;
    VertexArray va;
    MatrixObject model;

    bool Drawable = true;

    ShaderProgram* getShader() {
        return &shader;
    }

    MatrixObject* getModel() {
        return &model;
    }

    RenderObject* Pointer() {
        return this;
    }
};