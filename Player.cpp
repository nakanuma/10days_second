#include "Player.h"
#include <cassert>
#include <algorithm>
#include <numbers>

#include "imgui.h"

// MyClass
#include "Easing.h"

Player::~Player() {
	///
	///	各種開放
	///		
	
}

void Player::Initialize(Model* modelPlayer, Model* modelLaser) {
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
	// 初期位置を設定
	worldTransform_.translation_ = {0.0f, -18.0f, 0.0f};
	// X軸を回転させてモデルの上面を画面の向きに回転
	worldTransform_.rotation_ = {0.0f, 0.0f, 1.571f};

	// 引数で受け取ったモデルをNULLポインタチェックしてメンバ変数に記録
	assert(modelPlayer);
	modelPlayer_ = modelPlayer;

	// 移動速度の初期値を設定
	characterSpeed_ = 0.3f;
	// 体力の初期値を設定
	hp_ = 3;
	// スコアの初期化
	score_ = 0;

	// 無敵時間カウントを初期化
	invincibleCount_ = 0;

	// レーザーを撃つ際のボタンが押されたかの初期化（RBが押された瞬間のみプレイヤーを跳ねさせる処理に使用）
	wasLaserButtonPressed_ = false;

	// レーザー射撃中/非射撃中の自動上昇・下降の速度の初期値を設定（ImGuiでいじれるように）
	autoAscendingSpeed_ = 0.1f;
	autoDescendingSpeed_ = 0.15f;

	///
	///	レーザー関連の初期化
	/// 
	
	assert(modelLaser);
	modelLaser_ = modelLaser;
}

void Player::Update() {
	///
	///	移動
	/// 

	// 左スティックで移動 & 移動している方向へ向ける
	Move();

	///
	///	攻撃
	///		

	// RBでレーザーを発射
	Attack();

	///
	///	無敵時間中（ダメージを受けた後）に行う処理
	/// 

	Invincible();

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

	///
	///	左スティックでプレイヤーの左右移動
	///
	{
		// 移動ベクトル
		Vector3 move = {0.0f, 0.0f, 0.0f};

		// 左スティックで移動
		move.x += (float)joyState.Gamepad.sThumbLX / SHRT_MAX * characterSpeed_;
		/*move.y += (float)joyState.Gamepad.sThumbLY / SHRT_MAX * characterSpeed_;*/

		// ベクトルを加算して移動
		worldTransform_.translation_ += move;
	}

	///
	/// 右スティックでプレイヤーの向きを制御
	///
	{
		Vector3 rotation = worldTransform_.rotation_;
		float rightStickX = (float)joyState.Gamepad.sThumbRX / SHRT_MAX;
		float rightStickY = (float)joyState.Gamepad.sThumbRY / SHRT_MAX;

		// デッドゾーンの設定
		const float deadZone = 0.01f;
		if (fabs(rightStickX) > deadZone || fabs(rightStickY) > deadZone) {
			// 右スティックの方向を角度に変換
			float targetAngle = atan2f(rightStickY, rightStickX);

			// 現在の角度と目標角度の差を計算
			float angleDifference = targetAngle - rotation.z;

			// 角度差を -π ～ +π の範囲に正規化
			if (angleDifference > std::numbers::pi_v<float>) {
				angleDifference -= 2.0f * std::numbers::pi_v<float>;
			} else if (angleDifference < -std::numbers::pi_v<float>) {
				angleDifference += 2.0f * std::numbers::pi_v<float>;
			}

			// 角度を滑らかに補間
			const float smoothingFactor = 0.04f; // 補間の速さ（低いほどゆっくり、1.0はすぐにターゲット角度になる）
			rotation.z += smoothingFactor * angleDifference;

			// 回転角度を -π ～ +π の範囲にクランプ
			if (rotation.z > std::numbers::pi_v<float>) {
				rotation.z -= 2.0f * std::numbers::pi_v<float>;
			} else if (rotation.z < -std::numbers::pi_v<float>) {
				rotation.z += 2.0f * std::numbers::pi_v<float>;
			}
		}

		// プレイヤーの回転を更新
		worldTransform_.rotation_ = rotation;
	}

	///
	///	射撃中のレーザーのON/OFFによって、自動で上昇と下降を行う
	/// 

	// レーザーが有効な場合、自動で上昇
	if (laser_.IsActive()) {
		worldTransform_.translation_.y += autoAscendingSpeed_;
	} else {
		worldTransform_.translation_.y -= autoDescendingSpeed_;
	}

	///
	///	範囲外へ行かないようにする
	/// 
	
	// 移動限界座標
	const float kMoveLimitX = 14.0f;
	const float kMoveLimitY = 18.0f;

	// 範囲を超えない処理
	worldTransform_.translation_.x = std::clamp(worldTransform_.translation_.x, -kMoveLimitX, +kMoveLimitX);
	worldTransform_.translation_.y = std::clamp(worldTransform_.translation_.y, -kMoveLimitY, +kMoveLimitY);
}

