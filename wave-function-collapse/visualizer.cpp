#include "visualizer.h"

void Visualizer::setup_animation(const std::vector<std::vector<Pixel>>& rule_pixels) {
	WaveFunctionCollapse wfc;
	auto order = wfc.generate(rule_pixels, num_of_pixels());
	for (auto it : order) {
		if (it.index.x == 0 || it.index.x > num_of_pixels().x) {
			continue;
		}
		if (it.index.y == 0 || it.index.y > num_of_pixels().y) {
			continue;
		}

		pixels[it.index.x - 1][it.index.y - 1].set_color(it.color);
	}
}
