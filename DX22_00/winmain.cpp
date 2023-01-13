#undef UNICODE  // Unicode�ł͂Ȃ��A�}���`�o�C�g�������g��

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
#include <map> // �A�z�z��
#include <vector> // �ϔz��
#include<algorithm>


#pragma comment (lib, "winmm.lib") // timeGetTime�֐��̂���

// �}�N����`
#define CLASS_NAME    "DX21Smpl"// �E�C���h�E�N���X�̖��O
#define WINDOW_NAME  "�X�P���g���v���O����"// �E�B���h�E�̖��O

#define SCREEN_WIDTH (1024)	// �E�C���h�E�̕�
#define SCREEN_HEIGHT (576+30)	// �E�C���h�E�̍���

// �\���̂̒�`


// �֐��̃v���g�^�C�v�錾
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// �Q�[���̉�ʕ`��̊֐�
void Game_Init();
void Game_Draw();
void Game_Update();

void titleUpdate();
void titleDraw();
void gameUpdate();
void gameDraw();
void resultUpdate();
void resultDraw();

void Game_Release();



// �O���[�o���ϐ��̐錾

// ���_�o�b�t�@�p�̕ϐ�
ID3D11Buffer* gpVertexBuffer;

// ���_�������ϐ�
int gNumVertex;

// �e�N�X�`���p�̕ϐ�
ID3D11ShaderResourceView* gpTextureHalOsaka; // HAL OSAKA


extern ID3D11Buffer* gpConstBuffer;

using namespace DirectX;

// GameObject�N���X�̕ϐ�
GameObject* gpCottage;
GameObject* gpGun;
GameObject* gpShot;
#define MAX_CHAR  3
// 2D�L�����̕ϐ�	����ւ����ړ��p��+1
GameObject* gp2DChar[MAX_CHAR];
GameObject* gpShadow;

typedef std::vector<GameObject*> GameObjectVector;

GameObjectVector gObjectList;
GameObjectVector gMonsterObjectList;

#define MAX_GROUND  10
GameObject* gpGround[MAX_GROUND][MAX_GROUND];

// �e�Ǘ��p�z��
GameObjectVector gShotManager;

// �G�t�F�N�g�Ǘ��p�z��
GameObjectVector gEffectManager;

// ���f���}�l�[�W���[
// �A�z�z�񁁓Y�����ɐ����ȊO���g����z��
// �e���v���[�g���g���Ƃ��ɓ����̌^�����߂�
// map�̏ꍇ�A�O���Y�����^�A��낪�i�[����v�f�̌^
std::map<std::string, ModelData> gModelManager;


// Camera�N���X�̕ϐ�
Camera* gpCamera;

// �f���^�^�C���p�̕ϐ�
DWORD gDeltaTime;

//�V�[���؂�ւ��p
enum eScene {
	Title,
	Game,
	Result,
};

int sceneNum = eScene::Game;

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

