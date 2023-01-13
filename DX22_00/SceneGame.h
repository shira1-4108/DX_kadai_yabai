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
	// モデルマネージャー
	// 連想配列＝添え字に整数以外も使える配列
	// テンプレート＝使うときに内部の型を決める
	// mapの場合、前が添え字型、後ろが格納する要素の型
	std::map<std::string, ModelData> gModelManager;

	// 弾管理用配列
	GameObjectVector gShotManager;

	// エフェクト管理用配列
	GameObjectVector gEffectManager;

	GameObjectVector gObjectList;
	GameObjectVector gMonsterObjectList;

	GameObject* gpGround[MAX_GROUND][MAX_GROUND];
	// 2Dキャラの変数
	GameObject* gp2DChar[MAX_CHAR];
	GameObject* gpShadow;
	// GameObjectクラスの変数
	GameObject* gpGun;
	GameObject* gpShot;

	// Cameraクラスの変数
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

