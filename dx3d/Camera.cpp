#include "Camera.h"

namespace dx = DirectX;

Camera::Camera():
    position(0, 0.2f, 2),
    yaw(dx::XM_PI),
    pitch(0.0f),
    look_direction(0, 0, -1),
    up_direction(0, 1, 0),
    move_speed(0.02f),
    turn_speed(0.02f),
    keys_pressed{}
{
}

void Camera::update() {
    dx::XMFLOAT3 move(0, 0, 0);

    if (keys_pressed.a) {
        move.x -= 1.0f;
    }
    if (keys_pressed.d) {
        move.x += 1.0f;
    }
    if (keys_pressed.w) {
        move.z -= 1.0f;
    }
    if (keys_pressed.s) {
        move.z += 1.0f;
    }

    if (fabs(move.x) > 0.1f && fabs(move.z) > 0.1f) {
        dx::XMVECTOR normalized = dx::XMVector3Normalize(dx::XMLoadFloat3(&move));
        move.x = dx::XMVectorGetX(normalized);
        move.z = dx::XMVectorGetZ(normalized);
    }

    float move_distance = move_speed;
    float rotate_distance = turn_speed;

    if (keys_pressed.left) {
        yaw += rotate_distance;
    }
    if (keys_pressed.right) {
        yaw -= rotate_distance;
    }
    if (keys_pressed.up) {
        pitch += rotate_distance;
    }
    if (keys_pressed.down) {
        pitch -= rotate_distance;
    }

    pitch = std::min(pitch, dx::XM_PIDIV4);
    pitch = std::max(-dx::XM_PIDIV4, pitch);

    float x = move.x * -cosf(yaw) - move.z * sinf(yaw);
    float z = move.x * sinf(yaw) - move.z * cosf(yaw);
    position.x += x * move_distance;
    position.z += z * move_distance;

    look_direction.x = cosf(pitch) * sinf(yaw);
    look_direction.y = sinf(pitch);
    look_direction.z = cosf(pitch) * cosf(yaw);
}

dx::XMMATRIX Camera::get_view_matrix() {
    return dx::XMMatrixLookToRH(
        dx::XMLoadFloat3(&position),
        dx::XMLoadFloat3(&look_direction),
        dx::XMLoadFloat3(&up_direction));
}

dx::XMMATRIX Camera::get_projection_matrix(float fov, float aspect_ratio, float near_plane, float far_plane) {
    return dx::XMMatrixPerspectiveFovRH(fov, aspect_ratio, near_plane, far_plane);
}

void Camera::set_move_speed(float unit_per_second) {
    move_speed = unit_per_second;
}

void Camera::set_turn_speed(float radians_per_second) {
    turn_speed = radians_per_second;
}

void Camera::on_key_down(WPARAM key) {
    switch (key) {
    case 'W':
        keys_pressed.w = true;
        break;
    case 'A':
        keys_pressed.a = true;
        break;
    case 'S':
        keys_pressed.s = true;
        break;
    case 'D':
        keys_pressed.d = true;
        break;
    case VK_LEFT:
        keys_pressed.left = true;
        break;
    case VK_RIGHT:
        keys_pressed.right = true;
        break;
    case VK_UP:
        keys_pressed.up = true;
        break;
    case VK_DOWN:
        keys_pressed.down = true;
        break;
    }
}

void Camera::on_key_up(WPARAM key) {
    switch (key) {
    case 'W':
        keys_pressed.w = false;
        break;
    case 'A':
        keys_pressed.a = false;
        break;
    case 'S':
        keys_pressed.s = false;
        break;
    case 'D':
        keys_pressed.d = false;
        break;
    case VK_LEFT:
        keys_pressed.left = false;
        break;
    case VK_RIGHT:
        keys_pressed.right = false;
        break;
    case VK_UP:
        keys_pressed.up = false;
        break;
    case VK_DOWN:
        keys_pressed.down = false;
        break;
    }
}
