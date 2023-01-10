#include "color_view.h"
#include "bitmaps.h"

HRESULT ColorView::create_resources(CComPtr<ID2D1HwndRenderTarget> render_target) {
	HRESULT hr = Pixel::create_resources(render_target); CHECK;

	if (!Bitmaps::wic_bitmaps.contains(L"slider.bmp")) {
		return E_FAIL;
	}

	return render_target->CreateBitmapFromWicBitmap(
		Bitmaps::wic_bitmaps.at(L"slider.bmp"),
		&bitmap
	);
}

void ColorView::draw(CComPtr<ID2D1HwndRenderTarget> render_target) {
	Pixel::draw(render_target);

	D2D1_MATRIX_3X2_F prev_transform;
	render_target->GetTransform(&prev_transform);
	render_target->SetTransform(prev_transform * D2D1::Matrix3x2F::Translation(
		position.x, position.y
	));

	render_target->DrawBitmap(bitmap);

	render_target->SetTransform(prev_transform);
}


