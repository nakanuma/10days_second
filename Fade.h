#pragma once

#include "algorithm"
#include <ImGui.h>

#include "Sprite.h"
#include "TextureManager.h"

/// <summary>
/// フェード
/// </summary>
class Fade {

public:
	// フェードの状態
	enum class Status {
		None,    // フェードなし
		FadeIn,  // フェードイン
		FadeOut, // フェードアウト
	};

	Fade();
	~Fade();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	// フェード開始
	void Start(Status status, float duration);
	// フェード停止
	void Stop();

	// 　フェード終了判定
	bool IsFinished() const;

private:
	// テクスチャハンドル
	uint32_t textureHandle_ = 0;
	// スプライト
	Sprite* sprite_ = nullptr;

	// 現在のフェード状態
	Status status_ = Status::None;
	// フェードの持続時間
	float duration_ = 0.0f;
	// 経過時間カウンター
	float counter_ = 0.0f;
};