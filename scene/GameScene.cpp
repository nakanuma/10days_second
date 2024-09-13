#include "GameScene.h"
#include "TextureManager.h"
#include "imgui.h"

#include <cassert>
#include <numbers>
#include <random>

// MyClass
#include "Easing.h"

GameScene::GameScene() {
	srand(static_cast<unsigned>(time(0))); // 乱数の初期化
}

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

	// パーティクルをすべて開放
	for (Particle* particle : particles_) {
		delete particle;
	}

	///
	///	レーザー
	///

	delete modelLaser_;
	delete modelPlayerLaser_;

	///
	///	パーティクル用モデル
	///

	delete modelParticle_;

	///
	///	スプライト
	///

	delete spriteWaveNum_;

	delete spriteBackGround_;

	delete spriteScreenLeft_;
	delete spriteScreenRight_;

	delete spriteResultBackGround_;
	delete spriteResultText_;

	/* スコア（6桁分） */
	for (int32_t i = 0; i < kMaxScoreDigit; i++) {
		delete spriteScore_[i];
	}

	/* 空っぽの星 */
	for (int32_t i = 0; i < kMaxStarNum; i++) {
		delete spriteEmptyStar_[i];
	}

	/* 星 */
	for (int32_t i = 0; i < kMaxStarNum; i++) {
		delete spriteFullStar_[i];
	}

	/* 残り時間 */
	for (int32_t i = 0; i < 2; i++) {
		delete spriteRemainingTime_[i];
	}

	delete spriteRemainingTimeText_;

	delete spriteControl_;
	delete spriteBackText_;
}

