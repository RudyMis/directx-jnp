#pragma once
#include "pixel_matrix.h"
#include "generator.h"

class Visualizer : public PixelMatrix {
public:
	Visualizer(Point2 _position, Point2 screen_size) : PixelMatrix(_position, screen_size) {}

	Visualizer() : Visualizer({ 0, 0 }, { 0, 0 }) {}

	void setup_animation(const std::vector<std::vector<Pixel>>& rule_pixels);
};