// �r���{�[�h�p�̃|���S�����쐬���A�e�N�X�`�����ǂݍ���
ModelData CreateSquarePolygon(float width, float height, float uvWidth, float uvHeight, const wchar_t* texFile)
{
	// ���f�����_�f�[�^�쐬
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

	modelData.mNumVertex = sizeof(vx) / sizeof(ModelVertex); // ���_�����v�Z

	// ���f���p�̒��_�o�b�t�@�쐬
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.ByteWidth = sizeof(vx);
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	// �o�b�t�@�̏����l�w��
	D3D11_SUBRESOURCE_DATA initialVertexData;
	// ���_�o�b�t�@�̏����l
	initialVertexData.pSysMem = vx;
	// ���_�o�b�t�@�ł͎g�p���Ȃ�
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

// WinMain�֐������
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

	RegisterClassEx(&wc); // Windows�ɃE�C���h�E����o�^

	HWND hWnd; // H=Handle=�|�C���^ WND=Window
	hWnd = CreateWindowEx(0,// �g���E�B���h�E�X�^�C��
		CLASS_NAME,// �E�B���h�E�N���X�̖��O
		WINDOW_NAME,// �E�B���h�E�̖��O
		WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX,// �E�B���h�E�X�^�C��
		CW_USEDEFAULT,// �E�B���h�E�̍���w���W
		CW_USEDEFAULT,// �E�B���h�E�̍���x���W 
		SCREEN_WIDTH,// �E�B���h�E�̕�
		SCREEN_HEIGHT,// �E�B���h�E�̍���
		NULL,// �e�E�B���h�E�̃n���h��
		NULL,// ���j���[�n���h���܂��͎q�E�B���h�EID
		hInstance,// �C���X�^���X�n���h��
		NULL);// �E�B���h�E�쐬�f�[�^

	// �w�肳�ꂽ�E�B���h�E�̕\����Ԃ�ݒ�(�E�B���h�E��\��)
	ShowWindow(hWnd, nCmdShow);
	// �E�B���h�E�̏�Ԃ𒼂��ɔ��f(�E�B���h�E�̃N���C�A���g�̈���X�V)
	UpdateWindow(hWnd);

	// Direct3D�̏������֐����Ăяo��
	Direct3D_Init(hWnd);

	Game_Init();



	MSG msg;
	// ���C�����[�v
	for (;;) {
		// �O��̃��[�v���烆�[�U�[���삪�����������ׂ�
		BOOL doesMessageExist = PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);

		if (doesMessageExist)
		{
			// �ԐړI�ɃE�C���h�E�v���V�[�W�����Ăяo��
			DispatchMessage(&msg);

			// �A�v���I�����߂�����
			if (msg.message == WM_QUIT) {
				break;
			}
		}
		else
		{
			// �f���^�^�C���̌v��
			static DWORD lastTime = timeGetTime(); // �O��v�����̎���
			timeBeginPeriod(1); // ���x���P�~���b�ɏグ��
			DWORD nowTime = timeGetTime(); // ���݂̎���
			timeEndPeriod(1); // ���x�����ɖ߂�
			gDeltaTime = nowTime - lastTime; // �������f���^�^�C��
			lastTime = nowTime; // �O��v�����ԂƂ��ĕۑ�

			// �Q�[�����[�v
			Game_Update(); // �Q�[������
			Game_Draw();   // �Q�[���`��
			Input_Refresh(); // �L�[��Ԃ̍X�V
		}
	}

	Game_Release();
	// Direct3D�̉���֐����Ăяo��
	Direct3D_Release();

	UnregisterClass(CLASS_NAME, hInstance);

	return (int)msg.wParam;
}


// �E�C���h�E�v���V�[�W���֐������
// ���֐�������̂̓O���[�o���̈�(=�ǂ̊֐��̒��ł��Ȃ��ꏊ)�����I
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// uMsg�i���̊֐��̑�2�����j���A���[�U�[�����ID���
	switch (uMsg)
	{
	case WM_DESTROY:// �E�B���h�E�j���̃��b�Z�[�W
		PostQuitMessage(0);// �gWM_QUIT�h���b�Z�[�W�𑗂�@���@�A�v���I��
		break;

	case WM_CLOSE:  // x�{�^���������ꂽ��
		DestroyWindow(hWnd);  // �gWM_DESTROY�h���b�Z�[�W�𑗂�
		break;

	case WM_LBUTTONDOWN: // ���N���b�N���ꂽ�Ƃ�
		Input_SetKeyDown(VK_LBUTTON);
		break;

	case WM_LBUTTONUP: // ���N���b�N���ꂽ�Ƃ�
		Input_SetKeyUp(VK_LBUTTON);
		break;

	case WM_RBUTTONDOWN: // ���N���b�N���ꂽ�Ƃ�
		Input_SetKeyDown(VK_RBUTTON);
		break;

	case WM_RBUTTONUP: // ���N���b�N���ꂽ�Ƃ�
		Input_SetKeyUp(VK_RBUTTON);
		break;

	case WM_MOUSEMOVE: // �}�E�X�J�[�\�����������Ƃ�
		break;

	case WM_KEYDOWN:
		// �L�[�������ꂽ���̃��A�N�V����������
		// ESC�������ꂽ�̂��ǂ����`�F�b�N
		if (LOWORD(wParam) == VK_ESCAPE)
		{
			// ���b�Z�[�W�{�b�N�X�ŏC���m�F
			int result;
			result = MessageBox(NULL, "�I�����Ă�낵���ł����H",
				"�I���m�F", MB_YESNO | MB_ICONQUESTION);
			if (result == IDYES) // �u�͂��v�{�^���������ꂽ��
			{
				// x�{�^���������ꂽ�̂Ɠ������ʂ𔭊�����
				PostMessage(hWnd, WM_CLOSE, wParam, lParam);
			}
		}
		Input_SetKeyDown(LOWORD(wParam));
		break;

	case WM_KEYUP: // �L�[�������ꂽ�C�x���g
		Input_SetKeyUp(LOWORD(wParam));
		break;

	default:
		// ���case�ȊO�̏ꍇ�̏��������s
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
		break;
	}

	return 0;
}

