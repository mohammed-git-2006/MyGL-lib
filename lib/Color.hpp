#pragma once

class Color {
    public:
    float r = 0, g = 0, b = 0, a = 1;

    void setColor(float r, float g, float b, float a) {
        this->a = a;
        this->r = r;
        this->g = g;
        this->b = b;
    }

    void setColor_FF(float r, float g, float b, float a) {
        this->a = a / 255;
        this->r = r / 255;
        this->g = g / 255;
        this->b = b / 255;
    }

    Color(float r, float g, float b, float a) {
        setColor_FF(r, g, b, a);
    }

    Color() {}
};

namespace Colors {
    Color RED(255, 0, 0, 255);
    Color GREEN(0, 255, 0, 255);
    Color BLUE(0, 0, 255, 255);
    Color WHITE(255, 255, 255, 255);
    Color BLACK(0, 0, 0, 255);
    Color GOLD(245,189,2, 255);
    Color SKY(135, 206, 235, 255);
    
    Color White(int alpha) {
        return Color(255, 255, 255, alpha);
    }
}