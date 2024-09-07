#include "Enemy.h"
#include <cassert>

Enemy::~Enemy() {}

void Enemy::Initialize(Model* modelEnemy, Vector3 position) { 
	// メンバ変数にモデルを設定
	assert(modelEnemy);
	modelEnemy_ = modelEnemy;

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;

	// 半径の初期値を設定
	radius_ = 1.0f;

	// ワールドトランスフォームを更新しておく
	worldTransform_.UpdateMatrix();
}

void Enemy::Update() {
	


	// 行列の更新
	worldTransform_.UpdateMatrix();
}

void Enemy::Draw(ViewProjection& viewProjection) {
	// モデルの描画
	modelEnemy_->Draw(worldTransform_, viewProjection);
}

void Enemy::OnCollision() {
	// あとでサイズとかを変える処理に変更するけどデバッグ用で一旦死亡させる
	isDead_ = true;
}

Vector3 Enemy::GetWorldPosition() { 
	Vector3 worldPos;

	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}
