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
	// ���f���ǂݍ���
	ObjModelLoader loader;
	gModelManager["cottage"] = loader.Load(
		"assets/cottage.obj", L"assets/cottage.png");

	// �e���f���ǂݍ���
	loader = ObjModelLoader();
	gModelManager["gun"] = loader.Load(
		"assets/gun.obj", L"assets/gun.png");


	// �n�ʃ��f���ǂݍ���
	loader = ObjModelLoader();
	gModelManager["ground1"] = loader.Load(
		"assets/ground1.obj", L"assets/ground1.jpg"
	);



	// �e���f���ǂݍ���
	loader = ObjModelLoader();
	gModelManager["shot"] = loader.Load(
		"assets/billboard.obj", L"assets/effect000.jpg"
	);

	// �J�����̍쐬
	gpCamera = new BackCamera();
	Camera::mMainCamera = gpCamera;
	// �����l�ݒ�
	// ���ӁFeye��focus���������ƃ_��
	// ���ӁFup��xyz�S�ă[�����ƃ_��
	gpCamera->SetEye(DirectX::XMFLOAT3(0.0f, 0.0f, -2.0f));
	gpCamera->SetFocus(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
	gpCamera->SetUp(DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f));

	// 2D�L�����p�|���S������
	gModelManager["2Dchar1"] =
		IScene::CreateSquarePolygon(1.0f, 1.2f, 0.33f, 0.25f,
			L"assets/char01.png");
	gModelManager["2Dchar2"] =
		IScene::CreateSquarePolygon(1.0f, 1.2f, 0.33f, 0.25f,
			L"assets/char01.png");
	gModelManager["2Dchar3"] =
		IScene::CreateSquarePolygon(1.0f, 1.2f, 0.33f, 0.25f,
			L"assets/char01.png");

	// 2D�L�����p�I�u�W�F�N�g����
	gp2DChar[0] = new BillboardObject();
	Model* p2DModel = gp2DChar[0]->GetModel();
	p2DModel->SetModelData(gModelManager["2Dchar1"]);
	p2DModel->SetScale(1.0f);
	p2DModel->mPos.y = 0.6f;
	p2DModel->mPos.z = 5.0f;
	p2DModel->mPos.x = 5.0f;
	p2DModel->mCamera = gpCamera;
	HitSphere* pHit = gp2DChar[0]->GetHit();
	pHit->SetHankei(0.3f);// �����苅�̔��a��ݒ�
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
	pHit->SetHankei(0.3f);// �����苅�̔��a��ݒ�
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
	pHit->SetHankei(0.3f);// �����苅�̔��a��ݒ�
	pHP = gp2DChar[2]->getHP();
	pHP->setHP(3);

	gMonsterObjectList.emplace_back(gp2DChar[0]);
	gMonsterObjectList.emplace_back(gp2DChar[1]);
	gMonsterObjectList.emplace_back(gp2DChar[2]);
	gObjectList.emplace_back(gpGun);

	// �e�pModel�I�u�W�F�N�g����
	gpGun = new NormalObject();
	Model* pGunModel = gpGun->GetModel();
	pGunModel->SetModelData(gModelManager["gun"]);
	pGunModel->SetScale(1.5f);
	pGunModel->mPos.y = 0.4f;
	pGunModel->mPos.z = 1.0f;
	pGunModel->mRotate.y = 90.0f;
	pGunModel->mCamera = gpCamera;

	// �e�p�I�u�W�F�N�g����
	gpShot = new BillboardObject();
	Model* pShotModel = gpShot->GetModel();
	pShotModel->SetModelData(gModelManager["shot"]);
	pShotModel->SetScale(0.2f);
	pShotModel->mPos.y = 0.2f;
	pShotModel->mPos.z = 1.0f;
	pShotModel->mCamera = gpCamera;

	// �n�ʐ���
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

	//�G�t�F�N�g
	gModelManager["explosion"] = IScene::CreateSquarePolygon(1.0f, 1.0f, 0.25f, 0.25f, L"assets/explosion.png");
	GameObject* tmp = new Explosion();
	Model* pModel = tmp->GetModel();
	pModel->SetModelData(gModelManager["explosion"]);
	gObjectList.emplace_back(tmp);

	//�^�C�g��
	gModelManager["title"] = IScene::CreateSquarePolygon(1.0f, 0.7f, 1.0f, 1.0f, L"assets/TitleLogo640.png");
	tmp = new NormalObject();
	pModel = tmp->GetModel();
	pModel->SetModelData(gModelManager["title"]);
	pModel->Set2dRender(true);
	pModel->SetDiffuse(DirectX::XMFLOAT4(1, 1, 1, 0.5f));
	pModel->mPos.y = 3.0f;
	gObjectList.emplace_back(tmp);

	// �J�����̒Ǐ]�^�[�Q�b�g���w��
	//((BackCamera*)gpCamera)->SetTarget(gpGun);  // C�X�^�C���L���X�g
	dynamic_cast<BackCamera*>(gpCamera)->SetTarget(gpGun); // C++�L���X�g
}

