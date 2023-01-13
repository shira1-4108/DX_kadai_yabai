#pragma once

#include "Model.h"

// ビルボード機能ありのモデル
class BillboardModel :
	public Model
{

public:
	BillboardModel()
	{
		mLightActive = false;
	}

	//void Draw() override;
	void GetWVPRMatrix(ConstBufferData& out) override;

};

