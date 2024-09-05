#include "Player.h"
#include <cassert>
#include "imgui.h"

Player::~Player() {
	///
	///	各種開放
	///		
	
}

void Player::Initialize(Model* modelPlayer) {
	///
	///	汎用機能初期化
	///		
	
	// 入力のインスタンス取得
	input_ = Input::GetInstance();

	///
	///	基本的な情報初期化
	/// 
	
	// ワールドトランスフォーム初期化
	worldTransform_.Initialize();
	// X軸回転をしてモデルの上面を画面の向きに変更
	worldTransform_.rotation_ = {1.57f, 0.0f, 0.0f};

	// 引数で受け取ったモデルをNULLポインタチェックしてメンバ変数に記録
	assert(modelPlayer);
	modelPlayer_ = modelPlayer;

}

void Player::Update() {
	///
	///	移動
	/// 

	// 左スティックで移動 & 移動している方向へ向ける
	Move();

	///
	///	行列の更新
	/// 
	
	worldTransform_.UpdateMatrix();

	///
	///	デバッグ表示
	/// 
	
	Debug();
}

void Player::Move() {
	// ゲームパッド状態取得
	XINPUT_STATE joyState;
	Input::GetInstance()->GetJoystickState(0, joyState);

	{
		///
		///	プレイヤーの移動
		///

		// 移動ベクトル
		Vector3 move = {0.0f, 0.0f, 0.0f};
		// キャラクターの移動速度
		const float kCharacterSpeed = 0.5f;

		// 左スティックで移動
		move.x += (float)joyState.Gamepad.sThumbLX / SHRT_MAX * kCharacterSpeed;
		move.y += (float)joyState.Gamepad.sThumbLY / SHRT_MAX * kCharacterSpeed;

		// ベクトルを加算して移動
		worldTransform_.translation_ += move;

		///
		///	プレイヤーの向きを移動している方向へ向ける
		///

		// デッドゾーンの設定
		const float deadZone = 0.2f;
		// スティックが動いている場合のみ向きを更新
		if (fabs(move.x) > deadZone || fabs(move.y) > deadZone) {
			// 移動ベクトルの方向を計算（Z軸回転に対応する角度を計算）
			float angle = atan2f(move.y, move.x);
			worldTransform_.rotation_.z = angle; // Z軸回転に角度を設定
		}
	}
}

void Player::Draw(ViewProjection& viewProjection) {
	///
	///	プレイヤー本体描画
	/// 
	
	modelPlayer_->Draw(worldTransform_, viewProjection);
}

void Player::Debug() { 
	ImGui::Begin("player"); 

	ImGui::DragFloat3("translation", &worldTransform_.translation_.x, 0.01f);
	ImGui::DragFloat3("rotation", &worldTransform_.rotation_.x, 0.01f);

	ImGui::End();
}
