#include "input.h"

// 全キー状態を保存する配列
BOOL gKeyState[256];
// 全キー状態（１フレーム前）を保存する配列
BOOL gOldKeyState[256];

void Input_SetKeyDown(int key)
{
	gKeyState[key] = TRUE;
}

void Input_SetKeyUp(int key)
{
	gKeyState[key] = FALSE;
}

BOOL Input_GetKeyDown(int key)
{
	return gKeyState[key];
}

BOOL Input_GetKeyTrigger(int key)
{
	// １フレーム前は押されておらず、今は押されている
	return gOldKeyState[key] == FALSE
		&& gKeyState[key] == TRUE;
}

void Input_Refresh()
{
	// ゲームループの最後で現在のキー状態をgOldKeyStateに保存
	memcpy_s(gOldKeyState, sizeof(gOldKeyState),
		gKeyState, sizeof(gKeyState));
}

Input::Input()
{
	ZeroMemory(mKeyState, sizeof(mKeyState));
}

void Input::SetKeyDown(int key)
{
	mKeyState[key] = true;
}

bool Input::GetKeyDown(int key)
{
	return mKeyState[key];
}
