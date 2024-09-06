#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>

GameScene::GameScene() {}

GameScene::~GameScene() {
	/* 各種開放 */
	
	///
	///	プレイヤー関連
	/// 
	
	// プレイヤー弾モデル
	delete modelPlayerBullet_;
	// プレイヤーモデル
	delete modelPlayer_;
	// プレイヤー
	delete player_;

}

void GameScene::Initialize() {
	/* 各種初期化 */

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	// ビュープロジェクション初期化
	viewProjection_.Initialize();

	///
	///	プレイヤー関連
	/// 
	
	// プレイヤーモデル生成
	modelPlayer_ = Model::CreateFromOBJ("player", true);
	// プレイヤー弾モデル生成
	modelPlayerBullet_ = Model::CreateFromOBJ("playerBullet", true);
	// プレイヤー生成と初期化
	player_ = new Player();
	player_->Initialize(modelPlayer_, modelPlayerBullet_);

	///
	///	線の初期化（最初から存在している外側の線4つについて）
	///		
	
	InitializeLine();
}

void GameScene::Update() {
	///
	///	プレイヤー更新
	/// 
	
	player_->Update();
}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>

	///
	///	プレイヤー描画
	/// 
	
	player_->Draw(viewProjection_);

	///
	///	登録されている線全ての描画（最後に描画する必要あり）
	/// 

	for (Line& line : lines_) {
		line.Draw();
	}

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

void GameScene::InitializeLine() {
	// 最初から存在している4つの線を初期化して登録

	// 外側の線の範囲
	const float kOutsizeLineRange = 16.0f;

	// 上の線
	{ 
		Vector3 startUpLine = {-kOutsizeLineRange, kOutsizeLineRange, 0.0f}; // 左上
		Vector3 endUpLine = {kOutsizeLineRange, kOutsizeLineRange, 0.0f};    // 右上

		Line newLine;
		newLine.Initialize(startUpLine, endUpLine, &viewProjection_);
		// lines_に追加
		lines_.push_back(newLine);
	}
	// 右の線
	{
		Vector3 startRightLine = {kOutsizeLineRange, kOutsizeLineRange, 0.0f}; // 右上
		Vector3 endRightLine = {kOutsizeLineRange, -kOutsizeLineRange, 0.0f};  // 右下

		Line newLine;
		newLine.Initialize(startRightLine, endRightLine, &viewProjection_);
		// lines_に追加
		lines_.push_back(newLine);
	}
	// 下の線
	{
		Vector3 startRightLine = {kOutsizeLineRange, -kOutsizeLineRange, 0.0f}; // 右下
		Vector3 endRightLine = {-kOutsizeLineRange, -kOutsizeLineRange, 0.0f};  // 左下

		Line newLine;
		newLine.Initialize(startRightLine, endRightLine, &viewProjection_);
		// lines_に追加
		lines_.push_back(newLine);
	}
	// 左の線
	{
		Vector3 startRightLine = {-kOutsizeLineRange, -kOutsizeLineRange, 0.0f}; // 左下
		Vector3 endRightLine = {-kOutsizeLineRange, kOutsizeLineRange, 0.0f};  // 左上

		Line newLine;
		newLine.Initialize(startRightLine, endRightLine, &viewProjection_);
		// lines_に追加
		lines_.push_back(newLine);
	}
}
