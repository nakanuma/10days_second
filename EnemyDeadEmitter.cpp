#include "EnemyDeadEmitter.h"
#include <cassert>
#include <random>

EnemyDeadEmitter::EnemyDeadEmitter() {}

EnemyDeadEmitter::~EnemyDeadEmitter() {}

void EnemyDeadEmitter::Initialize(Model* model) { 
	assert(model); 
	model_ = model;
}

void EnemyDeadEmitter::Update() {
	// リスト内のパーティクル全てを更新
	for (auto itr = particleList_.begin(); itr != particleList_.end();) {
		(*itr)->Update();

		// DelFlagがtrueだったらリストから削除
		if ((*itr)->GetDelFlag()) {
			itr = particleList_.erase(itr);
		// eraseしなかった場合イテレーターをインクリメント
		} else {
			itr++;
		}
	}
}

void EnemyDeadEmitter::Draw(ViewProjection& viewProjection) {
	// リスト内のパーティクル全てを描画
	for (auto& particle : particleList_) {
		particle->Draw(viewProjection);
	}
}

void EnemyDeadEmitter::Emit(Vector3 translation) {
	// 乱数生成器を初期化
	std::random_device rd;
	std::mt19937 rng(rd());

	const float kMaxVelocity = 0.2f;
	std::uniform_real_distribution<float> dist(-kMaxVelocity, kMaxVelocity);

	for (uint32_t i = 0; i < kParticleNum; i++) {
		// 速度ベクトルをランダムに生成
		Vector3 velocity(dist(rng), dist(rng), dist(rng));

		std::unique_ptr<EnemyDeadParticle> newParticle = std::make_unique<EnemyDeadParticle>(model_, translation, velocity);
		particleList_.push_back(std::move(newParticle));
	}
}
