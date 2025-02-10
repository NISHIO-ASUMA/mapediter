//==========================
//
// ブロック処理 [block.h]
// Author: Asuma Nishio
//
//==========================
#ifndef _BLOCK_H_
#define _BLOCK_H_

//***************************
// インクルードファイル宣言
//***************************
#include "main.h"
#include "mapedit.h"

//********************************
// ブロックのモデルの構造体宣言
//********************************
typedef struct
{
	LPD3DXMESH pMesh;		// メッシュのポインタ
	LPD3DXBUFFER pBuffMat;	// マテリアルのポインタ
	DWORD dwNumMat;			// マテリアル数
	LPDIRECT3DTEXTURE9 pTexture[MAX_TEXTURE]; // テクスチャのポインタ
	D3DXVECTOR3 Vtxmax;		// 最大値を保持する変数
	D3DXVECTOR3 Vtxmin;		// 最小値を保持する変数
	D3DXVECTOR3 size;		// サイズ情報
	char FileName[64];      // 文字列
}BlockModel;

//***************************
// ブロックの構造体宣言
//***************************
typedef struct
{
	D3DXVECTOR3 pos; // 座標
	D3DXVECTOR3 rot; // 角度
	D3DXMATRIX mtxWorld; // ワールドマトリックス
	D3DXVECTOR3 move;	// 移動量
	int  nType;			// 種類
	D3DXVECTOR3 Scal;	// 拡大率
}Block;

//***********************************
// ブロックモデル保存用の構造体宣言
//***********************************
typedef struct
{
	bool bUse;		// 使用状態かどうか
	Block aBlock;   // 構造体情報
}BlockInfo;

//**********************
// プロトタイプ宣言
//**********************
void InitBlock(void);		// ブロック初期化処理
void UninitBlock(void);		// ブロック終了処理
void UpdateBlock(void);		// ブロック更新処理
void DrawBlock(void);		// ブロック描画処理
void SetBlock(D3DXVECTOR3 pos, int nType, D3DXVECTOR3 Scal); // ブロックの設定処理
void CollisionBlock(D3DXVECTOR3 *pPos,D3DXVECTOR3 *pPosOld,D3DXVECTOR3 *pMove,D3DXVECTOR3 *pSize); // 当たり判定
BlockInfo* GetBlock();		// 情報の取得
void LoadBlock();			// 読み込み処理

#endif // !_BLOCK_H_