void SceneGame::Update()
{
	// �J�����ړ��ϐ�
	static float angle = 0.0f; // ��]�p�x
	static float zoom = 3.0f;  // �Y�[��

	// �@�J�����̈ʒu���L�[����ňړ�����
	if (Input_GetKeyDown(VK_RIGHT))
	{
		angle += 0.06f * gDeltaTime;
	}
	if (Input_GetKeyDown(VK_LEFT))
	{
		angle -= 0.06f * gDeltaTime;
	}
	// �Y�[������
	if (Input_GetKeyDown(VK_UP))
		zoom -= 0.01f * gDeltaTime;
	if (Input_GetKeyDown(VK_DOWN))
		zoom += 0.01f * gDeltaTime;

	// �A�J�����̒����_�𒆐S�ɃJ��������]����

	// �J�����ʒuX�@���@sinf(�p�x���W�A��)
	// �J�����ʒuZ�@���@cosf(�p�x���W�A��)
	// ���_�𒆐S�ɔ��a1.0f�̉~����̓_�����߂�
	Model* pGunModel = gpGun->GetModel();
	float radian = DirectX::XMConvertToRadians(angle);
	gpCamera->mEye.x =
		sinf(radian) * zoom + pGunModel->mPos.x;
	gpCamera->mEye.z =
		cosf(radian) * zoom + pGunModel->mPos.z;
	gpCamera->mEye.y = 2.0f;

	// �J���������_���R�e�[�W�̈ʒu�ɂ���
	gpCamera->SetFocus(pGunModel->mPos);

	// �L�����N�^�[�ړ�
	// �L�����N�^�[�������Ă�������ɑO�i����
	// �����ύX��AD�L�[�@�O�i��W�L�[
	// �u�O�����x�N�g���v���v�Z����
	// �ړ����x��W�L�[�Ō��܂�
	gpGun->mSpeed = 0.0f;

	float speed = 0.002f;

	// �e�̑O�i
	if (Input_GetKeyDown('W'))
		gpGun->mSpeed = speed * gDeltaTime;
	if (Input_GetKeyDown('S'))
		gpGun->mSpeed = -speed * gDeltaTime;

	//Model* pGunModel = gpGun->GetModel();

	float rotSpeed = 0.09f;
	// �e�̕����]��
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

	// �e�̔���
	if (Input_GetKeyTrigger(VK_LBUTTON))
	{
		// �e�I�u�W�F�N�g����
		GameObject* pShot = new BillboardObject();
		Model* pShotModel = pShot->GetModel();
		pShotModel->SetModelData(gModelManager["shot"]);
		pShotModel->SetScale(0.2f);
		pShot->GetHit()->SetHankei(0.19f);

		// ���˒n�_�Ɉړ�������
		DirectX::XMFLOAT3 forwardVec = gpGun->GetForwardVector();

		pShotModel->mPos.x =
			pGunModel->mPos.x + forwardVec.x * 0.8f;
		pShotModel->mPos.y = pGunModel->mPos.y + 0.2f;
		pShotModel->mPos.z =
			pGunModel->mPos.z + forwardVec.z * 0.8f;
		// �e�ɑ��x��ݒ肷��
		pShot->mSpeed = 0.01f;
		// �e����ԕ�����ݒ�
		pShotModel->mRotate.y = pGunModel->mRotate.y;

		// �e�Ǘ��z��ɒǉ�����
		gShotManager.emplace_back(pShot);
	}

//	vector_UpdateAll(gObjectList);

	for (auto e : gObjectList)
	{
		e->Update();
	}
	vector_UpdateAll(gMonsterObjectList);

	// �e�Ǘ��z��̒��g��S��Update����
	vector_UpdateAll(gShotManager);

	

	// �e�̉������
	for (int i = 0; i < gShotManager.size(); i++)
	{
		// (a)�J�����̈ʒu
		DirectX::XMFLOAT3 cameraPos = gpCamera->mEye;
		// (b)�e�̈ʒu
		DirectX::XMFLOAT3 shotPos = gShotManager[i]->GetModel()->mPos;
		// (b)��(a)�x�N�g��
		DirectX::XMVECTOR abVector = XMLoadFloat3(&cameraPos) - XMLoadFloat3(&shotPos);
		// (b)��(a)�x�N�g���̒������v�Z
		DirectX::XMVECTOR abVecLength = DirectX::XMVector3Length(abVector);
		float abLength = DirectX::XMVectorGetX(abVecLength);
		// abLength�������ȏ�Ȃ�e���������
		if (abLength >= 50.0f)
		{
			// erase�֐������ł͒e��delete�͍s���Ȃ�
			delete gShotManager[i];
			// �e�Ǘ��z�񂩂�e���폜
			gShotManager.erase(gShotManager.begin() + i);
		}
	}

	// �e��NPC�����蔻��
	if (gMonsterObjectList.size() != 0) {

		for (int i = 0; i < gShotManager.size(); i++)
		{
			HitSphere* pShotHit = gShotManager[i]->GetHit();

			std::vector<decltype(gMonsterObjectList)::value_type> tes;
			for (auto & e : gMonsterObjectList) {

				if (pShotHit->IsHit(e->GetHit()))
				{
					delete gShotManager[i];
					// �e�Ǘ��z�񂩂�e���폜
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

	// �Đ����I������G�t�F�N�g������
	for (int i = 0; i < gEffectManager.size(); i++)
	{
		Explosion* e = dynamic_cast<Explosion*>(gEffectManager[i]);
		if (e->IsFinished())
		{
			delete gEffectManager[i];
			// �e�Ǘ��z�񂩂�e���폜
			gEffectManager.erase(gEffectManager.begin() + i);
			i--;
		}
	}

	// �J�����X�V�i�r���[�ϊ��s��v�Z�j
	gpCamera->Update();
}

void SceneGame::Draw()
{

	// DIRECT3D�\���̂ɃA�N�Z�X����
	DIRECT3D* d3d = Direct3D_Get();

	// �Q�[���I�u�W�F�N�g��`��
	d3d->context->OMSetBlendState(d3d->blendAlpha, NULL, 0xffffffff);// �A���t�@�u�����h

	vector_DrawAll(gObjectList);
	vector_DrawAll(gMonsterObjectList);

	d3d->context->OMSetBlendState(d3d->blendAdd, NULL, 0xffffffff);// ���Z����
	// �e�Ǘ��z��̒��g��S��Draw����
	vector_DrawAll(gShotManager);

	vector_DrawAll(gEffectManager);
}

void SceneGame::End()
{
	vector_DeleteAll(gObjectList);
	vector_DeleteAll(gMonsterObjectList);

	// �e�Ǘ��z��̒��g��S��delete����
	vector_DeleteAll(gShotManager);

	vector_DeleteAll(gEffectManager);

	// ���f���}�l�[�W���[�����S���f�������
	for (auto i = gModelManager.begin();//�A�z�z��̐擪���
		i != gModelManager.end();//�A�z�z��̍Ō�����
		i++)
	{
		// first �c �Y����
		// second �c �v�f���̂���
		COM_SAFE_RELEASE(i->second.mSRV);//�e�N�X�`��
		COM_SAFE_RELEASE(i->second.mVertexBuffer);//���_�o�b�t�@
	}
	// �A�z�z��̑S�v�f���폜����i�v�f���O�ɂ���j
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