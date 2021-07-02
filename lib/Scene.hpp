#pragma once

#include <iostream>
#include <vector>

#include "RenderObject.hpp"

class SceneRenderObject {
    public:
    std::string name;
    RenderObject* render_object;
};

class Scene {
    public:
    std::string name;

    Scene(std::string sceneName) {
        this->name = sceneName;
    }

    std::vector<SceneRenderObject> renderObjects;

    void addRenderObject(SceneRenderObject sceneRenderObject) {

    }

    SceneRenderObject* getRenderObject(std::string name) {
        for(SceneRenderObject renderObject : this->renderObjects) {
            if(renderObject.name == name) {
                return &renderObject;
            }
        }

        return nullptr;
    }
};