#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

#include <glm/glm.hpp>

class OBJLoader {
    private:
    std::vector<std::string> _split(std::string line, char seperator) {
        std::vector<std::string> result;
        std::stringstream ss;

        for(char character : line) {
            if (character == seperator) {
                result.push_back(ss.str());
                ss = std::stringstream();
                continue;
            }

            ss << character;
        }

        if (ss.str().length() > 0) result.push_back(ss.str());

        return result;
    }

    std::string _s1 (std::vector<std::string> s, int first, int end, char b) {
        std::stringstream ss;
        for(int i=first;i<end;++i) ss << s[i] << b;
        return ss.str();
    }

    public:

//    std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
    std::vector< glm::vec3 > temp_vertices;
    std::vector< glm::vec2 > temp_uvs;
    std::vector< glm::vec3 > temp_normals;

    std::vector< float > vertices, texCoords, normals;

    std::string error = "No Errors";

    bool loadFromFile(std::string path) {
        std::ifstream ifs(path);

        if (! ifs.is_open()) {
            error = "Failed to open file";
            return false;
        }

        std::string line;
        std::vector<std::string> split_result;

        while(std::getline(ifs, line)) {
            split_result = this->_split(line, ' ');
            
            if (split_result.size() == 4 && split_result[0] == "v"){
                glm::vec3 vector;
                vector.x = std::atof(split_result[1].c_str());
                vector.y = std::atof(split_result[2].c_str());
                vector.z = std::atof(split_result[3].c_str());

                this->temp_vertices.push_back(vector);
            } else if (split_result.size() == 3 && split_result[0] == "vt") {
                glm::vec2 vector;
                vector.x = std::atof(split_result[1].c_str());
                vector.y = std::atof(split_result[2].c_str());

                this->temp_uvs.push_back(vector);
            } else if (split_result.size() == 4 && split_result[0] == "vn"){
                glm::vec3 vector;
                vector.x = std::atof(split_result[1].c_str());
                vector.y = std::atof(split_result[2].c_str());
                vector.z = std::atof(split_result[3].c_str());

                this->temp_normals.push_back(vector);
            } else if (split_result.size() == 4 && split_result[0] == "f") {
                //std::cout << "Face Line : " << line << ";\n";
                std::string _line = this->_s1(split_result, 1, split_result.size(), ' ');
                std::vector<std::string> faces_split = this->_split(_line, ' '), face_split;
                //std::cout << "New Face : " << faces_split.size() << std::endl;

                for(std::string item : faces_split) {
                    face_split = this->_split(item, '/');
                    //std::cout << "Face : " << this->_s1(face_split, 0, face_split.size(), ' ') << '\n';

                    glm::vec3 vector, normal;
                    glm::vec2 texcoord;

                    vector = this->temp_vertices[std::atoi(face_split[0].c_str())   - 1];
                    texcoord = this->temp_uvs[std::atoi(face_split[1].c_str())      - 1];
                    normal = this->temp_normals[std::atoi(face_split[2].c_str())    - 1];

                    //std::cout << "Vector[" << vector.x << ", " << vector.y << ", " << vector.z << "]\n";

                    vertices.push_back(vector.x);
                    vertices.push_back(vector.y);
                    vertices.push_back(vector.z);

                    texCoords.push_back(texcoord.x);
                    texCoords.push_back(texcoord.y);
                    //texCoords.push_back(texcoord.z);

                    normals.push_back(normal.x);
                    normals.push_back(normal.y);
                    normals.push_back(normal.z);
                }
            }
        }


            std::cout << "Vertices : " << vertices.size() << "\n";
            std::cout << "Tex Coords : " << texCoords.size() << "\n";
            std::cout << "Normals : " << normals.size() << "\n";

        return true;
    }
};