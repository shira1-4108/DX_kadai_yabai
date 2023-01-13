#pragma once
#include "IScene.h"
class SceneResult :
	public IScene
{
public:
	void Init() override;
	void Update() override;
	void Draw() override;
	void End() override;
};