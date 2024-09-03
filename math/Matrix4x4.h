#pragma once
#include <cmath>
#include <cassert>

struct Vector3;

/// <summary>
/// 4x4行列
/// </summary>
struct Matrix4x4 final {
	float m[4][4];

	///
	///	演算子オーバーロード
	/// 
	
	Matrix4x4 operator+(const Matrix4x4& other) const;
	Matrix4x4 operator-(const Matrix4x4& other) const;
	Matrix4x4 operator*(const Matrix4x4& other) const;

	Matrix4x4& operator+=(const Matrix4x4& other);
	Matrix4x4& operator-=(const Matrix4x4& other);
	Matrix4x4& operator*=(const Matrix4x4& other);

	///
	///	基本的な関数
	/// 
	
	// 単位行列
	static Matrix4x4 MakeIdentity();

	// 平行移動行列
	static Matrix4x4 MakeTranslate(const Vector3& translate);
	// 拡大縮小行列
	static Matrix4x4 MakeScale(const Vector3& scale);

	// X軸回転行列
	static Matrix4x4 MakeRotateX(float radian);
	// Y軸回転行列
	static Matrix4x4 MakeRotateY(float radian);
	// Z軸回転行列
	static Matrix4x4 MakeRotateZ(float radian);
	// XYZ全て掛けて回転
	static Matrix4x4 MakeRotateXYZ(float roll, float pitch, float yaw);

	// 逆行列
	static Matrix4x4 Inverse(const Matrix4x4& m);
	// 転置行列
	static Matrix4x4 Transpose(const Matrix4x4& m);
	// アフィン変換行列
	static Matrix4x4 MakeAffine(const Vector3& scale, const Vector3& rotate, const Vector3& translate);
};
