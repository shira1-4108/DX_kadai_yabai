#pragma once
#include "IScene.h"
#include <map>

#define MAX_CHAR  3
#define MAX_GROUND  10


class SceneGame :
	public IScene
{
private:
	typedef std::vector<GameObject*> GameObjectVector;
	// ���f���}�l�[�W���[
	// �A�z�z�񁁓Y�����ɐ����ȊO���g����z��
	// �e���v���[�g���g���Ƃ��ɓ����̌^�����߂�
	// map�̏ꍇ�A�O���Y�����^�A��낪�i�[����v�f�̌^
	std::map<std::string, ModelData> gModelManager;

	// �e�Ǘ��p�z��
	GameObjectVector gShotManager;

	// �G�t�F�N�g�Ǘ��p�z��
	GameObjectVector gEffectManager;

	GameObjectVector gObjectList;
	GameObjectVector gMonsterObjectList;

	GameObject* gpGround[MAX_GROUND][MAX_GROUND];
	// 2D�L�����̕ϐ�
	GameObject* gp2DChar[MAX_CHAR];
	GameObject* gpShadow;
	// GameObject�N���X�̕ϐ�
	GameObject* gpGun;
	GameObject* gpShot;

	// Camera�N���X�̕ϐ�
	Camera* gpCamera;

public:
	void Init() override;
	void Update() override;
	void Draw() override;
	void End() override;

	void vector_DrawAll(GameObjectVector& vec);
	void vector_UpdateAll(GameObjectVector& vec);
	void vector_DeleteAll(GameObjectVector& vec);
};

