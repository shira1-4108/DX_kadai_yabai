#include "Camera.h"

using namespace DirectX;

Camera* Camera::mMainCamera;

DirectX::XMMATRIX Camera::GetViewMatrix()
{
	return mViewMatrix;
}

void Camera::SetEye(DirectX::XMFLOAT3 newEye)
{
	mEye = newEye;
}

void Camera::SetFocus(DirectX::XMFLOAT3 newFocus)
{
	mFocus = newFocus;
}

void Camera::SetUp(DirectX::XMFLOAT3 newUp)
{
	mUp = newUp;
}

void Camera::Update()
{
	// XMFLOAT3Å®XMVECTORÇ÷ïœä∑
	XMVECTOR eye = XMLoadFloat3(&mEye);
	XMVECTOR focus = XMLoadFloat3(&mFocus);
	XMVECTOR up = XMLoadFloat3(&mUp);
	// ÉrÉÖÅ[ïœä∑çsóÒçÏê¨
	mViewMatrix = XMMatrixLookAtLH(
		eye, focus, up);
}
