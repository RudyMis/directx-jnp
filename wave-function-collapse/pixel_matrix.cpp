#include "pixel_matrix.h"

const FLOAT PixelMatrix::pixel_size{ 20.0f };

Point2 PixelMatrix::num_of_pixels() {
	return { size.x / pixel_size, size.y / pixel_size };
}

void PixelMatrix::draw(CComPtr<ID2D1HwndRenderTarget> render_target) {
	D2D1_MATRIX_3X2_F prev_transform = {};
	render_target->GetTransform(&prev_transform);

	render_target->SetTransform(prev_transform * D2D1::Matrix3x2F::Translation({ position.x, position.y }));
	for (auto& row : pixels) {
		for (auto& pixel : row) {
			pixel.draw(render_target);
		}
	}
	render_target->SetTransform(prev_transform);
}

HRESULT PixelMatrix::create_resources(CComPtr<ID2D1HwndRenderTarget> render_target) {
	HRESULT hr = S_OK;
	auto num = num_of_pixels();
	pixels.resize(num.x);
	Point2 pos = { 0, 0 };

	for (auto& row : pixels) {
		row.resize(num.y);
		for (auto& pixel : row) {
			hr = pixel.create_resources(render_target); CHECK;
			pixel.position = pos;
			pixel.set_size({ pixel_size, pixel_size });
			pos.y += pixel_size;
		}
		pos.x += pixel_size;
		pos.y = 0;
	}
	return hr;
}

void PixelMatrix::discard_resources() {
	for (auto& row : pixels) {
		for (auto& pixel : row) {
			pixel.discard_resources();
		}
		row.clear();
	}
	pixels.clear();
}

const std::vector<std::vector<Pixel>>& PixelMatrix::get_pixels() const {
	return pixels;
}

void PixelMatrix::init(Point2 _position, Point2 screen_size) {
	position = _position;
	size = D2D1::Point2(screen_size.x - position.x, screen_size.y - position.y);
}
