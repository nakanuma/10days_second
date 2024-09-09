#pragma once
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "Model.h"

// MyClass
#include "Laser.h"

class Enemy {
public:
	~Enemy();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="modelEnemy">敵のモデル</param>
	/// <param name="modelLaser">レーザーのモデル</param>
	/// <param name="position">初期スポーン位置</param>
	/// /// <param name="position">半径</param>
	void Initialize(Model* modelEnemy, Model* modelLaser, Vector3 position, float radius);

	// 更新
	void Update();
	// 攻撃（一番したまで到達したら上にレーザー発射）
	void Attack();

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
	// レーザーの取得
	Laser& GetLaser() { return laser_; }

private:
	///
	///	基本的な情報
	/// 

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

	///
	///	レーザー
	/// 
	
	// レーザー本体
	Laser laser_;
	// モデル
	Model* modelLaser_ = nullptr;

};
