#pragma once
#include "../helper.h"
#include "color_rect.h"
#include "../pixel.h"
#include "../color_view.h"
#include <array>
#include <optional>

class ColorPicker {
public:
	ColorPicker();

	void draw(CComPtr<ID2D1HwndRenderTarget> render_target);
		
	HRESULT create_resources(CComPtr<ID2D1HwndRenderTarget> render_target);
	void discard_resources();

	bool contains(const Point2& pos);

	void on_click(const Point2& pos, Context& ctx);
	void on_drag(const Point2& pos, Context& ctx);
	void on_release(Context& ctx);

private:

	static const UINT rect_width = 200;
	static const UINT rect_height = 30;
	static const UINT rect_spacing = 20;
	static const std::array<D2D1::ColorF, 3> COLORS;

	D2D1_COLOR_F get_color() const;

	std::array<ColorRect, 3> rects;
	ColorView color_view;

	std::optional<std::reference_wrapper<ColorRect>> clicked_rect;

};

