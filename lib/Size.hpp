#pragma once

class Size {
    public:

    float x = 0, y = 0, z = 0;

    void setSize(float x, float y, float z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    Size() {

    }

    Size(float x, float y, float z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }
};