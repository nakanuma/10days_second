#pragma once

#include "Model.h"
#include "WorldTransform.h"
#include "ViewProjection.h"

/// <summary>
/// 敵が出現する際に画面上部に出すマーク
/// </summary>
class EnemyAppearMark {
public:
	// デストラクタ
	~EnemyAppearMark();

	
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="model">敵出現マークのモデル</param>
	/// <param name="position">マーク表示を行う座標</param>
	void Initialize(Model* model, Vector3 position);

	// 更新
	void Update();

	// 描画
	void Draw(ViewProjection& viewProjection);

	///
	///	アクセッサ
	/// 

	// 死亡したかを取得
	bool IsDead() { return isDead_; }
	void Dead() { isDead_ = true; }

private:
	// ワールドトランスフォーム
	WorldTransform worldTransform_;
	// モデル
	Model* model_ = nullptr;

	// 死亡したか（マーク表示時間を設定するため、最終的に死亡させる）
	bool isDead_ = false;
	// このマークの表示時間（フレーム）
	const int32_t kMaxDisplayTime = 60;
	// 生存時間のカウント
	int32_t lifeTime_;

	/*Alpha値関連*/

	// 初期Alpha値
	float alpha_ = 1.0f;
	// Alpha値が増加中かどうか
	bool alphaIncreasing_ = false; 

};
