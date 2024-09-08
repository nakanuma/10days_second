#pragma once
#include "Vector3.h"

class SphereCollider {
public:
	Vector3 center; // 中心点
	float radius;   // 半径

	// 球と球の衝突判定
	bool IsCollision(SphereCollider& sphere);
};
