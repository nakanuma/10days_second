#include "Player.h"
#include <cassert>
#include <algorithm>

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
	///	プレイヤー情報の初期化
	/// 
	
	// ワールドトランスフォーム初期化
	worldTransform_.Initialize();
	// X軸を回転させてモデルの上面を画面の向きに回転
	worldTransform_.rotation_ = {1.571f, 0.0f, 1.571f};

	// 引数で受け取ったモデルをNULLポインタチェックしてメンバ変数に記録
	assert(modelPlayer);
	modelPlayer_ = modelPlayer;

	// 移動速度の初期値を設定
	characterSpeed_ = 0.3f;

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

		// 左スティックで移動
		move.x += (float)joyState.Gamepad.sThumbLX / SHRT_MAX * characterSpeed_;
		/*move.y += (float)joyState.Gamepad.sThumbLY / SHRT_MAX * characterSpeed_;*/

		// ベクトルを加算して移動
		worldTransform_.translation_ += move;
	}

	{
		///
		/// 右スティックでプレイヤーの向きを制御
		///

		Vector3 rotation = worldTransform_.rotation_;
		float rightStickX = (float)joyState.Gamepad.sThumbRX / SHRT_MAX;
		float rightStickY = (float)joyState.Gamepad.sThumbRY / SHRT_MAX;

		// デッドゾーンの設定
		const float deadZone = 0.1f;
		if (fabs(rightStickX) > deadZone || fabs(rightStickY) > deadZone) {
			// 右スティックの方向を角度に変換
			float angle = atan2f(rightStickY, rightStickX);
			rotation.z = angle; // Z軸回転に適用
		}

		// プレイヤーの回転を更新
		worldTransform_.rotation_ = rotation;
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
	// WorldTransform
	ImGui::Text("WorldTransform");
	ImGui::DragFloat3("Translation", &worldTransform_.translation_.x, 0.01f);
	ImGui::DragFloat3("Rotation", &worldTransform_.rotation_.x, 0.01f);
	ImGui::DragFloat3("Scale", &worldTransform_.scale_.x, 0.01f);
	// Parameter
	ImGui::Text("Parameter");
	ImGui::DragFloat("PlayerSpeed", &characterSpeed_, 0.01f);

	ImGui::End();
}

Vector3 Player::GetWorldPosition() { 
	Vector3 worldPos;

	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}
