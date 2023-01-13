#pragma once

#include "ModelData.h"
#include "Camera.h"
#include <DirectXMath.h>

// 3Dモデルを表示する
class Model
{

public:
	virtual void Draw(); // 表示する

	// このクラスで表示する3Dデータをセットする
	void SetModelData(ModelData model);

	// 仮想世界（ワールド）上の位置を指定する座標
	DirectX::XMFLOAT3 mPos;
	// 回転角度
	DirectX::XMFLOAT3 mRotate;
	// 拡大縮小率
	DirectX::XMFLOAT3 mScale = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);

	// 拡大縮小率をxyzに同じ値で設定する
	void SetScale(float newScale);

	// このモデルに適用するカメラオブジェクト
	Camera* mCamera;

	bool m2dRender = false;
	
	// 2D描画を行うかどうか切り替える
	void Set2dRender(bool is2D)
	{
		m2dRender = is2D;
		mLightActive = !is2D;
	}

	// モデルのディフューズマテリアル色を設定
	void SetDiffuse(DirectX::XMFLOAT4 diffuse)
	{
		mDiffuse = diffuse;
	}

	// UVアニメーション用オフセット
	DirectX::XMFLOAT2 mUvOffset = DirectX::XMFLOAT2(0,0);

protected:
	// 3Dデータを持つ変数
	ModelData mModelData;

	// ライトを有効にするか
	bool mLightActive = true;

	// ディフューズマテリアル色
	DirectX::XMFLOAT4 mDiffuse = DirectX::XMFLOAT4(1,1,1,1);

	// 定数バッファの転送用
	struct ConstBufferData
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
		DirectX::XMMATRIX worldRotate;
		// UVアニメーション用オフセット（zとwは使わない）
		DirectX::XMFLOAT4 uvOffset;
		// ディフューズ・マテリアル
		DirectX::XMFLOAT4 diffuse;
		// 平行光源
		DirectX::XMFLOAT3 lightVector;
		float lightPower;
		// アンビエント
		DirectX::XMFLOAT4 ambient;
	};

	// ワールド・ビュー・プロジェクション・回転行列を作成してoutにセットする
	virtual void GetWVPRMatrix(ConstBufferData& out);

};