void GameScene::Initialize() {
	/* 各種初期化 */

	dxCommon_ = DirectXCommon::GetInstance(); 
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	// ビュープロジェクション初期化
	viewProjection_.Initialize();

	// ウェーブ管理
	wave_ = std::make_unique<Wave>();

	///
	///	レーザー
	///

	// レーザーモデル（敵用）生成
	modelLaser_ = Model::CreateFromOBJ("enemy_laser", true);
	// レーザーモデル（プレイヤー用）生成
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
	modelEnemy_ = Model::CreateFromOBJ("Enemy_2", true);
	// 敵出現マークのモデル生成
	modelEnemyAppearMark_ = Model::CreateFromOBJ("danger", true);

	///
	///	スプライト生成
	///
	uint32_t textureWhite1x1 = TextureManager::Load("./Resources/images/title/back_white.png");

	/* 各ウェーブ開始時に出てくるウェーブの文字（1, 2, 3のテクスチャハンドルをセットして使い回す）*/
	wave1TextureHandle_ = TextureManager::Load("images/WAVE1.png");
	wave2TextureHandle_ = TextureManager::Load("images/WAVE2.png");
	wave3TextureHandle_ = TextureManager::Load("images/WAVE3.png");

	spriteWaveNum_ = Sprite::Create(wave1TextureHandle_, {0.0f, 0.0f});
	spriteWaveNum_->SetAnchorPoint({0.5f, 0.5f}); // アンカーポイントを中心に設定

	/* 背景 */
	spriteBackGround_ = Sprite::Create(textureWhite1x1, {0.0f, 0.0f}, {0.5f, 0.5f, 0.5f, 1.0f});
	spriteBackGround_->SetSize({1280.0f, 720.0f});

	/* ゲーム領域ではない画面両側を隠すスプライト */
	spriteScreenLeft_ = Sprite::Create(textureWhite1x1, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f});
	spriteScreenLeft_->SetSize({350.0f, 720.0f});
	spriteScreenRight_ = Sprite::Create(textureWhite1x1, {930.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f});
	spriteScreenRight_->SetSize({350.0f, 720.0f});

	// 画面右に表示する操作説明
	uint32_t textureControl = TextureManager::Load("images/control.png");
	spriteControl_ = Sprite::Create(textureControl, {965.0f, 0.0f});
	// Xボタンでタイトルへ
	uint32_t textureTitleBack = TextureManager::Load("images/select_titleBack.png");
	spriteBackText_ = Sprite::Create(textureTitleBack, {955.0f, 650.0f});

	/* リザルト関連 */

	// リザルトの最も後ろの背景（拡縮するやつ）
	uint32_t textureResultBackGround = TextureManager::Load("images/clear_back.png");
	spriteResultBackGround_ = Sprite::Create(textureResultBackGround, {640.0f, 360.0f}, {1.0f, 1.0f, 1.0f, 1.0f});
	spriteResultBackGround_->SetAnchorPoint({0.5f, 0.5f}); // アンカーポイントを中心に設定
	spriteResultBackGround_->SetSize({0.0f, 520.0f});      // 最初はXのサイズ0で生成。最大サイズは480

	// 「リザルト」文字
	uint32_t textureResultText = TextureManager::Load("images/clear.png");
	spriteResultText_ = Sprite::Create(textureResultText, {640.0f, 180.0f});
	spriteResultText_->SetAnchorPoint({0.5f, 0.5f}); // アンカーポイントを中心に設定

	// スコア（6桁分）
	textureNumber_ = TextureManager::Load("images/number.png");
	for (int32_t i = 0; i < kMaxScoreDigit; i++) {
		spriteScore_[i] = Sprite::Create(textureNumber_, {0.0f, 0.0f});
		spriteScore_[i]->SetSize({21.0f, 30.0f}); // 数字1文字分のサイズを指定
	}

	// 空っぽの星（3つ）
	uint32_t textureEmptyStar = TextureManager::Load("images/star_empty.png");
	for (int32_t i = 0; i < kMaxStarNum; i++) {
		spriteEmptyStar_[i] = Sprite::Create(textureEmptyStar, {0.0f, 0.0f});
		spriteEmptyStar_[i]->SetAnchorPoint({0.5f, 0.5f}); // アンカーポイントを中心に設定
	}

	// 星（3つ）
	uint32_t textureFullStar = TextureManager::Load("images/star_full.png");
	for (int32_t i = 0; i < kMaxStarNum; i++) {
		spriteFullStar_[i] = Sprite::Create(textureFullStar, {0.0f, 0.0f});
		spriteFullStar_[i]->SetAnchorPoint({0.5f, 0.5f}); // アンカーポイントを中心に設定
	}

	// "残り・・・秒"と書いてあるスプライト
	uint32_t textureRemainingText = TextureManager::Load("images/game_time_nokori.png");
	spriteRemainingTimeText_ = Sprite::Create(textureRemainingText, {175.0f, 360.0f}); // 左側の領域でのど真ん中
	spriteRemainingTimeText_->SetAnchorPoint({0.5f, 0.5f});

	// 残り時間（2桁分）
	for (int32_t i = 0; i < 2; i++) {
		spriteRemainingTime_[i] = Sprite::Create(textureNumber_, {0.0f, 0.0f});
		spriteRemainingTime_[i]->SetSize({50.0f, 65.0f}); // 1文字分のサイズ
	}

	///
	///	パーティクル
	///

	modelParticle_ = Model::CreateFromOBJ("particle", true);
	enemyDeadEmitter_.Initialize(modelParticle_);

	///
	///	サウンド生成
	///

	// クリアサウンド
	clearSH_ = audio_->LoadWave("./Resources/sounds/SE_clear.wav");
	// ゲームオーバーサウンド
	gameoverSH_ = audio_->LoadWave("./Resources/sounds/SE_gameover.wav");

	///
	///	その他
	///

	// 初期ウェーブ
	int wave = Wave::GetInstance().GetWave();

	// ウェーブの値によって開始ウェーブが異なる
	if (wave == 1) {
		// ウェーブ1の開始時間をセット
		gameTime_ = SecToFrame(0);
	} else if (wave == 2) {
		// ウェーブ2の開始時間をセット
		gameTime_ = SecToFrame(50);
	} else if (wave == 3) {
		// ウェーブの開始時間をセット
		gameTime_ = SecToFrame(100);
	} else {
		gameTime_ = SecToFrame(0);
	}

	///
	/// 画面遷移(フェード)関連
	///

	fade_ = std::make_unique<Fade>();

	fade_->Initialize();

	fade_->Start(Fade::Status::FadeIn, fadeTimer_);
}

