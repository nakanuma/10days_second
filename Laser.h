#pragma once
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "Model.h"

// MyClass
#include "OBBCollider.h"

class Laser {
public:
	~Laser();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="model">レーザーのモデル</param>
	/// <param name="translation">発射するオブジェクトの位置</param>
	/// <param name="rotation">発射するオブジェクトの回転角</param>
	void Initialize(Model* model, Vector3 translation, Vector3 rotation, Vector3 scale);

	// 更新
	void Update(Vector3 translation, Vector3 rotation);
	// 
	void UpdateCollision();

	// 描画
	void Draw(ViewProjection& viewProjection);

	// レーザーの長さ
	inline const static float kLength = 50.0f;

	///
	///	アクセッサ
	/// 

	// レーザーが有効かを取得
	bool IsActive() { return isActive_; }
	// レーザーの有効/無効を設定
	void SetActive(bool flag) { isActive_ = flag; }
	// OBBの取得
	const OBBCollider& GetOBB() { return obb_; }

private:
	// ワールドトランスフォーム
	WorldTransform worldTransform_;
	// OBB
	OBBCollider obb_;

	// モデル
	Model* model_ = nullptr;
	// 始点
	Vector3 origin_;
	// 回転角
	Vector3 rotation_;

	// レーザーが有効かどうか
	bool isActive_ = false;
};
