#pragma once
#include "helper.h"

class Camera {
public:
	Camera();

	void update();
	DirectX::XMMATRIX get_view_matrix();
	DirectX::XMMATRIX get_projection_matrix(float fov, float aspect_ratio, float near_plane = 1.0f, float far_plane = 1000.0f);
	void set_move_speed(float unit_per_second);
	void set_turn_speed(float radians_per_second);

	void on_key_down(WPARAM key);
	void on_key_up(WPARAM key);

private:

	struct KeysPressed {
		bool w, a, s, d;
		bool left, right, up, down;
	};

	DirectX::XMFLOAT3 position;
	float yaw;
	float pitch;
	DirectX::XMFLOAT3 look_direction;
	DirectX::XMFLOAT3 up_direction;
	float move_speed;
	float turn_speed;

	KeysPressed keys_pressed;
};