void GameScene::Update() {

	/*WAVE1の場合*/
	uint32_t w1s1score = 200;  // このスコアに達してたら星1
	uint32_t w1s2score = 600;  // このスコアに達してたら星2
	uint32_t w1s3score = 1000; // このスコアに達してたら星3

	/*WAVE2の場合*/
	uint32_t w2s1score = 200;  // このスコアに達してたら星1
	uint32_t w2s2score = 600;  // このスコアに達してたら星2
	uint32_t w2s3score = 1000; // このスコアに達してたら星3

	/*WAVE3の場合*/
	uint32_t w3s1score = 200;  // このスコアに達してたら星1
	uint32_t w3s2score = 600;  // このスコアに達してたら星2
	uint32_t w3s3score = 1000; // このスコアに達してたら星3

	switch (phase_) {
	case Phase::kFadeIn:
		if (fade_->IsFinished()) {
			phase_ = Phase::kMain;
		}
		///
		///	プレイヤー更新
		///

		player_->Update();
		break;
	case Phase::kMain:
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
			player_->AddScore(200);
		}

		// 敵死亡時パーティクルの生成
		if (enemy->IsDead()) {
			enemyDeadEmitter_.Emit(enemy->GetWorldPosition());
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

		// パーティクルの更新
		for (Particle* particle : particles_) {
			particle->Update();
		}

		///
		/// プレイヤーの生存確認
		///

		CheckPlayerAlive();

		///
		/// パーティクルの自動生成
		///
		///

		ParticleGeneration();

		///
		/// ゲームシーン全ての流れの処理
		///

		GameSceneFlow();

		///
		///	残り時間の管理（各WAVE開始時にそのWAVEの残り時間(f)を指定し、左側で表示するために使用）
		///

		// 残り時間が存在している場合、減らしていく
		if (remainingTime_ > 0) {
			remainingTime_--;
			// 残り時間が無くなった場合、0で固定
		} else {
			remainingTime_ = 0;
		}

		///
		///	各WAVEリザルト表示時、現在スコアに応じて表示する星の数を設定
		///

		/*WAVE1の場合*/
		// uint32_t w1s1score = 200;  // このスコアに達してたら星1
		// uint32_t w1s2score = 600;  // このスコアに達してたら星2
		// uint32_t w1s3score = 1000; // このスコアに達してたら星3

		if (gameTime_ >= SecToFrame(45) && gameTime_ <= SecToFrame(50)) {
			// 星1個の場合
			if (player_->GetScore() >= w1s1score && player_->GetScore() < w1s2score) {
				displayStarNum_ = 1;
			}

			// 星2個の場合
			if (player_->GetScore() >= w1s2score && player_->GetScore() < w1s3score) {
				displayStarNum_ = 2;
			}

			// 星3個の場合
			if (player_->GetScore() >= w1s3score) {
				displayStarNum_ = 3;
			}
		}

		/*WAVE2の場合*/
		// uint32_t w2s1score = 200;  // このスコアに達してたら星1
		// uint32_t w2s2score = 600;  // このスコアに達してたら星2
		// uint32_t w2s3score = 1000; // このスコアに達してたら星3
		if (gameTime_ >= SecToFrame(95) && gameTime_ <= SecToFrame(100)) {
			// 星1個の場合
			if (player_->GetScore() >= w2s1score && player_->GetScore() < w2s2score) {
				displayStarNum_ = 1;
			}

			// 星2個の場合
			if (player_->GetScore() >= w2s2score && player_->GetScore() < w2s3score) {
				displayStarNum_ = 2;
			}

			// 星3個の場合
			if (player_->GetScore() >= w2s3score) {
				displayStarNum_ = 3;
			}
		}

		if (gameTime_ >= SecToFrame(145) && gameTime_ <= SecToFrame(150)) {
			// 星1個の場合
			if (player_->GetScore() >= w3s1score && player_->GetScore() < w3s2score) {
				displayStarNum_ = 1;
			}

			// 星2個の場合
			if (player_->GetScore() >= w3s2score && player_->GetScore() < w3s3score) {
				displayStarNum_ = 2;
			}

			// 星3個の場合
			if (player_->GetScore() >= w3s3score) {
				displayStarNum_ = 3;
			}
		}

	///
	///	シェイクを行う
	///

	// ゴリ押しでプレイヤー被弾時に一回だけシェイクを行う
	if (player_->IsInvincible() && player_->GetInvincibleCount() == 118) {
		CameraShake(2.0f, 180);
	}

	if (shakeTimer_ == shakeDuration_) {
		originalPosition_.x = viewProjection_.translation_.x;
		originalPosition_.y = viewProjection_.translation_.y;
	}

	if (shakeTimer_ > 0) {
		// ランダムにXとYに値を加算
		float offsetX = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * shakeAmountX_;
		float offsetY = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * shakeAmountY_;

		viewProjection_.translation_.x = originalPosition_.x + offsetX;
		viewProjection_.translation_.y = originalPosition_.y + offsetY;

		// シェイク効果を徐々に減少させる（イージングを適用）
		float t = static_cast<float>(shakeTimer_) / static_cast<float>(shakeDuration_);
		float easeFactor = t * t * (3.0f - 2.0f * t); // スムーズステップ (SmoothStep)
		shakeAmountX_ *= easeFactor;
		shakeAmountY_ *= easeFactor;

		// タイマーを更新
		shakeTimer_--;
	} else {
		// シェイクが終了したら徐々に元の位置に戻る
		viewProjection_.translation_.x = originalPosition_.x;
		viewProjection_.translation_.y = originalPosition_.y;
	}
	viewProjection_.UpdateMatrix();

	///
	///	プレイヤーの体力が0になったらフェードしてセレクトへ戻る
	///

		if (player_->GetHP() <= 0) {
			// フェードしてセレクト画面へ
		}

		///
		///	全ての衝突判定を行う
		///

		CheckAllCollision();

		/// ゲームシーン経過時間をカウント
		gameTime_++;

		break;
	case Phase::kDeath:
		// デスパーティクル存在時のみ更新
		if (deathParticles_) {
			deathParticles_->Update();
		}
		// デスパーティクル消滅後画面遷移開始
		if (deathParticles_->IsFinished()) {
			phase_ = Phase::kFadeOut;

			// フェードアウト開始
			fade_->Start(Fade::Status::FadeOut, fadeTimer_);
		}
		break;
	case Phase::kFadeOut:

		if (fade_->IsFinished()) {
			isFinished_ = true;
		}
		if (!isPlayGameoverSH_) {
			audio_->PlayWave(gameoverSH_);
			isPlayGameoverSH_ = true;
		}
		break;
	}

	// フェードの更新
	fade_->Update();

	///
	///	パーティクル関連更新
	///

	// 敵死亡時パーティクル更新
	enemyDeadEmitter_.Update();

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

	/* ゲーム背景の描画 */
	spriteBackGround_->Draw();
	// パーティクルの描画
	for (Particle* particle : particles_) {
		particle->Draw();
	}

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

	if (player_->IsAlive()) {
		player_->Draw(viewProjection_);
	}

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

	///
	///	パーティクル関連
	///

	// 敵死亡時パーティクル描画
	enemyDeadEmitter_.Draw(viewProjection_);

	if (!player_->IsAlive()) {
		deathParticles_->Draw();
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
	///	各ウェーブ開始時に、右から左へ出てくるウェーブ数の書いたスプライトの描画
	///

	spriteWaveNum_->Draw();

	///
	///	リザルト関連の描画
	///

	// リザルト表示時の最も後ろのスプライト描画（リザルト表示の最も奥に配置する必要）
	spriteResultBackGround_->Draw();

	// リザルトの最も後ろのスプライトが最大横幅になるまでのフレーム
	const int32_t adjust = 30;
	// リザルト表示のタイミングかつ、リザルト背景が最大になっている間のみ描画
	if (gameTime_ >= SecToFrame(45) + adjust && gameTime_ <= SecToFrame(50) - adjust || gameTime_ >= SecToFrame(95) + adjust && gameTime_ <= SecToFrame(100) - adjust ||
	    gameTime_ >= SecToFrame(145) + adjust && gameTime_ <= SecToFrame(150) - adjust) {

		/* "CLAER"と書かれた文字を描画 */
		spriteResultText_->Draw();

		/* そのWAVEで取得したスコアを描画 */
		DrawScoreToResult();

		/* 空っぽの星を描画 */
		for (int32_t i = 0; i < kMaxStarNum; i++) {
			// 位置を計算
			Vector2 position = Vector2{640.0f - 120.0f, 330.0f} + Vector2{static_cast<float>(i) * 120.0f, 0.0f}; // アンカーポイントが中心なのでいい感じになるように
			spriteEmptyStar_[i]->SetPosition(position);
			spriteEmptyStar_[i]->Draw();
		}

		/* 実際の星を描画 */
		for (int32_t i = 0; i < displayStarNum_; i++) {
			// 位置を計算
			Vector2 position = Vector2{640.0f - 120.0f, 330.0f} + Vector2{static_cast<float>(i) * 120.0f, 0.0f}; // アンカーポイントが中心なのでいい感じになるように
			spriteFullStar_[i]->SetPosition(position);
			spriteFullStar_[i]->Draw();
		}
	}

	///
	///	ゲーム領域ではない画面両側を隠すスプライトの描画
	///

	// 左側
	spriteScreenLeft_->Draw();
	// 右側
	spriteScreenRight_->Draw();

	// 操作説明を描画
	spriteControl_->Draw();
	// Xボタンでタイトルへ
	spriteBackText_->Draw();

	///
	///	残り時間の描画について
	///

	// "残り・・・秒"と書いてあるスプライト描画（これは常に描画）
	spriteRemainingTimeText_->Draw();

	// 動的に残り時間を管理
	int32_t timeDigit[2];                   //
	int32_t timeTemp = remainingTime_ / 60; // 残り時間を、フレーム->秒に変換

	// timeDigitに現在残り時間を格納
	for (int32_t i = 0; i < 2; i++) {
		timeDigit[i] = timeTemp % 10;
		timeTemp /= 10;
	}

	// 残り時間の表示位置
	Vector2 remainingTimePosition = {140.0f, 328.0f}; // "残り・・・秒"のスプライトの上にいい感じに配置

	// 残り時間を2桁表示
	for (int32_t i = 0; i < 2; i++) {
		// 各桁の位置を計算
		Vector2 timePosition = remainingTimePosition + Vector2{static_cast<float>(i) * (50.0f), 0.0f};
		// 読み込んだテクスチャから適切な数字を抜き取る
		spriteRemainingTime_[i]->SetTextureRect({0.0f + (timeDigit[1 - i] * 50.0f), 0.0f}, {50.0f, 65.0f});
		spriteRemainingTime_[i]->SetPosition(timePosition);
		spriteRemainingTime_[i]->Draw();
	}

	///
	///	プレイヤーのUIを描画
	///

	player_->DrawUI();

	///
	/// フェードの描画
	///

	fade_->Draw();

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

void GameScene::Debug() {
#ifdef _DEBUG

	ImGui::Begin("GameScene");

	//// 敵を出現させる
	// if (ImGui::Button("EnemySpawn")) {
	//	Enemy* newEnemy = new Enemy();
	//	newEnemy->Initialize(modelEnemy_, modelLaser_, {4.0f, 12.0f, 0.0f});

	//	enemies_.push_back(newEnemy);
	//}

	// ゲームシーン経過時間を表示
	ImGui::DragInt("GameTime", &gameTime_);

	if (ImGui::Button("AddScore")) {
		player_->AddScore(200);
	}

	// ビュープロジェクションの表示
	ImGui::DragFloat3("viewProjection.translation", &viewProjection_.translation_.x, 0.01f);
	viewProjection_.UpdateMatrix();

	ImGui::DragFloat("amountX", &shakeAmountX_);
	ImGui::DragFloat("amountY", &shakeAmountY_);
	ImGui::DragInt("duration", &shakeDuration_);
	ImGui::DragInt("timer", &shakeTimer_);
	ImGui::Text("isShake : %d", isShake_);

	ImGui::End();

#endif
}

void GameScene::CheckPlayerAlive() {

	// if (!player_->IsAlive()) {
	//	// フェード切り替え
	//	phase_ = Phase::kFadeOut;
	//	fade_->Start(Fade::Status::FadeOut, fadeTimer_);
	// }

	if (!player_->IsAlive()) {
		// 死亡演出フェーズに切り替え
		phase_ = Phase::kDeath;
		// 自キャラの座標を取得
		const Vector3& deathParticlesPosition = player_->GetWorldPosition();
		// デスパーティクルの生成
		deathParticles_ = std::make_unique<DeathParticles>();
		// 自キャラの3Dモデルの生成
		modelDeathParticle_.reset(Model::CreateFromOBJ("deathParticle", true));
		// デスパーティクルの初期化
		deathParticles_->Initialize(modelDeathParticle_.get(), &viewProjection_, deathParticlesPosition);
	}
}

void GameScene::CheckAllCollision() {
#pragma region プレイヤーのレーザー->敵（OBB->Sphere）

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
					enemy->OnCollision(playerAttackDamage_);
				}
			}
		}
	}

