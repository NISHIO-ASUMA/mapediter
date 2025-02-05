//============================
//
//  入力処理[input.h]
// Author:Asuma Nishio
//
//============================
#ifndef _INPUT_H_
#define _INPUT_H_

//****************************
// インクルードファイル宣言
//****************************
#include "main.h"

//***************************
// ジョイパッドのキーの列挙型
//***************************
typedef enum
{
	JOYKEY_UP = 0,				//十字キー(上)
	JOYKEY_DOWN,				//十字キー(下)
	JOYKEY_LEFT,				//十字キー(左)
	JOYKEY_RIGHT,				//十字キー(右)

	JOYKEY_START,				//スタートキーが押された
	JOYKEY_BACK,				//バックキーが押された

	JOYKEY_LS,					//L3(Lスティック)が押された
	JOYKEY_RS,					//R3(Rスティック)が押された

	JOYKEY_LEFT_B,				//LBキーが押された
	JOYKEY_RIGHT_B,				//RBキーが押された

	JOYKEY_LEFT_TRIGGER,		//LTキーが押された
	JOYKEY_RIGHT_TRIGGER,		//RTキーが押された

	JOYKEY_A,					//Aボタンが押された
	JOYKEY_B,					//Bボタンが押された
	JOYKEY_X,					//Xボタンが押された
	JOYKEY_Y,					//Yボタンが押された

	JOYKEY_MAX					//パッドの最大数
}JOYKEY;

//***********************
// マウスの種類の列挙型
//***********************
typedef enum
{
	MOUSE_LEFTBUTTON = 0,
	MOUSE_RIGHTBUTTON,
	MOUSE_WHEEL,
	MOUSE_MAX
}MOUSEBUTTON;

//********************
// プロトタイプ宣言
//********************
HRESULT InitKeyboard(HINSTANCE hInstance, HWND hWnd);	// キーの初期化
void UninitKeyboard(void);							// キーの終了
void UpdateKeyboard(void);							// キーの更新
bool GetKeyboardPress(int nkey);					// キーのプレス
bool KeyboardTrigger(int nKey);						// キーのトリガー
bool KeyboardRelease(int nKey);						// キーのリリース
bool KeyboardRepeat(int nKey);						// キーのリピート

HRESULT InitJoypad(void);							// ジョイパッドの初期化
void UninitJoypad(void);							// ジョイパッドの終了
void UpdateJoypad(void);							// ジョイパッドの更新
bool JoypadPress(JOYKEY key);						// ジョイパッドのプレス情報を取得
bool JoypadTrigger(JOYKEY key);						// ジョイパッドのトリガー情報を取得
bool JoypadRelease(JOYKEY key);						// キーのリリース
bool JoypadRepeat(JOYKEY key);						// キーのリピート

bool GetJoyStick(void);								//ジョイパッドのスティック情報(L)
bool GetJoyStickRrepeat(void);						//ジョイパッドのスティック情報(R)
void UpdateStick(void);								//Rスティック

XINPUT_STATE* GetJoyStickAngle(void);				//スティックの角度情報

#endif