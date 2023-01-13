#pragma once

#include <DirectXMath.h>

// カメラを表すクラス
class Camera
{

public:
	// ビュー変換行列を返す
	DirectX::XMMATRIX GetViewMatrix();

	// カメラ位置を設定
	void SetEye(DirectX::XMFLOAT3 newEye);
	// カメラ注視点
	void SetFocus(DirectX::XMFLOAT3 newFocus);
	// カメラ上方向ベクトル
	void SetUp(DirectX::XMFLOAT3 newUp);

	// カメラに必要なパラメータ変数
	DirectX::XMFLOAT3 mEye, mFocus, mUp;

	// 更新処理（毎ループ１回呼び出す）
	virtual void Update();

	static Camera* mMainCamera;

private:
	// 計算したビュー変換行列を保存する変数
	DirectX::XMMATRIX mViewMatrix;

};

