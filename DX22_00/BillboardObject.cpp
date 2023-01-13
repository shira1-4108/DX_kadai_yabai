#include "BillboardObject.h"
#include "BillboardModel.h"

BillboardObject::BillboardObject()
{
	// ビルボード機能ありのモデル
	mModel = new BillboardModel();
	mHit = new HitSphere(mModel->mPos, 1.0f);
}
