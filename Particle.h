#pragma once
#include "Sprite.h"
#include "TextureManager.h"

#include <list>

class Particle {

public:

	// デストラクタ
	~Particle();

	// 初期化
	void Initialize(Vector2 position);

	// 更新
	void Update();

	// 描画
	void Draw();

private:

	// テクスチャハンドル
	uint32_t textureHandle_ = 0;

	// パーティクルのスプライト
	Sprite* sprite_ = nullptr;

	// アルファ値
	float alpha_ = 1;

	Vector4 color_ = {1, 1, 1, 1};
};