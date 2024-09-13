#pragma once
#include "Matrix4x4.h"
#include "Vector3.h"
#include "SphereCollider.h"

class OBBCollider {
public:
	// AABB
	struct AABB {
		Vector3 min;
		Vector3 max;
	};

	Vector3 center; // 中心点
	Vector3 size;   // 座標軸方向の長さの半分
	Vector3 rotate; // 回転角

public:
	// AABBと球の衝突判定
	bool IsCollision(const AABB& aabb, const SphereCollider& sphere);
	// OBBと球の衝突判定
	bool IsCollision(const SphereCollider& sphere);
};
