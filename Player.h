#pragma once
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "Model.h"
#include "Sprite.h"
#include "Input.h"
#include <list>
#include <vector>

// MyClass
#include "Laser.h"

class Player {
public:
	///
	///	基本的な関数
	///

	// デストラクタ
	~Player();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="modelPlayer">プレイヤー本体のモデル</param>
	void Initialize(Model* modelPlayer, Model* modelLaser);

	// 更新
	void Update();
	// 左スティックで左右移動・右スティックで角度変更・レーザー射撃状態によって自動で上昇下降・プレイヤーが範囲外へ行かないようにする処理
	void Move();
	// 予備動作の開始。これが呼ばれている間はプレイヤーの半径まで予備動作レーザーを拡大する
	void StartAntic();
	// RBでレーザーを発射・RBを押した瞬間に少し上に跳ねる処理
	void Attack();
	// 無敵状態中の処理（ダメージを受けた際）
	void Invincible();
	// 衝突時コールバック（HPを減らして無敵にする）
	void OnCollision();

	// 描画
	void Draw(ViewProjection& viewProjection);
	// UIの描画（スプライトのみ）
	void DrawUI();

	// デバッグ表示
	void Debug();

	// 指定した値、スコアを加算
	void AddScore(uint32_t amount) { score_ += amount; }

	///
	///	アクセッサ
	/// 
	
	// ワールド座標取得
	Vector3 GetWorldPosition();
	// 半径を取得
	float GetRadius() { return kRadius_; }
	// レーザーの取得
	Laser& GetLaser() { return laser_; }

private:
	///
	///	汎用機能
	/// 
	
	Input* input_ = nullptr;

	///
	///	プレイヤーの基本的な情報
	/// 
	
	// ワールドトランスフォーム
	WorldTransform worldTransform_;
	// モデル
	Model* modelPlayer_ = nullptr;
	// 移動速度
	float characterSpeed_;
	// 半径
	const float kRadius_ = 1.0f;

	// 体力
	int32_t hp_;
	// 現在スコア
	uint32_t score_;
	// 無敵状態かどうか
	bool isInvincible_ = true;
	// 無敵時間（フレーム）: 要調整
	const int32_t kInvincibleTime_ = 120;
	// 無敵時間カウント
	int32_t invincibleCount_;

	/*RBを押した瞬間のみ、プレイヤーを上に跳ねさせる処理に使用*/

	// レーザーを撃つ際のボタンが押されたかを記録
	bool wasLaserButtonPressed_;

	///
	///	RBを押した瞬間の上昇について
	/// 

	bool isRising_ = false;
	float riseStartY_ = 0.0f;
	float riseEndY_ = 0.0f;
	float riseTime_ = 0.0f;
	const float riseDuration_ = 15.0f; // 上昇にかけるフレーム数

	///
	/// 下降時について
	///

	float currentDescendingSpeed_; // 現在の落下速度。autoDescendingSpeed_まで増加させる
	const float kDescentAcceleration_ = 0.01f; // 現在の落下速度を、毎フレームこの値増加させる

	///
	///	レーザー関連の情報
	/// 
	
	// レーザー
	Laser laser_;
	// モデル
	Model* modelLaser_ = nullptr;


	/* 予備動作に使用 */

	// 予備動作用ワールドトランスフォーム
	WorldTransform anticWorldTransform_;
	// 予備動作が終了したか
	bool endAntic_ = false;
	// 予備動作タイマー（予備動作レーザーの拡大が開始する時間を決める）
	uint32_t anticTimer_ = 0;

	///
	///	その他パラメーターなど（ImGuiでの調整項目）
	/// 
	
	// レーザー射撃中/非射撃中の自動上昇・下降の速度
	float autoAscendingSpeed_;
	float autoDescendingSpeed_;

	///
	///	スプライト関連
	/// 
	
	/* 左上の残りHP表示 */

	// ハートのスプライト（最大HPの数だけ）
	std::vector<Sprite*> heartSprites_;

	/* 左上の現在スコア表示 */

	// スコアの桁数
	const int32_t kMaxScoreDigit = 6;
	// スコア表示に使用するスプライト（6桁分）
	Sprite* spriteScore_[6];
	// 0~9までの数字が書いてある数字テクスチャ
	uint32_t textureNumber_;
};
