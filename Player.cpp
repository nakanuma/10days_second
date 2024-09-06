#include "Player.h"
#include <cassert>
#include "imgui.h"

Player::~Player() {
	///
	///	各種開放
	///		
	
	// 発射する弾のリスト開放
	for (PlayerBullet* bullet : bullets_) {
		delete bullet;
	}

}

void Player::Initialize(Model* modelPlayer, Model* modelBullet) {
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
	worldTransform_.rotation_ = {1.57f, 0.0f, 0.0f};

	// 引数で受け取ったモデルをNULLポインタチェックしてメンバ変数に記録
	assert(modelPlayer);
	modelPlayer_ = modelPlayer;

	// 移動速度の初期値を設定
	characterSpeed_ = 0.3f;

	///
	///	発射する弾の情報初期化
	/// 

	// 引数で受け取ったモデルをNULLポインタチェックしてメンバ変数に記録
	assert(modelBullet);
	modelBullet_ = modelBullet;

	// 弾の速度の初期値を設定
	bulletSpeed_ = 1.0f;
	// 発射間隔の初期値を設定（フレーム）
	fireRate_ = 6;
}

void Player::Update() {
	///
	///	移動
	/// 

	// 左スティックで移動 & 移動している方向へ向ける
	Move();

	///
	///	弾の発射
	/// 

	// Aボタンで射撃
	Attack();

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
		move.y += (float)joyState.Gamepad.sThumbLY / SHRT_MAX * characterSpeed_;

		// ベクトルを加算して移動
		worldTransform_.translation_ += move;

		///
		///	プレイヤーの向きを移動している方向へ向ける
		///

		// デッドゾーンの設定
		const float deadZone = 0.1f;
		// スティックが動いている場合のみ向きを更新
		if (fabs(move.x) > deadZone || fabs(move.y) > deadZone) {
			// 移動ベクトルの方向を計算（Z軸回転に対応する角度を計算）
			float angle = atan2f(move.y, move.x);
			worldTransform_.rotation_.z = angle; // Z軸回転に角度を設定
		}
	}
}

void Player::Attack() {
	// ゲームパッド状態取得
	XINPUT_STATE joyState;
	Input::GetInstance()->GetJoystickState(0, joyState);

	///
	///	弾の射撃
	/// 
	{
		// 弾のクールダウンタイマーを減少させる（毎フレーム射撃を回避）
		if (fireCooldown_ > 0) {
			fireCooldown_--;
		}

		// Aボタンを押している間、射撃
		if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_A && fireCooldown_ <= 0) {
			///
			///	プレイヤーの少し前方から弾を生成するように座標を設定
			/// 

			// 自キャラの座標を取得
			Vector3 position = GetWorldPosition();

			const float offsetDistance = 1.5f; // 前方に移動させる距離
			Vector3 offset;
			offset.x = std::cosf(worldTransform_.rotation_.z) * offsetDistance; // X方向のオフセット
			offset.y = std::sinf(worldTransform_.rotation_.z) * offsetDistance; // Y方向のオフセット
			offset.z = 0.0f;

			// 弾を実際に生成する位置
			Vector3 bulletPosition = position + offset;

			///
			///	弾をプレイヤーが向いている方向へ発射するように設定
			/// 

			// 弾の速度をプレイヤーの向きに応じて計算
			Vector3 velocity;
			velocity.x = std::cosf(worldTransform_.rotation_.z) * bulletSpeed_; // X方向の速度
			velocity.y = std::sinf(worldTransform_.rotation_.z) * bulletSpeed_; // Y方向の速度
			velocity.z = 0.0f;

			///
			///	弾を生成してリストに登録
			/// 

			// 弾を生成して初期化
			PlayerBullet* newBullet = new PlayerBullet();
			newBullet->Initialize(modelBullet_, bulletPosition, velocity);

			// 弾をリストに登録
			bullets_.push_back(newBullet);

			// クールダウンタイマーをリセット
			fireCooldown_ = fireRate_;
		}
	}
	///
	///	弾の更新
	/// 
	{ 
		// 全ての弾を更新
		for (PlayerBullet* bullet : bullets_) {
			bullet->Update();
		}
		// デスフラグの立った弾を削除
		bullets_.remove_if([](PlayerBullet* bullet) {
			if (bullet->IsDead()) {
				delete bullet;
				return true;
			}
			return false;
		});
	}
}

void Player::Draw(ViewProjection& viewProjection) {
	///
	///	プレイヤー本体描画
	/// 
	
	modelPlayer_->Draw(worldTransform_, viewProjection);

	///
	///	弾全ての描画
	/// 
	
	for (PlayerBullet* bullet : bullets_) {
		bullet->Draw(viewProjection);
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
	ImGui::DragFloat("BulletSpeed", &bulletSpeed_, 0.01f);
	ImGui::DragInt("FireRate(frame)", &fireRate_, 0.1f);

	ImGui::End();
}

Vector3 Player::GetWorldPosition() { 
	Vector3 worldPos;

	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}
