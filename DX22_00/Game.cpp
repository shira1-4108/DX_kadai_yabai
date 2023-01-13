#include "Game.h"
#include "Direct3D.h"
#include "Input.h"

// static変数の実体宣言
GameObject* Game::mHalOsaka1;
GameObject* Game::mHalOsaka2;
GameObject* Game::mHalOsaka[];


void Game::Initialize()
{
	// ゲームオブジェクトのインスタンスを作成
	mHalOsaka1 = new GameObject();
	mHalOsaka1->x = 1;
	mHalOsaka2 = new GameObject();
	mHalOsaka2->x = -1;

	for (int i = 0; i < mMaxGameObject; i++)
	{
		mHalOsaka[i] = new GameObject();
		mHalOsaka[i]->z = 10;
		mHalOsaka[i]->y = 2 - i * 1.2f;
	}
}

void Game::Update()
{
	Input::Update();

	// ゲームオブジェクトを更新
	//mHalOsaka1->Update();
}

void Game::Draw()
{
	// 画面クリア（指定色で塗りつぶし）
	// 塗りつぶし色の指定（小数：0.0f～1.0f）
	float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f }; //red,green,blue,alpha

	DIRECT3D* d3d = Direct3D_Get();

	d3d->context->ClearRenderTargetView(d3d->renderTarget, clearColor);

	// Zバッファ、ステンシルバッファクリア
	d3d->context->ClearDepthStencilView(
		d3d->depthStencilView,			// デプスステンシルビュー
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,			// Ｚバッファを1.0でクリアする(0.0f～1.0f)
		0);				// ステンシルバッファを0でクリアする

	// ↓　自前の描画処理をここに書く *******

	// ゲームオブジェクトを描画
	mHalOsaka1->Draw();
	mHalOsaka2->Draw();

	for (int i = 0; i < mMaxGameObject; i++)
	{
		mHalOsaka[i]->Draw();
	}

	// ↑　自前の描画処理をここに書く *******

	// ダブル・バッファのディスプレイ領域へのコピー命令
	d3d->swapChain->Present(0, 0);
}

void Game::Release()
{
	// ゲームオブジェクトを全て解放
	delete mHalOsaka1;
	delete mHalOsaka2;

	for (int i = 0; i < mMaxGameObject; i++)
	{
		delete mHalOsaka[i];
	}
}
