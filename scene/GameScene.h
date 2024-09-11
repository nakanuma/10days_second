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
#include <memory>

// MyClass
#include "Player.h"
#include "Particle.h"
#include "Wave.h"
#include "Enemy.h"
#include "EnemyAppearMark.h"
#include "Fade.h"

class Wave;

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {

public: // メンバ関数

	// ゲームのフェーズ
	enum class Phase {
		kFadeIn,  // フェードイン
		kMain,    // メイン部
		kFadeOut, // フェードアウト
	};

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
	/// プレイヤーが生きているかの確認
	/// </summary>
	void CheckPlayerAlive();

	/// <summary>
	/// 全ての衝突判定
	/// </summary>
	void CheckAllCollision();

	/// <summary>
	/// 敵の自動生成 && 敵出現マークの生成
	/// </summary>
	void EnemyGeneration();

	/// <summary>
	/// パーティクルの自動生成
	/// </summary>
	void ParticleGeneration();

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

	/// <summary>
	/// 終了フラグの取得
	/// </summary>
	bool GetIsFinished();

private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;

	/// <summary>
	/// ゲームシーン用
	/// </summary>
	
	// ビュープロジェクション
	ViewProjection viewProjection_;

	// ウェーブ管理
	std::unique_ptr<Wave> wave_ = nullptr;

	/*==================================================================================*/
	// フェード関連

	// フェード
	std::unique_ptr<Fade> fade_ = nullptr;
	// 現在のフェーズ
	Phase phase_ = Phase::kFadeIn;

	// フェードタイマー
	const float fadeTimer_ = 1.0f;

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
	// 「リザルト」の文字が書いてあるスプライト
	Sprite* spriteResultText_ = nullptr;


	// パーティクルのリスト
	std::list<Particle*> particles_;

	///
	///	その他
	/// 
	
	// ゲームシーン経過時間をカウント
	int32_t gameTime_;
	// 敵の生成頻度(フレーム)
	uint32_t nextGenerationFrame_;

	// 終了フラグ
	bool isFinished_ = false;
};