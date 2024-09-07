#include "GameScene.h"
#include "TextureManager.h"
#include "imgui.h"

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

	///
	///	敵関連
	/// 

	// 敵モデル開放
	delete modelEnemy_;
	// 敵リスト開放
	for (Enemy* enemy : enemies_) {
		delete enemy;
	}

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
	///	敵関連
	/// 

	// 敵モデル生成
	modelEnemy_ = Model::CreateFromOBJ("enemy", true);

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

	///
	///	敵全ての更新
	/// 
	
	for (Enemy* enemy : enemies_) {
		enemy->Update();
	}

	///
	///	デバッグ情報
	/// 
	
	Debug();
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
	///	敵全ての描画
	/// 

	for (Enemy* enemy : enemies_) {
		enemy->Draw(viewProjection_);
	}

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

void GameScene::Debug() { 
	ImGui::Begin("GameScene"); 

	// デバッグ用で、ボタンを押したら敵をスポーンさせる
	if (ImGui::Button("EnemySpawn")) {
		// 生成と初期化
		Enemy* newEnemy = new Enemy();
		newEnemy->Initialize(modelEnemy_, kOutsideFrameTopLeftPos); // 左上からスポーンさせる
		// リストに登録
		enemies_.push_back(newEnemy);
	}

	ImGui::End();

	ImGui::Begin("Enemy");

	// 敵の番号と座標を表示
	uint32_t enemyIndex = 0;
	for (Enemy* enemy : enemies_) {
		ImGui::Text("Enemy[%d] translation:(%.2f, %.2f, %.2f)", enemyIndex, enemy->GetWorldPosition().x, enemy->GetWorldPosition().y, enemy->GetWorldPosition().z);
		enemyIndex++;
	}

	ImGui::End();
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
		Vector3 startDownLine = {kOutsizeLineRange, -kOutsizeLineRange, 0.0f}; // 右下
		Vector3 endDownLine = {-kOutsizeLineRange, -kOutsizeLineRange, 0.0f};  // 左下

		Line newLine;
		newLine.Initialize(startDownLine, endDownLine, &viewProjection_);
		// lines_に追加
		lines_.push_back(newLine);
	}
	// 左の線
	{
		Vector3 startLeftLine = {-kOutsizeLineRange, -kOutsizeLineRange, 0.0f}; // 左下
		Vector3 endLeftLine = {-kOutsizeLineRange, kOutsizeLineRange, 0.0f};    // 左上

		Line newLine;
		newLine.Initialize(startLeftLine, endLeftLine, &viewProjection_);
		// lines_に追加
		lines_.push_back(newLine);
	}
}
