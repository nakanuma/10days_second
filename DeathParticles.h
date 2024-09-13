#pragma once

#include <Algorithm>
#include <array>
#include <cassert>
#include <numbers>
#include <cmath>
#define _USE_MATH_DEFINES
#include <math.h>

#include "Model.h"
#include "ViewProjection.h"
#include "WorldTransform.h"

/// <summary>
/// デス演出用パーティクル
/// </summary>
class DeathParticles {

public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Model* model, ViewProjection* viewProjection, const Vector3& position);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// 終了フラグ取得
	/// </summary>
	/// <returns></returns>
	bool IsFinished() const;

private:
	// モデル
	Model* model_;

	// ビュープロジェクション
	ViewProjection* viewProjection_;

	// 色変更オブジェクト
	ObjectColor objectColor_;
	// 色の数値
	Vector4 color_;

	// パーティクルの個数
	static inline const uint32_t kNumParticles = 8;

	std::array<WorldTransform, kNumParticles> worldTransforms_;

	// 消滅時間
	static inline const float kDuration = 2.0f;
	// 移動の速さ
	static inline const float kSpeed = 0.1f;
	// 分割した一個分の角度
	static inline const float kAngle = 2 * float(M_PI) / kNumParticles;
	// 終了フラグ
	bool isFinished_ = false;
	// 経過時間カウント
	float counter_ = 0.0f;
};