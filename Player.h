#pragma once
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "Model.h"
#include "Input.h"

class Player {
public:
	// デストラクタ
	~Player();
	// 初期化
	void Initialize();
	// 更新
	void Update();
	// 描画
	void Draw();

private:
	///
	///	汎用機能
	/// 
	
	Input* input_ = nullptr;

	///
	///	基本的な情報
	/// 
	
	// ワールドトランスフォーム
	WorldTransform worldTransform_;
	// モデル
	Model* modelPlayer_ = nullptr;

};
