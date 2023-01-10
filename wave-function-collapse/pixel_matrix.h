#pragma once
#include "pixel.h"
class PixelMatrix : public Pixel {
public:

	PixelMatrix(Point2 _position, Point2 screen_size) :
		position{ _position },
		size{ D2D1::Point2(screen_size.x - _position.x, 140.0f) }
	{}

	PixelMatrix() : PixelMatrix({ 0, 0 }, { 0, 0 }) {}

	void draw(CComPtr<ID2D1HwndRenderTarget> render_target);

	HRESULT create_resources(CComPtr<ID2D1HwndRenderTarget> render_target);
	void discard_resources();

	void on_resize(const Point2& new_size);

	void init(Point2 _position, Point2 screen_size);

	const std::vector<std::vector<Pixel>>& get_pixels() const;

protected:
	static const FLOAT pixel_size;
	std::vector<std::vector<Pixel>> pixels;
	
	Point2 num_of_pixels();

	Point2 position;
	Point2 size;
};

