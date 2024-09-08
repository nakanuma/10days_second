#include "GameScene.h"
#include "TextureManager.h"
#include "imgui.h"

#include <cassert>
#include <random>
#include <numbers>

GameScene::GameScene() {}

GameScene::~GameScene() {
	/* 各種開放 */
	
	///
	///	プレイヤー関連
	/// 
	
	// プレイヤーモデル
	delete modelPlayer_;
	// プレイヤー
	delete player_;

	///
	///	敵
	/// 

	// 敵モデル
	delete modelEnemy_;
	// 敵全てを開放
	for (Enemy* enemy : enemies_) {
		delete enemy;
	}

	///
	///	レーザー
	/// 
	
	delete modelLaser_;
}

void GameScene::Initialize() {
	/* 各種初期化 */

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	// ビュープロジェクション初期化
	viewProjection_.Initialize();

	///
	///	レーザー
	/// 

	// レーザーモデル生成
	modelLaser_ = Model::CreateFromOBJ("laser", true);

	///
	///	プレイヤー関連
	/// 
	
	// プレイヤーモデル生成
	modelPlayer_ = Model::CreateFromOBJ("player", true);
	// プレイヤー生成と初期化
	player_ = new Player();
	player_->Initialize(modelPlayer_, modelLaser_);

	///
	///	敵関連
	/// 
	
	// 敵モデル生成
	modelEnemy_ = Model::CreateFromOBJ("enemy", true);

	///
	///	その他
	/// 
	
	// ゲームシーン経過時間の初期化
	gameTime_ = 0;
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
	// 死んだ敵をリストから削除
	enemies_.remove_if([](Enemy* enemy) {
		if (enemy->IsDead()) {
			delete enemy;
			return true;
		}
		return false;
	});

	// 敵の自動生成を行う
	/*EnemyGeneration();*/

	///
	///	全ての衝突判定を行う
	/// 

	CheckAllCollision();

	///
	///	デバッグ情報
	/// 
	
	Debug();

	/// ゲームシーン経過時間をカウント
	gameTime_++;
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

	// 敵を出現させる
	if (ImGui::Button("EnemySpawn")) {
		Enemy* newEnemy = new Enemy();
		newEnemy->Initialize(modelEnemy_, {4.0f, 12.0f, 0.0f});

		enemies_.push_back(newEnemy);
	}

	// ゲームシーン経過時間を表示
	ImGui::Text("GameTime : %d", gameTime_);

	ImGui::End();
}

void GameScene::CheckAllCollision() {
	#pragma region プレイヤーのレーザー->敵

	// プレイヤーのレーザーが敵のサイズを増加させる量
	const float kIncrementSize = 0.01f;

	// プレイヤーのレーザーが有効である場合
	if (player_->GetLaser().IsActive()) {
		Laser& laser = player_->GetLaser();
		OBBCollider laserOBB = laser.GetOBB();

		// 全ての敵に対して衝突判定
		for (Enemy* enemy : enemies_) {
			// 敵を球体として扱う
			SphereCollider enemyCollider;
			enemyCollider.center = enemy->GetWorldPosition();
			enemyCollider.radius = enemy->GetRadius();

			// 衝突判定を行う
			if (laserOBB.IsCollision(enemyCollider)) {
				// 衝突した際の処理
				enemy->OnCollision(kIncrementSize);
			}
		}
	}

	#pragma endregion
}

void GameScene::EnemyGeneration() {
	///
	/// 乱数生成器を初期化
	/// 
	
	std::random_device rd;
	std::mt19937 rng(rd());

	///
	///	生成時の各座標について
	/// 
	
	// X座標。指定範囲の間をランダムで生成
	const float kGenerateX = 12.0f;
	std::uniform_real_distribution<float> distX(-kGenerateX, kGenerateX);
	// 指定範囲のランダムなX座標を生成
	float randomX = distX(rng);

	// Y座標。固定（全ての敵を同じY座標で生成する）
	const float kGenerateY = 12.0f;

	///
	///	生成の頻度について
	/// 

	// 敵を生成するまでの最小フレーム数と最大フレーム数
	const uint32_t minFrames = 120;
	const uint32_t maxFrames = 120;
	std::uniform_int_distribution<uint32_t> distFrame(minFrames, maxFrames);
	nextGenerationFrame_ = distFrame(rng);

	///
	///	実際に敵の生成を行う
	/// 
	
	if (gameTime_ % nextGenerationFrame_ == 0) {
		Enemy* newEnemy = new Enemy();
		newEnemy->Initialize(modelEnemy_, {randomX, kGenerateY, 0.0f});

		enemies_.push_back(newEnemy);
	}
}
