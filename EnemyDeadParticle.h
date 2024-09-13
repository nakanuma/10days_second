#pragma once
#include "Model.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include <DirectXCommon.h>

class EnemyDeadParticle {
public:
	EnemyDeadParticle(Model* model, Vector3 translation, Vector3 velocity);
	~EnemyDeadParticle();

	void Update();
	void Draw(ViewProjection& viewProjection);

	bool GetDelFlag() { return del_; }

private:
	// モデル
	Model* model_ = nullptr;
	// ワールドトランスフォーム
	WorldTransform worldTransform_;
	// 速度
	Vector3 velocity_;
	float acceleratorY = 0.01f;
	// 削除フラグ
	bool del_ = false;
	// タイマー
	int32_t timer_;
	const int32_t totalTime = 25;
};
