#include "Enemy.h"

#include "imgui.h"

#include <cassert>
#include <algorithm>

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
	// 自動で下降する速度を設定
	fallSpeed_ = 0.1f;

	// ワールドトランスフォームを更新しておく
	worldTransform_.UpdateMatrix();
}

void Enemy::Update() {
	///
	///	自動で下降させる
	/// 
	worldTransform_.translation_.y -= fallSpeed_;

	///	
	///	スケールと半径を同期させる（レーザーが当たると大きくなるにあたって、見た目と当たり判定を合わせる）
	/// 

	worldTransform_.scale_.x = radius_;
	worldTransform_.scale_.y = radius_;
	worldTransform_.scale_.z = radius_;

	///
	///	Y座標が一定の場所まで降りたら停止
	/// 

	const float kStopYposition = -18.0f;
	worldTransform_.translation_.y = (std::max)(worldTransform_.translation_.y, kStopYposition);
	// 一番したまで到達したことを示す
	if (worldTransform_.translation_.y <= kStopYposition) {
		hasReachedBottom_ = true;
	}

	///
	///	行列の更新
	/// 
	worldTransform_.UpdateMatrix();

	///
	///	デバッグ表示
	/// 
	
	Debug();
}

void Enemy::Draw(ViewProjection& viewProjection) {
	// モデルの描画
	modelEnemy_->Draw(worldTransform_, viewProjection);
}

void Enemy::OnCollision(float incrementSize) {
	///
	///	当たっているフレーム、引数で指定された値、自身のサイズ(半径)を増加させる
	/// 
	radius_ += incrementSize;

	///
	///	自身のサイズ(半径)が一定以上になったら死亡させる	
	/// 

	const float kMaxSize = 3.0f;
	if (radius_ >= kMaxSize) {
		isDead_ = true;
	}

	// あとでサイズとかを変える処理に変更するけどデバッグ用で一旦死亡させる
	/*isDead_ = true;*/
}

void Enemy::Debug(){ 
	ImGui::Begin("Enemy");

	ImGui::Text("radius : %.2f", radius_);
	ImGui::Text("reachedBottom : %d", hasReachedBottom_);

	ImGui::End();
}

Vector3 Enemy::GetWorldPosition() { 
	Vector3 worldPos;

	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}
