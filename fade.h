//=============================
//
// フェード処理 [fade.h]
// Author:Asuma Nishio
//
//=============================
#ifndef _FADE_H_
#define _FADE_H_

//*********************************
// インクルードファイル宣言
//*********************************
#include "main.h"

//**************************
// フェードの状態列挙型
//**************************
typedef enum
{
	FADE_NONE = 0,			// フェードなしの状態
	FADE_IN,				// フェードインの状態
	FADE_OUT,				// フェードアウトの状態
	FAFE_MAX
}FADE;

//*****************************
// プロトタイプ宣言
//*****************************
void InitFade(MODE modeNext);		// フェードの初期化
void UninitFade(void);				// フェードの終了
void UpdateFade(void);				// フェードの更新
void DrawFade(void);				// フェードの描画
void SetFade(MODE modeNext);		// フェードの設定
FADE GetFade(void);					// フェードの取得

#endif