#undef UNICODE  // Unicodeではなく、マルチバイト文字を使う

#include <Windows.h>
#include "Direct3d.h"
#include "input.h"
#include <DirectXMath.h>
#include "WICTextureLoader.h"
#include "ObjModelLoader.h"
#include "Model.h"
#include "GameObject.h"
#include "BackCamera.h"
#include "NormalObject.h"
#include "Explosion.h"
#include <map> // 連想配列
#include <vector> // 可変配列
#include<algorithm>


#pragma comment (lib, "winmm.lib") // timeGetTime関数のため

// マクロ定義
#define CLASS_NAME    "DX21Smpl"// ウインドウクラスの名前
#define WINDOW_NAME  "スケルトンプログラム"// ウィンドウの名前

#define SCREEN_WIDTH (1024)	// ウインドウの幅
#define SCREEN_HEIGHT (576+30)	// ウインドウの高さ

// 構造体の定義


// 関数のプロトタイプ宣言
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// ゲームの画面描画の関数
void Game_Init();
void Game_Draw();
void Game_Update();
void Game_Release();



// グローバル変数の宣言

// 頂点バッファ用の変数
ID3D11Buffer* gpVertexBuffer;

// 頂点数を持つ変数
int gNumVertex;

// テクスチャ用の変数
ID3D11ShaderResourceView* gpTextureHalOsaka; // HAL OSAKA


extern ID3D11Buffer* gpConstBuffer;

using namespace DirectX;

// GameObjectクラスの変数
GameObject* gpCottage;
GameObject* gpGun;
GameObject* gpShot;
#define MAX_CHAR  3
// 2Dキャラの変数	入れ替え時移動用に+1
GameObject* gp2DChar[MAX_CHAR];
GameObject* gpShadow;

typedef std::vector<GameObject*> GameObjectVector;

GameObjectVector gObjectList;
GameObjectVector gMonsterObjectList;

#define MAX_GROUND  10
GameObject* gpGround[MAX_GROUND][MAX_GROUND];

// 弾管理用配列
GameObjectVector gShotManager;

// エフェクト管理用配列
GameObjectVector gEffectManager;

// モデルマネージャー
// 連想配列＝添え字に整数以外も使える配列
// テンプレート＝使うときに内部の型を決める
// mapの場合、前が添え字型、後ろが格納する要素の型
std::map<std::string, ModelData> gModelManager;


// Cameraクラスの変数
Camera* gpCamera;

// デルタタイム用の変数
DWORD gDeltaTime;


void vector_DrawAll(GameObjectVector vec)
{
	for (auto e : vec)
	{
		e->Draw();
	}
}

void vector_UpdateAll(GameObjectVector vec)
{
	for (auto e : vec)
	{
		e->Update();
	}
}

void vector_DeleteAll(GameObjectVector vec)
{
	for (auto e : vec)
	{
		delete e;
	}
}

// ビルボード用のポリゴンを作成し、テクスチャも読み込む
ModelData CreateSquarePolygon(float width, float height, float uvWidth, float uvHeight, const wchar_t* texFile)
{
	// モデル頂点データ作成
	const float w = width / 2.0f;
	const float h = height / 2.0f;
	ModelVertex vx[] = {
		{ -w,  h, 0, 0.0f, 0.0f },
		{  w,  h, 0, uvWidth, 0.0f },
		{  w, -h, 0, uvWidth, uvHeight },

		{  w, -h, 0, uvWidth, uvHeight },
		{ -w, -h, 0, 0.0f, uvHeight },
		{ -w,  h, 0, 0.0f, 0.0f },
	};

	vx[0].u = 0.0f;
	vx[0].v = 0.0f;
	vx[1].u = uvWidth;
	vx[1].v = 0.0f;
	vx[2].u = uvWidth;
	vx[2].v = uvHeight;
	vx[3].u = uvWidth;
	vx[3].v = uvHeight;
	vx[4].u = 0.0f;
	vx[4].v = uvHeight;
	vx[5].u = 0.0f;
	vx[5].v = 0.0f;

	ModelData modelData;

	modelData.mNumVertex = sizeof(vx) / sizeof(ModelVertex); // 頂点数を計算

	// モデル用の頂点バッファ作成
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.ByteWidth = sizeof(vx);
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	// バッファの初期値指定
	D3D11_SUBRESOURCE_DATA initialVertexData;
	// 頂点バッファの初期値
	initialVertexData.pSysMem = vx;
	// 頂点バッファでは使用しない
	initialVertexData.SysMemPitch = 0;
	initialVertexData.SysMemSlicePitch = 0;

	HRESULT hr = Direct3D_Get()->device->CreateBuffer(&bufferDesc, &initialVertexData, &modelData.mVertexBuffer);

	if (FAILED(hr))
	{
		throw hr;
	}

	Direct3D_LoadTexture(texFile, &modelData.mSRV);

	return modelData;
}

