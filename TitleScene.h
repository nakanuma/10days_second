#pragma once
#include "DirectXCommon.h"
#include "Input.h"
#include "Model.h"
#include "Sprite.h"
#include "TextureManager.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "Audio.h"

#include "Fade.h"
#include "Particle.h"

#ifdef DEBUG
#include <ImGui.h>
#endif // DEBUG

#include <list>
#include <memory>

/// <summary>
/// タイトルシーン
/// </summary>
class TitleScene {

public:
	// ゲームのフェーズ
	enum class Phase {
		kFadeIn,  // フェードイン
		kMain,    // メイン部
		kFadeOut, // フェードアウト
	};

	// デストラクタ
	~TitleScene();

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

	// ロゴの動き
	void MoveLogo();

	// アクセッサ
	bool GetIsFinished();

private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Audio* audio_ = nullptr;

	// 終了フラグ
	bool isFinished_ = false;

	// フェード
	std::unique_ptr<Fade> fade_ = nullptr;
	// 現在のフェーズ
	Phase phase_ = Phase::kFadeIn;

	// フェードタイマー
	const float fadeTimer_ = 1.0f;

	// パーティクル
	std::unique_ptr<Particle> particle_ = nullptr;
	// パーティクルのリスト
	std::list<Particle*> particles_;

	///
	/// テクスチャハンドル
	///

	// 背景
	uint32_t bgTex_ = 0;
	// タイトルロゴ
	uint32_t logoTex_ = 0;
	// スタートボタン
	uint32_t startButtonTex_ = 0;
	// サイドの黒い部分
	uint32_t sideTex_ = 0;

	///
	/// スプライト
	///

	// 背景
	std::unique_ptr<Sprite> bgSprite_ = nullptr;
	// タイトルロゴ
	std::unique_ptr<Sprite> logoSprite_ = nullptr;
	// スタートボタン
	std::unique_ptr<Sprite> startButtonSprite_ = nullptr;
	// サイド
	std::unique_ptr<Sprite> sideSprite_[2] = {nullptr};

	///
	/// スプライト座標
	///

	// タイトルロゴ
	Vector2 logoPos_ = {};

	///
	/// サウンドハンドル
	///

	uint32_t clickSH_ = 0;
};