#include "Enemy.h"
#include "imgui.h"
#include <algorithm>
#include <cassert>

// MyClass
#include "Easing.h"

Enemy::~Enemy() {}

void Enemy::Initialize(Model* modelEnemy, Model* modelLaser, Vector3 position, float radius, float fallSpeed) {

	audio_ = Audio::GetInstance();

	// メンバ変数にモデルを設定
	assert(modelEnemy);
	modelEnemy_ = modelEnemy;

	assert(modelLaser);
	modelLaser_ = modelLaser;

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_ = {0.0f, 0.0f, 1.57f}; // 最初は右を向いていることになっているので、上向きに設定

	// 半径の初期値を設定
	radius_ = radius;
	// 自動で下降する速度を設定（低いほどゆっくり降りてくる）
	fallSpeed_ = fallSpeed;

	///
	///	予備動作用レーザーの設定
	///

	// 予備動作用ワールドトランスフォームの初期化
	anticWorldTransform_.Initialize();

	Vector3 offset = {0.0f, Laser::kLength, 0.0f};
	anticWorldTransform_.translation_ = worldTransform_.translation_ + offset;
	anticWorldTransform_.rotation_ = {0.0f, 0.0f, 1.57f}; // 最初は右を向いていることになっているので、上向きに設定
	anticWorldTransform_.scale_ = {Laser::kLength, 0.1f, 0.01f};

	// ワールドトランスフォームを更新しておく
	worldTransform_.UpdateMatrix();
	anticWorldTransform_.UpdateMatrix();

	// サウンド
	laserSE_ = audio_->LoadWave("./Resources/sounds/SE_enemy_laser.mp3");
}

void Enemy::Update() {
	///
	///	自動で下降させる処理
	///

	// 現在の半径に基づいて降下速度を調整（大きければ大きいほど早く降下させるため）
	float adjustedFallspeed = fallSpeed_ + (radius_ * 0.03f); // ここの値は要調整

	// 自動で降下する速度の分だけ降下させる
	worldTransform_.translation_.y -= adjustedFallspeed;

	///
	///	スケールと半径を同期させる（レーザーが当たると大きくなるにあたって、見た目と当たり判定を合わせる）
	///

	// 地面に到達していない間のみ（一番下でレーザーを撃っている際にScaleを縮小させるため、不都合が起きないように）
	if (!hasReachedBottom_) {
		worldTransform_.scale_.x = radius_;
		worldTransform_.scale_.y = radius_;
		worldTransform_.scale_.z = radius_;
	}

	///
	///	Y座標が一定の場所まで降りたら停止
	///

	const float kStopYposition = -18.0f;
	worldTransform_.translation_.y = (std::max)(worldTransform_.translation_.y, kStopYposition);
	// 一番したまで到達したことを示す
	if (worldTransform_.translation_.y <= kStopYposition) {
		hasReachedBottom_ = true;
	}

	///
	///	一番下まで到達したら予備動作の開始
	///

	if (hasReachedBottom_) {
		if (!endAntic_) {
			// 予備動作を実行
			StartAntic();
		} else {
			// 予備動作が終了したら攻撃開始
			Attack();
		}
	}

	///
	/// レーザーが有効な間は敵本体のscale.xを縮小させる
	///

	// 毎フレーム縮小させるサイズ
	const float kDecrementSize = 0.01f;

	if (laser_.IsActive()) {
		worldTransform_.scale_.x -= kDecrementSize;
	}
	// スケールの縮小によって消えたら死亡させる
	if (worldTransform_.scale_.x <= 0.0f) {
		isDead_ = true;
	}

	///
	///	予備動作レーザーの更新
	///

	Vector3 offset = {0.0f, Laser::kLength, 0.0f};
	anticWorldTransform_.translation_ = worldTransform_.translation_ + offset;

	///
	///	行列の更新
	///
	worldTransform_.UpdateMatrix();
	anticWorldTransform_.UpdateMatrix();

	///
	///	デバッグ表示
	///

	/*Debug();*/
}

void Enemy::StartAntic() {
	// 予備動作レーザーの拡大にかける時間
	const float kAnticDuration = 30.0f; // フレーム数を設定

	// 進行度を0.0 ~ 1.0fの範囲で計算
	if (anticTimer_++ >= 30) {                                // 地面に到達して指定フレームしたら開始
		float progress = (anticTimer_ - 30) / kAnticDuration; // 指定フレーム後からスタート
		progress = (std::min)(progress, 1.0f);                // 進行度が1.0を超えないように

		// EaseInQuad関数で拡大率を計算
		float scale = Easing::EaseInQuart(progress) * (radius_ - 0.1f) + 0.1f; // 初期スケールYが0.1であるため、0.1から始まるよう調整
		anticWorldTransform_.scale_.y = scale;
	}

	// 予備動作が終了したかを確認
	if (anticWorldTransform_.scale_.y >= radius_) {
		endAntic_ = true; // 予備動作終了
	}
}

void Enemy::Attack() {
	// 一番下に到達した際にレーザーを発射する
	if (hasReachedBottom_) {
		// レーザーの初期化
		laser_.Initialize(modelLaser_, worldTransform_.translation_, worldTransform_.rotation_, Vector3{Laser::kLength, worldTransform_.scale_.y, 0.01f});
	}

	// 有効な場合、レーザーを更新
	if (laser_.IsActive()) {
		laser_.Update(worldTransform_.translation_, worldTransform_.rotation_);
	}
}

void Enemy::Draw(ViewProjection& viewProjection) {
	///
	///	敵本体の描画
	///

	modelEnemy_->Draw(worldTransform_, viewProjection);

	///
	///	予備動作レーザーの描画
	///

	// 地面に着いたかつ、まだ予備動作が終わっていない場合のみ
	if (hasReachedBottom_ /*&& !endAntic_*/) { // 1フレームだけレーザーが無い状態ができてしまうので、両方のレーザーを重ねて描画
		// 予備動作レーザー本体の描画
		modelLaser_->Draw(anticWorldTransform_, viewProjection);
	}
}

void Enemy::DrawLaser(ViewProjection& viewProjection) {
	///
	///	レーザーの描画
	///

	// 有効な場合のみ
	if (laser_.IsActive()) {
		laser_.Draw(viewProjection);
	}
}

void Enemy::OnCollision(float incrementSize) {
	///
	///	当たっているフレーム、引数で指定された値、自身のサイズ(半径)を増加させる
	///
	radius_ += incrementSize;

	///
	///	自身のサイズ(半径)が一定以上になったら死亡させる
	///

	const float kMaxSize = 3.0f;
	if (radius_ >= kMaxSize) {
		isDead_ = true;
	}

	// あとでサイズとかを変える処理に変更するけどデバッグ用で一旦死亡させる
	/*isDead_ = true;*/
}

void Enemy::Debug() {
#ifdef _DEBUG

	ImGui::Begin("Enemy");

	ImGui::Text("radius : %.2f", radius_);
	ImGui::Text("reachedBottom : %d", hasReachedBottom_);
	ImGui::Text("LaserIsActive : %d", laser_.IsActive());

	ImGui::End();

#endif
}

Vector3 Enemy::GetWorldPosition() {
	Vector3 worldPos;

	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}
