#include "GameObject.h"

extern DWORD gDeltaTime;

using namespace DirectX;

GameObject::GameObject()
{
	mModel = nullptr;
	mSpeed = 0.0f;
	mHit = nullptr;
	mHP = new HP();
}

GameObject::~GameObject()
{
	delete mModel;
	delete mHit;
	delete mHP;
}

void GameObject::Draw()
{
	mModel->Draw();
}

void GameObject::Update()
{
	// �O�����x�N�g�����v�Z����
	XMFLOAT3 forwardVector;
	float radianY = XMConvertToRadians(mModel->mRotate.y-90.0f);
	forwardVector.x = sinf(radianY);
	forwardVector.z = cosf(radianY);
	forwardVector.y = 0.0f;
	// �O�����x�N�g���������o�[�ϐ��ɕۑ�
	mForwardVector = forwardVector;

	// �ړ�����
	mModel->mPos.x += forwardVector.x * mSpeed * gDeltaTime;
	mModel->mPos.z += forwardVector.z * mSpeed * gDeltaTime;
}

Model * GameObject::GetModel()
{
	return mModel;
}

float GameObject::GetDistaceVector(DirectX::XMFLOAT3 pOther)
{

	// (a)�n���ꂽ���f���̈ʒu���擾����
	XMFLOAT3 otherPos = pOther;

	// (b)�����̈ʒu���擾����
	XMFLOAT3 thisPos = this->mModel->mPos;

	// (a)-(b)���v�Z����(b)��(a)�x�N�g�������
	XMVECTOR baVec = XMLoadFloat3(&otherPos) - XMLoadFloat3(&thisPos);

	// (b)��(a)�x�N�g���̒����𑪂�
	XMVECTOR baVecLength = XMVector3Length(baVec);

	// ���ۂ̋��������o��
	this->mDistanceVector = XMVectorGetX(baVecLength);

	return this->mDistanceVector;
}

DirectX::XMFLOAT3 GameObject::GetForwardVector()
{
	return mForwardVector;
}