// WinMain関数を作る
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wc; // WND = Window

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_CLASSDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = CLASS_NAME;
	wc.hIconSm = NULL;

	RegisterClassEx(&wc); // Windowsにウインドウ情報を登録

	HWND hWnd; // H=Handle=ポインタ WND=Window
	hWnd = CreateWindowEx(0,// 拡張ウィンドウスタイル
		CLASS_NAME,// ウィンドウクラスの名前
		WINDOW_NAME,// ウィンドウの名前
		WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX,// ウィンドウスタイル
		CW_USEDEFAULT,// ウィンドウの左上Ｘ座標
		CW_USEDEFAULT,// ウィンドウの左上Ｙ座標 
		SCREEN_WIDTH,// ウィンドウの幅
		SCREEN_HEIGHT,// ウィンドウの高さ
		NULL,// 親ウィンドウのハンドル
		NULL,// メニューハンドルまたは子ウィンドウID
		hInstance,// インスタンスハンドル
		NULL);// ウィンドウ作成データ

	// 指定されたウィンドウの表示状態を設定(ウィンドウを表示)
	ShowWindow(hWnd, nCmdShow);
	// ウィンドウの状態を直ちに反映(ウィンドウのクライアント領域を更新)
	UpdateWindow(hWnd);

	// Direct3Dの初期化関数を呼び出す
	Direct3D_Init(hWnd);

	Game_Init();



	MSG msg;
	// メインループ
	for (;;) {
		// 前回のループからユーザー操作があったか調べる
		BOOL doesMessageExist = PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);

		if (doesMessageExist)
		{
			// 間接的にウインドウプロシージャを呼び出す
			DispatchMessage(&msg);

			// アプリ終了命令が来た
			if (msg.message == WM_QUIT) {
				break;
			}
		}
		else
		{
			// デルタタイムの計測
			static DWORD lastTime = timeGetTime(); // 前回計測時の時間
			timeBeginPeriod(1); // 精度を１ミリ秒に上げる
			DWORD nowTime = timeGetTime(); // 現在の時間
			timeEndPeriod(1); // 精度を元に戻す
			gDeltaTime = nowTime - lastTime; // 差分がデルタタイム
			lastTime = nowTime; // 前回計測時間として保存

			// ゲームループ
			Game_Update(); // ゲーム処理
			Game_Draw();   // ゲーム描画
			Input_Refresh(); // キー状態の更新
		}
	}

	Game_Release();
	// Direct3Dの解放関数を呼び出す
	Direct3D_Release();

	UnregisterClass(CLASS_NAME, hInstance);

	return (int)msg.wParam;
}


