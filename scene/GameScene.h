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
#include "EnemyAppearMark.h"

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
	/// 敵の自動生成 && 敵出現マークの生成
	/// </summary>
	void EnemyGeneration();

	/// <summary>
	/// ゲームシーンでの経過時間による流れを全て記述
	/// </summary>
	void GameSceneFlow();

	/// <summary>
	/// WAVE数スプライトをいい感じに右から左へ移動させる（5秒間(300f)で行うので、制限必須）
	/// </summary>
	void WaveSpriteMove();

	/// <summary>
	/// リザルトの表示（そのWAVEの取得スコア、スコアによる獲得星を表示）
	/// 備考 : 細い状態から広がる->広げた状態から細くする
	/// </summary>
	void ShowResult();

	/// <summary>
	/// 秒をフレームに変換
	/// </summary>
	int32_t SecToFrame(int32_t Second) { return Second * 60; }

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
	// 敵出現マークのリスト
	std::list<EnemyAppearMark*> enemyAppearMarks_;
	
	// 敵のモデル
	Model* modelEnemy_ = nullptr;
	// 敵出現マークのモデル
	Model* modelEnemyAppearMark_ = nullptr;

	///
	///	レーザー関連
	/// 

	// モデル（敵のレーザー）
	Model* modelLaser_ = nullptr;
	// モデル（プレイヤーのレーザー）
	Model* modelPlayerLaser_ = nullptr;

	///
	///	スプライト
	/// 

	// 各ウェーブ開始時に出てくるウェーブの文字（1,2,3のテクスチャハンドルをセットして使い回す）
	Sprite* spriteWaveNum_ = nullptr;
	uint32_t wave1TextureHandle_;
	uint32_t wave2TextureHandle_;
	uint32_t wave3TextureHandle_;

	// ゲームシーン背景
	Sprite* spriteBackGround_ = nullptr;

	/*ゲーム領域ではない画面両側を隠すスプライト*/
	Sprite* spriteScreenLeft_ = nullptr;
	Sprite* spriteScreenRight_ = nullptr;

	/* リザルトで使用するもの */
	// リザルトの最も後ろにあるスプライト（拡縮させるやつ）
	Sprite* spriteResultBackGround_ = nullptr;

	///
	///	その他
	/// 
	
	// ゲームシーン経過時間をカウント
	int32_t gameTime_;
	// 敵の生成頻度(フレーム)
	uint32_t nextGenerationFrame_;
};
