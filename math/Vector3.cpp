#include "Vector3.h"
#include "Matrix4x4.h"

Vector3 Vector3::operator+(const Vector3& other) const { 
	return {x + other.x, y + other.y, z + other.z}; 
}

Vector3 Vector3::operator-(const Vector3& other) const { 
	return {x - other.x, y - other.y, z - other.z}; 
}

Vector3 Vector3::operator*(float scalar) const { 
	return {x * scalar, y * scalar, z * scalar}; 
}

Vector3 Vector3::operator/(float scalar) const { 
	return {x / scalar, y / scalar, z / scalar}; 
}

Vector3& Vector3::operator+=(const Vector3& other) {
	x += other.x;
	y += other.y;
	z += other.z;
	return *this;
}

Vector3& Vector3::operator-=(const Vector3& other) {
	x -= other.x;
	y -= other.y;
	z -= other.z;
	return *this;
}

Vector3& Vector3::operator*=(const float& scalar) {
	x *= scalar;
	y *= scalar;
	z *= scalar;
	return *this;
}

Vector3& Vector3::operator/=(const float& scalar) {
	x /= scalar;
	y /= scalar;
	z /= scalar;
	return *this;
}

Vector3 Vector3::Multiply(float scalar, const Vector3& v) { 
	return Vector3{
		scalar * v.x, 
		scalar * v.y, 
		scalar * v.z}; 
}

float Vector3::Length(const Vector3& v) { 
	return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z); 
}

Vector3 Vector3::Normalize(const Vector3& v) {
	float norm = Length(v);

	return Vector3(v.x / norm, v.y / norm, v.z / norm);
}

float Vector3::Dot(const Vector3& v1, const Vector3& v2) { 
	return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z); 
}

Vector3 Vector3::Cross(const Vector3& v1, const Vector3& v2) { 
	return Vector3{
		(v1.y * v2.z) - (v1.z * v2.y), 
		(v1.z * v2.x) - (v1.x * v2.z), 
		(v1.x * v2.y) - (v1.y * v2.x)
	}; 
}

Vector3 Vector3::Transform(const Vector3& v, const Matrix4x4 m) { 
	Vector3 result;

	result.x = v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0] + 1.0f * m.m[3][0];
	result.y = v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1] + 1.0f * m.m[3][1];
	result.z = v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2] + 1.0f * m.m[3][2];
	float w = v.x * m.m[0][3] + v.y * m.m[1][3] + v.z * m.m[2][3] + 1.0f * m.m[3][3];
	assert(w != 0.0f);

	result.x /= w;
	result.y /= w;
	result.z /= w;

	return result;
}
