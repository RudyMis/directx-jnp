#include "rules_canvas.h"

HRESULT RulesCanvas::create_path(CComPtr<ID2D1Factory> dx_factory) {
	HRESULT hr = dx_factory->CreatePathGeometry(&circle_path); CHECK;

	CComPtr<ID2D1GeometrySink> sink;
	hr = circle_path->Open(&sink); CHECK;

	sink->BeginFigure(
		D2D1::Point2F(173, 0),
		D2D1_FIGURE_BEGIN_HOLLOW
	);

	sink->AddBezier(
		D2D1::BezierSegment(
			D2D1::Point2F(170, 0),
			D2D1::Point2F(170, 150),
			D2D1::Point2F(160, 160))
	);

	sink->AddBezier(
		D2D1::BezierSegment(
			D2D1::Point2F(150, 170),
			D2D1::Point2F(10, 170),
			D2D1::Point2F(0, 160))
	);

	sink->AddBezier(
		D2D1::BezierSegment(
			D2D1::Point2F(-10, 150),
			D2D1::Point2F(-10, 10),
			D2D1::Point2F(0, 0))
	);

	sink->AddBezier(
		D2D1::BezierSegment(
			D2D1::Point2F(10, -10),
			D2D1::Point2F(150, -10),
			D2D1::Point2F(160, 0))
	);

	sink->AddBezier(
		D2D1::BezierSegment(
			D2D1::Point2F(170, 10),
			D2D1::Point2F(175, 60),
			D2D1::Point2F(175, 80))
	);

	sink->EndFigure(D2D1_FIGURE_END_OPEN);

	hr = sink->Close(); 
	return hr;
}

HRESULT RulesCanvas::create_resources(CComPtr<ID2D1HwndRenderTarget> render_target) {
	HRESULT hr = PixelMatrix::create_resources(render_target); CHECK;
	return render_target->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &black_brush);
}

void RulesCanvas::draw(CComPtr<ID2D1HwndRenderTarget> render_target) {
	PixelMatrix::draw(render_target);

	D2D1_MATRIX_3X2_F prev_transform;
	render_target->GetTransform(&prev_transform);
	render_target->SetTransform(prev_transform * D2D1::Matrix3x2F::Translation({ position.x, position.y }));
	
	render_target->DrawGeometry(circle_path, black_brush, 4.f);
	
	render_target->SetTransform(prev_transform);
}

bool RulesCanvas::contains(const Point2& pos) {
	return clicked_pixel(pos).has_value();
}

void RulesCanvas::on_click(const Point2& pos, Context& ctx) {
	held_pixel = clicked_pixel(pos);
	if (!held_pixel) {
		return;
	}
	held_pixel->get().set_color(ctx.selected_color);
}

void RulesCanvas::on_drag(const Point2& pos, Context& ctx) {
	if (held_pixel && held_pixel->get().contains(pos)) {
		return;
	}
	held_pixel = clicked_pixel(pos);
	if (!held_pixel) {
		return;
	}
	held_pixel->get().set_color(ctx.selected_color);
}

void RulesCanvas::on_release(Context& ctx) {
	held_pixel = {};
}

std::optional<std::reference_wrapper<Pixel>> RulesCanvas::clicked_pixel(const Point2& pos) {
	auto chg_pos = D2D1::Point2(pos.x - position.x, pos.y - position.y);
	for (auto& row : pixels) {
		for (auto& pixel : row) {
			if (pixel.contains(chg_pos)) {
				return pixel;
			}
		}
	}
	return {};
}
