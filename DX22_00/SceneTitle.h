#pragma once
#include "IScene.h"
class SceneTitle :
	public IScene
{
public:
	void Init() override;
	void Update() override;
	void Draw() override;
	void End() override;
};

