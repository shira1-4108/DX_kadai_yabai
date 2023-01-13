#pragma once

#include <d3d11.h>

// 3Dモデルを表す構造体
struct ModelData
{
	ID3D11Buffer* mVertexBuffer;	// 頂点バッファのポインタ
	ID3D11ShaderResourceView* mSRV;	// テクスチャのSRV　（SRVはIDみたいなもの）
	int mNumVertex;					// 頂点数
};

// モデルを構成する頂点１つを表す構造体
struct ModelVertex
{
	float x, y, z;		// 位置
	
//	float r, g, b, a;	// 頂点色
	float u, v, uv1;			// テクスチャUV座標
	float nx, ny, nz;	// 法線情報
	
};

// 色を表す構造体
struct RGBA
{
	float r, g, b, a;
};


struct VERTEX3D  // VERTEX = 頂点
{
	float x, y, z;  // 頂点の座標（位置）
	RGBA color;  // 頂点の色
	float u, v;  // テクスチャのUV座標
};
