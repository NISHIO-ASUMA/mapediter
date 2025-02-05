//==========================
//
// 影の処理 [shadow.h]
// Author: Asuma Nishio
//
//==========================
#ifndef _SHADOW_H_
#define _SHADOW_H_

//***************************
// インクルードファイル宣言
//***************************
#include "main.h"

//**************************
// マクロ定義
//**************************
#define MAX_SHADOW (10)			// 影の最大数
#define SHADOW_SIZE (20.0f)			// 影のサイズ

//**********************
// 構造体宣言
//**********************
typedef struct
{
	D3DXVECTOR3 posShadow;		// 位置
	D3DXVECTOR3 rotShadow;		// 向き
	D3DXMATRIX  mtxWorldShadow;	// ワールドマトリックス
	D3DXVECTOR3 Scal;			// 大きさ
	bool bUse;					// 使用判定
}Shadow;

//**********************
// プロトタイプ宣言
//**********************
void InitShadow(void);		// 影初期化処理
void UninitShadow(void);	// 影終了処理
void UpdateShadow(void);	// 影更新処理
void DrawShadow(void);		// 影描画処理
int SetShadow(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 Scal);		// 影の設定
void SetPositionShadow(int nidxSahdow, D3DXVECTOR3 pos);// 影の座標更新設定
void DeleteShadow(int nidxSahdow);// 影の消去

#endif // !_SHADOW_H_

