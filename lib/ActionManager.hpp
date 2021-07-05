#pragma once

#include <vector>

class ActionManager {
    public:
    float current_time = 0, time_scale = 1;

    std::string name;

    ActionManager(float start_value) {
        this->current_time = start_value;
    }

    void setName(std::string _name) {
        this->name = _name;
    } 

    ActionManager() {

    }

    void Increase(float value) {
        current_time += value * time_scale;
    } 

    bool isAvailable(float required_time) {
        return current_time >= required_time;
    }

    void Zero() {
        this->current_time = 0;
    }

    ActionManager* Pointer() {
        return this;
    }
};

class Actions {
    public:

    std::vector<ActionManager*> actions;

    void addAction(ActionManager* action) {
        this->actions.push_back(action);
    }

    ActionManager* operator [] (std::string name) {
        for(ActionManager* action: this->actions) if (action->name == name) return action;
        return nullptr;
    }

    void IncreaseAll(float value) {
        for(ActionManager* action : this->actions) {
            action->Increase(value);
        }
    }
};