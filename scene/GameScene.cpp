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
	// 敵出現マークのモデル
	delete modelEnemyAppearMark_;
	// 敵出現マーク全てを開放
	for (EnemyAppearMark* mark : enemyAppearMarks_) {
		delete mark;
	}

	///
	///	レーザー
	/// 
	
	delete modelLaser_;
	delete modelPlayerLaser_;

	///
	///	スプライト
	/// 
	
	delete spriteScreenLeft_;
	delete spriteScreenRight_;
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
	// レーザーモデル（プレイヤー）生成
	modelPlayerLaser_ = Model::CreateFromOBJ("playerLaser", true);

	///
	///	プレイヤー関連
	/// 
	
	// プレイヤーモデル生成
	modelPlayer_ = Model::CreateFromOBJ("player", true);
	// プレイヤー生成と初期化
	player_ = new Player();
	player_->Initialize(modelPlayer_, modelPlayerLaser_);

	///
	///	敵関連
	/// 
	
	// 敵モデル生成
	modelEnemy_ = Model::CreateFromOBJ("enemy", true);
	// 敵出現マークのモデル生成
	modelEnemyAppearMark_ = Model::CreateFromOBJ("enemyAppearMark", true);

	///
	///	スプライト生成
	/// 

	uint32_t textureWhite1x1 = TextureManager::Load("white1x1.png");
	/*ゲーム領域ではない画面両側を隠すスプライト*/
	spriteScreenLeft_ = Sprite::Create(textureWhite1x1, {0.0f, 0.0f}, {1.0f, 0.5f, 0.0f, 1.0f});
	spriteScreenLeft_->SetSize({360.0f, 720.0f});
	spriteScreenRight_ = Sprite::Create(textureWhite1x1, {920.0f, 0.0f}, {1.0f, 0.5f, 0.0f, 1.0f});
	spriteScreenRight_->SetSize({360.0f, 720.0f});

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
	///	敵関連の更新
	/// 

	// 敵全て
	for (Enemy* enemy : enemies_) {
		enemy->Update();

		// 空中にいる敵が死んだ際、プレイヤーにスコアを100与える
		if (enemy->IsDead() && !enemy->HasReachedBottom()) {
			player_->AddScore(100);
		}

	}
	// 死んだ敵をリストから削除
	enemies_.remove_if([](Enemy* enemy) {
		if (enemy->IsDead()) {
			delete enemy;
			return true;
		}
		return false;
	});

	// 敵出現マーク全て
	for (EnemyAppearMark* mark : enemyAppearMarks_) {
		mark->Update();
	}
	// 死んだマークをリストから削除
	enemyAppearMarks_.remove_if([](EnemyAppearMark* mark) {
		if (mark->IsDead()) {
			delete mark;
			return true;
		}
		return false;
	});

	// 敵の自動生成を行う
	EnemyGeneration();

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
	///	敵関連の描画
	/// 
	
	// 敵全て
	for (Enemy* enemy : enemies_) {
		// レーザーの描画
		enemy->DrawLaser(viewProjection_);
		// 本体の描画
		enemy->Draw(viewProjection_);
	}
	// 敵の出現マーク全て
	for (EnemyAppearMark* mark : enemyAppearMarks_) {
		mark->Draw(viewProjection_);
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

	///
	///	ゲーム領域ではない画面両側を隠すスプライトの描画（これが一番手前にあるべき）
	/// 

	// 左側
	spriteScreenLeft_->Draw();
	// 右側
	spriteScreenRight_->Draw();

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

void GameScene::Debug() { 
	ImGui::Begin("GameScene"); 

	//// 敵を出現させる
	//if (ImGui::Button("EnemySpawn")) {
	//	Enemy* newEnemy = new Enemy();
	//	newEnemy->Initialize(modelEnemy_, modelLaser_, {4.0f, 12.0f, 0.0f});

	//	enemies_.push_back(newEnemy);
	//}

	// ゲームシーン経過時間を表示
	ImGui::Text("GameTime : %d", gameTime_);

	ImGui::End();
}

void GameScene::CheckAllCollision() {
	#pragma region プレイヤーのレーザー->敵（OBB->Sphere）

	// プレイヤーのレーザーが敵のサイズを増加させる量（要調整。今は定数にしてるけどあとでいじれるようにしたい）
	const float kIncrementSize = 0.01f;

	// プレイヤーのレーザーが有効である場合
	if (player_->GetLaser().IsActive()) {
		Laser& laser = player_->GetLaser();
		// レーザーをOBBとして扱う
		OBBCollider laserOBB = laser.GetOBB();

		// 全ての敵に対して衝突判定
		for (Enemy* enemy : enemies_) {
			// 敵を球体として扱う
			SphereCollider enemyCollider;
			enemyCollider.center = enemy->GetWorldPosition();
			enemyCollider.radius = enemy->GetRadius();

			// 衝突判定を行う
			if (laserOBB.IsCollision(enemyCollider)) {
				// 敵が地面にいないときのみ衝突した際の処理を呼ぶ（地面にいる敵を大きくしないようにするため）
				if (!enemy->HasReachedBottom()) {
					enemy->OnCollision(kIncrementSize);
				}
			}
		}
	}

	#pragma endregion

	#pragma region 敵のレーザー->敵（OBB->Sphere）

	// 敵のレーザーが敵のサイズを増加させる量（要調整。今は定数にしてるけどあとでいじれるようにしたい）
	const float kEnemyIncrementSize = 0.008f;

	// 各敵のレーザーについて処理を行う
	for (Enemy* shooterEnemy : enemies_) {
		if (shooterEnemy->GetLaser().IsActive()) {
			Laser& laser = shooterEnemy->GetLaser();
			// レーザーをOBBとして扱う
			OBBCollider laserOBB = laser.GetOBB();

			// 自分自身のレーザーとの衝突は無視
			for (Enemy* targetEnemy : enemies_) {
				if (shooterEnemy != targetEnemy) {
					// 敵を球体として扱う
					SphereCollider enemyColider;
					enemyColider.center = targetEnemy->GetWorldPosition();
					enemyColider.radius = targetEnemy->GetRadius();

					// 衝突判定を行う
					if (laserOBB.IsCollision(enemyColider)) {
						// 衝突した敵のサイズを増加させる（地面にいる敵には当たらないようにする）
						if (!targetEnemy->HasReachedBottom()) {
							targetEnemy->OnCollision(kEnemyIncrementSize);
						}
					}
				}
			}
		}
	}

	#pragma endregion

	#pragma region 敵のレーザー->プレイヤー（OBB->Sphere）

	// 各敵のレーザーについて処理を行う
	for (Enemy* shooterEnemy : enemies_) {
		if (shooterEnemy->GetLaser().IsActive()) {
			Laser& laser = shooterEnemy->GetLaser();
			// レーザーをOBBとして扱う
			OBBCollider laserOBB = laser.GetOBB();
			
			// プレイヤーを球体として扱う
			SphereCollider playerCollider;
			playerCollider.center = player_->GetWorldPosition();
			playerCollider.radius = player_->GetRadius();

			// 衝突判定を行う
			if (laserOBB.IsCollision(playerCollider)) {
				// プレイヤーの衝突時コールバックを呼び出す
				player_->OnCollision();
			}
		}
	}

	#pragma endregion

	#pragma region 敵本体->プレイヤー（Sphere->Sphere）

	// プレイヤーのコライダーを設定
	SphereCollider playerCollider;
	playerCollider.center = player_->GetWorldPosition();
	playerCollider.radius = player_->GetRadius();

	// 全ての敵に対して衝突判定
	for (Enemy* enemy : enemies_) {
		// 敵のコライダーを設定
		SphereCollider enemyCollider;
		enemyCollider.center = enemy->GetWorldPosition();
		enemyCollider.radius = enemy->GetRadius();

		// 衝突判定を行う
		if (playerCollider.IsCollision(enemyCollider)) {
			// プレイヤーの衝突時コールバックを呼び出す
			player_->OnCollision();
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
	const float kGenerateY = 28.0f;

	///
	///	生成時の半径について
	/// 

	// 敵の半径を決定するための確率
	std::uniform_real_distribution<float> distProbability(0.0f, 1.0f);
	float randomProbability = distProbability(rng); // 0.0f ~ 1.0fの間で生成される

	float randomRadius;

	// 20％の確率で大きい敵（半径2.0f ~ 2.5f）が生成
	if (randomProbability < 0.2f) {
		std::uniform_real_distribution<float> distRadiusLarge(2.0f, 2.5f);
		randomRadius = distRadiusLarge(rng);
	// 80％の確率で小さい敵（半径1.0f ~ 1.5f）が生成
	} else {
		std::uniform_real_distribution<float> distRadiusSmall(1.0f, 1.5f);
		randomRadius = distRadiusSmall(rng);
	}

	///
	///	生成の頻度について
	/// 

	// 敵を生成するまでの最小フレーム数と最大フレーム数
	const uint32_t minFrames = 180;
	const uint32_t maxFrames = 180;
	std::uniform_int_distribution<uint32_t> distFrame(minFrames, maxFrames);
	nextGenerationFrame_ = distFrame(rng);

	///
	///	実際に敵の生成を行う & 敵出現マークの生成も行う
	/// 
	
	// 敵出現マークのY座標（一律で固定）
	const float kEnemyAppearMarkY = 14.0f;

	if (gameTime_ % nextGenerationFrame_ == 0) {
		Enemy* newEnemy = new Enemy();
		newEnemy->Initialize(modelEnemy_, modelLaser_, {randomX, kGenerateY, 0.0f}, randomRadius);

		enemies_.push_back(newEnemy);

		///
		///	敵出現マークの生成
		///		
		
		EnemyAppearMark* newMark = new EnemyAppearMark();
		newMark->Initialize(modelEnemyAppearMark_, {randomX, kEnemyAppearMarkY, 0.0f}); // 敵と同じX座標を指定

		enemyAppearMarks_.push_back(newMark);
	}
}
