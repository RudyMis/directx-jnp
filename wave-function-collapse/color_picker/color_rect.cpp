#include "color_rect.h"
#include "../bitmaps.h"

HRESULT ColorRect::create_resources(CComPtr<ID2D1HwndRenderTarget> render_target) {
	HRESULT hr = S_OK;

	D2D1_GRADIENT_STOP gradient_stops_colors[2];

	gradient_stops_colors[0].color = D2D1::ColorF(D2D1::ColorF::Black, 1);
	gradient_stops_colors[0].position = 0.0f;
	gradient_stops_colors[1].color = color;
	gradient_stops_colors[1].position = 1.0f;

	hr = render_target->CreateGradientStopCollection(
		gradient_stops_colors,
		2,
		D2D1_GAMMA_2_2,
		D2D1_EXTEND_MODE_CLAMP,
		&gradient_stops
	); CHECK;

	hr = render_target->CreateLinearGradientBrush(
		D2D1::LinearGradientBrushProperties(
			D2D1::Point2F(0.0f, 0.0f),
			D2D1::Point2F(size.x, 0.0f)),
		gradient_stops,
		&brush
	); CHECK;

	hr = render_target->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::LightGray, 1),
		&light_gray_brush
	); CHECK;

	if (!Bitmaps::wic_bitmaps.contains(L"slider.bmp")) {
		return E_FAIL;
	}

	hr = render_target->CreateBitmapFromWicBitmap(
		Bitmaps::wic_bitmaps.at(L"slider.bmp"),
		&slider_bitmap
	);

	return hr;
}

void ColorRect::discard_resources() {
	brush.Release();
	light_gray_brush.Release();
	gradient_stops.Release();
}

void ColorRect::draw(CComPtr<ID2D1HwndRenderTarget> render_target) {
	D2D1_MATRIX_3X2_F prev_transform;
	render_target->GetTransform(&prev_transform);
	auto translation = prev_transform * D2D1::Matrix3x2F::Translation(
		position.x, position.y
	);
	render_target->SetTransform(translation);
	D2D1_RECT_F rect = D2D1::RectF(
		0,
		0,
		size.x,
		size.y
	);
	render_target->FillRectangle(&rect, brush);

	render_target->DrawBitmap(slider_bitmap);

	Point2 circ_pos = D2D1::Point2(size.x * bar_value, size.y / 2.0f);
	D2D1_ELLIPSE circ = D2D1::Ellipse(
		circ_pos, 5, 5
	);
	render_target->DrawEllipse(circ, light_gray_brush, 4);

	render_target->SetTransform(prev_transform);
}

bool ColorRect::contains(const Point2& pos) {
	return position.x <= pos.x && pos.x <= position.x + size.x &&
		position.y <= pos.y && pos.y <= position.y + size.y;
}

void ColorRect::set_bar_value(FLOAT relative_x) {
	if (relative_x <= clamp_val * size.x) {
		bar_value = clamp_val;
	} else if (relative_x >= size.x * (1.0f - clamp_val)) {
		bar_value = 1.0f - clamp_val;
	} else {
		bar_value = (relative_x) / size.x;
	}
}

FLOAT ColorRect::get_value() const {
	return (bar_value - clamp_val) / (1.0f - 2.0f * clamp_val);
}

void ColorRect::on_click(const Point2& pos) {
	set_bar_value(pos.x - position.x);
}

void ColorRect::on_drag(const Point2& pos) {
	set_bar_value(pos.x - position.x);
}

void ColorRect::on_release() {
}