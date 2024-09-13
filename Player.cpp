#include "Player.h"
#include <algorithm>
#include <cassert>
#include <numbers>

#include "TextureManager.h"
#include "imgui.h"

// MyClass
#include "Easing.h"

Player::~Player() {
	///
	///	各種開放
	///

	///
	///	スプライト
	///

	/* ハート（HP） */
	for (Sprite* sprite : heartSprites_) {
		delete sprite;
	}

	/* スコア（6桁分） */
	for (int32_t i = 0; i < kMaxScoreDigit; i++) {
		delete spriteScore_[i];
	}
}

void Player::Initialize(Model* modelPlayer, Model* modelLaser) {
	///
	///	汎用機能初期化
	///

	// 入力のインスタンス取得
	input_ = Input::GetInstance();
	// オーディオのインスタンス取得
	audio_ = Audio::GetInstance();

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

	// 生存フラグの初期化
	isAlive_ = true;

	// レーザー射撃中/非射撃中の自動上昇・下降の速度の初期値を設定（ImGuiでいじれるように）
	autoAscendingSpeed_ = 0.1f;
	autoDescendingSpeed_ = 0.25f;

	// 現在の下降速度
	currentDescendingSpeed_ = 0.0f;

	///
	///	予備動作hpレーザー関連の初期化
	///

	// モデルを設定
	assert(modelLaser);
	modelLaser_ = modelLaser;

	anticWorldTransform_.Initialize();

	Vector3 offset{0.0f, Laser::kLength, 0.0f};
	anticWorldTransform_.translation_ = worldTransform_.translation_ + offset; // プレイヤーの座標 + オフセット
	anticWorldTransform_.rotation_ = {0.0f, 0.0f, 1.57f};                      // 最初は右向きなので上向きにする
	anticWorldTransform_.scale_ = {Laser::kLength, 0.05f, 0.05f};              // 初期スケール

	// ワールドトランスフォームを更新しておく
	anticWorldTransform_.UpdateMatrix();

	///
	///	スプライト生成
	///

	/* ハート（HP） */

	// ハートのテクスチャ取得
	uint32_t textureHeart = TextureManager::Load("images/player_life.png");
	// ハートスプライトのインスタンスをHPの値だけ取得
	for (int32_t i = 0; i < hp_; ++i) {
		Sprite* heartSprite = Sprite::Create(textureHeart, {70.0f + (i * 70), 40.0f}); // x70の位置から、70pixelずつ間隔を開ける
		heartSprites_.push_back(heartSprite);
	}

	/* スコア（6桁分） */
	textureNumber_ = TextureManager::Load("images/number.png");
	for (int32_t i = 0; i < kMaxScoreDigit; i++) {
		spriteScore_[i] = Sprite::Create(textureNumber_, {0.0f, 0.0f});
		spriteScore_[i]->SetSize({21.0f, 30.0f}); // 数字1文字分のサイズを指定
	}

	///
	///	サウンド
	///

	// 被弾時サウンド
	hitSH_ = audio_->LoadWave("./Resources/sounds/SE_player_damage.wav");
	// レーザー発射時サウンド
	laserSH_ = audio_->LoadWave("./Resources/sounds/SE_player_laser.wav");
}

void Player::Update() {
	///
	/// 移動
	///

	// 左スティックで移動 & 移動している方向へ向ける
	Move();

	///
	/// 攻撃
	///

	// RBでレーザーを発射
	Attack();

	///
	///	予備動作レーザーの更新
	///

	// 通常レーザーと予備動作レーザーの位置と回転角を一致させる
	Vector3 offset = {Laser::kLength + 1.5f, 0.0f, 0.0f};
	Vector3 rot = worldTransform_.rotation_;
	offset = Vector3::Transform(offset, Matrix4x4::MakeRotateXYZ(rot.x, rot.y, rot.z));

	anticWorldTransform_.translation_ = worldTransform_.translation_ + offset;
	anticWorldTransform_.rotation_ = worldTransform_.rotation_;

	///
	///	無敵時間中（ダメージを受けた後）に行う処理
	///

	Invincible();

	///
	/// 生存確認
	///

	CheckIsAlive();

	///
	/// 行列の更新
	///

	worldTransform_.UpdateMatrix();
	anticWorldTransform_.UpdateMatrix();

	///
	/// デバッグ表示
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
			const float smoothingFactor = 0.025f; // 補間の速さ（低いほどゆっくり、1.0はすぐにターゲット角度になる）
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
		currentDescendingSpeed_ = 0.0f; // 上昇中は下降速度をリセット
	} else {
		// 下降速度を更新
		if (currentDescendingSpeed_ < autoDescendingSpeed_) {
			currentDescendingSpeed_ += kDescentAcceleration_;
		}
		// 速度がautoDescendingSpeedを超えないようにクランプ
		currentDescendingSpeed_ = (std::min)(currentDescendingSpeed_, autoDescendingSpeed_);

		// 現在の下降速度を使って位置を更新
		worldTransform_.translation_.y -= currentDescendingSpeed_;
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

	///
	///	RBを押した瞬間の上昇処理
	///

	if (isRising_) {
		riseTime_ += 1.0f;                                           // 経過時間を増加
		float t = std::clamp(riseTime_ / riseDuration_, 0.0f, 1.0f); // 0.0f~1.0fの間にクランプ

		// イージングを用いてY座標を更新
		worldTransform_.translation_.y = Easing::EaseOutQuad(riseStartY_, riseEndY_, t);

		// 上昇が完了したらフラグをリセット
		if (t >= 1.0f) {
			isRising_ = false;
		}
	}
}

