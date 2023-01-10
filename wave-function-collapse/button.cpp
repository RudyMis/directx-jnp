#include "button.h"

void Button::draw(CComPtr<ID2D1HwndRenderTarget> render_target) {
	Pixel::draw(render_target);

	D2D1_MATRIX_3X2_F prev_transform;
	render_target->GetTransform(&prev_transform);
	render_target->SetTransform(prev_transform * D2D1::Matrix3x2F::Translation(
		position.x, position.y
	));
	render_target->DrawTextW(
		text, 22,
		text_format,
		D2D1::RectF(0, 0, size.x, size.y),
		black_brush
	);

	render_target->SetTransform(prev_transform);
}

HRESULT Button::create_text_format(CComPtr<IDWriteFactory> dw_factory) {
	HRESULT hr = dw_factory->CreateTextFormat(
		L"Times New Roman",
		nullptr,
		DWRITE_FONT_WEIGHT_BOLD,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		21.0f,
		L"en-us",
		&text_format
	); CHECK;

	hr = text_format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);

	return hr;
}

HRESULT Button::create_resources(CComPtr<ID2D1HwndRenderTarget> render_target) {
	HRESULT hr = Pixel::create_resources(render_target); CHECK;

	set_color(D2D1::ColorF(D2D1::ColorF::Beige));
	return render_target->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &black_brush);
}

void Button::on_click(const Point2& pos, Context& ctx) {
	callback();
}

void Button::set_callback(std::function<void(void)> cb) {
	callback = cb;
}