void Game_Init()
{
	// �萔�o�b�t�@�쐬
	// �R���X�^���g�o�b�t�@�Ƃ��č쐬���邽�߂̏��ݒ�
	D3D11_BUFFER_DESC contstat_buffer_desc;
	contstat_buffer_desc.ByteWidth = 512;	// �o�b�t�@�̃T�C�Y�i4x4�s��x4�j
	contstat_buffer_desc.Usage = D3D11_USAGE_DYNAMIC;		// �g�p���@
	contstat_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;	// �o�b�t�@�̎��(�R���X�^���g�o�b�t�@)
	contstat_buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;			// CPU�A�N�Z�X�ݒ�
	contstat_buffer_desc.MiscFlags = 0;				// ���̑��̃I�v�V����
	contstat_buffer_desc.StructureByteStride = 0;			// �\���̃T�C�Y(�s����g���������0�ł����삷�邱�Ƃ����؂���)

	Direct3D_Get()->device->CreateBuffer(&contstat_buffer_desc,
		nullptr, &gpConstBuffer);

	// �J�����̍쐬
	gpCamera = new BackCamera();
	Camera::mMainCamera = gpCamera;
	// �����l�ݒ�
	// ���ӁFeye��focus���������ƃ_��
	// ���ӁFup��xyz�S�ă[�����ƃ_��
	gpCamera->SetEye(XMFLOAT3(0.0f, 0.0f, -2.0f));
	gpCamera->SetFocus(XMFLOAT3(0.0f, 0.0f, 0.0f));
	gpCamera->SetUp(XMFLOAT3(0.0f, 1.0f, 0.0f));

	// ���f���ǂݍ���
	ObjModelLoader loader;
	gModelManager["cottage"] = loader.Load(
		"assets/cottage.obj", L"assets/cottage.png");

	// �e���f���ǂݍ���
	loader = ObjModelLoader();
	gModelManager["gun"] = loader.Load(
		"assets/gun.obj", L"assets/gun.png");

	// �e�pModel�I�u�W�F�N�g����
	gpGun = new NormalObject();
	Model* pGunModel = gpGun->GetModel();
	pGunModel->SetModelData(gModelManager["gun"]);
	pGunModel->SetScale(1.5f);
	pGunModel->mPos.y = 0.4f;
	pGunModel->mPos.z = 1.0f;
	pGunModel->mRotate.y = 90.0f;
	pGunModel->mCamera = gpCamera;

	// �n�ʃ��f���ǂݍ���
	loader = ObjModelLoader();
	gModelManager["ground1"] = loader.Load(
		"assets/ground1.obj", L"assets/ground1.jpg"
	);

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

	// �e���f���ǂݍ���
	loader = ObjModelLoader();
	gModelManager["shot"] = loader.Load(
		"assets/billboard.obj", L"assets/effect000.jpg"
	);

	// �e�p�I�u�W�F�N�g����
	gpShot = new BillboardObject();
	Model* pShotModel = gpShot->GetModel();
	pShotModel->SetModelData(gModelManager["shot"]);
	pShotModel->SetScale(0.2f);
	pShotModel->mPos.y = 0.2f;
	pShotModel->mPos.z = 1.0f;
	pShotModel->mCamera = gpCamera;

	// 2D�L�����p�|���S������
	gModelManager["2Dchar1"] =
		CreateSquarePolygon(1.0f, 1.2f, 0.33f, 0.25f,
			L"assets/char01.png");
	gModelManager["2Dchar2"] =
		CreateSquarePolygon(1.0f, 1.2f, 0.33f, 0.25f,
			L"assets/char01.png");
	gModelManager["2Dchar3"] =
		CreateSquarePolygon(1.0f, 1.2f, 0.33f, 0.25f,
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

	// �R�e�[�W�p�I�u�W�F�N�g����
	for (int i = 0; i < 5; i++)
	{
		gpCottage = new NormalObject();
		Model* pCottageModel = gpCottage->GetModel();
		pCottageModel->SetModelData(gModelManager["cottage"]); // 3D�f�[�^���Z�b�g����
		pCottageModel->SetScale(0.002f);
		pCottageModel->mPos.x = -30.0f + i * 6.5f;
		pCottageModel->mPos.z = 5.0f;
		pCottageModel->mPos.y = 0.0f;
		//gObjectList.emplace_back(gpCottage);
	}

	//�G�t�F�N�g
	gModelManager["explosion"] = CreateSquarePolygon(1.0f, 1.0f, 0.25f, 0.25f, L"assets/explosion.png");
	GameObject* tmp = new Explosion();
	Model* pModel = tmp->GetModel();
	pModel->SetModelData(gModelManager["explosion"]);
	gObjectList.emplace_back(tmp);

	//�^�C�g��
	gModelManager["title"] = CreateSquarePolygon(1.0f, 0.7f, 1.0f, 1.0f, L"assets/TitleLogo640.png");
	tmp = new NormalObject();
	pModel = tmp->GetModel();
	pModel->SetModelData(gModelManager["title"]);
	pModel->Set2dRender(true);
	pModel->SetDiffuse(XMFLOAT4(1, 1, 1, 0.5f));
	//pModel->mPos.y = 3.0f;
	//gObjectList.emplace_back(tmp);

	// �J�����̒Ǐ]�^�[�Q�b�g���w��
	//((BackCamera*)gpCamera)->SetTarget(gpGun);  // C�X�^�C���L���X�g
	dynamic_cast<BackCamera*>(gpCamera)->SetTarget(gpGun); // C++�L���X�g
}



void Game_Draw()
{
	// DIRECT3D�\���̂ɃA�N�Z�X����
	DIRECT3D* d3d = Direct3D_Get();

	// ��ʃN���A�i�w��F�œh��Ԃ��j
	// �h��Ԃ��F�̎w��i�����F0.0f�`1.0f�j
	float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f }; //red,green,blue,alpha

	d3d->context->ClearRenderTargetView(d3d->renderTarget, clearColor);

	// Z�o�b�t�@�A�X�e���V���o�b�t�@�N���A
	d3d->context->ClearDepthStencilView(
		d3d->depthStencilView,			// �f�v�X�X�e���V���r���[
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,			// �y�o�b�t�@��1.0�ŃN���A����(0.0f�`1.0f)
		0);				// �X�e���V���o�b�t�@��0�ŃN���A����

	// ���@���O�̕`�揈���������ɏ��� *******

	switch (sceneNum)
	{
	case eScene::Title:
		titleDraw();
		break;

	case eScene::Game:
		gameDraw();
		break;

	case eScene::Result:
		resultDraw();
		break;
	default:
		break;
	}

	// �_�u���o�b�t�@�̐؂�ւ�
	d3d->swapChain->Present(0, 0);
}