void Player::Attack() {
	// ゲームパッド状態取得
	XINPUT_STATE joyState;
	Input::GetInstance()->GetJoystickState(0, joyState);

	///
	///	RBでレーザー発射
	///	

	// RBボタンの現在の押下状態を記録
	bool isLaserButtonPressed = (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER);

	// RBを押した場合
	if (isLaserButtonPressed) {
		// レーザーの初期化
		laser_.Initialize(
			modelLaser_, 
			worldTransform_.translation_, 
			worldTransform_.rotation_, 
			Vector3{Laser::kLength, worldTransform_.scale_.y, worldTransform_.scale_.z}
		);

		///
		/// RBが押された瞬間（前フレームで押されていなかった場合）のみ、プレイヤーを上に跳ねさせる
		/// 
		if (!wasLaserButtonPressed_) {
			worldTransform_.translation_.y += 1.5f;
		}

	// 押していない場合はレーザーを無効化
	} else {
		laser_.SetActive(false);
	}

	// 有効な場合、レーザーを更新
	if (laser_.IsActive()) {
		laser_.Update(worldTransform_.translation_, worldTransform_.rotation_);
	}

	// 現在のRBボタン状態を次フレームに向けて記録
	wasLaserButtonPressed_ = isLaserButtonPressed;
}

void Player::Invincible() {
	// 無敵の場合、無敵カウントを減らしていく
	if (isInvincible_) {
		invincibleCount_--;
	}

	// 無敵カウントが0になったら無敵時間を解除
	if (invincibleCount_ <= 0) {
		isInvincible_ = false;
	}
}

void Player::OnCollision() {
	// 現在が無敵じゃない場合のみ処理を行う
	if (!isInvincible_) {
		hp_--;
		// 無敵状態にする
		isInvincible_ = true;
		// 無敵カウントに無敵時間をセット
		invincibleCount_ = kInvincibleTime_;
	}
}

void Player::Draw(ViewProjection& viewProjection) {
	///
	///	プレイヤー本体描画
	/// 
	
	// 無敵じゃない場合は普通に描画
	if (!isInvincible_) {
		modelPlayer_->Draw(worldTransform_, viewProjection);
	// 無敵状態のときは点滅させる
	} else {
		if (invincibleCount_ % 5 == 0) {
			modelPlayer_->Draw(worldTransform_, viewProjection);
		}
	}

	///
	///	レーザーの描画
	/// 
	
	// 有効な場合のみ描画
	if (laser_.IsActive()) {
		laser_.Draw(viewProjection);
	}
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
	ImGui::Text("HP : %d", hp_);
	ImGui::Text("Score : %d", score_);
	ImGui::DragFloat("AscendingSpeed", &autoAscendingSpeed_, 0.01f);
	ImGui::DragFloat("DescendingSpeed", &autoDescendingSpeed_, 0.01f);

	ImGui::End();
}

Vector3 Player::GetWorldPosition() { 
	Vector3 worldPos;

	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}
