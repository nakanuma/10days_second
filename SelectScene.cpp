#include "SelectScene.h"

#include <random>
#include <wrl.h>

// シーン
enum class Stage {

	kFirst,
	kSeconsd,
	kThird,
};

Stage stage_ = Stage::kFirst;

SelectScene::~SelectScene() {

	for (Particle* particle : particles_) {
		delete particle;
	}
}

void SelectScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();

	// ビュープロジェクションの初期化
	viewProjection_.Initialize();

	// ゲームシーン
	gameScene_ = std::make_unique<GameScene>();

	// ウェーブ管理
	wave_ = std::make_unique<Wave>();

	// 初期ステージ
	stage_ = Stage::kFirst;

	///
	/// テクスチャハンドル
	///

	bgTex_ = TextureManager::Load("./Resources/images/title/back_white.png");
	startButtonTex_ = TextureManager::Load("./Resources/images/title/title_startbutton.png");
	sideTex_ = TextureManager::Load("./Resources/images/title/back_black.png");

	rightArrowTex_ = TextureManager::Load("./Resources/images/select/select_right_arrow.png");
	leftArrowTex_ = TextureManager::Load("./Resources/images/select/select_left_arrow.png");
	waveSelectTex_ = TextureManager::Load("./Resources/images/select/select_waveselect.png");
	titleSelectTex_ = TextureManager::Load("./Resources/images/select/select_titleback.png");

	///
	/// 2Dスプライト
	///

	bgSprite_.reset(Sprite::Create(bgTex_, {0, 0}));
	startButtonSprite_.reset(Sprite::Create(startButtonTex_, {480, 540}));
	sideSprite_[0].reset(Sprite::Create(sideTex_, {0, 0}));
	sideSprite_[1].reset(Sprite::Create(sideTex_, {930, 0}));

	rightArrowSprite_.reset(Sprite::Create(rightArrowTex_, {745, 335}));
	leftArrowSprite_.reset(Sprite::Create(leftArrowTex_, {490, 335}));
	waveSelectSprite_.reset(Sprite::Create(waveSelectTex_, {450, 100}));
	titleSelectSprite_.reset(Sprite::Create(titleSelectTex_, {960, 640}));

	///
	/// ワールドトランスフォーム
	///

	for (int i = 0; i < 3; i++) {
		boxWorldTransform_[i].Initialize();
		boxWorldTransform_[i].translation_ = {0.0f + 15.0f * i, 0.0f, 0.0f};
	}

	///
	/// 3Dモデル
	///

	boxModel_[0].reset(Model::CreateFromOBJ("select_box_1", true));
	boxModel_[1].reset(Model::CreateFromOBJ("select_box_2", true));
	boxModel_[2].reset(Model::CreateFromOBJ("select_box_3", true));

	// フェード
	fade_ = std::make_unique<Fade>();

	fade_->Initialize();

	fade_->Start(Fade::Status::FadeIn, fadeTimer_);
}

void SelectScene::Update() {

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

	for (Particle* particle : particles_) {
		particle->Update();
	}

	// パーティクルの自動生成
	ParticleGeneration();

	for (int i = 0; i < 3; i++) {
		boxWorldTransform_[i].UpdateMatrix();
	}

	// デバッグ表示
	// Debug();
}

void SelectScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	///
	/// 背景2Dスプライト描画
	///

#pragma region 背景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	// 背景描画
	bgSprite_->Draw();
	// パーティクル描画
	for (Particle* particle : particles_) {
		particle->Draw();
	}

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

	///
	/// 3Dモデル描画
	///

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	for (int i = 0; i < 3; i++) {
		boxModel_[i]->Draw(boxWorldTransform_[i], viewProjection_);
	}

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

	///
	/// 前景2Dスプライト描画
	///

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	// ロゴ描画
	// logoSprite_->Draw();
	// スタートボタン描画
	startButtonSprite_->Draw();
	// サイド
	for (int i = 0; i < 2; i++) {
		sideSprite_[i]->Draw();
	}

	if (stage_ != Stage::kThird) {
		rightArrowSprite_->Draw();
	}
	if (stage_ != Stage::kFirst) {
		leftArrowSprite_->Draw();
	}
	waveSelectSprite_->Draw();
	titleSelectSprite_->Draw();

	fade_->Draw();

	// スプライト描画後処理
	Sprite::PostDraw();
#pragma endregion
}

