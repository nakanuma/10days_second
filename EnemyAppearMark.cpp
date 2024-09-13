#include "EnemyAppearMark.h"
#include <cassert>

EnemyAppearMark::~EnemyAppearMark() { }

void EnemyAppearMark::Initialize(Model* model, Vector3 position) { 
	// 引数で受け取ったモデルをメンバ変数に設定
	assert(model);
	model_ = model;

	// ワールドトランスフォーム初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = position; // 引数の座標を設定
	worldTransform_.scale_ = {1.5f, 1.5f, 1.5f}; // ちょっと大きくする

	// 生存時間をセット
	lifeTime_ = kMaxDisplayTime;


	// ワールドトランスフォームを更新しておく
	worldTransform_.UpdateMatrix();
}

void EnemyAppearMark::Update() { 
	// 生存時間が0になったら死亡
	if (lifeTime_-- <= 0) {
		isDead_ = true;
	}

	///
	///	Alpha値の更新
	/// 

	// Alpha値の変化スピード
	const float kAlphaSpeed = 0.1f; // 大きいほど早く点滅する

	// 薄い状態 -> 濃くする（Alpha値増加状態）
	if (alphaIncreasing_) {
		alpha_ += kAlphaSpeed;
		if (alpha_ >= 1.0f) {
			alpha_ = 1.0f;
			alphaIncreasing_ = false; // Alpha減少状態に切り替える
		}
	// 濃い状態 -> 薄くする（Alpha値減少状態）
	} else {
		alpha_ -= kAlphaSpeed;
		if (alpha_ <= 0.0f) {
			alpha_ = 0.0f;
			alphaIncreasing_ = true; // Alpha値増加状態に切り替える
		}
	}

	// ワールドトランスフォームの更新
	worldTransform_.UpdateMatrix();
}

void EnemyAppearMark::Draw(ViewProjection& viewProjection) { 
	// Alpha値の更新
	model_->SetAlpha(alpha_);
	// モデルの描画
	model_->Draw(worldTransform_, viewProjection);
}
