#include "Enemy.h"
#include <cassert>
#include <algorithm>

#include "imgui.h"

Enemy::~Enemy() {}

void Enemy::Initialize(Model* modelEnemy, Vector3 spawnPosition) {
	///
	///	敵の情報初期化
	/// 
	
	// ワールドトランスフォーム初期化
	worldTransform_.Initialize();
	// 引数で受け取ったスポーン座標に設定
	worldTransform_.translation_ = spawnPosition;

	// 引数で受け取ったモデルをメンバ変数に記録
	assert(modelEnemy);
	modelEnemy_ = modelEnemy;

	// 移動速度の初期値を設定
	speed_ = 0.2f;
	// 現在の向きの初期値を設定
	// 最初は左上で湧いて、右に進んでいく想定
	direction_ = kRightDirection;

	// スポーン時に原点に一瞬現れてしまうのを回避するために、ワールド行列を更新
	worldTransform_.UpdateMatrix();
}

void Enemy::Update() {
	///
	///	向いている方向へ移動させる
	/// 

	worldTransform_.translation_ += (direction_ * speed_);

	///
	///	敵が四つ角へ到達した時に向きを変える処理
	///
	
	ChangeDirection();

	///
	///	行列の更新
	/// 
	
	worldTransform_.UpdateMatrix();

	///
	///	デバッグ表示
	/// 
	
	Debug();
}

void Enemy::ChangeDirection() {
	// 現在が右向きかつ右の線（x=16）を超えたら下向きに変更
	if (direction_.x == kRightDirection.x && direction_.y == kRightDirection.y && GetWorldPosition().x >= 16.0f) {
		// 微弱なズレを修正
		worldTransform_.translation_.x = 16.0f;
		// 移動方向を下向きにする
		direction_ = kDownDirection;
	}

	// 現在が下向きかつ下の線（y=-16）を超えたら左向きに変更
	if (direction_.x == kDownDirection.x && direction_.y == kDownDirection.y && GetWorldPosition().y <= -16.0f) {
		// 微弱なズレを修正
		worldTransform_.translation_.y = -16.0f;
		// 移動方向を左向きにする
		direction_ = kLeftDirection;
	}

	// 現在が左向きかつ左の線（x=-16）を超えたら上向きに変更
	if (direction_.x == kLeftDirection.x && direction_.y == kLeftDirection.y && GetWorldPosition().x <= -16.0f) {
		// 微弱なズレを修正
		worldTransform_.translation_.x = -16.0f;
		// 移動方向を上向きにする
		direction_ = kUpDirection;
	}

	// 現在が上向きかつ上の線（y=16）を超えたら右向きに変更
	if (direction_.x == kUpDirection.x && direction_.y == kUpDirection.y && GetWorldPosition().y >= 16.0f) {
		// 微弱なズレを修正
		worldTransform_.translation_.y = 16.0f;
		// 移動方向を右向きにする
		direction_ = kRightDirection;
	}

}

void Enemy::Draw(ViewProjection& viewProjection) {
	///
	///	敵本体の描画
	/// 
	
	modelEnemy_->Draw(worldTransform_, viewProjection);
}

void Enemy::Debug() { 
	/*ImGui::Begin("Enemy"); 

	ImGui::DragFloat3("Translation", &worldTransform_.translation_.x, 0.01f);

	ImGui::End();*/
}

Vector3 Enemy::GetWorldPosition() { 
	Vector3 worldPos;

	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}
