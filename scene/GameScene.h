#pragma once

#include "Audio.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "Model.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "PrimitiveDrawer.h"
#include "WinApp.h"

#include <list>

// MyClass
#include "Player.h"
#include "Enemy.h"

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

	/// <summary>
	/// 全ての衝突判定
	/// </summary>
	void CheckAllCollision();

	/// <summary>
	/// 敵の自動生成
	/// </summary>
	void EnemyGeneration();

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
	///	敵関連
	/// 

	// 敵のリスト
	std::list<Enemy*> enemies_;
	
	// 敵のモデル
	Model* modelEnemy_ = nullptr;

	///
	///	レーザー関連
	/// 

	// モデル（敵のレーザー）
	Model* modelLaser_ = nullptr;
	// モデル（プレイヤーのレーザー）
	Model* modelPlayerLaser_ = nullptr;

	///
	///	その他
	/// 
	
	// ゲームシーン経過時間をカウント
	uint32_t gameTime_;
	// 敵の生成頻度(フレーム)
	uint32_t nextGenerationFrame_;
};
