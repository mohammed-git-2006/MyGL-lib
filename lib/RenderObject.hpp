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
    VerticesLoader vl;
    MatrixObject model;
};