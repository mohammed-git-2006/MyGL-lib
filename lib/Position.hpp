#pragma once

class Position {
    public:

    float x = 0, y = 0, z = 0;

    Position() {

    }

    Position(float _x, float _y, float _z) {
        x = _x;
        y = _y;
        z = _z;
    }

    void Move(float _x, float _y, float _z) {
        x += _x;
        y += _y;
        z += _z;
    }
};