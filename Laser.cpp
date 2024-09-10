#include "Laser.h"
#include <cassert>

Laser::~Laser() {}

void Laser::Initialize(Model* model, Vector3 translation, Vector3 rotation, Vector3 scale) { 
	// NULLポインタチェックをしてメンバ変数に記録
	assert(model); 
	model_ = model;

	// ワールドトランスフォーム初期化
	worldTransform_.Initialize();
	// 引数で受け取った数値を設定
	worldTransform_.translation_ = translation;
	worldTransform_.rotation_ = rotation;
	worldTransform_.scale_ = scale;

	// OBB初期化
	obb_.center = worldTransform_.translation_;
	obb_.rotate = worldTransform_.rotation_;
	obb_.size = worldTransform_.scale_;

	// レーザーを有効にする
	isActive_ = true;
}

void Laser::Update(Vector3 translation, Vector3 rotation) {
	// レーザー発射中のオブジェクトの移動に座標を合わせる
	origin_ = translation;
	worldTransform_.rotation_ = rotation;

	// レーザーの始点を元に、オブジェクトの座標を求める
	Vector3 offset = {kLength + 1.5f, 0.0f, 0.0f}; // ここのX値に足す数値でどれくらい前からレーザーが出るか設定
	offset = Vector3::Transform(offset, Matrix4x4::MakeRotateXYZ(rotation.x, rotation.y, rotation.z));

	worldTransform_.translation_ = origin_ + offset;

	// 行列の更新
	worldTransform_.UpdateMatrix();

	// OBBの更新
	UpdateCollision();
}

void Laser::UpdateCollision() {
	obb_.center = worldTransform_.translation_;
	obb_.rotate = worldTransform_.rotation_;
	obb_.size = worldTransform_.scale_;
}

void Laser::Draw(ViewProjection& viewProjection) {
	/*model_->SetAlpha(0.5f);*/
	// モデルの描画
	model_->Draw(worldTransform_, viewProjection);
}
