

// C++から定数バッファで転送されたデータがセットされる
cbuffer ConstantBufferMatrix : register(b1)
{
	matrix gWorld;		// ワールド変換行列
	matrix gView;		// ビュー変換行列
	matrix gProjection;	// プロジェクション変換行列
	matrix gWorldRotate;// 回転行列
	float4 gUvOffset;	// UVアニメーション用
	float4 gDiffuse;	// マテリアル→表面の色
	float3 gLightVector;// 光の向き
	float gLightPower;	// 光の強さ
	float4 gAmbient;	// 環境光の色と強さ
}

