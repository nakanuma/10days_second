#pragma once
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "Model.h"
#include "Input.h"
#include <list>

// MyClass
#include "PlayerBullet.h"

class Player {
public:
	///
	///	基本的な関数
	///

	// デストラクタ
	~Player();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="modelPlayer">プレイヤー本体のモデル</param>
	/// <param name="modelBullet">プレイヤーの弾のモデル</param>
	void Initialize(Model* modelPlayer, Model* modelBullet);

	// 更新
	void Update();
	// 左スティックで移動（移動している方向へプレイヤーを向ける）
	void Move();
	// Aボタンで弾を発射する処理（プレイヤーの向いている方向へ発射）
	void Attack();

	// 描画
	void Draw(ViewProjection& viewProjection);

	// デバッグ表示
	void Debug();

	///
	///	アクセッサ
	/// 
	
	// ワールド座標取得
	Vector3 GetWorldPosition();
	// 半径を取得
	float GetRadius() { return kRadius_; }

private:
	///
	///	汎用機能
	/// 
	
	Input* input_ = nullptr;

	///
	///	プレイヤーの基本的な情報
	/// 
	
	// ワールドトランスフォーム
	WorldTransform worldTransform_;
	// モデル
	Model* modelPlayer_ = nullptr;
	// 移動速度
	float characterSpeed_;
	// 半径
	const float kRadius_ = 1.0f;

	///
	///	プレイヤーの発射する弾の情報
	/// 
	
	// モデル
	Model* modelBullet_ = nullptr;
	// 弾のリスト
	std::list<PlayerBullet*> bullets_;
	// 弾の速度
	float bulletSpeed_;
	// 発射間隔のクールダウン
	int32_t fireCooldown_ = 0;
	// 発射する間隔（フレーム）
	int32_t fireRate_;
};
