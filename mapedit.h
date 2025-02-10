//===================================
//
// マップエディター処理 [mapedit.h]
// Author: Asuma Nishio
//
//===================================
#ifndef _MAPEDIT_H_
#define _MAPEDIT_H_

//****************************
// インクルードファイル宣言
//****************************
#include "main.h"

//****************************
// マクロ定義
//****************************
#define MAX_EDITOBJ (256)	// 最大配置数
#define EDIT_MOVE   (20.0f) // 移動量(配置時)
#define MAX_TEXTURE (256)   // 最大テクスチャ数

//*************************************
// マップエディターモデル構造体宣言
//*************************************
typedef struct
{
	LPD3DXMESH pMesh;		// メッシュのポインタ
	LPD3DXBUFFER pBuffMat;	// マテリアルのポインタ
	DWORD dwNumMat;			// マテリアル数
	LPDIRECT3DTEXTURE9 pTexture[MAX_TEXTURE]; // テクスチャのポインタ
	D3DXVECTOR3 Vtxmax;	    // 最大値を保持する変数
	D3DXVECTOR3 Vtxmin;	    // 最小値を保持する変数
	D3DXVECTOR3 size;	    // サイズ情報
	char FileName[64];      // 文字列
}EDITMODEL;

//****************************
// マップエディター構造体宣言
//****************************
typedef struct
{
	D3DXVECTOR3 pos; // 座標
	D3DXVECTOR3 rot; // 角度
	D3DXMATRIX mtxWorld; // ワールドマトリックス
	D3DXVECTOR3 move;	 // 移動量
	int nType;		     // 種類
	D3DXVECTOR3 Scal;	 // 拡大率
}MapEdit;

//***********************************
// マップエディター配置用構造体宣言
//***********************************
typedef struct
{
	bool isputblock;     // 配置したかどうか
	bool bUse;		     // 使用状態かどうか
	MapEdit mapedit;	 // 構造体変数
}MAPMODELINFO;

//****************************
// プロトタイプ宣言
//****************************
void InitMapEdit();		// 初期化処理
void UninitMapEdit();	// 終了処理
void UpdateMapEdit();	// 更新処理
void DrawMapEdit();		// 描画処理
void SaveEdit();		// 保存処理
void ReloadEdit();		// 再読み込み処理
void LoadXFile();		// 外部からの読み込み
EDITMODEL* GetBlockInfo(int type); // エディターモデル情報取得
MapEdit* getMap();		// 構造体情報取得
int ReturnEdit();		// 配置数情報
float ReturnSpeed();	// 移動
int ReturnType();		// 種類数
float ReturnJump();		// ジャンプ量
int Filepass();			// ファイルパスを返す
bool IsGetLoad();		// リロードしたか
MAPMODELINFO* MapInfo();// 配置時の状況取得

#endif