void Game_Update()
{
	// �f���^�^�C�����z��O�̒l�ɂȂ����ꍇ�̏���
	if (gDeltaTime >= 100)
	{
		gDeltaTime = 0;
	}
	if (gDeltaTime <= 0)
	{
		gDeltaTime = 1;
	}

	switch (sceneNum)
	{
	case eScene::Title:

		break;

	case eScene::Game:
		gameUpdate();
		break;

	case eScene::Result:
		break;
	default:
		break;
	}

}

void titleUpdate()
{
}

void titleDraw()
{
}

void gameUpdate()
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
	Model* pCottageModel = gpCottage->GetModel();
	float radian = XMConvertToRadians(angle);
	gpCamera->mEye.x =
		sinf(radian) * zoom + pCottageModel->mPos.x;
	gpCamera->mEye.z =
		cosf(radian) * zoom + pCottageModel->mPos.z;
	gpCamera->mEye.y = 2.0f;

	// �J���������_���R�e�[�W�̈ʒu�ɂ���
	gpCamera->SetFocus(pCottageModel->mPos);

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

	Model* pGunModel = gpGun->GetModel();

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
		XMFLOAT3 forwardVec = gpGun->GetForwardVector();

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

	vector_UpdateAll(gObjectList);
	vector_UpdateAll(gMonsterObjectList);

	// �e�Ǘ��z��̒��g��S��Update����
	vector_UpdateAll(gShotManager);

	// �e�̉������
	for (int i = 0; i < gShotManager.size(); i++)
	{
		// (a)�J�����̈ʒu
		XMFLOAT3 cameraPos = gpCamera->mEye;
		// (b)�e�̈ʒu
		XMFLOAT3 shotPos = gShotManager[i]->GetModel()->mPos;
		// (b)��(a)�x�N�g��
		XMVECTOR abVector =
			XMLoadFloat3(&cameraPos) - XMLoadFloat3(&shotPos);
		// (b)��(a)�x�N�g���̒������v�Z
		XMVECTOR abVecLength = XMVector3Length(abVector);
		float abLength = XMVectorGetX(abVecLength);
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

void gameDraw()
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

void resultUpdate()
{
}

void resultDraw()
{
}

void Game_Release()
{
	vector_DeleteAll(gObjectList);
	vector_DeleteAll(gMonsterObjectList);

	// �e�Ǘ��z��̒��g��S��delete����
	vector_DeleteAll(gShotManager);

	vector_DeleteAll(gEffectManager);

	COM_SAFE_RELEASE(gpConstBuffer);

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
