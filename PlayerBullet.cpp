#include "PlayerBullet.h"
#include <cassert>

void PlayerBullet::Initialize(Model* model, const Vector3& position, const Vector3& velocity) {
	// NULLポインタチェックをしてメンバ変数に設定
	assert(model);
	model_ = model;

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	// 位置と速度を設定
	worldTransform_.translation_ = position;
	velocity_ = velocity;
}

void PlayerBullet::Update() {
	// 座標の移動
	worldTransform_.translation_ += velocity_;

	// 時間経過でデス
	if (--deathTimer_ <= 0) {
		isDead_ = true;
	}

	// 行列の更新
	worldTransform_.UpdateMatrix();
}

void PlayerBullet::Draw(const ViewProjection& viewProjection) {
	// モデル描画
	model_->Draw(worldTransform_, viewProjection);
}

void PlayerBullet::OnCollision() {
	// 弾を死亡させる
	isDead_ = true;
}

Vector3 PlayerBullet::GetWorldPosition() { 
	Vector3 worldPos;

	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}
