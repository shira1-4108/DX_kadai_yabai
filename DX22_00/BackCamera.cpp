#include "BackCamera.h"

using namespace DirectX;

void BackCamera::Update()
{
	// 追尾カメラの処理
	// ①操作キャラの前向きベクトルを取得する
	XMFLOAT3 forwardVec = mTarget->GetForwardVector();
	// ②前向きベクトルを反転させて後ろ向きベクトルを作る
	XMFLOAT3 backVec;
	backVec.x = -forwardVec.x;
	backVec.y = -forwardVec.y;
	backVec.z = -forwardVec.z;
	// ③後ろ向きベクトル上にカメラを置く
	XMFLOAT3 camEye;
	Model* pTargetModel = mTarget->GetModel();
	camEye.x = pTargetModel->mPos.x + backVec.x * 2.0f;
	camEye.y = pTargetModel->mPos.y + backVec.y * 2.0f + 0.5f;
	camEye.z = pTargetModel->mPos.z + backVec.z * 2.0f;
	// 緩やかカメラ処理
	// 1フレーム前のカメラ位置保存変数
	static XMFLOAT3 lastCamEye;
	float blendFactor = 0.997f; // 平均化の重み
	camEye.x = lastCamEye.x * blendFactor
		+ camEye.x * (1.0f - blendFactor);
	camEye.y = lastCamEye.y * blendFactor
		+ camEye.y * (1.0f - blendFactor);
	camEye.z = lastCamEye.z * blendFactor
		+ camEye.z * (1.0f - blendFactor);

	this->SetEye(camEye);
	lastCamEye = camEye; // 次のループのため保存

	// ④カメラ注視点（focus）をキャラの少し前に設定
	XMFLOAT3 camFocus;
	camFocus.x = pTargetModel->mPos.x + forwardVec.x;
	camFocus.y = pTargetModel->mPos.y + forwardVec.y;
	camFocus.z = pTargetModel->mPos.z + forwardVec.z;
	this->SetFocus(camFocus);

	// ビュー変換行列計算
	Camera::Update();
}

void BackCamera::SetTarget(GameObject * pTarget)
{
	mTarget = pTarget;
}
