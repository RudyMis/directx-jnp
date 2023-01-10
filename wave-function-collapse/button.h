#pragma once
#include <functional>
#include "pixel.h"
class Button : public Pixel {
public:
	Button(Point2 _position, Point2 _size) : Pixel(_position, _size), text{ (WCHAR*)L"Stwórz dziwny obrazek" } {}

	void draw(CComPtr<ID2D1HwndRenderTarget> render_target);

	HRESULT create_text_format(CComPtr<IDWriteFactory> dw_factory);
	HRESULT create_resources(CComPtr<ID2D1HwndRenderTarget> render_target);

	void on_click(const Point2& pos, Context& ctx) override;

	void set_callback(std::function<void(void)> cb);

private:
	const LPWSTR text;
	CComPtr<IDWriteTextFormat> text_format;
	CComPtr<ID2D1SolidColorBrush> black_brush;
	std::function<void(void)> callback;
};

