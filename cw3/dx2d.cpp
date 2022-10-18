#include "dx2d.h"

namespace {
	using D2D1::RenderTargetProperties;
	using D2D1::HwndRenderTargetProperties;
	using D2D1::SizeU;

	using D2D1::Point2F;
	using D2D1::StrokeStyleProperties;

	ID2D1Factory7* d2d_factory = nullptr;
	ID2D1HwndRenderTarget* d2d_render_target = nullptr;

	RECT rc;

	ID2D1SolidColorBrush* brush = nullptr;
	ID2D1StrokeStyle* brush_style = nullptr;

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

void init_direct_2d(HWND hwnd)
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
	d2d_render_target->CreateSolidColorBrush(brush_color_1, &brush);
	d2d_factory->CreateStrokeStyle(
		StrokeStyleProperties(
			D2D1_CAP_STYLE_ROUND, D2D1_CAP_STYLE_ROUND,
			D2D1_CAP_STYLE_ROUND, D2D1_LINE_JOIN_MITER, 10.0f,
			D2D1_DASH_STYLE_DOT, 0.0f),
		nullptr, 0, &brush_style);

}

void destroy_direct_2d()
{
	// Bezpieczne zwolnienie zasobów
	if (d2d_render_target) d2d_render_target->Release();
	if (d2d_factory) d2d_factory->Release();
}

void paint(HWND hwnd)
{
	d2d_render_target->BeginDraw();
	d2d_render_target->Clear(clear_color);
	brush->SetColor(brush_color_1);
	d2d_render_target->DrawLine(Point2F(0.0f, 200.0f),
		Point2F(400.0f, 200.0f),
		brush, 1.0f);
	brush->SetColor(brush_color_2);
	d2d_render_target->DrawLine(Point2F(0.0f, 300.0f),
		Point2F(400.0f, 300.0f),
		brush, 8.0f, brush_style);
	brush->SetColor(brush_color_3);
	d2d_render_target->DrawLine(Point2F(0.0f, 400.0f),
		Point2F(400.0f, 400.0f),
		brush, 10.0f);
	brush->SetColor(brush_color_4);
	d2d_render_target->DrawLine(Point2F(0.0f, 0.0f),
		Point2F(400.0f, 600.0f),
		brush, 26.0f);

	d2d_render_target->EndDraw();
}
