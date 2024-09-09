#include "Enemy.h"

#include "imgui.h"

#include <cassert>
#include <algorithm>

Enemy::~Enemy() {}

void Enemy::Initialize(Model* modelEnemy, Model* modelLaser, Vector3 position, float radius) { 
	// メンバ変数にモデルを設定
	assert(modelEnemy);
	modelEnemy_ = modelEnemy;

	assert(modelLaser);
	modelLaser_ = modelLaser;

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_ = {0.0f, 0.0f, 1.57f}; // 最初は右を向いていることになっているので、上向きに設定

	// 半径の初期値を設定
	radius_ = radius;
	// 自動で下降する速度を設定（低いほどゆっくり降りてくる）
	fallSpeed_ = 0.05f;

	// ワールドトランスフォームを更新しておく
	worldTransform_.UpdateMatrix();
}

void Enemy::Update() {
	///
	///	自動で下降させる処理
	/// 

	// 現在の半径に基づいて降下速度を調整（大きければ大きいほど早く降下させるため）
	float adjustedFallspeed = fallSpeed_ + (radius_ * 0.03f); // ここの値は要調整

	// 自動で降下する速度の分だけ降下させる
	worldTransform_.translation_.y -= adjustedFallspeed;

	///	
	///	スケールと半径を同期させる（レーザーが当たると大きくなるにあたって、見た目と当たり判定を合わせる）
	/// 

	// 地面に到達していない間のみ（一番下でレーザーを撃っている際にScaleを縮小させるため、不都合が起きないように）
	if (!hasReachedBottom_) {
		worldTransform_.scale_.x = radius_;
		worldTransform_.scale_.y = radius_;
		worldTransform_.scale_.z = radius_;
	}

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
	///	一番下まで到達したらレーザーを発射
	/// 

	Attack();

	///
	///	レーザーが有効な間はscale.xを縮小させる
	/// 

	// 毎フレーム縮小させるサイズ
	const float kDecrementSize = 0.01f;

	if (laser_.IsActive()) {
		worldTransform_.scale_.x -= kDecrementSize;
	}
	// スケールの縮小によって消えたら死亡させる
	if (worldTransform_.scale_.x <= 0.0f) {
		isDead_ = true;
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

void Enemy::Attack() {
	// 一番下に到達した際にレーザーを発射する
	if (hasReachedBottom_) {
		// レーザーの初期化
		laser_.Initialize(
			modelLaser_, 
			worldTransform_.translation_, 
			worldTransform_.rotation_, 
			Vector3{Laser::kLength, worldTransform_.scale_.y, 0.01f}
		);
	}

	// 有効な場合、レーザーを更新
	if (laser_.IsActive()) {
		laser_.Update(worldTransform_.translation_, worldTransform_.rotation_);
	}

}

void Enemy::Draw(ViewProjection& viewProjection) {
	///
	///	敵本体の描画
	///
	
	modelEnemy_->Draw(worldTransform_, viewProjection);
}

void Enemy::DrawLaser(ViewProjection& viewProjection) {
	///
	///	レーザーの描画
	///

	// 有効な場合のみ
	if (laser_.IsActive()) {
		laser_.Draw(viewProjection);
	}
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
	ImGui::Text("LaserIsActive : %d", laser_.IsActive());

	ImGui::End();
}

Vector3 Enemy::GetWorldPosition() { 
	Vector3 worldPos;

	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}
