#include "Direct3d.h"
#include "WICTextureLoader.h"

// DirectX11のライブラリをリンク指定
#pragma comment (lib, "d3d11.lib")

// 関数のプロトタイプ宣言

// デバイスを作成する関数
BOOL CreateDevice(HWND hWnd);

// レンダーターゲットを作成する関数
BOOL CreateRenderTarget();

// シェーダーオブジェクトを作成する関数
BOOL CreateShader();

// ビューポートを設定する関数
void SetViewPort();


// グローバル変数の宣言

static DIRECT3D* gpD3D; // DIRECT3D構造体を動的確保したアドレスを入れる
static UINT gScreenWidth, gScreenHeight; // 画面の幅・高さ

#include "vs.h"  // コンパイル済み頂点シェーダー
#include "ps.h"  // コンパイル済みピクセルシェーダー


BOOL Direct3D_Init(HWND hWnd)
{
	// DIRECT3D構造体を動的確保してゼロクリアする
	// malloc関数　→　引数で指定したバイト数のメモリを確保して
	// 　使えるようにする。
	gpD3D = (DIRECT3D*)malloc(sizeof(DIRECT3D));
	// ZeroMemory関数　→　第一引数で指定したアドレスから、
	// 　第二引数で指定したサイズ分、ゼロで埋める。
	ZeroMemory(gpD3D, sizeof(DIRECT3D));

	// デバイス作成関数を呼び出す
	CreateDevice(hWnd);

	// レンダーターゲット作成関数を呼び出す
	CreateRenderTarget();

	// シェーダーオブジェクトを作成する関数を呼び出す
	CreateShader();

	// ビューポートを設定する関数を呼び出す
	SetViewPort();

	return 0;
}

void Direct3D_Release()
{
	// DIRECT3D構造体の中の変数について
	// 解放を行う。
	COM_SAFE_RELEASE(gpD3D->samplerPoint);
	COM_SAFE_RELEASE(gpD3D->blendAdd);
	COM_SAFE_RELEASE(gpD3D->blendSub);
	COM_SAFE_RELEASE(gpD3D->blendAlpha);
	COM_SAFE_RELEASE(gpD3D->inputLayout);
	COM_SAFE_RELEASE(gpD3D->pixelShader);
	COM_SAFE_RELEASE(gpD3D->vertexShader);

	COM_SAFE_RELEASE(gpD3D->renderTarget);
	COM_SAFE_RELEASE(gpD3D->swapChain);
	COM_SAFE_RELEASE(gpD3D->context);
	COM_SAFE_RELEASE(gpD3D->device);

	// malloc関数で確保したメモリを解放
	if (gpD3D != NULL)
	{
		free(gpD3D); // mallocで確保したメモリを解放する関数
		gpD3D = NULL;
	}
}

DIRECT3D * Direct3D_Get()
{
	return gpD3D;
}

BOOL CreateDevice(HWND hWnd)
{
	UINT flags = 0;
	// Graphic Toolを使ってデバッグを行う場合は、下の行のコメントを外してデバッグモードでデバイスを作成する。
	//flags |= D3D11_CREATE_DEVICE_DEBUG;

	D3D_FEATURE_LEVEL pLevels[] = { D3D_FEATURE_LEVEL_11_0 };
	D3D_FEATURE_LEVEL level;

	RECT                rect;
	DXGI_SWAP_CHAIN_DESC scDesc;

	// ウインドウのクライアント領域（実際に描画できる範囲）のサイズを取得
	GetClientRect(hWnd, &rect);
	ZeroMemory(&scDesc, sizeof(scDesc));

	gScreenWidth = (UINT)(rect.right - rect.left);
	gScreenHeight = (UINT)(rect.bottom - rect.top);

	scDesc.BufferCount = 1;
	scDesc.BufferDesc.Width = gScreenWidth;
	scDesc.BufferDesc.Height = gScreenHeight;
	scDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scDesc.OutputWindow = hWnd;
	scDesc.SampleDesc.Count = 1;
	scDesc.SampleDesc.Quality = 0;
	scDesc.Windowed = TRUE; // ウインドウモードとフルスクリーンモード切り替え

	   // 機能レベル
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,					// DirectX11.1対応GPUレベル
		D3D_FEATURE_LEVEL_11_0,					// DirectX11対応GPUレベル
		D3D_FEATURE_LEVEL_10_1,					// DirectX10.1対応GPUレベル
		D3D_FEATURE_LEVEL_10_0,					// DirectX10対応GPUレベル
		D3D_FEATURE_LEVEL_9_3,					// DirectX9.3対応GPUレベル
		D3D_FEATURE_LEVEL_9_2,					// DirectX9.2対応GPUレベル
		D3D_FEATURE_LEVEL_9_1					// Direct9.1対応GPUレベル
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	HRESULT  hr;
	// DirectX11デバイス、コンテキスト、スワップチェインの作成
	hr = D3D11CreateDeviceAndSwapChain(NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		flags,
		pLevels,
		1,
		D3D11_SDK_VERSION,
		&scDesc,
		&gpD3D->swapChain,
		&gpD3D->device,
		&level,
		&gpD3D->context);

	return SUCCEEDED(hr);

}

