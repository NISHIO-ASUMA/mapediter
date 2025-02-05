//===========================
//
//  入力処理[input.cpp]
// Author:Asuma Nishio
//
//===========================

//*****************************
// インクルードファイル宣言
//****************************
#include "input.h"

//*********************
// マクロ定義
//*********************
#define NUM_KEY_MAX (256) // キーの最大数

//********************
// 構造体を定義
//********************
typedef struct
{
	bool bWasDir;		// 前回傾いていたかどうか
	bool bIsDir;		// 今回傾いたかどうか
	bool bTrigger;		// スティックのトリガー
	bool bRepeat;		// スティックのリピート
	DWORD prevTime;		// 時間
}Stick;

//*******************
//列挙型
//*******************
typedef enum
{
	STICK_TYPE_LEFT = 0,
	STICK_TYPE_RIGHT,
	STICK_TYPE_MAX
}STICK_TYPE;

//*******************
//グローバル変数
//*******************
LPDIRECTINPUT8 g_pInput = NULL;					// Directinputオブジェクトへのポインタ
LPDIRECTINPUTDEVICE8 g_pDevKeyboard = NULL;		// 入力デバイスへのポインタ
LPDIRECTINPUTDEVICE8 g_pDIMouse = NULL;			// マウスデバイス
DIMOUSESTATE g_zdiMouseState;					// マウス状態
DIMOUSESTATE g_MousePress;
DIMOUSESTATE g_MouseTrigger;
static DIMOUSESTATE g_CurrentMouseState;		// マウスの現在の入力情報
static DIMOUSESTATE g_PrevMouseState;			// マウスの一フレーム前の入力情報

BYTE g_aKeyState[NUM_KEY_MAX];					// キーボードのプレス情報
BYTE g_aOldState[NUM_KEY_MAX];					// 過去の入力内容を保存
XINPUT_STATE g_joyKeyState;						// ジョイパッドのプレス情報
XINPUT_STATE g_OldKeyState;						// ジョイパッドのプレス情報(判定)
XINPUT_STATE g_joyKeyStateTrigger;				// ジョイパッドのトリガー情報
												   
Stick g_stick[STICK_TYPE_MAX];					// スティック構造体の情報
												   
bool g_Rstickrepeat;							// スティックのリピート

