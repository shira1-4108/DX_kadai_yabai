#include "HitLine.h"



bool HitLine::IsHit(HitSphere * pOther)
{
	//ベクトル1
	//XMFloat3からXMVECTORに変換する
	XMVECTOR sphereCenter = XMLoadFloat3(&pOther->mCenter);
	XMVECTOR point1 = XMLoadFloat3(&mPoint1);
	//引き算でベクトルを計算する
	XMVECTOR vector1 = sphereCenter - point1;

	//ベクトル2
	//XMFloat3からXMVECTORに変換する
	XMVECTOR point2 = XMLoadFloat3(&mPoint2);
	//引き算でベクトルを計算する
	XMVECTOR vector2 = point1 - point2;

	//ベクトル2を単位ベクトルにする
	XMVECTOR vector2Norm = XMVector2Normalize(vector2);

	//ベクトル1とベクトル2の内積を計算する
	XMVECTOR Dot = XMVector3Dot(vector1, vector2Norm);

	//円の中心から直線に垂線を降ろす
	XMVECTOR point3 = point1 + vector2Norm * Dot;
	XMVECTOR vectorVer = sphereCenter - point3;

	const auto length = XMVector3Length(vectorVer);

	const auto hankei = XMLoadFloat(&pOther->mHankei);

	


	return false;
}