// ウインドウプロシージャ関数を作る
// ※関数を作れるのはグローバル領域(=どの関数の中でもない場所)だけ！
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// uMsg（この関数の第2引数）が、ユーザー操作のID情報
	switch (uMsg)
	{
	case WM_DESTROY:// ウィンドウ破棄のメッセージ
		PostQuitMessage(0);// “WM_QUIT”メッセージを送る　→　アプリ終了
		break;

	case WM_CLOSE:  // xボタンが押されたら
		DestroyWindow(hWnd);  // “WM_DESTROY”メッセージを送る
		break;

	case WM_LBUTTONDOWN: // 左クリックされたとき
		Input_SetKeyDown(VK_LBUTTON);
		break;

	case WM_LBUTTONUP: // 左クリックされたとき
		Input_SetKeyUp(VK_LBUTTON);
		break;

	case WM_RBUTTONDOWN: // 左クリックされたとき
		Input_SetKeyDown(VK_RBUTTON);
		break;

	case WM_RBUTTONUP: // 左クリックされたとき
		Input_SetKeyUp(VK_RBUTTON);
		break;

	case WM_MOUSEMOVE: // マウスカーソルが動いたとき
		break;

	case WM_KEYDOWN:
		// キーが押された時のリアクションを書く
		// ESCが押されたのかどうかチェック
		if (LOWORD(wParam) == VK_ESCAPE)
		{
			// メッセージボックスで修了確認
			int result;
			result = MessageBox(NULL, "終了してよろしいですか？",
				"終了確認", MB_YESNO | MB_ICONQUESTION);
			if (result == IDYES) // 「はい」ボタンが押された時
			{
				// xボタンが押されたのと同じ効果を発揮する
				PostMessage(hWnd, WM_CLOSE, wParam, lParam);
			}
		}
		Input_SetKeyDown(LOWORD(wParam));
		break;

	case WM_KEYUP: // キーが離されたイベント
		Input_SetKeyUp(LOWORD(wParam));
		break;

	default:
		// 上のcase以外の場合の処理を実行
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
		break;
	}

	return 0;
}

