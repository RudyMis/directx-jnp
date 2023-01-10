#pragma once
#include "helper.h"
#include "context.hpp"

class Pixel {
public:

	Pixel(Point2 _position, Point2 _size) : position{ _position }, size{ _size } {}
	Pixel() : Pixel({ 0, 0 }, { 0, 0 }) {}

	virtual HRESULT create_resources(CComPtr<ID2D1HwndRenderTarget> render_target);
	virtual void discard_resources();

	virtual void set_color(const D2D1_COLOR_F& color);
	virtual void set_size(const Point2& size);
	virtual void draw(CComPtr<ID2D1HwndRenderTarget> render_target);

	bool contains(const Point2& pos) const;

	virtual void on_click(const Point2& pos, Context& ctx);
	virtual void on_drag(const Point2& pos, Context& ctx);
	virtual void on_release(Context& ctx);

	D2D1_COLOR_F color() const;

	Point2 position;

protected:
	Point2 size;

private:

	CComPtr<ID2D1SolidColorBrush> brush;
};

