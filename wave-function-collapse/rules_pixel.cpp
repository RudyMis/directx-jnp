#include "rules_pixel.h"


void RulesPixel::enable() {
	enabled = !enabled;
	if (enabled) {
		Pixel::set_color(color);
	} else {
		Pixel::set_color(D2D1::ColorF(D2D1::ColorF::White, 1));
	}
}

void RulesPixel::set_color(const D2D1_COLOR_F& color) {
	this->color = color;
}
