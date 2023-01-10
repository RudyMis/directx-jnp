#pragma once


#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <d2d1_3.h>


class Dx2d {
	ID2D1Factory7* d2d_factory = nullptr;
	ID2D1HwndRenderTarget* d2d_render_target = nullptr;

	RECT rc;

	ID2D1SolidColorBrush* _brush = nullptr;
	ID2D1StrokeStyle* brush_style = nullptr;

public:
	Dx2d(HWND hwnd);

	~Dx2d();
	ID2D1HwndRenderTarget* render_target() const;
	ID2D1SolidColorBrush* brush() const;
};


