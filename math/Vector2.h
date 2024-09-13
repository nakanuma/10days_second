#pragma once

/// <summary>
/// 2次元ベクトル
/// </summary>
struct Vector2 final {
	float x;
	float y;

	Vector2 operator+(const Vector2& other) const { return {x + other.x, y + other.y}; }
};
