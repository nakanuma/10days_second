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
	// 誤差許容範囲を設定
	const float epsilon = 0.01f;

	// 右上の角に到達した際には下向きに移動させる
	{
		// 右上の角に到達した際には下向きに移動させる
		if (fabs(worldTransform_.translation_.x - kOutsideFrameTopRightPos.x) < epsilon && fabs(worldTransform_.translation_.y - kOutsideFrameTopRightPos.y) < epsilon) {
			// 微小にずれたX座標を正しい値にする
			worldTransform_.translation_.x = kOutsideFrameBottomRightPos.x;
			direction_ = kDownDirection;
		}
	}
	// 右下の角に到達した際には左向きに移動させる
	{
		if (fabs(worldTransform_.translation_.x - kOutsideFrameBottomRightPos.x) < epsilon && fabs(worldTransform_.translation_.y - kOutsideFrameBottomRightPos.y) < epsilon) {
			// 微小にずれたY座標を正しい値にする
			worldTransform_.translation_.y = kOutsideFrameBottomLeftPos.y;
			direction_ = kLeftDirection;
		}
	}
	// 左下の角に到達した際には上向きに移動させる
	{
		if (fabs(worldTransform_.translation_.x - kOutsideFrameBottomLeftPos.x) < epsilon && fabs(worldTransform_.translation_.y - kOutsideFrameBottomLeftPos.y) < epsilon) {
			// 微小にずれたX座標を正しい値にする
			worldTransform_.translation_.x = kOutsideFrameTopLeftPos.x;
			direction_ = kUpDirection;
		}
	}
	// 左上の角に到達した際には右向きに移動させる
	{
		if (fabs(worldTransform_.translation_.x - kOutsideFrameTopLeftPos.x) < epsilon && fabs(worldTransform_.translation_.y - kOutsideFrameTopLeftPos.y) < epsilon) {
			// 微小にずれたY座標を正しい値にする
			worldTransform_.translation_.y = kOutsideFrameTopRightPos.y;
			direction_ = kRightDirection;
		}
	}
}

void Enemy::Draw(ViewProjection& viewProjection) {
	///
	///	敵本体の描画
	/// 
	
	modelEnemy_->Draw(worldTransform_, viewProjection);
}

void Enemy::Debug() { 
	ImGui::Begin("Enemy"); 

	ImGui::DragFloat3("Translation", &worldTransform_.translation_.x, 0.01f);

	ImGui::End();
}

Vector3 Enemy::GetWorldPosition() { 
	Vector3 worldPos;

	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}