BOOL CreateRenderTarget()
{
	// バックバッファ取得
	ID3D11Texture2D* pBackBuffer = NULL;

	HRESULT hr;
	hr = gpD3D->swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

	if (SUCCEEDED(hr))
	{
		// レンダーターゲットビューを作成
		hr = gpD3D->device->CreateRenderTargetView(pBackBuffer, NULL, &gpD3D->renderTarget);


		if (SUCCEEDED(hr))
		{
			//深度ステンシルバッファ作成
			D3D11_TEXTURE2D_DESC txDesc;

			ZeroMemory(&txDesc, sizeof(txDesc));
			txDesc.Width = gScreenWidth;
			txDesc.Height = gScreenHeight;
			txDesc.MipLevels = 1;
			txDesc.ArraySize = 1;
			txDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			txDesc.SampleDesc.Count = 1;
			txDesc.SampleDesc.Quality = 0;
			txDesc.Usage = D3D11_USAGE_DEFAULT;
			txDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
			txDesc.CPUAccessFlags = 0;
			txDesc.MiscFlags = 0;

			hr = gpD3D->device->CreateTexture2D(&txDesc, NULL, &gpD3D->depthStencilTexture);

			if (SUCCEEDED(hr)) {
				D3D11_DEPTH_STENCIL_VIEW_DESC dsDesc;
				ZeroMemory(&dsDesc, sizeof(dsDesc));
				dsDesc.Format = txDesc.Format;
				dsDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
				dsDesc.Texture2D.MipSlice = 0;
				hr = gpD3D->device->CreateDepthStencilView(gpD3D->depthStencilTexture, &dsDesc, &gpD3D->depthStencilView);
			}

			if (SUCCEEDED(hr))
			{
				// レンダリングターゲットを設定　（注）今は2DなのでZバッファ無効にしておく
				gpD3D->context->OMSetRenderTargets(1, &gpD3D->renderTarget, gpD3D->depthStencilView);

				return TRUE;
			}
		}
	}

	COM_SAFE_RELEASE(pBackBuffer); // バックバッファ解放

	return FALSE;
}

BOOL CreateShader()
{
	HRESULT hr;
	//頂点シェーダー生成
	hr = gpD3D->device->CreateVertexShader(&g_vs_main, sizeof(g_vs_main), NULL, &gpD3D->vertexShader);

	if (FAILED(hr))
		return FALSE;

	//ピクセルシェーダー生成
	hr = gpD3D->device->CreatePixelShader(&g_ps_main, sizeof(g_ps_main), NULL, &gpD3D->pixelShader);

	if (FAILED(hr))
		return FALSE;

	

	//頂点レイアウト作成　※頂点データを変更したら、この配列も変更する
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] = {
		//{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }, // xyz
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
		  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },// xy

		

//		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
//		  D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },// color

		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
		  D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },// uv

		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
		  D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },// normal

		

	};

	hr = gpD3D->device->CreateInputLayout(vertexDesc, ARRAYSIZE(vertexDesc),
		g_vs_main, sizeof(g_vs_main), &gpD3D->inputLayout);

	if (FAILED(hr))
		return FALSE;

	// 頂点シェーダーのパラメータセット
	gpD3D->context->VSSetShader(gpD3D->vertexShader, NULL, 0);
	// ピクセルシェーダーのパラメータセット
	gpD3D->context->PSSetShader(gpD3D->pixelShader, NULL, 0);

	// 頂点レイアウトをセット
	gpD3D->context->IASetInputLayout(gpD3D->inputLayout);
	// トポロジー（プリミティブタイプ）をセット
	gpD3D->context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // 三角形リストを指定

	// ブレンドステート設定
	D3D11_BLEND_DESC BlendDesc;

	// テクスチャ透過処理が行われるように設定
	ZeroMemory(&BlendDesc, sizeof(BlendDesc));
	BlendDesc.AlphaToCoverageEnable = FALSE;
	BlendDesc.IndependentBlendEnable = FALSE;
	BlendDesc.RenderTarget[0].BlendEnable = TRUE;
	BlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
	BlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	BlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].RenderTargetWriteMask = 0x0f;

	hr = gpD3D->device->CreateBlendState(&BlendDesc, &gpD3D->blendAlpha);

	// 加算合成用ブレンドステートを作成
	BlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;

	hr = gpD3D->device->CreateBlendState(&BlendDesc, &gpD3D->blendAdd);

	// 減算合成
	BlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ZERO;
	BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_COLOR;

	hr = gpD3D->device->CreateBlendState(&BlendDesc, &gpD3D->blendSub);

	if (SUCCEEDED(hr)) {
		gpD3D->context->OMSetBlendState(gpD3D->blendAlpha, NULL, 0xffffffff);
	}

	// サンプラーステートを作成
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT; // ポイント補完
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;

	hr = gpD3D->device->CreateSamplerState(&samplerDesc, &gpD3D->samplerPoint);


	return TRUE;
}

void SetViewPort()
{
	//ビューポート設定
	D3D11_VIEWPORT viewport;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = (FLOAT)gScreenWidth;
	viewport.Height = (FLOAT)gScreenHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	gpD3D->context->RSSetViewports(1, &viewport);
}

void Direct3D_LoadTexture(const wchar_t * fileName, ID3D11ShaderResourceView ** outTexture)
{
	HRESULT hr;
	DIRECT3D* d3d = Direct3D_Get();
	// テクスチャを読み込む
	hr = DirectX::CreateWICTextureFromFile(d3d->device,
		fileName,
		NULL,
		outTexture);

	// エラー処理
	if (FAILED(hr)) // 失敗した場合
	{
		MessageBoxA(NULL, "テクスチャ読み込み失敗", "エラー発生", MB_OK);
	}
}
