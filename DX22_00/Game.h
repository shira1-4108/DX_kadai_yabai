#pragma once

#include "GameObject.h"

// ゲーム全体の枠となるクラス。
// このクラスにゲーム内で必要となるオブジェクトを持たせる。
// このクラスはstaticなクラスとして設計するため、
// このクラスのpublicメンバーにはプログラムのどこからでもアクセスできる。

class Game
{

public: // クラスの公開要素

	// メンバー関数
	static void Initialize();	// ゲームの初期化処理
	static void Update();		// 更新処理
	static void Draw();			// 描画処理
	static void Release();		// 解放処理

	// 定数
	static constexpr int mMaxGameObject = 5;

private: // クラスの非公開要素
	
	// このゲームに登場するゲームオブジェクト
	static GameObject* mHalOsaka1;
	static GameObject* mHalOsaka2;
	static GameObject* mHalOsaka[mMaxGameObject];

};

