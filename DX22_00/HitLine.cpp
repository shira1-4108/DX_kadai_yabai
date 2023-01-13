#include "HitLine.h"



bool HitLine::IsHit(HitSphere * pOther)
{
	//�x�N�g��1
	//XMFloat3����XMVECTOR�ɕϊ�����
	XMVECTOR sphereCenter = XMLoadFloat3(&pOther->mCenter);
	XMVECTOR point1 = XMLoadFloat3(&mPoint1);
	//�����Z�Ńx�N�g�����v�Z����
	XMVECTOR vector1 = sphereCenter - point1;

	//�x�N�g��2
	//XMFloat3����XMVECTOR�ɕϊ�����
	XMVECTOR point2 = XMLoadFloat3(&mPoint2);
	//�����Z�Ńx�N�g�����v�Z����
	XMVECTOR vector2 = point1 - point2;

	//�x�N�g��2��P�ʃx�N�g���ɂ���
	XMVECTOR vector2Norm = XMVector2Normalize(vector2);

	//�x�N�g��1�ƃx�N�g��2�̓��ς��v�Z����
	XMVECTOR Dot = XMVector3Dot(vector1, vector2Norm);

	//�~�̒��S���璼���ɐ������~�낷
	XMVECTOR point3 = point1 + vector2Norm * Dot;
	XMVECTOR vectorVer = sphereCenter - point3;

	const auto length = XMVector3Length(vectorVer);

	const auto hankei = XMLoadFloat(&pOther->mHankei);

	


	return false;
}
