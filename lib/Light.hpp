#pragma once

#include "Matrix.hpp"
#include "Color.hpp"

class Light  {
    public:

    MatrixObject model;
    Color lightColor;
    float lightDistance = 5, brightness = 1;
};