#include "Bitmap.h"
#include <array>

Bitmap::Bitmap(ID2D1HwndRenderTarget* render_target) {
	UINT const height = 2;
	UINT const width = 2;
	std::array<BYTE, 4 * width * height> bits = {
		0, 0, 230, 230,			128, 128, 128, 128,
		255, 255, 255, 255		0, 0, 255, 255
	};
	D2D1_COLOR_F const clear_color =
	{ .r = 0.75f, .g = 0.5f, .b = 0.25f, .a = 1.0f };
	D2D1_COLOR_F const brush_color =
	{ .r = 0.0f, .g = 0.5f, .b = 0.0f, .a = 1.0f };

	render_target->CreateBitmap(
		D2D1::SizeU(width, height),
		bits.data(),
		4 * width,
		D2D1::BitmapProperties(
			D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
				D2D1_ALPHA_MODE_PREMULTIPLIED)
		),
		&bitmap
	);
}

Bitmap::~Bitmap() {
	if (bitmap) {
		bitmap->Release();
	}
}

const ID2D1Bitmap* Bitmap::get() const {
	return bitmap;
}