#pragma once

#include "Model.h"
#include "HitSphere.h"
#include "HP.h"

// このクラスを派生させて、いろいろなモノを作る
// ・モデルを表示する
// ・向いてる方向に移動する
// ・前向きベクトルを提供する
class GameObject
{

public:
	GameObject();
	~GameObject();
	virtual void Draw();
	virtual void Update(); // 移動も担当

	Model* GetModel(); // このクラスが持つModelオブジェクトを返す

	//渡された位置との距離を測る関数
	float GetDistaceVector(DirectX::XMFLOAT3 pOther);

	// 前向きベクトルを返す関数
	DirectX::XMFLOAT3 GetForwardVector();

	// 移動速度
	float mSpeed;

	// 当たり判定オブジェクト getter setter
	HitSphere* GetHit()
	{
		return mHit;
	}

	HP* getHP() {
		return mHP;
	}

protected:
	Model* mModel; // このクラスで表示するモデル

	HitSphere* mHit; // 当たり判定

	HP* mHP;

private:
	// 前向きベクトルを保存しておく変数
	DirectX::XMFLOAT3 mForwardVector;

	//他のGameObjectとの距離を保存する変数
	float mDistanceVector;

};

