#include "TitleScene.h"

#include <random>
#include <wrl.h>

TitleScene::~TitleScene() {

	for (Particle* particle : particles_) {
		delete particle;
	}
}

void TitleScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();

	///
	/// テクスチャハンドル
	///

	bgTex_ = TextureManager::Load("./Resources/images/title/back_white.png");
	logoTex_ = TextureManager::Load("./Resources/images/title/title_logo.png");
	startButtonTex_ = TextureManager::Load("./Resources/images/title/title_startbutton.png");
	sideTex_ = TextureManager::Load("./Resources/images/bg/back_black.png");

	///
	/// スプライト
	///

	bgSprite_.reset(Sprite::Create(bgTex_, {0, 0}));
	logoSprite_.reset(Sprite::Create(logoTex_, {430, 200}));
	startButtonSprite_.reset(Sprite::Create(startButtonTex_, {480, 540}));
	sideSprite_[0].reset(Sprite::Create(sideTex_, {0, 0}));
	sideSprite_[1].reset(Sprite::Create(sideTex_, {930, 0}));


	// フェード
	fade_ = std::make_unique<Fade>();

	fade_->Initialize();

	fade_->Start(Fade::Status::FadeIn, fadeTimer_);
}

void TitleScene::Update() {

	switch (phase_) {
	case Phase::kFadeIn:
		if (fade_->IsFinished()) {
			phase_ = Phase::kMain;
		}
		break;
	case Phase::kMain:
		// 操作
		Operation();
		break;
	case Phase::kFadeOut:
		if (fade_->IsFinished()) {
			isFinished_ = true;
		}
		break;
	}

	// フェードの更新
	fade_->Update();

	// パーティクル
	for (Particle* particle : particles_) {
		particle->Update();
	}

	// パーティクルの自動生成
	ParticleGeneration();

	MoveLogo();

	// デバッグ表示
	Debug();
}

void TitleScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	// 背景描画
	bgSprite_->Draw();
	// パーティクル描画
	for (Particle* particle : particles_) {
		particle->Draw();
	}
	// ロゴ描画
	logoSprite_->Draw();
	// スタートボタン描画
	startButtonSprite_->Draw();
	// サイド
	for (int i = 0; i < 2; i++) {
		sideSprite_[i]->Draw();
	}

	fade_->Draw();

	// スプライト描画後処理
	Sprite::PostDraw();
}

void TitleScene::Debug() {}

void TitleScene::Operation() {

	XINPUT_STATE joyState;

	XINPUT_STATE joyPreState;

	// ゲームパッド未接続なら何もせず抜ける
	if (!Input::GetInstance()->GetJoystickState(0, joyState)) {
		return;
	}

	if (!Input::GetInstance()->GetJoystickStatePrevious(0, joyPreState)) {
		return;
	}

	// Aボタンを押したら終了
	if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_A) {
		if (!(joyPreState.Gamepad.wButtons & XINPUT_GAMEPAD_A)) {
			// 終了フラグをtrue
			// isFinished_ = true;
			// フェード切り替え
			phase_ = Phase::kFadeOut;
			fade_->Start(Fade::Status::FadeOut, fadeTimer_);
		}
	}
}

void TitleScene::ParticleGeneration() {

	// フレームカウンタを保持する静的変数
	static int frameCounter = 0;

	// フレームカウンタを増加
	frameCounter++;

	// 5フレームごとにパーティクルを生成
	if (frameCounter % 20 == 0) {
		///
		/// パーティクル生成のX座標ランダム化
		///
		std::random_device rd;
		std::mt19937 rng(rd());

		// X座標。指定範囲の間をランダムで生成
		const float kRightGenerateX = 350;
		const float kLeftGenerateX = 930;
		std::uniform_real_distribution<float> distX(kRightGenerateX, kLeftGenerateX);
		// 指定範囲のランダムなX座標を生成
		float randomX = distX(rng);

		// Y座標。固定（800で生成）
		const float kGenerateY = 800.0f;

		// パーティクルの座標を設定
		Particle* newParticle = new Particle();
		newParticle->Initialize({randomX, kGenerateY});

		// パーティクルリストに追加
		particles_.push_back(newParticle);
	}
}

void TitleScene::MoveLogo() {

	// ロゴスプライト座標
	logoPos_ = logoSprite_->GetPosition();

	// ロゴがふよふよ動く感じの処理
	// フレームごとのカウンタを用意
	static int frameCounter = 0;

	// カウンタを増加
	frameCounter++;

	// サイン波で上下に動かす
	float floatAmplitude = 3.0f; // 上下の移動幅
	float frequency = 0.1f;      // 動きの速さ（数値を変えると速度が変わる）
	logoPos_.y += sin(frameCounter * frequency) * floatAmplitude;

	logoSprite_->SetPosition(logoPos_);
}

bool TitleScene::GetIsFinished() { return isFinished_; }