#pragma once
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "Model.h"
#include "Input.h"
#include <list>

// MyClass
#include "Laser.h"

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
	void Initialize(Model* modelPlayer, Model* modelLaser);

	// 更新
	void Update();
	// 左スティックで移動
	// 右スティックで角度変更
	void Move();
	// RBでレーザーを発射
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
	///	レーザー
	/// 
	
	// レーザー
	Laser laser_;
	// モデル
	Model* modelLaser_ = nullptr;
};