void Game_Init()
{
	// 定数バッファ作成
	// コンスタントバッファとして作成するための情報設定
	D3D11_BUFFER_DESC contstat_buffer_desc;
	contstat_buffer_desc.ByteWidth = 512;	// バッファのサイズ（4x4行列x4個）
	contstat_buffer_desc.Usage = D3D11_USAGE_DYNAMIC;		// 使用方法
	contstat_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;	// バッファの種類(コンスタントバッファ)
	contstat_buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;			// CPUアクセス設定
	contstat_buffer_desc.MiscFlags = 0;				// その他のオプション
	contstat_buffer_desc.StructureByteStride = 0;			// 構造体サイズ(行列を使うが今回は0でも動作することを実証する)

	Direct3D_Get()->device->CreateBuffer(&contstat_buffer_desc,
		nullptr, &gpConstBuffer);

	// カメラの作成
	gpCamera = new BackCamera();
	Camera::mMainCamera = gpCamera;
	// 初期値設定
	// 注意：eyeとfocusが同じだとダメ
	// 注意：upのxyz全てゼロだとダメ
	gpCamera->SetEye(XMFLOAT3(0.0f, 0.0f, -2.0f));
	gpCamera->SetFocus(XMFLOAT3(0.0f, 0.0f, 0.0f));
	gpCamera->SetUp(XMFLOAT3(0.0f, 1.0f, 0.0f));

	// モデル読み込み
	ObjModelLoader loader;
	gModelManager["cottage"] = loader.Load(
		"assets/cottage.obj", L"assets/cottage.png");

	// 銃モデル読み込み
	loader = ObjModelLoader();
	gModelManager["gun"] = loader.Load(
		"assets/gun.obj", L"assets/gun.png");

	// 銃用Modelオブジェクト生成
	gpGun = new NormalObject();
	Model* pGunModel = gpGun->GetModel();
	pGunModel->SetModelData(gModelManager["gun"]);
	pGunModel->SetScale(1.5f);
	pGunModel->mPos.y = 0.4f;
	pGunModel->mPos.z = 1.0f;
	pGunModel->mRotate.y = 90.0f;
	pGunModel->mCamera = gpCamera;

	// 地面モデル読み込み
	loader = ObjModelLoader();
	gModelManager["ground1"] = loader.Load(
		"assets/ground1.obj", L"assets/ground1.jpg"
	);

	// 地面生成
	for (int i = 0; i < MAX_GROUND; i++)
	{
		for (int j = 0; j < MAX_GROUND; j++)
		{
			gpGround[i][j] = new NormalObject();
			Model* pGroundModel = gpGround[i][j]->GetModel();
			pGroundModel->SetModelData(gModelManager["ground1"]);
			pGroundModel->SetScale(2.0f);
			pGroundModel->mPos.x = -20.0f + 4.0f*j;
			pGroundModel->mPos.z = -10.0f + 4.0f*i;
			pGroundModel->mPos.y = -2.0f;
			pGroundModel->mCamera = gpCamera;

			gObjectList.emplace_back(gpGround[i][j]);
		}
	}

	// 弾モデル読み込み
	loader = ObjModelLoader();
	gModelManager["shot"] = loader.Load(
		"assets/billboard.obj", L"assets/effect000.jpg"
	);

	// 弾用オブジェクト生成
	gpShot = new BillboardObject();
	Model* pShotModel = gpShot->GetModel();
	pShotModel->SetModelData(gModelManager["shot"]);
	pShotModel->SetScale(0.2f);
	pShotModel->mPos.y = 0.2f;
	pShotModel->mPos.z = 1.0f;
	pShotModel->mCamera = gpCamera;

	// 2Dキャラ用ポリゴン生成
	gModelManager["2Dchar1"] =
		CreateSquarePolygon(1.0f, 1.2f, 0.33f, 0.25f,
			L"assets/char01.png");
	gModelManager["2Dchar2"] =
		CreateSquarePolygon(1.0f, 1.2f, 0.33f, 0.25f,
			L"assets/char01.png");
	gModelManager["2Dchar3"] =
		CreateSquarePolygon(1.0f, 1.2f, 0.33f, 0.25f,
			L"assets/char01.png");

	// 2Dキャラ用オブジェクト生成
	gp2DChar[0] = new BillboardObject();
	Model* p2DModel = gp2DChar[0]->GetModel();
	p2DModel->SetModelData(gModelManager["2Dchar1"]);
	p2DModel->SetScale(1.0f);
	p2DModel->mPos.y = 0.6f;
	p2DModel->mPos.z = 5.0f;
	p2DModel->mPos.x = 5.0f;
	p2DModel->mCamera = gpCamera;
	HitSphere* pHit = gp2DChar[0]->GetHit();
	pHit->SetHankei(0.3f);// 当たり球の半径を設定
	HP* pHP = gp2DChar[0]->getHP();
	pHP->setHP(3);

	gp2DChar[1] = new BillboardObject();
	p2DModel = gp2DChar[1]->GetModel();
	p2DModel->SetModelData(gModelManager["2Dchar2"]);
	p2DModel->SetScale(1.0f);
	p2DModel->mPos.y = 0.6f;
	p2DModel->mPos.z = 8.0f;
	p2DModel->mPos.x = 5.0f;
	p2DModel->mCamera = gpCamera;
	pHit = gp2DChar[1]->GetHit();
	pHit->SetHankei(0.3f);// 当たり球の半径を設定
	pHP = gp2DChar[1]->getHP();
	pHP->setHP(3);

	gp2DChar[2] = new BillboardObject();
	p2DModel = gp2DChar[2]->GetModel();
	p2DModel->SetModelData(gModelManager["2Dchar3"]);
	p2DModel->SetScale(1.0f);
	p2DModel->mPos.y = 0.6f;
	p2DModel->mPos.z = 5.0f;
	p2DModel->mPos.x = 9.0f;
	p2DModel->mCamera = gpCamera;
	pHit = gp2DChar[2]->GetHit();
	pHit->SetHankei(0.3f);// 当たり球の半径を設定
	pHP = gp2DChar[2]->getHP();
	pHP->setHP(3);

	gMonsterObjectList.emplace_back(gp2DChar[0]);
	gMonsterObjectList.emplace_back(gp2DChar[1]);
	gMonsterObjectList.emplace_back(gp2DChar[2]);
	gObjectList.emplace_back(gpGun);

	// コテージ用オブジェクト生成
	for (int i = 0; i < 5; i++)
	{
		gpCottage = new NormalObject();
		Model* pCottageModel = gpCottage->GetModel();
		pCottageModel->SetModelData(gModelManager["cottage"]); // 3Dデータをセットする
		pCottageModel->SetScale(0.002f);
		pCottageModel->mPos.x = -30.0f + i * 6.5f;
		pCottageModel->mPos.z = 5.0f;
		pCottageModel->mPos.y = 0.0f;
		//gObjectList.emplace_back(gpCottage);
	}

	//エフェクト
	gModelManager["explosion"] = CreateSquarePolygon(1.0f, 1.0f, 0.25f, 0.25f, L"assets/explosion.png");
	GameObject* tmp = new Explosion();
	Model* pModel = tmp->GetModel();
	pModel->SetModelData(gModelManager["explosion"]);
	gObjectList.emplace_back(tmp);

	//タイトル
	gModelManager["title"] = CreateSquarePolygon(1.0f, 0.7f, 1.0f, 1.0f, L"assets/TitleLogo640.png");
	tmp = new NormalObject();
	pModel = tmp->GetModel();
	pModel->SetModelData(gModelManager["title"]);
	pModel->Set2dRender(true);
	pModel->SetDiffuse(XMFLOAT4(1, 1, 1, 0.5f));
	//pModel->mPos.y = 3.0f;
	//gObjectList.emplace_back(tmp);

	// カメラの追従ターゲットを指定
	//((BackCamera*)gpCamera)->SetTarget(gpGun);  // Cスタイルキャスト
	dynamic_cast<BackCamera*>(gpCamera)->SetTarget(gpGun); // C++キャスト
}



