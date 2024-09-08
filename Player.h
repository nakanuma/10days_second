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
	// 左スティックで左右移動・右スティックで角度変更・レーザー射撃状態によって自動で上昇下降・プレイヤーが範囲外へ行かないようにする処理
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
	// レーザーの取得
	Laser& GetLaser() { return laser_; }

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
	// 体力
	uint32_t hp_;

	///
	///	レーザー関連の情報
	/// 
	
	// レーザー
	Laser laser_;
	// モデル
	Model* modelLaser_ = nullptr;

	///
	///	その他パラメーターなど
	/// 
	
	// レーザー射撃中/非射撃中の自動上昇・下降の速度
	float autoAscendingSpeed_;
	float autoDescendingSpeed_;
};
