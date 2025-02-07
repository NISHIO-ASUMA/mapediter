//========================================
//
// メッシュフィールド処理 [meshfield.cpp]
// Author: Asuma Nishio
//
//========================================

//****************************
// インクルードファイル宣言
//****************************
#include "meshfield.h"

//***************************
// マクロ定義
//***************************
#define MAX_XVTX (3)	// Xの値
#define MAX_ZVTX (5)	// Zの値

#define MAX_ALLVTX ((MAX_XVTX + 1) * (MAX_ZVTX + 1))	// 頂点数
#define POLYGON (((MAX_XVTX * MAX_ZVTX) * 2)) + (4 * (MAX_ZVTX - 1))	// ポリゴン数
#define MAX_INDEXBUFF (POLYGON + 2)		// インデックスバッファ数

#define MAXWIDTH  (1600.0f)	// フィールドのX
#define MAXHEIGHT (1600.0f)	// フィールドのZ

//*****************************
// グローバル変数宣言
//*****************************
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffMeshField = NULL;	// 頂点バッファのポインタ
LPDIRECT3DTEXTURE9 g_apTextureMeshField = NULL;	// テクスチャのポインタ
LPDIRECT3DINDEXBUFFER9 g_pIdxBuffMeshField = NULL;	// インデックスバッファへのポインタ
Meshfield g_Mesh;									// 構造体変数

//==============================
// メッシュフィールド初期化処理
//==============================
void InitMeshField(void)
{
	// デバイスポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,
		"data\\TEXTURE\\field100.jpg",
		&g_apTextureMeshField);

	// 変数の初期化
	g_Mesh.pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_Mesh.rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	// ローカル変数宣言------------------------
	int nCntPos = 0;				// 座標用
	float TexX = 1.0f / MAX_XVTX;	// テクスチャ座標のX
	float TexZ = 1.0f / MAX_ZVTX;	// テクスチャ座標のZ

	int nIdxNumber = 0;				// ローカル変数
	int nCntIdx3 = MAX_XVTX + 1;	// Xの値
	int nNum = 0;					// ローカル変数
	//------------------------------------------
	
	// 頂点バッファ(三角形は意識しない)
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * MAX_ALLVTX,
		D3DUSAGE_WRITEONLY, 
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffMeshField,
		NULL);

	// インデックスバッファの生成
	pDevice->CreateIndexBuffer(sizeof(WORD) * MAX_INDEXBUFF,
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&g_pIdxBuffMeshField,
		NULL);

	// 頂点情報のポインタ
	VERTEX_3D * pVtx = NULL;

	// 頂点バッファをロックし,頂点情報へのポインタを取得
	g_pVtxBuffMeshField->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntx = 0; nCntx <= MAX_ZVTX; nCntx++)
	{
		for (int nCntz = 0; nCntz <= MAX_XVTX; nCntz++)
		{
			// 頂点情報の設定
			pVtx[nCntPos].pos.x = (0.0f + (MAXWIDTH / MAX_XVTX) * nCntz)  - (MAXWIDTH * 0.5f);	// 半分ずつ左にずらしていく
			pVtx[nCntPos].pos.y = 0.0f;															// 地面なので0.0f
			pVtx[nCntPos].pos.z = (MAXHEIGHT - (MAXHEIGHT / MAX_ZVTX) * nCntx) - (MAXHEIGHT * 0.5f);	// 半分ずらす

			// 各頂点の法線(ベクトル)の設定
			pVtx[nCntPos].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);		// 法線情報

			// 頂点カラーの設定
			pVtx[nCntPos].col = D3DXCOLOR(1.0f,1.0f,1.0f, 1.0f);	// カラー

			// テクスチャ座標の設定
			pVtx[nCntPos].tex = D3DXVECTOR2(TexX *  nCntz, TexZ * nCntx);

			nCntPos++;												// インクリメントして1を足す
		}
	}
	
	// アンロック
	g_pVtxBuffMeshField->Unlock();

	// インデックスバッファのポインタ
	WORD* pIdx;

	//インデックスバッファをロック
	g_pIdxBuffMeshField->Lock(0, 0, (void**)&pIdx, 0);

	 //インデックスを設定
	for (int nCntIdx = 0; nCntIdx < MAX_ZVTX; nCntIdx++)
	{// MAX_ZVTXの数だけ回す
		for (int nCntIdx2 = 0; nCntIdx2 <= MAX_XVTX; nCntIdx2++, nCntIdx3++, nNum++)
		{// MAX_XVTXの数だけ回す
			pIdx[nIdxNumber] = nCntIdx3;	// 数字を代入
			pIdx[nIdxNumber + 1] = nNum;	// 数字を代入
			nIdxNumber += 2;				// 2個分進める
		}

		// NOTE: 最後の行に縮退ポリゴンがない
		if (nCntIdx !=  MAX_ZVTX -1)
		{
			pIdx[nIdxNumber] = nNum - 1;		// for文に入るため
			pIdx[nIdxNumber + 1] = nCntIdx3;	// 数字を代入
			nIdxNumber += 2;
		}
	}

	// アンロック
	g_pIdxBuffMeshField->Unlock();
}
//==============================
// メッシュフィールド終了処理
//==============================
void UninitMeshField(void)
{
	// テクスチャの破棄
	if (g_apTextureMeshField != NULL)
	{
		g_apTextureMeshField->Release();
		g_apTextureMeshField = NULL;
	}

	// 頂点バッファの破棄
	if (g_pVtxBuffMeshField != NULL)
	{
		g_pVtxBuffMeshField->Release();
		g_pVtxBuffMeshField = NULL;
	}

	// インデックスバッファの破棄
	if (g_pIdxBuffMeshField != NULL)
	{
		g_pIdxBuffMeshField->Release();
		g_pIdxBuffMeshField = NULL;
	}

}
//==============================
// メッシュフィールド更新処理
//==============================
void UpdateMeshField(void)
{
	// 無し
}
//==============================
// メッシュフィールド描画処理
//==============================
void DrawMeshField(void)
{
	// デバイスポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	
	// 計算用のマトリックスを宣言
	D3DXMATRIX mtxRot, mtxTrans;

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&g_Mesh.mtxWorld);

	// 向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, g_Mesh.rot.y, g_Mesh.rot.x, g_Mesh.rot.z);
	D3DXMatrixMultiply(&g_Mesh.mtxWorld, &g_Mesh.mtxWorld, &mtxRot);

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans, g_Mesh.pos.x, g_Mesh.pos.y, g_Mesh.pos.z);
	D3DXMatrixMultiply(&g_Mesh.mtxWorld, &g_Mesh.mtxWorld, &mtxTrans);

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &g_Mesh.mtxWorld);

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, g_pVtxBuffMeshField, 0, sizeof(VERTEX_3D));

	// インデックスバッファをデータストリームに設定
	pDevice->SetIndices(g_pIdxBuffMeshField);

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	//テクスチャの設定
	pDevice->SetTexture(0, g_apTextureMeshField);

	// ポリゴンの描画
	pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP,
		0,
		0,
		MAX_ALLVTX,
		0,
		POLYGON);
}