void Game_Draw()
{
	// DIRECT3D構造体にアクセスする
	DIRECT3D* d3d = Direct3D_Get();

	// 画面クリア（指定色で塗りつぶし）
	// 塗りつぶし色の指定（小数：0.0f～1.0f）
	float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f }; //red,green,blue,alpha

	d3d->context->ClearRenderTargetView(d3d->renderTarget, clearColor);

	// Zバッファ、ステンシルバッファクリア
	d3d->context->ClearDepthStencilView(
		d3d->depthStencilView,			// デプスステンシルビュー
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,			// Ｚバッファを1.0でクリアする(0.0f～1.0f)
		0);				// ステンシルバッファを0でクリアする

	// ↓　自前の描画処理をここに書く *******

	// ゲームオブジェクトを描画
	d3d->context->OMSetBlendState(d3d->blendAlpha, NULL, 0xffffffff);// アルファブレンド

	vector_DrawAll(gObjectList);
	vector_DrawAll(gMonsterObjectList);

	d3d->context->OMSetBlendState(d3d->blendAdd, NULL, 0xffffffff);// 加算合成
	// 弾管理配列の中身を全てDrawする
	vector_DrawAll(gShotManager);

	vector_DrawAll(gEffectManager);

	// ダブルバッファの切り替え
	d3d->swapChain->Present(0, 0);
}

