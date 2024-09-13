#pragma once
#include "DirectXCommon.h"
#include "Input.h"
#include "Model.h"
#include "Sprite.h"
#include "TextureManager.h"
#include "ViewProjection.h"
#include "WorldTransform.h"

#include "GameScene.h"
#include "Particle.h"
#include "Fade.h"
#include "Wave.h"

#include <ImGui.h>
#include <list>
#include <memory>

class GameScene;
class Wave;

class SelectScene {

public:

	// ゲームのフェーズ
	enum class Phase {
		kFadeIn,  // フェードイン
		kMain,    // メイン部
		kFadeOut, // フェードアウト
	};

	// デストラクタ
	~SelectScene();

	// 初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Draw();

	// デバッグ
	void Debug();

	// 操作
	void Operation();

	// パーティクルの自動生成
	void ParticleGeneration();

	// ボックスの動き
	void MoveBox(WorldTransform* worldTransform);

	// アクセッサ
	bool GetIsFinished();
	bool GetIsSelectTitle();

private: // メンバ変数
	// DirectX
	DirectXCommon* dxCommon_ = nullptr;

	// ビュープロジェクション
	ViewProjection viewProjection_;

	// ゲームシーン
	std::unique_ptr<GameScene> gameScene_ = nullptr;

	// ウェーブ管理
	std::unique_ptr<Wave> wave_ = nullptr;

	/*==================================================================================*/
	// フラグ

	// 終了フラグ
	bool isFinished_ = false;
	// タイトルセレクト
	bool isTitleSelect_ = false;
	// プレタイトルセレクト
	bool isPreTitleSelect_ = false;


	/*==================================================================================*/
	// フェード関連

	// フェード
	std::unique_ptr<Fade> fade_ = nullptr;
	// 現在のフェーズ
	Phase phase_ = Phase::kFadeIn;

	// フェードタイマー
	const float fadeTimer_ = 1.0f;


	/*==================================================================================*/
	// パーティクル

	// パーティクルのリスト
	std::list<Particle*> particles_;


	/*==================================================================================*/
	// テクスチャハンドル

	// 背景
	uint32_t bgTex_ = 0;
	// スタートボタン
	uint32_t startButtonTex_ = 0;
	// サイドの黒い部分
	uint32_t sideTex_ = 0;

	// 左右矢印
	uint32_t rightArrowTex_ = 0;
	uint32_t leftArrowTex_ = 0;
	// ウェーブセレクト
	uint32_t waveSelectTex_ = 0;
	// タイトルセレクト
	uint32_t titleSelectTex_ = 0;


	/*==================================================================================*/
	// スプライト

	// 背景
	std::unique_ptr<Sprite> bgSprite_ = nullptr;
	// スタートボタン
	std::unique_ptr<Sprite> startButtonSprite_ = nullptr;
	// サイド
	std::unique_ptr<Sprite> sideSprite_[2] = {nullptr};

	// 左右矢印
	std::unique_ptr<Sprite> rightArrowSprite_ = nullptr;
	std::unique_ptr<Sprite> leftArrowSprite_ = nullptr;
	// ウェーブセレクト
	std::unique_ptr<Sprite> waveSelectSprite_ = nullptr;
	// タイトルセレクト
	std::unique_ptr<Sprite> titleSelectSprite_ = nullptr;


	/*==================================================================================*/
	// ワールドトランスフォーム

	// ボックス
	WorldTransform boxWorldTransform_[3];


	/*==================================================================================*/
	// モデル

	// ボックス
	std::unique_ptr<Model> boxModel_[3];
};