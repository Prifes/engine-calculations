#pragma once
#include "defines.h"
#ifdef __GRAPHICS__

#include <vector>
#include <string>
#include "raylib.h"

class Plot {
public:
    Plot(int x0, int y0, int width, int height, std::string x_unit, std::string y_unit, Color color);
    void AddPoint(float x_value, float y_value);
    void Draw();
private:
    Color color_;
    std::vector<float> x_axis_;
    std::vector<float> y_axis_;
    std::string x_unit_;
    std::string y_unit_;
    int x0_;
    int y0_;
    int width_;
    int height_;
    float x_max_;
    float y_max_;
    float x_min_;
    float y_min_;

    float ConvertX_(float x) const;
    float ConvertY_(float y) const;
    void DrawCoordSys_();
};
#endif