#pragma endregion

#pragma region 敵のレーザー->敵（OBB->Sphere）

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
							targetEnemy->OnCollision(enemyAttackDamage_);
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
	/*const uint32_t minFrames = 180;
	const uint32_t maxFrames = 180;
	std::uniform_int_distribution<uint32_t> distFrame(minFrames, maxFrames);
	nextGenerationFrame_ = distFrame(rng);*/

	nextGenerationFrame_ = enemySpawnRate_;

	///
	///	実際に敵の生成を行う & 敵出現マークの生成も行う
	///

	// 敵出現マークのY座標（一律で固定）
	const float kEnemyAppearMarkY = 18.0f;

	if (gameTime_ % nextGenerationFrame_ == 0) {
		Enemy* newEnemy = new Enemy();
		newEnemy->Initialize(modelEnemy_, modelLaser_, {randomX, kGenerateY, 0.0f}, randomRadius, enemyFallSpeed_);

		enemies_.push_back(newEnemy);

		///
		///	敵出現マークの生成
		///

		EnemyAppearMark* newMark = new EnemyAppearMark();
		newMark->Initialize(modelEnemyAppearMark_, {randomX, kEnemyAppearMarkY, 0.0f}); // 敵と同じX座標を指定

		enemyAppearMarks_.push_back(newMark);
	}
}

