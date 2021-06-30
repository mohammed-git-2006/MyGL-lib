#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

class VerticesLoader {
    private:
    float* split_f(std::string line, int size) {
        float* result = new float[size];
        int index = 0;

        memset(result, 0, size);

        std::stringstream ss;

        for(char character : line) {
            if (character == ' ') {
                result[index] = std::atof(ss.str().c_str());
                index ++;
                ss = std::stringstream();
                continue;
            }

            ss << character;
        }

        result[index] = std::atof(ss.str().c_str());

        return result;
    }

    public:

    float* vertices;
    std::string error = "No Errors";

    int size = 0, vertexLength = 0, verticesSize = 0;
    
    bool loadFromFile(std::string path) {
        std::ifstream ifs(path);

        if (! ifs.is_open()) {
            error = "Failed to open file";
            return false;
        }

        std::string line;

        if (! std::getline(ifs, line)) {
            error = "Error in file format";
            return false;
        }

        verticesSize = std::atoi(line.c_str());

        if (! std::getline(ifs, line)) {
            error = "Error in file format";
            return false;
        }

        vertexLength = std::atoi(line.c_str());

        //printf("%d : %d, %d\n", verticesSize, vertexLength, verticesCount * vertexLength);

        int resultSize = verticesSize * vertexLength;

        this->vertices = new float[resultSize];
        int offset = 0;
        
        memset(this->vertices, 0, resultSize);

        for(int i=0;i<verticesSize;++i) {
            if (line == "") { i --; continue; }
            if (! std::getline(ifs, line)) {
                error = "Error in file format";
                return false;
            }

            float* split_result = this->split_f(line, vertexLength);

            for(int i=0;i<vertexLength;++i) {
                this->vertices[(offset*vertexLength)+i] = split_result[i];
            }

            offset ++;
        }

        this->size = resultSize;

        return true;
    }
};