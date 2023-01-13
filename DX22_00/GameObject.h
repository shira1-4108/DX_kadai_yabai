#pragma once

#include "Model.h"
#include "HitSphere.h"
#include "HP.h"

// ���̃N���X��h�������āA���낢��ȃ��m�����
// �E���f����\������
// �E�����Ă�����Ɉړ�����
// �E�O�����x�N�g����񋟂���
class GameObject
{

public:
	GameObject();
	~GameObject();
	virtual void Draw();
	virtual void Update(); // �ړ����S��

	Model* GetModel(); // ���̃N���X������Model�I�u�W�F�N�g��Ԃ�

	//�n���ꂽ�ʒu�Ƃ̋����𑪂�֐�
	float GetDistaceVector(DirectX::XMFLOAT3 pOther);

	// �O�����x�N�g����Ԃ��֐�
	DirectX::XMFLOAT3 GetForwardVector();

	// �ړ����x
	float mSpeed;

	// �����蔻��I�u�W�F�N�g getter setter
	HitSphere* GetHit()
	{
		return mHit;
	}

	HP* getHP() {
		return mHP;
	}

protected:
	Model* mModel; // ���̃N���X�ŕ\�����郂�f��

	HitSphere* mHit; // �����蔻��

	HP* mHP;

private:
	// �O�����x�N�g����ۑ����Ă����ϐ�
	DirectX::XMFLOAT3 mForwardVector;

	//����GameObject�Ƃ̋�����ۑ�����ϐ�
	float mDistanceVector;

};

