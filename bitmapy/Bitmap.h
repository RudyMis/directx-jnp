#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <d2d1_3.h>
#include <dcommon.h>
#include <array>
#include <functional>

class Bitmap {
	ID2D1Bitmap* bitmap;
public:
	Bitmap(ID2D1HwndRenderTarget* render_target);
	~Bitmap();
	const ID2D1Bitmap* get() const;
};