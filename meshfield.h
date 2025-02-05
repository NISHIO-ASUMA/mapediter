//======================================
//
// メッシュフィールド処理 [mashfield.h]
// Author: Asuma Nishio
//
//======================================
#ifndef _MESHFIELD_H_
#define _MESHFIELD_H_

//***************************
// インクルードファイル宣言
//***************************
#include "main.h"

//***************************
// 構造体宣言
//***************************
typedef struct
{
	D3DXVECTOR3 pos;		// 位置
	D3DXVECTOR3 rot;		// 向き
	D3DXMATRIX  mtxWorld;	// ワールドマトリックス
}Meshfield;

//**********************
// プロトタイプ宣言
//**********************
void InitMeshField(void);	// ポリゴン初期化処理
void UninitMeshField(void);	// ポリゴン終了処理
void UpdateMeshField(void);	// ポリゴン更新処理
void DrawMeshField(void);	// ポリゴン描画処理

#endif // !_POLYGON_H_
