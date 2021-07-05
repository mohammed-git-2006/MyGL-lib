#pragma once

#include "opengl.hpp"
#include <iostream>

class Texture {
    public:

    unsigned int texture;
    GLenum texture_target = GL_TEXTURE_2D;
    GLenum min_filter = GL_LINEAR, mag_filter = GL_LINEAR;

    unsigned char* image_buffer;

    std::string error = "No Errors";

    int image_width, image_height, image_comp;
    int req_comp = STBI_rgb_alpha;

    GLenum wrap_s = GL_CLAMP_TO_EDGE, wrap_t = GL_CLAMP_TO_EDGE;
    GLenum internalFormat = GL_RGBA, format = GL_RGBA;

    bool loadFromFile(std::string path) {
        stbi_set_flip_vertically_on_load(1);
        this->image_buffer =stbi_load(path.c_str(), &image_width, &image_height, &image_comp, req_comp);
        if (! image_buffer) {
            error = "Failed to load image";
            return false;
        }

        //memset(image_buffer, 0xff, image_width * image_height);
        return true;
    }

    void GenerateTexture() {
        

        glGenTextures(1, &this->texture);
        glBindTexture(texture_target, texture);
        
        glTexParameteri(texture_target, GL_TEXTURE_WRAP_S, this->wrap_s);
        glTexParameteri(texture_target, GL_TEXTURE_WRAP_T, this->wrap_t);

        glTexParameteri(texture_target, GL_TEXTURE_MIN_FILTER, this->min_filter);
        glTexParameteri(texture_target, GL_TEXTURE_MAG_FILTER, this->mag_filter);

        glTexImage2D(texture_target, 0, internalFormat, image_width, image_height, 0, format, GL_UNSIGNED_BYTE, this->image_buffer);
        //glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

        glGenerateMipmap(texture_target);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        //GL_NEAREST_MIPMAP_NEAREST
    }

    void Bind() {
        glBindTexture(texture_target, texture);
    }

    std::string imageSize_string() {
        return "[" + std::to_string(image_width) + "x" + std::to_string(image_height) + "]";
    }
};

Texture CreateTexture(std::string name) {
    Texture result;
    result.loadFromFile(name);
    result.GenerateTexture();

    return result;
}

namespace TextureBuffers {
    Texture Make(int r, int g, int b, int a, int width, int height) {
        Texture texture;
        int image_size = width * height * 4, offset;

        

        texture.image_buffer = new unsigned char[image_size];

        texture.image_width = width;
        texture.image_height = height;

        for(int i=0;i<image_size/4;++i) {
            offset = i * 4;
            texture.image_buffer[offset+0] = r;
            texture.image_buffer[offset+1] = g;
            texture.image_buffer[offset+2] = b;
            texture.image_buffer[offset+3] = a;
        }

        texture.GenerateTexture();

        return texture;
    }
}