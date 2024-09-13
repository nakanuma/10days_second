#include "OBBCollider.h"
#include <algorithm>

bool OBBCollider::IsCollision(const AABB& aabb, const SphereCollider& sphere) { 
	// 最近接点を求める
	Vector3 closestPoint{
	    std::clamp(sphere.center.x, aabb.min.x, aabb.max.x),
	    std::clamp(sphere.center.y, aabb.min.y, aabb.max.y),
	    std::clamp(sphere.center.z, aabb.min.z, aabb.max.z),
	};
	// 最近接点と球の中心との距離を求める
	float distance = Vector3::Length(closestPoint - sphere.center);
	// 距離が半径より小さければ衝突
	if (distance <= sphere.radius) {
		return true;
	}

	return false;
}

bool OBBCollider::IsCollision(const SphereCollider& sphere) { 
	// rotateMatrixの作成
	Matrix4x4 rotateMatrix = Matrix4x4::MakeRotateZ(rotate.z) * Matrix4x4::MakeRotateX(rotate.x) * Matrix4x4::MakeRotateY(rotate.y);

	// OBBのWorldMatrixを作る
	Matrix4x4 obbWorldMatrix = rotateMatrix;
	obbWorldMatrix.m[0][3] = 0;
	obbWorldMatrix.m[1][3] = 0;
	obbWorldMatrix.m[2][3] = 0;
	obbWorldMatrix.m[3][3] = 1;

	// 平行移動成分を代入
	obbWorldMatrix.m[3][0] = this->center.x;
	obbWorldMatrix.m[3][1] = this->center.y;
	obbWorldMatrix.m[3][2] = this->center.z;

	// OBBのWorldMatrixの逆行列を求める
	Matrix4x4 obbWorldMatrixInverse = Matrix4x4::Inverse(obbWorldMatrix);

	// 中心点をローカル空間上の点にする
	Vector3 centerInOBBLocalSpace = Vector3::Transform(sphere.center, obbWorldMatrixInverse);

	// OBBからAABBを作る
	AABB aabbOBBLocal{.min = Vector3::Multiply(-1.0f, this->size), .max = this->size};
	// 球を作る
	SphereCollider sphereOBBLocal{centerInOBBLocalSpace, sphere.radius};

	// ローカル空間で衝突判定
	if (IsCollision(aabbOBBLocal, sphereOBBLocal)) {
		return true;
	}

	return false;
}
