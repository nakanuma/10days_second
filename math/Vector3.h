#pragma once
#include <cmath>
#include <cassert>

struct Matrix4x4;

/// <summary>
/// 3次元ベクトル
/// </summary>
struct Vector3 final {
	float x;
	float y;
	float z;

	///
	///	演算子オーバーロード
	/// 

	Vector3 operator+(const Vector3& other) const;
	Vector3 operator-(const Vector3& other) const;
	Vector3 operator*(float scalar) const;
	Vector3 operator/(float scalar) const;

	Vector3& operator+=(const Vector3& other);
	Vector3& operator-=(const Vector3& other);
	Vector3& operator*=(const float& scalar);
	Vector3& operator/=(const float& scalar);

	///
	///	基本的な関数
	/// 

	// スカラー倍
	static Vector3 Multiply(float scalar, const Vector3& v);
	// 長さ
	static float Length(const Vector3& v);
	// 正規化
	static Vector3 Normalize(const Vector3& v);
	// ドット積
	static float Dot(const Vector3& v1, const Vector3& v2);
	// クロス積
	static Vector3 Cross(const Vector3& v1, const Vector3& v2);
	// 行列変換
	static Vector3 Transform(const Vector3& v, const Matrix4x4 m);
};

inline Vector3 operator*(float scalar, const Vector3& v) { return v * scalar; }