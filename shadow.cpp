//=============================
//
// 影の処理 [shadow.cpp]
// Author: Asuma Nishio
//
//=============================

//****************************
// インクルードファイル宣言
//****************************
#include "shadow.h"

//*****************************
// グローバル変数宣言
//*****************************
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffShadow = NULL;// 頂点バッファのポインタ
LPDIRECT3DTEXTURE9 g_apTextureShadow = NULL;	// テクスチャのポインタ
Shadow g_Shadow[MAX_SHADOW];					// 構造体変数

//=========================
//　影初期化処理
//=========================
void InitShadow(void)
{
	// デバイスポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,
		"data\\TEXTURE\\shadow000.jpg",
		&g_apTextureShadow);

	for (int nCnt = 0; nCnt < MAX_SHADOW; nCnt++)
	{// グローバル変数の初期化
		g_Shadow[nCnt].bUse = false;							  // 未使用判定
		g_Shadow[nCnt].posShadow = D3DXVECTOR3(0.0f, 0.5f, 0.0f); // 座標
		g_Shadow[nCnt].rotShadow = D3DXVECTOR3(0.0f, 0.0f, 0.0f); // 角度
		g_Shadow[nCnt].Scal = D3DXVECTOR3(0.0f, 0.5f, 0.0f);	  // 倍率
	}

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * 4 * MAX_SHADOW,
		D3DUSAGE_WRITEONLY, FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffShadow,
		NULL);

	// 頂点情報のポインタ
	VERTEX_3D* pVtx = NULL;

	// 頂点バッファをロックし,頂点情報へのポインタを取得
	g_pVtxBuffShadow->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntShadow = 0; nCntShadow < MAX_SHADOW;nCntShadow++)
	{
		// 頂点座標の設定
		pVtx[0].pos.x = -SHADOW_SIZE;
		pVtx[0].pos.y = 0.5f;  
		pVtx[0].pos.z = SHADOW_SIZE;
														  
		pVtx[1].pos.x = SHADOW_SIZE;
		pVtx[1].pos.y = 0.5f;  
		pVtx[1].pos.z = SHADOW_SIZE;
														   
		pVtx[2].pos.x = -SHADOW_SIZE;
		pVtx[2].pos.y = 0.5f;  
		pVtx[2].pos.z = -SHADOW_SIZE;
														   
		pVtx[3].pos.x = SHADOW_SIZE;
		pVtx[3].pos.y = 0.5f;  
		pVtx[3].pos.z = -SHADOW_SIZE;
		
		// 各頂点の法線(ベクトル)の設定
		pVtx[0].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);		// 1つ目の法線情報
		pVtx[1].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);		// 2つ目の法線情報
		pVtx[2].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);		// 3つ目の法線情報
		pVtx[3].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);		// 4つ目の法線情報

		// 頂点カラーの設定
		pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

		// テクスチャ座標の設定
		pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
		pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

		pVtx += 4;
	}

	// アンロック
	g_pVtxBuffShadow->Unlock();

}
//=========================
//　影終了処理
//=========================
void UninitShadow(void)
{
	// テクスチャの破棄
	if (g_apTextureShadow != NULL)
	{
		g_apTextureShadow->Release();
		g_apTextureShadow = NULL;
	}

	// 頂点バッファの破棄
	if (g_pVtxBuffShadow != NULL)
	{			  
		g_pVtxBuffShadow->Release();
		g_pVtxBuffShadow = NULL;
	}

}
//=========================
//　影更新処理
//=========================
void UpdateShadow(void)
{
	// ない
}
//=========================
//　影描画処理
//=========================
void DrawShadow(void)
{
	// デバイスポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// 減算合成の設定
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_REVSUBTRACT);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	// 計算用のマトリックスを宣言
	D3DXMATRIX mtxRot, mtxTrans,mtxScall;

	for (int nCnt = 0; nCnt < MAX_SHADOW; nCnt++)
	{
		if (g_Shadow[nCnt].bUse)
		{// 使われているとき

			// ワールドマトリックスの初期化
			D3DXMatrixIdentity(&g_Shadow[nCnt].mtxWorldShadow);

			// 倍率を設定
			D3DXMatrixScaling(&mtxScall, g_Shadow[nCnt].Scal.x, 0.5f, g_Shadow[nCnt].Scal.z);
			D3DXMatrixMultiply(&g_Shadow[nCnt].mtxWorldShadow, &g_Shadow[nCnt].mtxWorldShadow, &mtxScall);

			// 向きを反映
			D3DXMatrixRotationYawPitchRoll(&mtxRot, g_Shadow[nCnt].rotShadow.y, g_Shadow[nCnt].rotShadow.x, g_Shadow[nCnt].rotShadow.z);
			D3DXMatrixMultiply(&g_Shadow[nCnt].mtxWorldShadow, &g_Shadow[nCnt].mtxWorldShadow, &mtxRot);

			// 位置を反映
			D3DXMatrixTranslation(&mtxTrans, g_Shadow[nCnt].posShadow.x, g_Shadow[nCnt].posShadow.y, g_Shadow[nCnt].posShadow.z);
			D3DXMatrixMultiply(&g_Shadow[nCnt].mtxWorldShadow, &g_Shadow[nCnt].mtxWorldShadow, &mtxTrans);

			// ワールドマトリックスの設定
			pDevice->SetTransform(D3DTS_WORLD, &g_Shadow[nCnt].mtxWorldShadow);

			// 頂点バッファをデータストリームに設定
			pDevice->SetStreamSource(0, g_pVtxBuffShadow, 0, sizeof(VERTEX_3D));

			// 頂点フォーマットの設定
			pDevice->SetFVF(FVF_VERTEX_3D);

			//テクスチャの設定
			pDevice->SetTexture(0, g_apTextureShadow);

			// ポリゴンの描画
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, nCnt * 4,2);

		}
	}

	// 設定を戻す
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}
//=========================
//　影の設定処理
//=========================
int SetShadow(D3DXVECTOR3 pos, D3DXVECTOR3 rot,D3DXVECTOR3 Scal)
{
	// ローカル変数
	int nCntShadow;

	for (nCntShadow = 0; nCntShadow < MAX_SHADOW; nCntShadow++)
	{
		if (g_Shadow[nCntShadow].bUse)
		{// 使用状態なら
			continue;
		}

		g_Shadow[nCntShadow].posShadow = pos;	// 座標
		g_Shadow[nCntShadow].rotShadow = rot;	// 角度
		g_Shadow[nCntShadow].Scal = Scal;		// 大きさ
		g_Shadow[nCntShadow].bUse = true;		// 使用判定

		break;
		
	}
	return nCntShadow;		// 影の番号(index)を返す
}
//=============================
// 影の位置の更新処理
//=============================
void SetPositionShadow(int nidxSahdow, D3DXVECTOR3 pos)
{
	// 引数で指定された番号の影のposを設定
	g_Shadow[nidxSahdow].posShadow = pos;

}
//=============================
// 影を消す処理
//=============================
void DeleteShadow(int nidxSahdow)
{
	g_Shadow[nidxSahdow].bUse = false;
}


