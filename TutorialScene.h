#pragma once
#include "Player.h"

#ifdef DEBUG
#include <ImGui.h>
#endif // DEBUG

#include <memory>

class TutorialScene {

public:
	// デストラクタ
	~TutorialScene();

	// 初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Draw(ID3D12GraphicsCommandList* commandList);

	// 各チュートリアルの更新
	// 自機の操作のチュートリアル
	void MoveTutorial();
	// 敵を倒すチュートリアル
	void EliminateEnemy();

	// 操作
	void Operation();

	// デバッグ
	void Debug();

	// アクセッサ
	bool GetIsFinished();

private: // メンバ変数

	// ビュープロジェクション
	ViewProjection viewProjection_;

	// 終了フラグ
	bool isFinished_ = false;
	// チュートリアルの時間
	int timer_ = 300;
	// 現在のチュートリアルフェーズ
	int phase_ = 0;

	///
	/// チュートリアル用のプレイヤー関連
	///

	// プレイヤー本体
	std::unique_ptr<Player> player_ = nullptr;
	// プレイヤーモデルデータ
	std::unique_ptr<Model> modelPlayer_ = nullptr;
	// モデル（プレイヤーのレーザー）
	std::unique_ptr<Model> modelPlayerLaser_ = nullptr;
};