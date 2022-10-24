#include "dx2d.h"

namespace {
	using D2D1::RenderTargetProperties;
	using D2D1::HwndRenderTargetProperties;
	using D2D1::SizeU;

	using D2D1::Point2F;
	using D2D1::StrokeStyleProperties;

	// Sta³e z kolorami
	D2D1_COLOR_F const clear_color =
	{ .r = 0.0f, .g = 1.0f, .b = 1.0f, .a = 1.0f };
	D2D1_COLOR_F const brush_color_1 =
	{ .r = 0.0f, .g = 0.0f, .b = 0.75f, .a = 1.0f };
	D2D1_COLOR_F const brush_color_2 =
	{ .r = 1.0f, .g = 0.0f, .b = 0.0f, .a = 1.0f };
	D2D1_COLOR_F const brush_color_3 =
	{ .r = 1.0f, .g = 0.75f, .b = 0.0f, .a = 1.0f };
	D2D1_COLOR_F const brush_color_4 =
	{ .r = 0.0f, .g = 0.75f, .b = 0.0f, .a = 0.75f };
}

Dx2d::Dx2d(HWND hwnd)
{
	// Utworzenie fabryki Direct2D
	D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
		&d2d_factory);
	if (d2d_factory == nullptr) {
		exit(1);
	}

	GetClientRect(hwnd, &rc);

	// Utworzenie celu renderowania w oknie Windows
	d2d_factory->CreateHwndRenderTarget(
		RenderTargetProperties(),
		HwndRenderTargetProperties(hwnd,
			SizeU(static_cast<UINT32>(rc.right) -
				static_cast<UINT32>(rc.left),
				static_cast<UINT32>(rc.bottom) -
				static_cast<UINT32>(rc.top))),
		&d2d_render_target);

	// Utworzenie pêdzla i jego stylu
	d2d_render_target->CreateSolidColorBrush(brush_color_1, &_brush);
	d2d_factory->CreateStrokeStyle(
		StrokeStyleProperties(
			D2D1_CAP_STYLE_ROUND, D2D1_CAP_STYLE_ROUND,
			D2D1_CAP_STYLE_ROUND, D2D1_LINE_JOIN_MITER, 10.0f,
			D2D1_DASH_STYLE_DOT, 0.0f),
		nullptr, 0, &brush_style);

}

Dx2d::~Dx2d()
{
	// Bezpieczne zwolnienie zasobów
	if (d2d_render_target) d2d_render_target->Release();
	if (d2d_factory) d2d_factory->Release();
}

ID2D1HwndRenderTarget* Dx2d::render_target() const
{
	return d2d_render_target;
}

ID2D1SolidColorBrush* Dx2d::brush() const
{
	return _brush;
}
