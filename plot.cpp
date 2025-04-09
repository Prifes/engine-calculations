#ifdef __GRAPHICS__
#include "plot.h"

Plot::Plot(int x0, int y0, int width, int height, 
    std::string x_unit, std::string y_unit, Color color) : 
    x0_(x0), y0_(y0), width_(width), height_(height), 
    x_unit_(x_unit), y_unit_(y_unit), color_(color) {
	x_max_ = 0;
	y_max_ = 0;
	x_min_ = INFINITY;
	y_min_ = INFINITY;
}

void Plot::AddPoint(float x_value, float y_value) {
	x_axis_.push_back(x_value);
	y_axis_.push_back(y_value);
	if (x_value > x_max_) x_max_ = x_value;
	if (y_value > y_max_) y_max_ = y_value;
	if (x_value < x_min_) x_min_ = x_value;
	if (y_value < y_min_) y_min_ = y_value;
}

void Plot::Draw() {
    DrawCoordSys_();
	for (int i = 0; i < x_axis_.size()-1; i++) {
		DrawCircle(ConvertX_(x_axis_[i]), ConvertY_(y_axis_[i]), 5, color_);
		DrawLineEx({ ConvertX_(x_axis_[i]), ConvertY_(y_axis_[i]) }, { ConvertX_(x_axis_[i + 1]), ConvertY_(y_axis_[i + 1]) }, 3, color_);
	}
	DrawCircle(ConvertX_(x_axis_[x_axis_.size()-1]), ConvertY_(y_axis_[x_axis_.size()-1]), 5, color_);
}

float Plot::ConvertX_(float x) const {
	return x0_ + round((x - x_min_) / (x_max_ - x_min_) * width_);
}

float Plot::ConvertY_(float y) const {
	return height_ - (y0_ + round((y - y_min_) / (y_max_ - y_min_) * height_));
}

void Plot::DrawCoordSys_() {
    const int textSize = width_ / 36;
    const int paddingX = width_ / 384;
    const int paddingY = height_ / 51;

    double sum_x = 0;
    double sum_y = 0;
    for (auto value : x_axis_) { sum_x += value; }
    for (auto value : y_axis_) { sum_y += value; }
    double scale_x = round(log10(sum_x / x_axis_.size()));
    double scale_y = round(log10(sum_y / y_axis_.size()));
    double multiplier_x = pow(10, scale_x - 1);
    double multiplier_y = pow(10, scale_y - 1);

    // Vertical lines
    int no = 0;
    for (double value = ceil(x_min_ / multiplier_x) * multiplier_x; value < x_max_; value += multiplier_x) {
        // Drawing lines
        int x = (int)ConvertX_(value);
        if (no != 0) {
            DrawLine(x, y0_, x, y0_ + height_, LIGHTGRAY);
        }
        else {
            DrawLine(x - 1, y0_, x - 1, y0_ + height_, BLACK);
        }

        // Drawing text
        if (no++ % 2 == 0) {
            char text[10] = "";
            _itoa_s(value, text, 10, 10);
            std::string toPrint(text);
            toPrint.append(x_unit_);
            if (x + paddingX >= 0) {
                DrawText(toPrint.c_str(), x + paddingX, y0_ + height_ - textSize, textSize, BLACK);
            }
        }
    }
    // Horizontal lines
    for (double value = ceil(y_min_ / multiplier_y) * multiplier_y; value < y_max_; value += multiplier_y) {
        // Drawing lines
        int y = (int)ConvertY_(value);
        DrawLine(x0_, y, x0_ + width_, y, LIGHTGRAY);

        // Drawing text
        char text[10] = "";
        sprintf(text, "%.*f", (int)round(scale_x) -1, value);
        std::string toPrint(text);
        toPrint.append(y_unit_);
        if (y + textSize < height_) {
            DrawText(toPrint.c_str(), x0_ + paddingX, y - textSize, textSize, BLACK);
        }
    }
}
#endif