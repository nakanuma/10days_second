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

	/// <summary>
	/// リザルト表示時に、取得スコアを描画
	/// </summary>
	void DrawScoreToResult();

	/// <summary>
	/// 各WAVEでパラメーターの初期化を行う（プレイヤーと敵の攻撃力・敵のスポーン頻度・敵の落下速度）
	/// </summary>
	void InitializeParameterWAVE1();
	void InitializeParameterWAVE2();
	void InitializeParameterWAVE3();

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

	/*--------------------*/
	/* リザルトで使用するもの */
	/*--------------------*/

	/*背景*/

	// リザルトの最も後ろにあるスプライト（拡縮させるやつ）
	Sprite* spriteResultBackGround_ = nullptr;
	// 「リザルト」の文字が書いてあるスプライト
	Sprite* spriteResultText_ = nullptr;

	/*スコア表示*/

	// スコアの桁数
	const int32_t kMaxScoreDigit = 6;
	// スコア表示に使用するスプライト（6桁分）
	Sprite* spriteScore_[6];
	// 0~9までの数字が書いてある数字テクスチャ
	uint32_t textureNumber_;

	/*星表示*/
	const int32_t kMaxStarNum = 3;
	// 空っぽの星に使用するスプライト（3つ）
	Sprite* spriteEmptyStar_[3];

	/*残り時間表示*/

	// そのWAVEが何フレームあるかを記録（毎WAVEリセット時にセットする）
	int32_t remainingTime_ = 0; // テスト用で一旦30秒にしておく
	// 残り時間表示に使用するスプライト（2桁分）
	Sprite* spriteRemainingTime_[2];
	// "残り・・・秒"と書いてあるスプライト
	Sprite* spriteRemainingTimeText_ = nullptr;

	///
	///	その他
	/// 
	
	// ゲームシーン経過時間をカウント
	int32_t gameTime_;
	// 敵の生成頻度(フレーム)
	uint32_t nextGenerationFrame_;

	///
	///	調整可能項目
	///

	float playerAttackDamage_ = 0.01f; // デフォルト値
	float enemyAttackDamage_ = 0.008f; // デフォルト値
	int32_t enemySpawnRate_ = 180;     // デフォルト値(frame)
	float enemyFallSpeed_ = 0.05f;     // デフォルト値
};
