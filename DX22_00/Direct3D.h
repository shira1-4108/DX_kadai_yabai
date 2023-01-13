#pragma once

#include <d3d11.h> // DirectX11のインクルードファイル

// 解放用のマクロ
#define COM_SAFE_RELEASE(o)  if(o!=NULL){o->Release();o=NULL;}

// Direct3D関係の変数をまとめる構造体
struct DIRECT3D
{
	ID3D11Device* device; // デバイス
	ID3D11DeviceContext* context; // コンテキスト
	IDXGISwapChain* swapChain; // スワップチェイン
	ID3D11RenderTargetView* renderTarget; // レンダーターゲット
	ID3D11VertexShader* vertexShader; // 頂点シェーダー
	ID3D11PixelShader* pixelShader; // ピクセルシェーダー
	ID3D11InputLayout* inputLayout; // インプットレイアウト
	ID3D11BlendState* blendAlpha;  // アルファブレンディング用ブレンドステート
	ID3D11BlendState* blendAdd; // 加算合成用
	ID3D11BlendState* blendSub; // 減算合成用
	ID3D11SamplerState* samplerPoint; // ポイント補完用サンプラー
	ID3D11Texture2D*        depthStencilTexture;
	ID3D11DepthStencilView* depthStencilView;
};

// 関数のプロトタイプ宣言

// Direct3Dの初期化を行う関数
BOOL Direct3D_Init(HWND hWnd);

// Direct3Dの解放を行う関数
void Direct3D_Release();

// DIRECT3D構造体の実体のアドレスを返す
DIRECT3D* Direct3D_Get();

// テクスチャを読み込む関数
// 第一引数：テクスチャのファイル名
// 第二引数：読み込んだテクスチャを格納する変数のアドレス
void Direct3D_LoadTexture(const wchar_t* fileName,
	ID3D11ShaderResourceView** outTexture);

