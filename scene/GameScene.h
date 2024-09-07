#pragma once

#include "Audio.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "Model.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "PrimitiveDrawer.h"

#include <vector>

// MyClass
#include "Player.h"

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {

public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// デバッグ用
	/// </summary>
	void Debug();

private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;

	/// <summary>
	/// ゲームシーン用
	/// </summary>
	
	// ビュープロジェクション
	ViewProjection viewProjection_;

	///
	///	プレイヤー関連
	/// 

	// プレイヤー本体
	Player* player_ = nullptr;

	// プレイヤーモデルデータ
	Model* modelPlayer_ = nullptr;

	///
	///	レーザー
	/// 

	Model* modelLaser_ = nullptr;

	///
	///	その他
	/// 
	
	// 各方向を定義（敵の移動に使用）
	const Vector3 kRightDirection = {1.0f, 0.0f, 0.0f}; // 右移動（上のラインにいる場合）
	const Vector3 kDownDirection = {0.0f, -1.0f, 0.0f}; // 下移動（右のラインにいる場合）
	const Vector3 kLeftDirection = {-1.0f, 0.0f, 0.0f}; // 左移動（下のラインにいる場合）
	const Vector3 kUpDirection = {0.0f, 1.0f, 0.0f}; // 上移動（左のラインにいる場合）

	// 外側の枠の各頂点の座標を定義（敵のスポーン位置や、敵の移動方向を変えるトリガーに使用）
	const Vector3 kOutsideFrameTopLeftPos = {-16.0f, 16.0f, 0.0f}; // 外枠の左上座標
	const Vector3 kOutsideFrameTopRightPos = {16.0f, 16.0f, 0.0f}; // 外枠の右上座標
	const Vector3 kOutsideFrameBottomRightPos = {16.0f, -16.0f, 0.0f}; // 外枠の右下座標
	const Vector3 kOutsideFrameBottomLeftPos = {-16.0f, -16.0f, 0.0f}; // 外枠の左下座標
};
