#pragma once
#include"HitSphere.h"
#include<DirectXMath.h>

using namespace DirectX;

//�����蔻��̐����������N���X
class HitLine
{
private:
	//�����̒[�̍��W2��
	XMFLOAT3 mPoint1, mPoint2;

public:
	// HitSphere�ƁA����HitLine���Փ˂��Ă��邩����
	bool IsHit(HitSphere* pOther);
};

