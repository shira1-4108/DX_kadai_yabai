#pragma once
#include"HitSphere.h"
#include<DirectXMath.h>

using namespace DirectX;

//当たり判定の線分情報を持つクラス
class HitLine
{
private:
	//線分の端の座標2つ
	XMFLOAT3 mPoint1, mPoint2;

public:
	// HitSphereと、このHitLineが衝突しているか判定
	bool IsHit(HitSphere* pOther);
};