void Player::StartAntic() {
	// 予備動作レーザーの拡大にかける時間
	const float kAnticDuration = 20.0f; // フレーム数を設定

	// 進行度を0.0 ~ 1.0fの範囲で計算
	if (anticTimer_++ >= 20) {                                // 地面に到達して指定フレームしたら開始
		float progress = (anticTimer_ - 20) / kAnticDuration; // 指定フレーム後からスタート
		progress = (std::min)(progress, 1.0f);                // 進行度が1.0を超えないように

		// EaseInQuad関数で拡大率を計算
		float scale = Easing::EaseOutQuart(progress) * (kRadius_ - 0.05f) + 0.05f; // 初期スケールYが0.05であるため、0.05から始まるよう調整
		anticWorldTransform_.scale_.y = scale;
		anticWorldTransform_.scale_.z = scale;
	}

	// 予備動作が終了したかを確認
	if (anticWorldTransform_.scale_.y >= kRadius_) {
		endAntic_ = true; // 予備動作終了
	}
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

	/*if (worldTransform_.translation_.y >= 16.0f) {
	    isLaserButtonPressed = false;
	}*/

	// RBを押した場合
	if (isLaserButtonPressed) {
		// レーザーの初期化
		laser_.Initialize(modelLaser_, worldTransform_.translation_, worldTransform_.rotation_, Vector3{Laser::kLength, worldTransform_.scale_.y, worldTransform_.scale_.z});

		// サウンドが再生されていない場合のみ再生
		/*if (!isLaserSoundPlaying_) {
			audio_->PlayWave(laserSH_, true);
			isLaserSoundPlaying_ = true;
		}*/

		///
		///	押されている間は予備動作レーザーを更新
		///

		StartAntic();

		///
		/// RBが押された瞬間（前フレームで押されていなかった場合）のみ、プレイヤーを上に跳ねさせる跳ねさせる
		///
		if (!wasLaserButtonPressed_ && !isRising_ && worldTransform_.translation_.y <= 16.0f) { // あんまりにも上にいる時は跳ねさせない
			isRising_ = true;
			riseStartY_ = worldTransform_.translation_.y;
			riseEndY_ = riseStartY_ + 3.5f; // 3.5上に上昇
			riseTime_ = 0.0f;               // 上昇の経過時間リセット
		}

		// 押していない場合はレーザーを無効化
	} else {
		laser_.SetActive(false);

		// サウンドが再生中なら停止
		/*if (isLaserSoundPlaying_) {
			audio_->StopWave(laserSH_);
			isLaserSoundPlaying_ = false;
		}*/

		// 予備動作レーザーの情報を初期状態にリセット
		anticWorldTransform_.scale_ = {Laser::kLength, 0.05f, 0.05f};
		anticTimer_ = 0;
		endAntic_ = false;
	}

	// 有効な場合かつ予備動作が終わった場合、レーザーを更新
	if (laser_.IsActive() && endAntic_) {
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
		audio_->PlayWave(hitSH_);
		hp_--;
		// 無敵状態にする
		isInvincible_ = true;
		// 無敵カウントに無敵時間をセット
		invincibleCount_ = kInvincibleTime_;
	}
}

void Player::CheckIsAlive() {
	// もしHPが0になったらフラグをfalseにする
	if (hp_ <= 0) {
		isAlive_ = false;
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

	// 有効な場合かつ予備動作が終わった場合のみ描画
	if (laser_.IsActive() && endAntic_) {
		laser_.Draw(viewProjection);
	}

	///
	///	予備動作レーザーの描画
	///

	// 一旦常に描画
	modelLaser_->Draw(anticWorldTransform_, viewProjection);
}

void Player::DrawUI() {
	///
	///	左上の残りHP（ハート）の描画
	///

	for (int32_t i = 0; i < hp_; i++) {
		heartSprites_[i]->Draw();
	}

	///
	///	左上の現在スコアの描画
	///

	int32_t scoreDigit[6];      // 各桁の数字を格納
	int32_t scoreTemp = score_; // 現在スコアを一時的に保存

	// scoreDigitに現在スコアを格納
	for (int32_t i = 0; i < kMaxScoreDigit; i++) {
		scoreDigit[i] = scoreTemp % 10;
		scoreTemp /= 10;
	}

	// スコアの表示位置（左上）
	Vector2 scorePosition = {96.0f, 110.0f}; // ちょうど中央にくるいい感じの位置

	// スコアを6桁表示
	for (int32_t i = 0; i < kMaxScoreDigit; i++) {
		// 各桁の位置を計算
		Vector2 digitPosition = scorePosition + Vector2{static_cast<float>(i) * (21.0f + 3.0f), 0.0f}; // 21.0fは数字のサイズ。ちょっと間隔を開ける
		// 読み込んだテクスチャから適切な数字を抜き取る
		spriteScore_[i]->SetTextureRect({0.0f + (scoreDigit[5 - i] * 50.0f), 0.0f}, {50.0f, 65.0f});
		spriteScore_[i]->SetPosition(digitPosition);
		spriteScore_[i]->Draw();
	}
}

void Player::Debug() {
#ifdef _DEBUG

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

	ImGui::Text("Laser %d", isLaserSoundPlaying_);

	ImGui::End();

#endif
}

Vector3 Player::GetWorldPosition() {
	Vector3 worldPos;

	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}
