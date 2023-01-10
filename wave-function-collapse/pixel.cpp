#include "pixel.h"

HRESULT Pixel::create_resources(CComPtr<ID2D1HwndRenderTarget> render_target) {
	return render_target->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::White, 1),
		&brush
	);
}

void Pixel::discard_resources() {
	brush.Release();
}

void Pixel::set_color(const D2D1_COLOR_F& color) {
	brush->SetColor(color);
}

void Pixel::set_size(const Point2& size) {
	this->size = size;
}

void Pixel::draw(CComPtr<ID2D1HwndRenderTarget> render_target) {
	D2D1_MATRIX_3X2_F prev_transform;
	render_target->GetTransform(&prev_transform);
	render_target->SetTransform(prev_transform * D2D1::Matrix3x2F::Translation(
		position.x, position.y
	));
	D2D1_RECT_F rect = D2D1::RectF(
		0,
		0,
		size.x,
		size.y
	);
	render_target->FillRectangle(&rect, brush);

	render_target->SetTransform(prev_transform);
}

bool Pixel::contains(const Point2& pos) const {
	return position.x <= pos.x && pos.x <= position.x + size.x &&
		position.y <= pos.y && pos.y <= position.y + size.y;
}

void Pixel::on_click(const Point2& pos, Context& ctx) {
}

void Pixel::on_drag(const Point2& pos, Context& ctx) {
}

void Pixel::on_release(Context& ctx) {
}

D2D1_COLOR_F Pixel::color() const {
	if (!brush) {
		return D2D1_COLOR_F();
	}
	return brush->GetColor();
}
