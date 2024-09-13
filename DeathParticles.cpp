#include "DeathParticles.h"

bool DeathParticles::IsFinished() const { return isFinished_; }

void DeathParticles::Initialize(Model* model, ViewProjection* viewProjection, const Vector3& position) {

	// NULLポインタチェック
	assert(model);

	model_ = model;

	for (auto& worldTransform : worldTransforms_) {
		worldTransform.Initialize();
		worldTransform.translation_ = position;
	}

	objectColor_.Initialize();
	color_ = {1, 1, 1, 1};

	viewProjection_ = viewProjection;
}

void DeathParticles::Update() {

	if (isFinished_) {
		return;
	}

	for (uint32_t i = 0; i < kNumParticles; i++) {
		// 基本となる速度ベクトル
		Vector3 velocity = {kSpeed, 0, 0};
		// 回転角を計算する
		float angle = kAngle * i;
		// Z軸まわり回転行列
		Matrix4x4 matrixRotation = Matrix4x4::MakeRotateZ(angle);
		// 基本ベクトルを回転させて速度ベクトルを得る
		velocity = Vector3::Transform(velocity, matrixRotation);
		// 移動処理
		worldTransforms_[i].translation_ += velocity;
	}

	// カウンターを1フラーム分の数秒進ませる
	counter_ += 1.0f / 60.0f;

	// 存在時間の上限に達したら
	if (counter_ >= kDuration) {
		counter_ = kDuration;
		// 終了扱いになる
		isFinished_ = true;
	}

	color_.w = std::clamp(kDuration - counter_, 0.0f, 1.0f);
	// 色変更オブジェクトに色の数値を設定する
	objectColor_.SetColor(color_);
	// 色変更オブジェクトをVRAMに転送
	objectColor_.TransferMatrix();

	for (auto& worldTransform : worldTransforms_) {
		worldTransform.UpdateMatrix();
	}
}

void DeathParticles::Draw() {

	if (isFinished_) {
		return;
	}

	// 3Dモデル描画
	for (auto& worldTransform : worldTransforms_) {
		model_->Draw(worldTransform, *viewProjection_, &objectColor_);
	}
}