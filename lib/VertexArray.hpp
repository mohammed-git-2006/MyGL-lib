#pragma once
#include "opengl.hpp"

class VertexArray {
public:
    unsigned int VAO, VBO;
    int curentIndex = 0;
    int stride;

    GLenum draw_mode = GL_QUADS;
    int offset = 0;

    void Init(int stride, int draw_mode) {
        this->stride = stride;
        this->draw_mode = draw_mode;

        glGenVertexArrays(1, &this->VAO);
        glBindVertexArray(this->VAO);

        glGenBuffers(1, &this->VBO);
        glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    }

    void AddLayout(int size) {
        glEnableVertexAttribArray(this->curentIndex);
        glVertexAttribPointer(this->curentIndex,size,GL_FLOAT,GL_FALSE,this->stride, (void*)(offset*sizeof(float)));

        this->curentIndex ++;
        this->offset += size;
    }

    void LoadToBuffer(float* vertices_array, int array_size, GLenum usage) {
        glBufferData(GL_ARRAY_BUFFER, array_size, vertices_array, usage);
        glBindVertexArray(0);
    }

    void Bind() {
        glBindVertexArray(this->VAO);
    }

    void Draw(int first, int count) {
        glDrawArrays(draw_mode, first, count);
    }
};