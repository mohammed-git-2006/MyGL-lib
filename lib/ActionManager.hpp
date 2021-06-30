#pragma once

class ActionManager {
    public:
    float current_time = 0, time_scale = 1;

    ActionManager(float start_value) {
        this->current_time = start_value;
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
};