#pragma once
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "Model.h"
#include "Audio.h"

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
	void Initialize(Model* modelEnemy, Model* modelLaser, Vector3 position, float radius, float fallSpeed);

	// 更新
	void Update();
	// 攻撃の予備動作（一番下まで到達したら細いレーザーを出し、現在の半径になるまでscale.yを増加させる）
	void StartAntic();
	// 攻撃（予備動作が終了したらレーザー発射）
	void Attack();

	// 本体の描画
	void Draw(ViewProjection& viewProjection);
	//　レーザーの描画
	void DrawLaser(ViewProjection& viewProjection);

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
	Audio* audio_ = nullptr;
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

	/*予備動作に使用*/

	// 予備動作用ワールドトランスフォーム
	WorldTransform anticWorldTransform_;
	// 予備動作が終了したか
	bool endAntic_ = false;
	// 予備動作タイマー（予備動作レーザーの拡大が開始する時間を決める）
	uint32_t anticTimer_ = 0;


	// サウンド
	uint32_t laserSE_ = 0;
};
