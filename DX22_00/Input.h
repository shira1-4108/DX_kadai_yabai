#pragma once

#include <Windows.h>

// Inputクラス

// 公開メンバー関数

// キーが押されたイベントが発生したら、その状態を保存する関数
void Input_SetKeyDown(int key);

// キーが離されたイベントが発生したら、その状態を保存する関数
void Input_SetKeyUp(int key);

// 指定したキーが押されているか、その状態（プレス）を返す関数
BOOL Input_GetKeyDown(int key);

// トリガー　…　キーが押された次の１フレームだけ押された判定
BOOL Input_GetKeyTrigger(int key);

// ゲームループの最後でキー状態を更新する関数
void Input_Refresh();


// C++版

class Input
{
private:
	bool mKeyState[256];

public:
	Input();
	void SetKeyDown(int key);
	bool GetKeyDown(int key);
};