#include "TutorialScene.h"

TutorialScene::~TutorialScene() {}

void TutorialScene::Initialize() {

	// ビュープロジェクション初期化
	viewProjection_.Initialize();

	// チュートリアル時間の初期化
	timer_ = 300;
	// フェーズの初期化
	phase_ = 0;

	///
	///	プレイヤー関連
	///

	// モデル
	modelPlayer_.reset(Model::CreateFromOBJ("player", true));
	// レーザーのモデル
	modelPlayerLaser_.reset(Model::CreateFromOBJ("playerLaser", true));

	// プレイヤーの生成
	player_ = std::make_unique<Player>();
	player_->Initialize(modelPlayer_.get(), modelPlayerLaser_.get());
}

void TutorialScene::Update() {

	player_->Update();

	if (phase_ == 0) {
		MoveTutorial();
	}

	if (phase_ == 1) {
		// EliminateEnemy();
		isFinished_ = true;
	}

	Operation();

	Debug();
}

void TutorialScene::Draw(ID3D12GraphicsCommandList* commandList) {

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	// プレイヤー描画
	player_->Draw(viewProjection_);

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion
}

void TutorialScene::MoveTutorial() {

	if (timer_ <= 0) {

		phase_ = 1;
		timer_ = 300;
	}

	timer_--;
}

void TutorialScene::EliminateEnemy() {

	if (timer_ <= 0) {

		phase_ = 2;
		timer_ = 300;
	}

	timer_--;
}

void TutorialScene::Operation() {

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
			isFinished_ = true;
		}
	}
}

void TutorialScene::Debug() {
#ifdef DEBUG

	ImGui::Begin("Tutorial");

	ImGui::Text("Timer : %d", timer_);
	ImGui::Text("Phase : %d", phase_);

	ImGui::End();

#endif
}

bool TutorialScene::GetIsFinished() { return isFinished_; }