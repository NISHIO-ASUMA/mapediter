//==========================
//
// モデル処理 [model.h]
// Author: Asuma Nishio
//
//==========================
#ifndef _MODEL_H_
#define _MODEL_H_

//***************************
// インクルードファイル宣言
//***************************
#include "main.h"

//*****************************
// マクロ定義
//*****************************
#define MAX_TETURE (10)		// テクスチャ数

//***************************
// 構造体宣言
//***************************
typedef struct
{
	LPD3DXMESH pMeshModel;		// メッシュのポインタ
	LPD3DXBUFFER pBuffMatModel;	// マテリアルのポインタ
	DWORD dwNumMatModel;		// マテリアル数
	D3DXVECTOR3 pos;			// 位置 (オフセット)
	D3DXVECTOR3 rot;			// 向き
	D3DXMATRIX mtxWorld;		// ワールドマトリックス
	int nIndxModelParent;		// 親モデルのオフセット
	LPDIRECT3DTEXTURE9 pTexture[MAX_TETURE];	// テクスチャのポインタ
	D3DXVECTOR3 Offpos;
	D3DXVECTOR3 Offrot;
	D3DCOLORVALUE Diffuse;
	D3DCOLORVALUE FirstDiffuse;
	D3DXVECTOR3 size;
	D3DXVECTOR3 Vtxmin, Vtxmax;
}Model;

//**********************
// プロトタイプ宣言
//**********************
void InitModel(void);		// モデル初期化処理
void UninitModel(void);		// モデル終了処理
void UpdateModel(void);		// モデル更新処理
void DrawModel(void);		// モデル描画処理

#endif // !_MODEL_H_