void SelectScene::Debug() {}

void SelectScene::Operation() {

	XINPUT_STATE joyState;

	XINPUT_STATE joyPreState;

	// ゲームパッド未接続なら何もせず抜ける
	if (!Input::GetInstance()->GetJoystickState(0, joyState)) {
		return;
	}

	if (!Input::GetInstance()->GetJoystickStatePrevious(0, joyPreState)) {
		return;
	}

	switch (stage_) {
	case Stage::kFirst:
		// 右ボタンを押したら移動
		if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) {
			if (!(joyPreState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)) {
				stage_ = Stage::kSeconsd;
				for (int i = 0; i < 3; i++) {
					boxWorldTransform_[i].translation_.x -= 15.0f;
				}
			}
		}

		// Aボタンを押したら選択
		if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_A) {
			if (!(joyPreState.Gamepad.wButtons & XINPUT_GAMEPAD_A)) {
				// フェード切り替え
				phase_ = Phase::kFadeOut;
				fade_->Start(Fade::Status::FadeOut, fadeTimer_);

				Wave::GetInstance().SetWave(1);
			}
		}

		boxWorldTransform_[1].translation_.y = 0.0f;
		boxWorldTransform_[2].translation_.y = 0.0f;

		MoveBox(&boxWorldTransform_[0]);
		break;
	case Stage::kSeconsd:
		// 左ボタンを押したら移動
		if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) {
			if (!(joyPreState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)) {
				stage_ = Stage::kFirst;
				for (int i = 0; i < 3; i++) {
					boxWorldTransform_[i].translation_.x += 15.0f;
				}
			}
		}
		// 右ボタンを押したら移動
		if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) {
			if (!(joyPreState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)) {
				stage_ = Stage::kThird;
				for (int i = 0; i < 3; i++) {
					boxWorldTransform_[i].translation_.x -= 15.0f;
				}
			}
		}

		// Aボタンを押したら選択
		if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_A) {
			if (!(joyPreState.Gamepad.wButtons & XINPUT_GAMEPAD_A)) {
				// フェード切り替え
				phase_ = Phase::kFadeOut;
				fade_->Start(Fade::Status::FadeOut, fadeTimer_);

				Wave::GetInstance().SetWave(2);
			}
		}

		boxWorldTransform_[0].translation_.y = 0.0f;
		boxWorldTransform_[2].translation_.y = 0.0f;

		MoveBox(&boxWorldTransform_[1]);
		break;
	case Stage::kThird:
		// 左ボタンを押したら移動
		if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) {
			if (!(joyPreState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)) {
				stage_ = Stage::kSeconsd;
				for (int i = 0; i < 3; i++) {
					boxWorldTransform_[i].translation_.x += 15.0f;
				}
			}
		}

		// Aボタンを押したら選択
		if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_A) {
			if (!(joyPreState.Gamepad.wButtons & XINPUT_GAMEPAD_A)) {
				// フェード切り替え
				phase_ = Phase::kFadeOut;
				fade_->Start(Fade::Status::FadeOut, fadeTimer_);

				Wave::GetInstance().SetWave(3);
			}
		}

		boxWorldTransform_[0].translation_.y = 0.0f;
		boxWorldTransform_[1].translation_.y = 0.0f;

		MoveBox(&boxWorldTransform_[2]);
		break;
	default:
		break;
	}

	// Xボタンを押したらタイトルへ
	if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_X) {
		if (!(joyPreState.Gamepad.wButtons & XINPUT_GAMEPAD_X)) {
			// タイトルセレクトをtrueに
			isTitleSelect_ = true;
		}
	}
}

void SelectScene::ParticleGeneration() {

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

void SelectScene::MoveBox(WorldTransform* worldTransform) {

	// フレームカウンタを増加
	static int frameCounter = 0;
	frameCounter++;

	// サイン波でボックスを上下に動かす
	float floatAmplitude = 1.0f; // 上下の移動幅
	float frequency = 0.05f;     // 動きの速さ（数値を変えると速度が変わる）

	// boxWorldTransform_のy座標をサイン波で変化させる
	worldTransform->translation_.y = sin(frameCounter * frequency) * floatAmplitude;
	worldTransform->UpdateMatrix(); // 変更したら行列を更新
}

bool SelectScene::GetIsFinished() { return isFinished_; }

bool SelectScene::GetIsSelectTitle() { return isTitleSelect_; }