void GameScene::ParticleGeneration() {

	// フレームカウンタを保持する静的変数
	static int frameCounter = 0;

	// フレームカウンタを増加
	frameCounter++;

	// 5フレームごとにパーティクルを生成
	if (frameCounter % 20 == 0) {
		///
		/// パーティクル生成のX座標ランダム化
		///
		std::random_device rd;
		std::mt19937 rng(rd());

		// X座標。指定範囲の間をランダムで生成
		const float kRightGenerateX = 350;
		const float kLeftGenerateX = 930;
		std::uniform_real_distribution<float> distX(kRightGenerateX, kLeftGenerateX);
		// 指定範囲のランダムなX座標を生成
		float randomX = distX(rng);

		// Y座標。固定（800で生成）
		const float kGenerateY = 800.0f;

		// パーティクルの座標を設定
		Particle* newParticle = new Particle();
		newParticle->Initialize({randomX, kGenerateY});

		// パーティクルリストに追加
		particles_.push_back(newParticle);
	}
}

void GameScene::GameSceneFlow() {
	// ここで各項目リセット
	if (gameTime_ == SecToFrame(0)) {
		// プレイヤーのスコアをリセット
		player_->ResetScore();
		// プレイヤーのHPをリセット
		player_->ResetHP();
		// リザルトで表示する実際の星の数をリセット
		displayStarNum_ = 0;
		// WAVE1でのパラメーター設定初期化を呼ぶ
		InitializeParameterWAVE1();
	}

	///
	/// 0~5秒 : WAVE1表示スプライトを右から左へ
	///

	if (gameTime_ >= SecToFrame(0) && gameTime_ <= SecToFrame(5)) {
		WaveSpriteMove();
	}

	// 残り時間をセット
	if (gameTime_ == SecToFrame(5)) {
		// 残り時間（このWAVEで使う時間(frame))を設定
		remainingTime_ = SecToFrame(30) + SecToFrame(10); // 何もしない時間も含める（スポーン時間30秒 + 待機時間10秒）
	}

	///
	///	5~35秒 : WAVE1用の敵を30秒間生成し続ける(まだ作ってないので、あとで各WAVEの敵生成用関数を作成する）
	///

	if (gameTime_ >= SecToFrame(5) && gameTime_ <= SecToFrame(35)) {
		// 敵の自動生成を行う
		EnemyGeneration();
	}

	///
	///	35~45秒 : 何もしない（WAVE1で湧いた敵が完全に消えるくらいの想定がこのくらい）
	///

	///
	///	45~50秒 : リザルトの表示
	///

	if (gameTime_ >= SecToFrame(45) && gameTime_ <= SecToFrame(50)) {
		if (!isPlayclearSH_) {
			audio_->PlayWave(clearSH_);
			isPlayclearSH_ = true;
		}
		ShowResult();
	}

	/*------------------------------------------*/
	/*---ウェーブセレクトで2を選択したらここへ飛ばす---*/
	/*------------------------------------------*/

	// ここで各項目リセット
	if (gameTime_ == SecToFrame(50)) {
		// プレイヤーのスコアをリセット
		player_->ResetScore();
		// プレイヤーのHPをリセット
		player_->ResetHP();
		// リザルトで表示する実際の星の数をリセット
		displayStarNum_ = 0;
		// WAVE2でのパラメーター設定初期化を呼ぶ
		InitializeParameterWAVE2();
		isPlayclearSH_ = false;
	}

	///
	/// 50~55秒 : WAVE2表示スプライトを右から左へ
	///
	if (gameTime_ >= SecToFrame(50) && gameTime_ <= SecToFrame(55)) {
		spriteWaveNum_->SetTextureHandle(wave2TextureHandle_); // WAVE2のテクスチャをセット
		WaveSpriteMove();
	}

	// 残り時間をセット
	if (gameTime_ == SecToFrame(55)) {
		// 残り時間（このWAVEで使う時間(frame))を設定
		remainingTime_ = SecToFrame(30) + SecToFrame(10); // 何もしない時間も含める（スポーン時間30秒 + 待機時間10秒）
	}

	///
	///	55~85秒 : WAVE2用の敵を30秒間生成し続ける
	///

	if (gameTime_ >= SecToFrame(55) && gameTime_ <= SecToFrame(85)) {
		// 敵の自動生成を行う
		EnemyGeneration();
	}

	///
	///	85~95秒 : 何もしない（WAVE2で湧いた敵が完全に消えるくらいの想定がこのくらい）
	///

	///
	///	95~100秒 : リザルトの表示
	///

	if (gameTime_ >= SecToFrame(95) && gameTime_ <= SecToFrame(100)) {
		if (!isPlayclearSH_) {
			audio_->PlayWave(clearSH_);
			isPlayclearSH_ = true;
		}
		ShowResult();
	}

	/*------------------------------------------*/
	/*---ウェーブセレクトで3を選択したらここへ飛ばす---*/
	/*------------------------------------------*/

	// ここで各項目リセット
	if (gameTime_ == SecToFrame(100)) {
		// プレイヤーのスコアをリセット
		player_->ResetScore();
		// プレイヤーのHPをリセット
		player_->ResetHP();
		// リザルトで表示する実際の星の数をリセット
		displayStarNum_ = 0;
		// WAVE3でのパラメーター設定初期化を呼ぶ
		InitializeParameterWAVE3();
		isPlayclearSH_ = false;
	}

	///
	/// 100~105秒 : WAVE3表示スプライトを右から左へ
	///
	if (gameTime_ >= SecToFrame(100) && gameTime_ <= SecToFrame(105)) {
		spriteWaveNum_->SetTextureHandle(wave3TextureHandle_); // WAVE3のテクスチャをセット
		WaveSpriteMove();
	}

	// 残り時間をセット
	if (gameTime_ == SecToFrame(105)) {
		// 残り時間（このWAVEで使う時間(frame))を設定
		remainingTime_ = SecToFrame(30) + SecToFrame(10); // 何もしない時間も含める（スポーン時間30秒 + 待機時間10秒）
	}

	///
	///	105~135秒 : WAVE3用の敵を30秒間生成し続ける
	///

	if (gameTime_ >= SecToFrame(105) && gameTime_ <= SecToFrame(135)) {
		// 敵の自動生成を行う
		EnemyGeneration();
	}

	///
	///	135~145秒 : 何もしない（WAVE3で湧いた敵が完全に消えるくらいの想定がこのくらい）
	///

	///
	///	145~150秒 : リザルトの表示
	///

	if (gameTime_ >= SecToFrame(145) && gameTime_ <= SecToFrame(150)) {
		ShowResult();
		if (!isPlayclearSH_) {
			audio_->PlayWave(clearSH_);
			isPlayclearSH_ = true;
		}
	}

	///
	///	150秒~ : 終了
	///

	if (gameTime_ >= SecToFrame(150)) {
		// フェード切り替え
		phase_ = Phase::kFadeOut;
		fade_->Start(Fade::Status::FadeOut, fadeTimer_);
	}
}

