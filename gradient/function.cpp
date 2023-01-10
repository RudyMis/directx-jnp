#include "function.h"
#include <ranges>
#include <numbers>

namespace {
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

	FLOAT const ROTATION_SPEED = 1;
}

void Function::draw(Dx2d& dx2d)
{
	dx2d.render_target()->BeginDraw();
	dx2d.render_target()->Clear(clear_color);
	auto rect = dx2d.render_target()->GetPixelSize();
	PointMatrix p = scale_points(points, D2D1::Vector3F(250.0, 250.0, 250.0));
	p = move_points(p, D2D1::Vector3F(rect.width / 2, rect.height / 2, 0.0));
	for (size_t i = 0; i < p.size(); i++) {
		for (size_t j = 0; j < p[i].size(); j++) {
			dx2d.brush()->SetColor(brush_color_1);
			if (j < p[i].size() - 1) {
				dx2d.render_target()->DrawLine(Point2F(p[i][j].x, p[i][j].y),
					Point2F(p[i][j + 1].x, p[i][j + 1].y),
					dx2d.brush(), 1.0f);
			}
			if (i < p.size() - 1) {
				dx2d.render_target()->DrawLine(Point2F(p[i][j].x, p[i][j].y),
					Point2F(p[i + 1][j].x, p[i + 1][j].y),
					dx2d.brush(), 1.0f);
			}
		}
	}
	dx2d.render_target()->EndDraw();
}

void Function::update(FLOAT delta)
{
	points = rotate_points(points, D2D1::Vector3F(delta * ROTATION_SPEED, 0.0f, 0.0f));
}

void Function::calculate_function(std::function<FLOAT(FLOAT, FLOAT)> fun)
{
	for (size_t i = 0; i < points.size(); i++) {
		for (size_t j = 0; j < points[i].size(); j++) {
			points[i][j].x = (FLOAT)((FLOAT)i - (FLOAT)points.size() / 2.0f) / points.size() * 2.0f;
			points[i][j].y = (FLOAT)((FLOAT)j - (FLOAT)points[j].size() / 2.0f) / points[j].size() * 2.0f;
			points[i][j].z = fun(points[i][j].x, points[i][j].y);
		}
	}
	points = rotate_points(points, D2D1::Vector3F(0.0, 0.0, std::numbers::pi / 4.0));
}

PointMatrix Function::scale_points(const PointMatrix& points, D2D1_VECTOR_3F scale) const
{
	PointMatrix res = PointMatrix();
	for (size_t i = 0; i < points.size(); i++) {
		for (size_t j = 0; j < points[i].size(); j++) {
			res[i][j] = D2D1::Vector3F(
				points[i][j].x * scale.x,
				points[i][j].y * scale.y,
				points[i][j].z * scale.z);
		}
	}
	return res;
}

PointMatrix Function::rotate_points(const PointMatrix& points, D2D1_VECTOR_3F rotator) const {
	PointMatrix res = PointMatrix();
	for (size_t i = 0; i < points.size(); i++) {
		for (size_t j = 0; j < points[i].size(); j++) {
			res[i][j] = points[i][j];
			res[i][j] = D2D1::Vector3F(
				res[i][j].x * std::cos(rotator.z) - res[i][j].y * std::sin(rotator.z),
				res[i][j].x * std::sin(rotator.z) + res[i][j].y * std::cos(rotator.z),
				res[i][j].z);
			res[i][j] = D2D1::Vector3F(
				res[i][j].x,
				res[i][j].y * std::cos(rotator.x) - res[i][j].z * std::sin(rotator.x),
				res[i][j].y * std::sin(rotator.x) + res[i][j].z * std::cos(rotator.x));
			res[i][j] = D2D1::Vector3F(
				res[i][j].x * std::cos(rotator.y) - res[i][j].z * std::sin(rotator.y),
				res[i][j].y,
				res[i][j].x * std::sin(rotator.y) + res[i][j].z * std::cos(rotator.y));
		}
	}
	return res;
}

PointMatrix Function::move_points(const PointMatrix& points, D2D1_VECTOR_3F delta) const
{
	PointMatrix res = PointMatrix();
	for (size_t i = 0; i < points.size(); i++) {
		for (size_t j = 0; j < points[i].size(); j++) {
			res[i][j] = D2D1::Vector3F(
				points[i][j].x + delta.x,
				points[i][j].y + delta.y,
				points[i][j].z + delta.z);
		}
	}
	return res;
}
