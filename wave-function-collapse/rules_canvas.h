#pragma once
#include "helper.h"
#include "pixel.h"
#include "pixel_matrix.h"
#include <vector>
#include <optional>

class RulesCanvas : public PixelMatrix {
public:
	RulesCanvas(Point2 _position, Point2 screen_size) : PixelMatrix(_position, screen_size) {}

	RulesCanvas() : RulesCanvas({ 0, 0 }, { 0, 0 }) {}

	HRESULT create_path(CComPtr<ID2D1Factory> dx_factory);

	HRESULT create_resources(CComPtr<ID2D1HwndRenderTarget> render_target);
	
	void draw(CComPtr<ID2D1HwndRenderTarget> render_target);

	bool contains(const Point2& pos);

	void on_click(const Point2& pos, Context& ctx);
	void on_drag(const Point2& pos, Context& ctx);
	void on_release(Context& ctx);

	typedef std::optional<std::reference_wrapper<Pixel>> OptPixelRef;

	OptPixelRef clicked_pixel(const Point2& pos);

private:
	OptPixelRef held_pixel;

	CComPtr<ID2D1PathGeometry> circle_path;
	CComPtr<ID2D1SolidColorBrush> black_brush;
};

