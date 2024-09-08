#pragma once
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "Model.h"

// MyClass


class Enemy {
public:
	~Enemy();

	// 初期化
	void Initialize(Model* modelEnemy, Vector3 position);
	// 更新
	void Update();
	// 描画
	void Draw(ViewProjection& viewProjection);

	/// <summary>
	/// 衝突時コールバック（サイズを大きくする : プレイヤーのレーザーと敵のレーザーで増加量を変更）
	/// </summary>
	/// <param name="incrementSize">毎フレームの敵のサイズ増加量</param>
	void OnCollision(float incrementSize);

	/// <summary>
	/// デバッグ
	/// </summary>
	void Debug();

	///
	///	アクセッサ
	/// 

	// 半径の取得
	float GetRadius() { return radius_; }
	// ワールド座標を取得
	Vector3 GetWorldPosition();
	// 死亡したかを取得
	bool IsDead() { return isDead_; }
	// 一番下まで到達したかを取得
	bool HasReachedBottom() { return hasReachedBottom_; }

private:
	// ワールドトランスフォーム
	WorldTransform worldTransform_;
	// モデル
	Model* modelEnemy_ = nullptr;
	// 半径
	float radius_;
	// 死亡したか
	bool isDead_ = false;
	// 自動で下降する速度
	float fallSpeed_;
	// 一番下まで到達したか
	bool hasReachedBottom_ = false;
};
