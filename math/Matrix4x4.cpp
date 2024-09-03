#include "Matrix4x4.h"
#include "Vector3.h"

Matrix4x4 Matrix4x4::operator+(const Matrix4x4& other) const { 
	Matrix4x4 result;
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			result.m[i][j] = this->m[i][j] + other.m[i][j];
		}
	}
	return result;
}

Matrix4x4 Matrix4x4::operator-(const Matrix4x4& other) const { 
	Matrix4x4 result;
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			result.m[i][j] = this->m[i][j] - other.m[i][j];
		}
	}
	return result;
}

Matrix4x4 Matrix4x4::operator*(const Matrix4x4& other) const { 
	Matrix4x4 result;
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			result.m[i][j] = 0.0f;
			for (int k = 0; k < 4; ++k) {
				result.m[i][j] += this->m[i][k] * other.m[k][j];
			}
		}
	}
	return result;
}

Matrix4x4& Matrix4x4::operator+=(const Matrix4x4& other) {
	*this = *this + other;
	return *this;
}

Matrix4x4& Matrix4x4::operator-=(const Matrix4x4& other) {
	*this = *this - other;
	return *this;
}

Matrix4x4& Matrix4x4::operator*=(const Matrix4x4& other) {
	*this = *this * other;
	return *this;
}

Matrix4x4 Matrix4x4::MakeIdentity() { 
	Matrix4x4 result;

	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			result.m[i][j] = (i == j) ? 1.0f : 0.0f; // 対角成分のみ1を入れる
		}
	}

	return result;
}

Matrix4x4 Matrix4x4::MakeTranslate(const Vector3& translate) { 
	Matrix4x4 result = MakeIdentity();

	result.m[3][0] = translate.x;
	result.m[3][1] = translate.y;
	result.m[3][2] = translate.z;

	return result;
}

Matrix4x4 Matrix4x4::MakeScale(const Vector3& scale) { 
	Matrix4x4 result = MakeIdentity();

	result.m[0][0] = scale.x;
	result.m[1][1] = scale.y;
	result.m[2][2] = scale.z;

	return result;
}

Matrix4x4 Matrix4x4::MakeRotateX(float radian) { 
	Matrix4x4 result = MakeIdentity();

	result.m[1][1] = std::cosf(radian);
	result.m[2][1] = -std::sinf(radian);
	result.m[1][2] = std::sinf(radian);
	result.m[2][2] = std::cosf(radian);

	return result;
}

Matrix4x4 Matrix4x4::MakeRotateY(float radian) { 
	Matrix4x4 result = MakeIdentity();

	result.m[0][0] = std::cosf(radian);
	result.m[2][0] = std::sinf(radian);
	result.m[0][2] = -std::sinf(radian);
	result.m[2][2] = std::cosf(radian);

	return result;
}

Matrix4x4 Matrix4x4::MakeRotateZ(float radian) { 
	Matrix4x4 result = MakeIdentity();

	result.m[0][0] = std::cosf(radian);
	result.m[1][0] = -std::sinf(radian);
	result.m[0][1] = std::sinf(radian);
	result.m[1][1] = std::cosf(radian);

	return result;
}

Matrix4x4 Matrix4x4::MakeRotateXYZ(float roll, float pitch, float yaw) { 
	Matrix4x4 result = MakeIdentity();

	return result * MakeRotateX(roll) * MakeRotateY(pitch) * MakeRotateZ(yaw);
}

Matrix4x4 Matrix4x4::Inverse(const Matrix4x4& m) { 
	Matrix4x4 result = MakeIdentity();
	float temp[4][8] = {};
	float a;

	// 一時行列にコピー
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			temp[i][j] = m.m[i][j];

			// 右側の4列に単位行列を入れる
			if (i == j)
				temp[i][4 + j] = 1;
		}
	}

	// 各列を正規化
	for (int k = 0; k < 4; k++) {
		a = 1 / temp[k][k];

		// 対角成分を1に正規化
		for (int j = 0; j < 8; j++) {
			temp[k][j] *= a;
		}

		// 掃き出し法を使って他の要素を0にする
		for (int i = 0; i < 4; i++) {
			if (i == k) {
				continue;
			}

			a = -temp[i][k];

			for (int j = 0; j < 8; j++) {
				temp[i][j] += temp[k][j] * a;
			}
		}
	}

	// 逆行列をresultにコピー
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			result.m[i][j] = temp[i][4 + j];
		}
	}

	return result;
}

Matrix4x4 Matrix4x4::Transpose(const Matrix4x4& m) { 
	Matrix4x4 result;

	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			result.m[i][j] = m.m[j][i];
		}
	}

	return result;
}

Matrix4x4 Matrix4x4::MakeAffine(const Vector3& scale, const Vector3& rotate, const Vector3& translate) { 
	Matrix4x4 result = MakeIdentity();

	result *= MakeScale(scale);
	result *= MakeRotateXYZ(rotate.x, rotate.y, rotate.z);
	result *= MakeTranslate(translate);

	return result;
}