void Game_Update()
{
	// デルタタイムが想定外の値になった場合の処理
	if (gDeltaTime >= 100)
	{
		gDeltaTime = 0;
	}
	if (gDeltaTime <= 0)
	{
		gDeltaTime = 1;
	}

	// カメラ移動変数
	static float angle = 0.0f; // 回転角度
	static float zoom = 3.0f;  // ズーム

	// ①カメラの位置をキー操作で移動する
	if (Input_GetKeyDown(VK_RIGHT))
	{
		angle += 0.06f * gDeltaTime;
	}
	if (Input_GetKeyDown(VK_LEFT))
	{
		angle -= 0.06f * gDeltaTime;
	}
	// ズーム操作
	if (Input_GetKeyDown(VK_UP))
		zoom -= 0.01f * gDeltaTime;
	if (Input_GetKeyDown(VK_DOWN))
		zoom += 0.01f * gDeltaTime;

	// ②カメラの注視点を中心にカメラを回転する

	// カメラ位置X　＝　sinf(角度ラジアン)
	// カメラ位置Z　＝　cosf(角度ラジアン)
	// 原点を中心に半径1.0fの円周上の点を求める
	Model* pCottageModel = gpCottage->GetModel();
	float radian = XMConvertToRadians(angle);
	gpCamera->mEye.x =
		sinf(radian) * zoom + pCottageModel->mPos.x;
	gpCamera->mEye.z =
		cosf(radian) * zoom + pCottageModel->mPos.z;
	gpCamera->mEye.y = 2.0f;

	// カメラ注視点をコテージの位置にする
	gpCamera->SetFocus(pCottageModel->mPos);

	// キャラクター移動
	// キャラクターが向いている方向に前進する
	// 向き変更＝ADキー　前進＝Wキー
	// 「前向きベクトル」を計算する
	// 移動速度＝Wキーで決まる
	gpGun->mSpeed = 0.0f;

	float speed = 0.002f;

	// 銃の前進
	if (Input_GetKeyDown('W'))
		gpGun->mSpeed = speed * gDeltaTime;
	if (Input_GetKeyDown('S'))
		gpGun->mSpeed = -speed * gDeltaTime;

	Model* pGunModel = gpGun->GetModel();

	float rotSpeed = 0.09f;
	// 銃の方向転換
	if (Input_GetKeyDown('A') && !Input_GetKeyDown('S')) {
		pGunModel->mRotate.y += -rotSpeed * gDeltaTime;
	}
	else if (Input_GetKeyDown('A') && Input_GetKeyDown('S')) {
		pGunModel->mRotate.y += rotSpeed * gDeltaTime;
	}
	if (Input_GetKeyDown('D') && !Input_GetKeyDown('S')) {
		pGunModel->mRotate.y += rotSpeed * gDeltaTime;
	}
	else if (Input_GetKeyDown('D') && Input_GetKeyDown('S')) {
		pGunModel->mRotate.y += -rotSpeed * gDeltaTime;
	}

	// 弾の発射
	if (Input_GetKeyTrigger(VK_LBUTTON))
	{
		// 弾オブジェクト生成
		GameObject* pShot = new BillboardObject();
		Model* pShotModel = pShot->GetModel();
		pShotModel->SetModelData(gModelManager["shot"]);
		pShotModel->SetScale(0.2f);
		pShot->GetHit()->SetHankei(0.19f);

		// 発射地点に移動させる
		XMFLOAT3 forwardVec = gpGun->GetForwardVector();

		pShotModel->mPos.x =
			pGunModel->mPos.x + forwardVec.x * 0.8f;
		pShotModel->mPos.y = pGunModel->mPos.y + 0.2f;
		pShotModel->mPos.z =
			pGunModel->mPos.z + forwardVec.z * 0.8f;
		// 弾に速度を設定する
		pShot->mSpeed = 0.01f;
		// 弾が飛ぶ方向を設定
		pShotModel->mRotate.y = pGunModel->mRotate.y;

		// 弾管理配列に追加する
		gShotManager.emplace_back(pShot);
	}

	vector_UpdateAll(gObjectList);
	vector_UpdateAll(gMonsterObjectList);

	// 弾管理配列の中身を全てUpdateする
	vector_UpdateAll(gShotManager);

	// 弾の回収処理
	for (int i = 0; i < gShotManager.size(); i++)
	{
		// (a)カメラの位置
		XMFLOAT3 cameraPos = gpCamera->mEye;
		// (b)弾の位置
		XMFLOAT3 shotPos = gShotManager[i]->GetModel()->mPos;
		// (b)→(a)ベクトル
		XMVECTOR abVector =
			XMLoadFloat3(&cameraPos) - XMLoadFloat3(&shotPos);
		// (b)→(a)ベクトルの長さを計算
		XMVECTOR abVecLength = XMVector3Length(abVector);
		float abLength = XMVectorGetX(abVecLength);
		// abLengthが○○以上なら弾を回収する
		if (abLength >= 50.0f)
		{
			// erase関数だけでは弾のdeleteは行われない
			delete gShotManager[i];
			// 弾管理配列から弾を削除
			gShotManager.erase(gShotManager.begin() + i);
		}
	}

	// 弾とNPC当たり判定
	if (gMonsterObjectList.size() != 0) {

		for (int i = 0; i < gShotManager.size(); i++)
		{
			HitSphere* pShotHit = gShotManager[i]->GetHit();

			std::vector<decltype(gMonsterObjectList)::value_type> tes;
			for (auto & e : gMonsterObjectList) {

				if (pShotHit->IsHit(e->GetHit()))
				{
					delete gShotManager[i];
					// 弾管理配列から弾を削除
					gShotManager.erase(gShotManager.begin() + i);
					i--;


					// emitter
					int num = rand() % 4 + 3;
					for (int j = 0; j < num; j++)
					{
						GameObject* tmp = new Explosion();
						Model* pModel = tmp->GetModel();
						pModel->SetModelData(gModelManager["explosion"]);
						pModel->mPos = e->GetModel()->mPos;
						pModel->mPos.x += static_cast<float>(rand() % 100 - 50) / 100.0f;
						pModel->mPos.y += static_cast<float>(rand() % 100 - 50) / 100.0f;
						pModel->mPos.z += static_cast<float>(rand() % 100 - 50) / 100.0f;
						//tmp->mSpeed = static_cast<float>(rand() % 100) / 100000.0f;
						gEffectManager.emplace_back(tmp);
					}

					e->getHP()->addHP(-1);
					if (e->getHP()->checkDead()) {
						tes.emplace_back(e);
					}
					break;
				}
			}

			
			for (auto hoge : tes) {
				for (auto a = gMonsterObjectList.begin(); a != gMonsterObjectList.end(); ) {
					if (*a == hoge) {
						a = gMonsterObjectList.erase(a);
					}
					else {
						a++;
					}
				}
				delete hoge;
			}



		}

	}

	vector_UpdateAll(gEffectManager);

	// 再生が終わったエフェクトを消す
	for (int i = 0; i < gEffectManager.size(); i++)
	{
		Explosion* e = dynamic_cast<Explosion*>(gEffectManager[i]);
		if (e->IsFinished())
		{
			delete gEffectManager[i];
			// 弾管理配列から弾を削除
			gEffectManager.erase(gEffectManager.begin() + i);
			i--;
		}
	}

	// カメラ更新（ビュー変換行列計算）
	gpCamera->Update();
}

void Game_Release()
{
	vector_DeleteAll(gObjectList);
	vector_DeleteAll(gMonsterObjectList);

	// 弾管理配列の中身を全てdeleteする
	vector_DeleteAll(gShotManager);

	vector_DeleteAll(gEffectManager);

	COM_SAFE_RELEASE(gpConstBuffer);

	// モデルマネージャーが持つ全モデルを解放
	for (auto i = gModelManager.begin();//連想配列の先頭情報
		i != gModelManager.end();//連想配列の最後尾情報
		i++)
	{
		// first … 添え字
		// second … 要素そのもの
		COM_SAFE_RELEASE(i->second.mSRV);//テクスチャ
		COM_SAFE_RELEASE(i->second.mVertexBuffer);//頂点バッファ
	}
	// 連想配列の全要素を削除する（要素数０にする）
	gModelManager.clear();
}
