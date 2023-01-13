#pragma once

#include "Camera.h"
#include "GameObject.h"

// 追従カメラを表すクラス
// ・追従機能
class BackCamera :
	public Camera
{

public:
	void Update() override;

	// 追従ターゲットを指定する関数
	void SetTarget(GameObject* pTarget);

private:
	// 追従ターゲットを指定する変数
	GameObject* mTarget;

};