void GameScene::WaveSpriteMove() {
	// WAVEスプライトの初期位置
	const float startX = 1280.0f;
	// 中間の位置
	const float middleX = 640.0f;
	// 最終位置
	const float endX = 0.0f;
	// Y座標（固定）
	const float yPos = 360.0f;
	// 移動にかけるフレーム数（60フレーム = 1秒）
	const int moveDuration = 60;
	// 待機時間（1秒 = 60フレーム）
	const int waitDuration = 60;

	// 現在の経過時間を取得
	int time = gameTime_ % (moveDuration * 3 + waitDuration * 2); // 全体で移動と待機が3秒＋2秒

	// 1秒経過後、1280 -> 640までEaseOutQuartで移動
	if (time >= moveDuration && time < moveDuration * 2) {
		float t = static_cast<float>(time - moveDuration) / moveDuration;
		float easedPos = Easing::EaseOutQuart(startX, middleX, t);
		spriteWaveNum_->SetPosition({easedPos, yPos});
	}
	// 1秒の待機（640の位置で静止）
	else if (time >= moveDuration * 2 && time < moveDuration * 2 + waitDuration) {
		spriteWaveNum_->SetPosition({middleX, yPos});
	}
	// その後、640 -> 0までEaseInQuartで移動
	else if (time >= moveDuration * 2 + waitDuration && time < moveDuration * 3 + waitDuration) {
		float t = static_cast<float>(time - (moveDuration * 2 + waitDuration)) / moveDuration;
		float easedPos = Easing::EaseOutQuart(middleX, endX, t);
		spriteWaveNum_->SetPosition({easedPos, yPos});
	}
	// それ以外は静止（初期状態）
	else if (time < moveDuration) {
		spriteWaveNum_->SetPosition({startX, yPos});
	}
}

