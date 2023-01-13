#pragma once

#include <DirectXMath.h>

class HitLine;

class HitSphere
{
public:

	// 判定する
	// 引数で受け取った円と自分自身（円）が衝突してるかtrue/falseで返す
	bool IsHit(HitSphere* pOther);

	// 参照型メンバー変数に初期値設定するには、引数付きコンストラクタを使う
	HitSphere(DirectX::XMFLOAT3& center, float hankei);

	void SetHankei(float hankei)
	{
		mHankei = hankei;
	}

private:
	bool active = true; // 当たり判定のON/OFF切り替え変数

	DirectX::XMFLOAT3& mCenter;
	float mHankei; // 円の半径

	friend HitLine;
};