//=======================
// キーボードの初期化処理
//=======================
HRESULT InitKeyboard(HINSTANCE hInstance, HWND hWnd)
{
	// DirectInputオブジェクトの生成
	if (FAILED(DirectInput8Create(hInstance,
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&g_pInput,
		NULL)))
	{
		return E_FAIL;
	}
	// 入力デバイス生成
	if (FAILED(g_pInput->CreateDevice(GUID_SysKeyboard, &g_pDevKeyboard, NULL)))
	{
		return E_FAIL;


	}
	// データフォーマットを設定
	if (FAILED(g_pDevKeyboard->SetDataFormat(&c_dfDIKeyboard)))
	{
		return E_FAIL;
	}
	// 協調モードを設定
	if (FAILED(g_pDevKeyboard->SetCooperativeLevel(hWnd,
		(DISCL_FOREGROUND | DISCL_NONEXCLUSIVE))))
	{
		return E_FAIL;
	}
	// キーボードのアクセス権を獲得
	g_pDevKeyboard->Acquire();

	return S_OK;
}
//======================
// キーボードの終了処理
//======================
void UninitKeyboard(void)
{
	// 入力デバイス(キーボード)の破棄
	if (g_pDevKeyboard != NULL)
	{
		g_pDevKeyboard->Unacquire();	// アクセス権の破棄
		g_pDevKeyboard->Release();
		g_pDevKeyboard = NULL;
	}

	// DirectInputオブジェクトの破棄
	if (g_pInput != NULL)
	{
		g_pInput->Release();
		g_pInput = NULL;
	}
}
//======================
// キーボードの更新処理
//======================
void UpdateKeyboard(void)
{

	BYTE aKeyState[NUM_KEY_MAX];						// キーボードの入力情報
	int nCntkey;
	for (nCntkey = 0; nCntkey < NUM_KEY_MAX; nCntkey++)
	{
		g_aOldState[nCntkey] = g_aKeyState[nCntkey];	// キーボードのプレス情報を保存
	}

	//入力デバイスからデータを取得
	if (SUCCEEDED(g_pDevKeyboard->GetDeviceState(sizeof(aKeyState), &aKeyState[0])))
	{
		for (nCntkey = 0; nCntkey < NUM_KEY_MAX; nCntkey++)
		{
			g_aKeyState[nCntkey] = aKeyState[nCntkey];	// キーボードのプレス情報を保存
		}
	}
	else
	{
		g_pDevKeyboard->Acquire();			// キーボードのアクセス権を獲得
	}
}
//=============================
// キーボードのプレス情報を取得
//=============================
bool GetKeyboardPress(int nkey)
{
	return (g_aKeyState[nkey] & 0x80) ? true : false;	// 3項演算子を使用
}
//=============================
// キーボードをワンタップ
//=============================
bool KeyboardTrigger(int nKey)
{
	bool Trigger = false;
	if (g_aKeyState[nKey] & 0x80 && !(g_aOldState[nKey] & 0x80))
	{
		Trigger = true;
	}
	return Trigger;
}
//================================
// キーボードを長押し(チャージ攻撃)
//================================
bool KeyboardRelease(int nKey)
{
	bool Trigger = false;
	if (g_aOldState[nKey] & 0x80 && !(g_aKeyState[nKey] & 0x80))
	{
		Trigger = true;
	}
	return Trigger;
}
//===============================
// キーボードの継続処理(連射処理)
//===============================
bool KeyboardRepeat(int nKey)
{
	bool Trigger = false;
	if (g_aOldState[nKey] & 0x80 && (g_aKeyState[nKey] & 0x80))
	{
		Trigger = true;
	}
	return Trigger;
}
//==============================
// ジョイパッドの初期化処理
//==============================
HRESULT InitJoypad(void)
{
	// メモリのクリア
	memset(&g_joyKeyState, 0, sizeof(XINPUT_STATE));

	// メモリのクリア
	memset(&g_joyKeyStateTrigger, 0, sizeof(XINPUT_STATE));

	// Xinputのステートを設定(有効化)
	XInputEnable(true);

	// 初期化
	g_Rstickrepeat = false;		

	return S_OK;
}
//==============================
// ジョイパッドの終了
//==============================
void UninitJoypad(void)
{
	// Xinputのステートを設定(無効化)
	XInputEnable(false);
}
//==============================
// ジョイパッドの更新
//==============================
void UpdateJoypad(void)
{
	XINPUT_STATE joykeyState;			// 入力情報を取得

	//ジョイパッドの状態を取得
	if (XInputGetState(0, &joykeyState) == ERROR_SUCCESS)
	{
		WORD Button = joykeyState.Gamepad.wButtons;				// 押したときの入力情報
		WORD OldButton = g_joyKeyState.Gamepad.wButtons;		// 1F前の入力情報

		g_joyKeyStateTrigger.Gamepad.wButtons = Button &~OldButton;

		g_joyKeyState = joykeyState;							// ジョイパッドのプレス情報を保存(格納)
	}

	// 更新処理
	UpdateStick();
}
//==============================
// プレス情報を取得
//==============================
bool JoypadPress(JOYKEY key)
{
	return (g_joyKeyState.Gamepad.wButtons & (0x01 << key)) ? true : false;
}
//==============================
// トリガー情報を取得
//==============================
bool JoypadTrigger(JOYKEY key)
{
	return (g_joyKeyStateTrigger.Gamepad.wButtons & (0x01 << key)) ? true : false;
}
//==========================
// パッドのリリース処理
//==========================
bool JoypadRelease(JOYKEY key)
{
	bool Joykey = false;

	if (g_OldKeyState.Gamepad.wButtons & (0x01 << key) && !((g_OldKeyState.Gamepad.wButtons & (0x01 << key))))
	{
		Joykey = true;
	}

	return false;
}
//==========================
// パッドのリピート処理
//==========================
bool JoypadRepeat(JOYKEY key)
{
	bool Joykey = false;

	if (g_OldKeyState.Gamepad.wButtons & (0x01 << key) && !((g_OldKeyState.Gamepad.wButtons & (0x01 << key))))
	{
		Joykey = true;
	}

	return Joykey;
}
//======================
// パッドのLスティック処理
//======================
bool GetJoyStick(void)
{
	//ローカル変数宣言
	bool joykey = false;

	//中心じゃなかったら
	if (g_joyKeyState.Gamepad.sThumbLX >= 100 ||
		g_joyKeyState.Gamepad.sThumbLX <= -100 ||
		g_joyKeyState.Gamepad.sThumbLY >= 100 ||
		g_joyKeyState.Gamepad.sThumbLY <= -100)
	{
		joykey = true;
	}

	return joykey;
}
//=============================
// パッドのRスティックの取得
//=============================
bool GetJoyStickRrepeat(void)
{
	return g_Rstickrepeat;
}
//==============================
// パッドのRスティックのリピート
//==============================
void UpdateStick(void)
{
	g_stick[STICK_TYPE_RIGHT].bWasDir = g_stick[STICK_TYPE_RIGHT].bIsDir;

	//スティックの取得
	XINPUT_STATE* pStick;
	pStick = GetJoyStickAngle();

	// 現在の時刻を取得
	DWORD currTime = timeGetTime();

	//中心じゃなかったら
	g_stick[STICK_TYPE_RIGHT].bIsDir = false;

	if (g_joyKeyState.Gamepad.sThumbRX >= 1000 ||
		g_joyKeyState.Gamepad.sThumbRX <= -1000 ||
		g_joyKeyState.Gamepad.sThumbRY >= 1000 ||
		g_joyKeyState.Gamepad.sThumbRY <= -1000)
	{
		g_stick[STICK_TYPE_RIGHT].bIsDir = true;
	}

	g_Rstickrepeat = false;

	// トリガー判定
	if (g_stick[STICK_TYPE_RIGHT].bIsDir == true && g_stick[STICK_TYPE_RIGHT].bWasDir == false)
	{ // スティックが今回傾いている かつ 前回傾いていない
		g_stick[STICK_TYPE_RIGHT].bTrigger = true;

		// 時間を保持 (FPS 測定のときみたいに時刻を取得する)
		g_stick[STICK_TYPE_RIGHT].prevTime = currTime;
	}

	// リピート判定
	DWORD duration = (currTime - g_stick[STICK_TYPE_RIGHT].prevTime);
	if (g_stick[STICK_TYPE_RIGHT].bIsDir == true && duration >= 160)
	{ // スティックが傾いている かつ 一定フレーム経過 (この場合だと 60 フレームなので 1 秒)
		g_stick[STICK_TYPE_RIGHT].bRepeat = true;

		// 時間を保持 (FPS 測定のときみたいに時刻を取得する)
		g_stick[STICK_TYPE_RIGHT].prevTime = currTime;

		g_Rstickrepeat = true;
	}

}
//========================
// スティック情報の取得
//========================
XINPUT_STATE* GetJoyStickAngle(void)
{
	return &g_joyKeyState;
}
