#pragma once
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "Model.h"
#include "Input.h"

class Player {
public:
	///
	///	基本的な関数
	/// 

	// デストラクタ
	~Player();

	// 初期化
	void Initialize(Model* modelPlayer);

	// 更新
	void Update();
	// 左スティックで移動 & 移動している方向へ向ける
	void Move();

	// 描画
	void Draw(ViewProjection& viewProjection);

	// デバッグ表示
	void Debug();

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
