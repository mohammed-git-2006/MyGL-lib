#pragma once

#include "opengl.hpp"
#include "RenderObject.hpp"
#include "Position.hpp"

class TERRAIN_OBJECT {
    public:

    std::string position_uniform_name = "position";
    bool distance_render = false;

    RenderObject* render_object;

    int   x_count = 1, y_count = 1, z_count = 1;
    float x_source = 0, y_source = 0, z_source = 0;
    float x_offset = 0, y_offset = 0, z_offset = 0;
    int va_first = 0, va_count = 3;

    //virtual void Render();

    void setSource(float x, float y, float z) {
        this->x_source = x;
        this->y_source = y;
        this->z_source = -z;
    }

    void setSource(Position position) {
        this->setOffset(position.x, position.y, position.z);
    }

    void setOffset(float x, float y, float z) {
        this->x_offset = x;
        this->y_offset = y;
        this->z_offset = z;
    }

    void setCount(float x, float y, float z) {
        this->x_count = x;
        this->y_count = y;
        this->z_count = z;
    }

    std::string sources_str() {
        return "[" + std::to_string(x_source) + ", " + std::to_string(y_source) + ", " + 
            std::to_string(z_source) + "]";
    }

    std::string offsets_str() {
        return "[" + std::to_string(x_offset) + ", " + std::to_string(y_offset) + ", " + 
            std::to_string(z_offset) + "]";
    }

    void setVertexArrayOptions(int va_size, int va_first) {
        this->va_count = va_size;
        this->va_first = va_first;
    }
};

namespace Terrains {
    class Terrain : public TERRAIN_OBJECT {
        public:

        void Render() {
            for(int y=0;y<y_count;++y) {
                for(int z=0;z<z_count;++z) {
                    for(int x=0;x<x_count;++x) {
                        Position position(
                            x_source + x + (x_offset * x),
                            y_source + y + (y_offset * y),
                            z_source + z + (z_offset * z)
                        );

                        render_object->shader.UniformMatrix(position_uniform_name, 
                            glm::translate(glm::mat4(1), position.GLM_Vector()));

                        render_object->position = position;
                        
                        if (distance_render)
                            render_object->Draw_Distance(va_first, va_count);
                        else 
                            render_object->Draw(va_first, va_count);
                    }
                }
            }
        }
    };
}