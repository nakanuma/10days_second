#include "SphereCollider.h"

bool SphereCollider::IsCollision(SphereCollider& sphere) {
	// 2つの球の中心点間の距離を求める
	float distance = Vector3::Length(this->center - sphere.center);
	// 半径の合計よりも短ければ衝突
	if (distance <= this->radius + sphere.radius) {
		return true;
	} else {
		return false;
	}
}
