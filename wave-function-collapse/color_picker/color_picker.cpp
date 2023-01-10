#include "color_picker.h"
#include <ranges>

const std::array<D2D1::ColorF, 3> ColorPicker::COLORS = {
	D2D1::ColorF(D2D1::ColorF::Red, 1),
	D2D1::ColorF(D2D1::ColorF::Green, 1),
	D2D1::ColorF(D2D1::ColorF::Blue, 1)
};

ColorPicker::ColorPicker() {
	Point2 pos(0, 0);
	size_t i = 0;
	for (auto& rect : rects) {
		rect = ColorRect(pos, Point2(rect_width, rect_height), COLORS[i]);
		pos.y += rect_spacing + rect_height;
		i++;
	}
	color_view = ColorView(Point2(0.0f, pos.y), Point2((INT)rect_width, (INT)rect_height));
}

void ColorPicker::draw(CComPtr<ID2D1HwndRenderTarget> render_target) {
//}}}}}}}}} <3ja Ciebie te¿
	for (auto &rect : rects) {
		rect.draw(render_target);
	}

	color_view.draw(render_target);
}

HRESULT ColorPicker::create_resources(CComPtr<ID2D1HwndRenderTarget> render_target) {
	HRESULT hr = S_OK;
	for (auto& rect : rects) {
		hr = rect.create_resources(render_target); CHECK;
	}
	return color_view.create_resources(render_target);
}

void ColorPicker::discard_resources() {
	for (auto& rect : rects) {
		rect.discard_resources();
	}
	color_view.discard_resources();
}

bool ColorPicker::contains(const Point2& pos) {
	auto containing = rects | std::views::filter([pos](auto& r) { return r.contains(pos); });
	return !containing.empty();
}

D2D1_COLOR_F ColorPicker::get_color() const {
	return D2D1::ColorF(
		rects[0].get_value(),
		rects[1].get_value(),
		rects[2].get_value());
}

void ColorPicker::on_click(const Point2& pos, Context& ctx) {
	if (!contains(pos)) {
		return;
	}
	clicked_rect = std::ref(*(rects | std::views::filter([pos](auto& r) { return r.contains(pos); })).begin());
	clicked_rect->get().on_click(pos);

	ctx.selected_color = get_color();
	color_view.set_color(ctx.selected_color);
}

void ColorPicker::on_drag(const Point2& pos, Context& ctx) {
	if (!clicked_rect) {
		return;
	}
	clicked_rect->get().on_drag(pos);

	ctx.selected_color = get_color();
	color_view.set_color(ctx.selected_color);
}

void ColorPicker::on_release(Context& ctx) {
	if (!clicked_rect) {
		return;
	}
	clicked_rect->get().on_release();
	clicked_rect = {};

	ctx.selected_color = get_color();
	color_view.set_color(ctx.selected_color);
}

