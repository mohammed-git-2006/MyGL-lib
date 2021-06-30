#pragma once
#include "opengl.hpp"

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>

#include "Matrix.hpp"
#include "Position.hpp"

class ShaderMatrix {
    public:
    std::string name;
    MatrixObject* matrix;

    ShaderMatrix (std::string name, MatrixObject*  matrix) {
        this->name = name;
        this->matrix = matrix;
    }
};

class ShaderProgram {
public:
    unsigned int program, vs, fs;
    char* log;
    int log_length;
    std::string error;
    bool status = true;

    std::vector<ShaderMatrix*> matrices;

    void addMatrix(ShaderMatrix* shaderMatrix) {
        this->matrices.push_back(shaderMatrix);
    }

    std::string _vertex_shader, _fragment_shader;
    std::string _file_split = "#fragment_shader";

    bool loadFromFile(std::string path) {
        std::ifstream ifs(path);

        if (! ifs.is_open()) {
            error = "Failed to open file";
            status = false;

            return false;
        }

        std::string line;
        std::stringstream ss;

        //bool _ffs = false;

        while(std::getline(ifs, line)) {
            if (line == _file_split) {
                this->_vertex_shader = ss.str();
                ss = std::stringstream();
                continue;
            }

            ss << line << '\n';
        }

        this->_fragment_shader = ss.str();

        //std::cout << "VERTEX_SHADER: " << this->_vertex_shader << std::endl;
        //std::cout << "FRAGMENT_SHADER: " << this->_fragment_shader << std::endl;

        ifs.close();

        return true;
    }

    ShaderProgram* Make(std::string path) {
        if (! loadFromFile(path)) return nullptr;
        Compile();
        return this;
    }

    bool Compile() {
        if (! this->status) {
            return false;
        }

        int compileStatus;
        this->program = glCreateProgram();

        this->vs = glCreateShader(GL_VERTEX_SHADER);
        this->fs = glCreateShader(GL_FRAGMENT_SHADER);

        const char* vs_source = this->_vertex_shader.c_str();
        const char* fs_source = this->_fragment_shader.c_str();

        glShaderSource(vs, 1, &vs_source, nullptr);
        glCompileShader(vs);

        glGetShaderiv(vs, GL_COMPILE_STATUS, &compileStatus);

        if (! compileStatus) {
            glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &this->log_length);
            this->log = (char*) std::malloc(this->log_length * sizeof(char));

            glGetShaderInfoLog(vs, log_length, &log_length, log);

            std::stringstream ss;
            ss << "Failed to Compile Vertex Shader:\n" << std::string(log) << std::endl;

            this->error = ss.str();
            this->status = false;

            return false;
        }

        glShaderSource(fs, 1, &fs_source, nullptr);
        glCompileShader(fs);

        glGetShaderiv(fs, GL_COMPILE_STATUS, &compileStatus);

        if (! compileStatus) {
            glGetShaderiv(fs, GL_INFO_LOG_LENGTH, &this->log_length);
            this->log = (char*) std::malloc(this->log_length * sizeof(char));

            glGetShaderInfoLog(fs, log_length, &log_length, log);

            std::stringstream ss;
            ss << "Failed to Compile Fragment Shader:\n" << std::string(log) << std::endl;

            this->error = ss.str();
            this->status = false;
            
            return false;
        }

        glAttachShader(this->program, vs);
        glAttachShader(this->program, fs);

        glLinkProgram(this->program);
        glValidateProgram(this->program);

        return true;
    }
    
    void Use() {
        glUseProgram(this->program);
    }

    unsigned int UL(std::string name) {
        return glGetUniformLocation(this->program, name.c_str());
    }

    void UniformVec4f(std::string name, float x, float y, float z, float a) {
        glUniform4f(UL(name), x, y, z, a);
    }

    void UniformVec4f(std::string name, Position position, float a) {
        glUniform4f(UL(name), position.x, position.y, position.z, a);
    }

//    void UniformVec4f(std::string name, Position position, float a) {
//        UniformVec4f(name, position.x, position.y, position.z, a);
//    }
//
//    //void UniformVec4f(std::string name, glm::ve3)
//
//    void UniformVec4f(std::string name, Vector3f vector, float a) {
//        glUniform4f(UL(name), vector.x, vector.y, vector.z, a);
//    }

    void UniformMatrix(std::string name, MatrixObject* matrix) {
        glUniformMatrix4fv(UL(name), 1, GL_FALSE, matrix->Matrix());
    }

    void UniformMatrix(std::string name, float* matrix) {
        glUniformMatrix4fv(UL(name), 1, GL_FALSE, matrix);
    }

    void UniformMatrix(std::string name, glm::mat4 matrix) {
        glUniformMatrix4fv(UL(name), 1, GL_FALSE, glm::value_ptr(matrix));
    }

    void UniformMatrices() {
        for(ShaderMatrix* matrix : this->matrices) {
            UniformMatrix(matrix->name, matrix->matrix);
        }
    }

    void UniformInteger(std::string name, int value) {
        glUniform1i(UL(name), value);
    }

    void UniformFloat(std::string name, float value) {
        glUniform1f(UL(name), value);
    }
};