void GameScene::ShowResult() {
	///
	///	5秒かけてリザルト背景の拡縮
	/// （0.5秒で拡大->4秒待機->0.5秒で拡縮）
	///
	{
		// スプライトの初期サイズ
		const float startSize = 0.0f;
		// 中間のサイズ
		const float middleSize = 480.0f;
		// 最終サイズ
		const float endSize = 0.0f;
		// サイズ拡大にかけるフレーム数（0.5秒 = 30フレーム）
		const int scaleUpDuration = 30;
		// 待機時間（4秒 = 240フレーム）
		const int waitDuration = 240;
		// サイズ縮小にかけるフレーム数（0.5秒 = 30フレーム）
		const int scaleDownDuration = 30;

		// 現在の経過時間を取得
		int time = gameTime_ % (scaleUpDuration + waitDuration + scaleDownDuration); // 全体で拡大、待機、縮小

		// 0.5秒かけてサイズを0から480まで拡大
		if (time < scaleUpDuration) {
			float t = static_cast<float>(time) / scaleUpDuration;
			float easedSize = Easing::EaseOutQuart(startSize, middleSize, t);
			spriteResultBackGround_->SetSize({easedSize, 520.0f});
		}
		// 4秒待機
		else if (time < scaleUpDuration + waitDuration) {
			spriteResultBackGround_->SetSize({middleSize, 520.0f});
		}
		// 0.5秒かけてサイズを480から0まで縮小
		else if (time < scaleUpDuration + waitDuration + scaleDownDuration) {
			float t = static_cast<float>(time - (scaleUpDuration + waitDuration)) / scaleDownDuration;
			float easedSize = Easing::EaseInQuart(middleSize, endSize, t);
			spriteResultBackGround_->SetSize({easedSize, 520.0f});
		}
		// それ以外はサイズを0に設定（初期状態）
		else {
			spriteResultBackGround_->SetSize({startSize, 520.0f});
		}
	}
}

