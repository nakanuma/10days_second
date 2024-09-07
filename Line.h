#pragma once 
#include "ViewProjection.h"
#include "PrimitiveDrawer.h"

class Line {
public:
	// デストラクタ
	~Line();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="start">始点</param>
	/// <param name="end">終点</param>
	void Initialize(Vector3 start, Vector3 end, ViewProjection* viewProjection);

	// 描画
	void Draw();

	///
	///	アクセッサ
	/// 

	// 始点を取得
	Vector3 GetStart() { return start_; }
	// 終点を取得
	Vector3 GetEnd() { return end_; }
	// 方向ベクトルを取得
	Vector3 GetVector() { return vector_; }

private:
	// プリミティブドロワー
	PrimitiveDrawer* primitiveDrawer_ = nullptr;

	///
	///	基本的な情報
	/// 

	// 始点
	Vector3 start_;
	// 終点
	Vector3 end_;
	// 方向ベクトル（正規化）
	Vector3 vector_;
};
