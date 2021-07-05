#pragma once

#include <iostream>
#include <vector>

#include "RenderObject.hpp"

class SceneRenderObject {
    public:
    std::string name;
    RenderObject* render_object;
};

SceneRenderObject makeSceneRenderObject(std::string name, RenderObject *render_object_ptr) {
    SceneRenderObject result;

    result.render_object = render_object_ptr;
    result.name = name;

    return result;
}

class Scene {
    public:
    std::string name;

    Scene(std::string sceneName) {
        this->name = sceneName;
    }

    Scene() {}

    std::vector<SceneRenderObject> renderObjects;

    void addRenderObject(SceneRenderObject sceneRenderObject) {
        this->renderObjects.push_back(sceneRenderObject);
    }

    void setSceneName(std::string scene_name) {
        this->name = scene_name;
    }

    SceneRenderObject* getRenderObject(std::string name) {
        for(int i=0;i<this->renderObjects.size();++i) {
            if (renderObjects[i].name == name) {
                return &renderObjects[i];
            }
        }

        return nullptr;
    }

    SceneRenderObject* operator [] (std::string name) {
        return this->getRenderObject(name);
    }
};