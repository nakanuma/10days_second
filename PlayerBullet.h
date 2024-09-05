#pragma once
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "Model.h"

class PlayerBullet {
public:
	///
	///	基本的な関数
	/// 

	// 初期化
	void Initialize(Model* model, const Vector3& position, const Vector3& velocity);
	// 更新
	void Update();
	// 描画
	void Draw(const ViewProjection& viewProjection);
	// 衝突時コールバック
	void OnCollision();

	///
	/// アクセッサ
	/// 
	
	// 死亡したかを取得
	bool IsDead() { return isDead_; }
	// ワールド座標を取得
	Vector3 GetWorldPosition();
	// 半径を取得
	float GetRadius() { return kRadius_; }

private:
	// ワールドトランスフォーム
	WorldTransform worldTransform_;
	// モデル
	Model* model_ = nullptr;

	// 速度
	Vector3 velocity_;
	// 寿命
	static const int32_t kLifeTime = 60 * 2;
	// デスタイマー
	int32_t deathTimer_ = kLifeTime;
	// デスフラグ
	bool isDead_ = false;
	// 半径
	const float kRadius_ = 1.0f;
};
