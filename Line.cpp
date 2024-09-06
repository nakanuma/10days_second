#include "Line.h"

Line::~Line() {}

void Line::Initialize(Vector3 start, Vector3 end, ViewProjection* viewProjection) {
	// 引数で受け取ったデータをメンバ変数に記録
	start_ = start;
	end_ = end;
	// 始点と終点からベクトルを計算
	vector_ = end_ - start_;

	// プリミティブドロワーのインスタンス取得とビュープロジェクションのセット
	primitiveDrawer_ = PrimitiveDrawer::GetInstance();
	primitiveDrawer_->SetViewProjection(viewProjection);
}

void Line::Draw() {
	// 色
	Vector4 color = {1.0f, 1.0f, 1.0f, 1.0f};
	// 線を描画
	primitiveDrawer_->DrawLine3d(start_, end_, color);
}
