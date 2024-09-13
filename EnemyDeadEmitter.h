#pragma once
#include "EnemyDeadParticle.h"
#include "Model.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include <DirectXCommon.h>
#include <list>

class EnemyDeadEmitter {
public:
	EnemyDeadEmitter();
	~EnemyDeadEmitter();

	void Initialize(Model* model);
	void Update();
	void Draw(ViewProjection& viewProjection);
	void Emit(Vector3 translation);

private:
	// モデル
	Model* model_;
	// パーティクルリスト
	std::list<std::unique_ptr<EnemyDeadParticle>> particleList_;
	// 発生パーティクル数
	const uint32_t kParticleNum = 20;
};
