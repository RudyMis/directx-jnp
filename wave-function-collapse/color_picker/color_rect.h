#pragma once
#include "../helper.h"

class ColorRect {
public:
	ColorRect(const Point2& _position, const Point2& _size, const D2D1_COLOR_F& _color)
		: position{ _position }, size{ _size }, color{ _color }, bar_value{ 0.0f }
	{
		set_bar_value(0.0f);
	}
	ColorRect() : position{}, size{}, color{ D2D1::ColorF::White } {}

	HRESULT create_resources(CComPtr<ID2D1HwndRenderTarget> render_target);
	void discard_resources();

	void draw(CComPtr<ID2D1HwndRenderTarget> render_target);

	bool contains(const Point2& pos);

	void on_click(const Point2& pos);
	void on_drag(const Point2& pos);
	void on_release();

	FLOAT get_value() const;

private:

	Point2 position;
	Point2 size;
	D2D1_COLOR_F color;

	FLOAT clamp_val = 0.075f;
	FLOAT bar_value;

	void set_bar_value(FLOAT relative_x);

	CComPtr<ID2D1LinearGradientBrush> brush;
	CComPtr<ID2D1SolidColorBrush> light_gray_brush;
	CComPtr<ID2D1GradientStopCollection> gradient_stops;
	ID2D1Bitmap* slider_bitmap;
};