#pragma once
#include "pixel.h"
class ColorView : public Pixel {
public:
	ColorView(Point2 _position, Point2 _size) : Pixel(_position, _size) {}
	ColorView() : Pixel({ 0, 0 }, { 0, 0 }) {}

	HRESULT create_resources(CComPtr<ID2D1HwndRenderTarget> render_target) override;

	void draw(CComPtr<ID2D1HwndRenderTarget> render_target) override;

private:
	ID2D1Bitmap* bitmap;
};

