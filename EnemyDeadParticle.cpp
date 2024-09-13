#include "EnemyDeadParticle.h"
#include <cassert>
#include "Easing.h"

EnemyDeadParticle::EnemyDeadParticle(Model* model, Vector3 translation, Vector3 velocity) {
	// 引数で受け取った値を設定
	assert(model);
	model_ = model;
	velocity_ = velocity;

	// ワールドトランスフォーム初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = translation;
	worldTransform_.scale_ = {0.5f, 0.5f, 0.5f};

	// 生存時間を設定（totalTimeと同期させる）
	timer_ = 25;
}

EnemyDeadParticle::~EnemyDeadParticle() {}

void EnemyDeadParticle::Update() {
	// タイマーのカウントダウン
	timer_--;

	// 移動
	worldTransform_.translation_ += velocity_;
	// タイマーが指定時間以下になったら、加速度を掛けて下げる
	if (timer_ <= 10) {
		velocity_.y -= acceleratorY;
	}

	// タイマーの進行度
	//float t = 1.0f - static_cast<float>(timer_) / static_cast<float>(totalTime);
	//// イージングを適用してスケールを調整
	//float easedScale = Easing::EaseOutQuart(t);
	//worldTransform_.scale_ = {0.5f * easedScale, 0.5f * easedScale, 0.5f * easedScale};

	//// サイズが0になったら削除
	//if (worldTransform_.scale_.x <= 0.0f &&
	//	worldTransform_.scale_.y <= 0.0f && 
	//	worldTransform_.scale_.z <= 0.0f) {
	//	del_ = true;
	//}

	// Yが一定以下になったら削除
	if (worldTransform_.translation_.y <= -22.0f) {
		del_ = true;
	}

	// 行列の更新
	worldTransform_.UpdateMatrix();
}

void EnemyDeadParticle::Draw(ViewProjection& viewProjection) { 
	model_->Draw(worldTransform_, viewProjection);
}
