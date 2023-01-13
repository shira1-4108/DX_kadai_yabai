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
	// 前向きベクトルを計算する
	XMFLOAT3 forwardVector;
	float radianY = XMConvertToRadians(mModel->mRotate.y-90.0f);
	forwardVector.x = sinf(radianY);
	forwardVector.z = cosf(radianY);
	forwardVector.y = 0.0f;
	// 前向きベクトルをメンバー変数に保存
	mForwardVector = forwardVector;

	// 移動処理
	mModel->mPos.x += forwardVector.x * mSpeed * gDeltaTime;
	mModel->mPos.z += forwardVector.z * mSpeed * gDeltaTime;
}

Model * GameObject::GetModel()
{
	return mModel;
}

float GameObject::GetDistaceVector(DirectX::XMFLOAT3 pOther)
{

	// (a)渡されたモデルの位置を取得する
	XMFLOAT3 otherPos = pOther;

	// (b)自分の位置を取得する
	XMFLOAT3 thisPos = this->mModel->mPos;

	// (a)-(b)を計算して(b)→(a)ベクトルを作る
	XMVECTOR baVec = XMLoadFloat3(&otherPos) - XMLoadFloat3(&thisPos);

	// (b)→(a)ベクトルの長さを測る
	XMVECTOR baVecLength = XMVector3Length(baVec);

	// 実際の距離を取り出す
	this->mDistanceVector = XMVectorGetX(baVecLength);

	return this->mDistanceVector;
}

DirectX::XMFLOAT3 GameObject::GetForwardVector()
{
	return mForwardVector;
}
