#include "SceneGame.h"
#include <DirectXMath.h>
#include "Input.h"
#include "Model.h"
#include "ObjModelLoader.h"
#include "NormalObject.h"
#include "BillboardObject.h"
#include "Explosion.h"
#include "BackCamera.h"

extern DWORD gDeltaTime;

using namespace DirectX;

void SceneGame::Init()
{
	// モデル読み込み
	ObjModelLoader loader;
	gModelManager["cottage"] = loader.Load(
		"assets/cottage.obj", L"assets/cottage.png");

	// 銃モデル読み込み
	loader = ObjModelLoader();
	gModelManager["gun"] = loader.Load(
		"assets/gun.obj", L"assets/gun.png");


	// 地面モデル読み込み
	loader = ObjModelLoader();
	gModelManager["ground1"] = loader.Load(
		"assets/ground1.obj", L"assets/ground1.jpg"
	);



	// 弾モデル読み込み
	loader = ObjModelLoader();
	gModelManager["shot"] = loader.Load(
		"assets/billboard.obj", L"assets/effect000.jpg"
	);

	// カメラの作成
	gpCamera = new BackCamera();
	Camera::mMainCamera = gpCamera;
	// 初期値設定
	// 注意：eyeとfocusが同じだとダメ
	// 注意：upのxyz全てゼロだとダメ
	gpCamera->SetEye(DirectX::XMFLOAT3(0.0f, 0.0f, -2.0f));
	gpCamera->SetFocus(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
	gpCamera->SetUp(DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f));

	// 2Dキャラ用ポリゴン生成
	gModelManager["2Dchar1"] =
		IScene::CreateSquarePolygon(1.0f, 1.2f, 0.33f, 0.25f,
			L"assets/char01.png");
	gModelManager["2Dchar2"] =
		IScene::CreateSquarePolygon(1.0f, 1.2f, 0.33f, 0.25f,
			L"assets/char01.png");
	gModelManager["2Dchar3"] =
		IScene::CreateSquarePolygon(1.0f, 1.2f, 0.33f, 0.25f,
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

	// 銃用Modelオブジェクト生成
	gpGun = new NormalObject();
	Model* pGunModel = gpGun->GetModel();
	pGunModel->SetModelData(gModelManager["gun"]);
	pGunModel->SetScale(1.5f);
	pGunModel->mPos.y = 0.4f;
	pGunModel->mPos.z = 1.0f;
	pGunModel->mRotate.y = 90.0f;
	pGunModel->mCamera = gpCamera;

	// 弾用オブジェクト生成
	gpShot = new BillboardObject();
	Model* pShotModel = gpShot->GetModel();
	pShotModel->SetModelData(gModelManager["shot"]);
	pShotModel->SetScale(0.2f);
	pShotModel->mPos.y = 0.2f;
	pShotModel->mPos.z = 1.0f;
	pShotModel->mCamera = gpCamera;

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

	//エフェクト
	gModelManager["explosion"] = IScene::CreateSquarePolygon(1.0f, 1.0f, 0.25f, 0.25f, L"assets/explosion.png");
	GameObject* tmp = new Explosion();
	Model* pModel = tmp->GetModel();
	pModel->SetModelData(gModelManager["explosion"]);
	gObjectList.emplace_back(tmp);

	//タイトル
	gModelManager["title"] = IScene::CreateSquarePolygon(1.0f, 0.7f, 1.0f, 1.0f, L"assets/TitleLogo640.png");
	tmp = new NormalObject();
	pModel = tmp->GetModel();
	pModel->SetModelData(gModelManager["title"]);
	pModel->Set2dRender(true);
	pModel->SetDiffuse(DirectX::XMFLOAT4(1, 1, 1, 0.5f));
	pModel->mPos.y = 3.0f;
	gObjectList.emplace_back(tmp);

	// カメラの追従ターゲットを指定
	//((BackCamera*)gpCamera)->SetTarget(gpGun);  // Cスタイルキャスト
	dynamic_cast<BackCamera*>(gpCamera)->SetTarget(gpGun); // C++キャスト
}

void SceneGame::Update()
{
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
	Model* pGunModel = gpGun->GetModel();
	float radian = DirectX::XMConvertToRadians(angle);
	gpCamera->mEye.x =
		sinf(radian) * zoom + pGunModel->mPos.x;
	gpCamera->mEye.z =
		cosf(radian) * zoom + pGunModel->mPos.z;
	gpCamera->mEye.y = 2.0f;

	// カメラ注視点をコテージの位置にする
	gpCamera->SetFocus(pGunModel->mPos);

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

	//Model* pGunModel = gpGun->GetModel();

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
		DirectX::XMFLOAT3 forwardVec = gpGun->GetForwardVector();

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

//	vector_UpdateAll(gObjectList);

	for (auto e : gObjectList)
	{
		e->Update();
	}
	vector_UpdateAll(gMonsterObjectList);

	// 弾管理配列の中身を全てUpdateする
	vector_UpdateAll(gShotManager);

	

	// 弾の回収処理
	for (int i = 0; i < gShotManager.size(); i++)
	{
		// (a)カメラの位置
		DirectX::XMFLOAT3 cameraPos = gpCamera->mEye;
		// (b)弾の位置
		DirectX::XMFLOAT3 shotPos = gShotManager[i]->GetModel()->mPos;
		// (b)→(a)ベクトル
		DirectX::XMVECTOR abVector = XMLoadFloat3(&cameraPos) - XMLoadFloat3(&shotPos);
		// (b)→(a)ベクトルの長さを計算
		DirectX::XMVECTOR abVecLength = DirectX::XMVector3Length(abVector);
		float abLength = DirectX::XMVectorGetX(abVecLength);
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

void SceneGame::Draw()
{

	// DIRECT3D構造体にアクセスする
	DIRECT3D* d3d = Direct3D_Get();

	// ゲームオブジェクトを描画
	d3d->context->OMSetBlendState(d3d->blendAlpha, NULL, 0xffffffff);// アルファブレンド

	vector_DrawAll(gObjectList);
	vector_DrawAll(gMonsterObjectList);

	d3d->context->OMSetBlendState(d3d->blendAdd, NULL, 0xffffffff);// 加算合成
	// 弾管理配列の中身を全てDrawする
	vector_DrawAll(gShotManager);

	vector_DrawAll(gEffectManager);
}

void SceneGame::End()
{
	vector_DeleteAll(gObjectList);
	vector_DeleteAll(gMonsterObjectList);

	// 弾管理配列の中身を全てdeleteする
	vector_DeleteAll(gShotManager);

	vector_DeleteAll(gEffectManager);

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


void SceneGame::vector_DrawAll(GameObjectVector& vec)
{
	for (auto e : vec)
	{
		e->Draw();
	}
}

void SceneGame::vector_UpdateAll(GameObjectVector& vec)
{
	for (auto e : vec)
	{
		e->Update();
	}
}

void SceneGame::vector_DeleteAll(GameObjectVector& vec)
{
	for (auto e : vec)
	{
		delete e;
	}
}