void GameScene::DrawScoreToResult() {
	int32_t scoreDigit[6];                   // 各桁の数字を格納
	int32_t scoreTemp = player_->GetScore(); // 現在のスコアを一時的に保存

	// scoreDigitに現在スコアを格納
	for (int32_t i = 0; i < kMaxScoreDigit; i++) {
		scoreDigit[i] = scoreTemp % 10;
		scoreTemp /= 10;
	}

	// スコアの表示位置（左上）
	Vector2 scorePosition = {568.0f, 440.0f}; // 中心になるようにいい感じに

	// スコアを6桁表示
	for (int32_t i = 0; i < kMaxScoreDigit; i++) {
		// 各桁の位置を計算
		Vector2 digitPosition = scorePosition + Vector2{static_cast<float>(i) * (21.0f + 3.0f), 0.0f}; // 21.0fは数字のサイズ。ちょっと間隔を開ける
		// 読み込んだテクスチャから適切な数字を抜き取る
		spriteScore_[i]->SetTextureRect({0.0f + (scoreDigit[5 - i] * 50.0f), 0.0f}, {50.0f, 65.0f});
		spriteScore_[i]->SetPosition(digitPosition);
		spriteScore_[i]->Draw();
	}
}

void GameScene::InitializeParameterWAVE1() {
	// WAVE1での各パラメーターを設定
	playerAttackDamage_ = 0.013f;
	enemyAttackDamage_ = 0.008f;
	enemySpawnRate_ = 240;
	enemyFallSpeed_ = 0.05f;
}

void GameScene::InitializeParameterWAVE2() {
	// WAVE2での各パラメーターを設定
	playerAttackDamage_ = 0.013f;
	enemyAttackDamage_ = 0.008f;
	enemySpawnRate_ = 180;
	enemyFallSpeed_ = 0.05f;
}

void GameScene::InitializeParameterWAVE3() {
	// WAVE3での各パラメーターを設定
	playerAttackDamage_ = 0.013f;
	enemyAttackDamage_ = 0.008f;
	enemySpawnRate_ = 100;
	enemyFallSpeed_ = 0.05f;
}

void GameScene::CameraShake(float amount, int32_t duration) {
	shakeAmountX_ = amount;
	shakeAmountY_ = amount;
	shakeDuration_ = duration;
	shakeTimer_ = duration;
}

bool GameScene::GetIsFinished() { return isFinished_; }