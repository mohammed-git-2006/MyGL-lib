#pragma once
#include "opengl.hpp"

class VertexArray {
public:
    unsigned int VAO, VBO, IBO;
    int currentIndex = 0;
    int stride;

    GLenum draw_mode = GL_QUADS;
    int offset = 0;

	void Init() {
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
	}

    void Init(int stride, int draw_mode) {
        this->stride = stride;
        this->draw_mode = draw_mode;

		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
    }

    void AddLayout(int size) {
        glEnableVertexAttribArray(this->currentIndex);
        glVertexAttribPointer(this->currentIndex,size,GL_FLOAT,GL_FALSE,this->stride, (void*)(offset*sizeof(float)));

        this->currentIndex ++;
        this->offset += size;
    }

	void MakeArrayBuffer() {
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
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

    void LoadElementArrayBuffer(void* indices_buffer, int size, unsigned int usage) {
        glGenBuffers(1, &IBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices_buffer, usage);
    }
};
