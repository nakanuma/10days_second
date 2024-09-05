#include "Player.h"

Player::~Player() {}

void Player::Initialize() {
	///
	///	汎用機能初期化
	///		
	
	// 入力のインスタンス取得
	input_ = Input::GetInstance();

	///
	///	基本的な情報初期化
	/// 
	
	// ワールドトランスフォーム初期化
	worldTransform_.Initialize();

}

void Player::Update() {}

void Player::Draw() {}
