#pragma once
#include<vector>
#include "GameObject.h"
#include "Direct3d.h"

class IScene
{
protected:
	typedef std::vector<GameObject*> GameObjectVector;

	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual void End() = 0;


	ModelData CreateSquarePolygon(float width, float height, float uvWidth, float uvHeight, const wchar_t* texFile);
};
