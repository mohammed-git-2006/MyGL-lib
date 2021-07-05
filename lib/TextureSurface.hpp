#pragma once

#include "Texture.hpp"
#include "VertexArray.hpp"
#include "RenderObject.hpp"

class TextureSurface : public RenderObject {
public:
    std::string _vs = "#version " + SHADRING_LANGUAGE_VERSION + "\n" + 
        "in vec4 aPosition;in vec2 aTexCoord;uniform mat4 projection,position;\n" + 
        "out vec2 texCoord;" + 
        "void main(){gl_Position=projection*position*aPosition;texCoord=aTexCoord;}";
    
    std::string _fs = "#version " + SHADRING_LANGUAGE_VERSION + "\n" +
        "in vec2 texCoord;uniform sampler2D _tex;uniform vec4 blend_color;void main()" + 
        "{vec4 _color=texture(_tex,texCoord);if(_color==blend_color)discard;else gl_FragColor=_color;}";
        //"{gl_FragColor=vec4(texCoord, 1, 1);}";

    int _vertices_size = 5 * 4;

    float _vertices[5 * 4] = {
        -0.5, -0.5, 0, 0, 0,
        -0.5,  0.5, 0, 0, 1,
         0.5,  0.5, 0, 1, 1,
         0.5, -0.5, 0, 1, 0,
    };

    /*
    
    0, 0,
    0, 1,
    1, 1,
    1, 0,

    */

    std::string error;

    bool setTexture(std::string path) {
        if (! texture.loadFromFile(path)) {
            error = texture.error;
            return false;
        }

        texture.GenerateTexture();
        return true;
    }

    void setTexture(Texture texture) {
        this->texture = texture;
    }

    void setShader(ShaderProgram* shader) {
        this->_vs = shader->_vertex_shader;
        this->_fs = shader->_fragment_shader;
    }

    void Make() {
        va.Init(5 * sizeof(float), GL_QUADS);
        va.MakeArrayBuffer();
        va.AddLayout(3);
        va.AddLayout(2);
        va.LoadToBuffer(this->_vertices, sizeof(this->_vertices), GL_STATIC_DRAW);
        //std::cout << ">> " << sizeof(_vertices) << std::endl;
        
        va.draw_mode = GL_QUADS;

        shader._vertex_shader   = this->_vs;
        shader._fragment_shader = this->_fs;

        shader.Compile();

        std::cout << "TextureSurface::Make shader compile result >> " << shader.error << std::endl;
    }
};