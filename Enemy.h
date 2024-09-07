#pragma once
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "Model.h"

// MyClass
#include "Line.h"

class Enemy {
public:
	// デストラクタ
	~Enemy();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="modelEnemy">モデル</param>
	/// <param name="spawnPosition">スポーン座標</param>
	void Initialize(Model* modelEnemy, Vector3 spawnPosition);

	// 更新
	void Update();

	// 敵が四つ角へ到達した時に向きを変える処理
	// (プレイヤーが予測線に触れたら敵が予測線の上を移動する処理を書いたら多分整合性が取れなくなるので、とりあえず仮の処理である点に留意)
	void ChangeDirection();

	// 描画
	void Draw(ViewProjection& viewProjection);

	// デバッグ表示
	void Debug();

	///
	///	アクセッサ
	/// 

	// ワールド座標を取得
	Vector3 GetWorldPosition();
	// 敵の移動方向を設定
	void SetDirection(Vector3 direction) { direction_ = direction; }

private:
	///
	///	基本的な情報
	/// 
	
	// ワールドトランスフォーム
	WorldTransform worldTransform_;
	// モデル
	Model* modelEnemy_ = nullptr;
	// 移動速度（今は固定。あとでランダムに変えるかも）
	float speed_;
	// 現在の向き
	Vector3 direction_;

	///
	///	その他
	///

	// 各方向を定義（敵の移動に使用）
	const Vector3 kRightDirection = {1.0f, 0.0f, 0.0f}; // 右移動（上のラインにいる場合）
	const Vector3 kDownDirection = {0.0f, -1.0f, 0.0f}; // 下移動（右のラインにいる場合）
	const Vector3 kLeftDirection = {-1.0f, 0.0f, 0.0f}; // 左移動（下のラインにいる場合）
	const Vector3 kUpDirection = {0.0f, 1.0f, 0.0f};    // 上移動（左のラインにいる場合）

	// 外側の枠の各頂点の座標を定義（敵のスポーン位置や、敵の移動方向を変えるトリガーに使用）
	const Vector3 kOutsideFrameTopLeftPos = {-16.0f, 16.0f, 0.0f};     // 外枠の左上座標
	const Vector3 kOutsideFrameTopRightPos = {16.0f, 16.0f, 0.0f};     // 外枠の右上座標
	const Vector3 kOutsideFrameBottomRightPos = {16.0f, -16.0f, 0.0f}; // 外枠の右下座標
	const Vector3 kOutsideFrameBottomLeftPos = {-16.0f, -16.0f, 0.0f}; // 外枠の左下座標
};
