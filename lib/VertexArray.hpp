#pragma once
#include "opengl.hpp"

class VertexArray {
public:
    unsigned int VAO, VBO;
    int currentIndex = 0;
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
        glEnableVertexAttribArray(this->currentIndex);
        glVertexAttribPointer(this->currentIndex,size,GL_FLOAT,GL_FALSE,this->stride, (void*)(offset*sizeof(float)));

        this->currentIndex ++;
        this->offset += size;
    }

    void AddLayout(int size, void* buffer_data, int buffer_size, int usage) {
        unsigned int buffer;
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, buffer_size * FLOAT_SIZE, buffer_data, usage);
        glEnableVertexAttribArray(this->currentIndex);
        glVertexAttribPointer(this->currentIndex, size, GL_FLOAT, GL_FALSE, 0, nullptr);

        currentIndex ++;
    }

    void LoadToBuffer(void* vertices_array, int array_size, GLenum usage) {
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