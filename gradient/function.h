#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <d2d1_3.h>
#include <dcommon.h>
#include <array>
#include <functional>
#include "dx2d.h"

using PointMatrix = std::array<std::array<D2D1_VECTOR_3F, 51>, 51>;

class Function {
	PointMatrix points;

	PointMatrix scale_points(const PointMatrix& points, D2D1_VECTOR_3F scale) const;
	PointMatrix move_points(const PointMatrix& points, D2D1_VECTOR_3F delta) const;
	PointMatrix rotate_points(const PointMatrix& points, D2D1_VECTOR_3F rotator) const;

public:
	void calculate_function(std::function<FLOAT(FLOAT, FLOAT)> fun);
	void draw(Dx2d& dx2d);
	void update(FLOAT delta);
};
