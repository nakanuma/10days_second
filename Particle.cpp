#include "Particle.h"

#include <random>

Particle::~Particle() { delete sprite_; }

void Particle::Initialize(Vector2 position) {

	// ファイルを指定してテクスチャ読み込み
	textureHandle_ = TextureManager::Load("./Resources/images/bg/back_black.png");

	sprite_ = Sprite::Create(textureHandle_, {position});
	sprite_->SetSize(Vector2(10, 10));
	sprite_->SetColor(Vector4(1, 1, 1, 1));
}

void Particle::Update() {

	Vector2 position;

	position = sprite_->GetPosition();

	position.y -= 2;

	sprite_->SetPosition(position);

	color_ = sprite_->GetColor();

	color_.w -= 0.005f;

	sprite_->SetColor(color_);
}

void Particle::Draw() {

	// パーティクルの描画
	sprite_->Draw();
}