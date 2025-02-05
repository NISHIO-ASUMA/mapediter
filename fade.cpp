//=============================
// 
// フェード処理 [fade.cpp]
// Author:Asuma Nishio
//
//=============================

//**************************
// インクルードファイル宣言
//**************************
#include "fade.h"

//***************************
// グローバル変数宣言
//***************************
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffFade = NULL;	// 頂点バッファのポインタ
FADE g_fade;			// フェードの状態
MODE g_modeNext;		// 次の画面
D3DXCOLOR g_colorFade;	// ポリゴンの色

//====================
// フェード初期化
//====================
void InitFade(MODE modeNext)
{
	g_fade = FADE_IN;
	g_modeNext = modeNext;
	g_colorFade = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);	// 黒ポリゴン

	LPDIRECT3DDEVICE9 pDevice;		// デバイスへのポインタ

	// デバイスの取得
	pDevice = GetDevice();

	VERTEX_2D* pVtx;// 頂点情報のポインタ

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffFade,
		NULL);

	// 頂点バッファをロックし,頂点情報へのポインタを取得
	g_pVtxBuffFade->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標の設定
	pVtx[0].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 1つ目の頂点情報
	pVtx[1].pos = D3DXVECTOR3(1280.0f, 0.0f, 0.0f);		// 2つ目の頂点情報
	pVtx[2].pos = D3DXVECTOR3(0.0f, 720.0f, 0.0f);		// 3つ目の頂点情報
	pVtx[3].pos = D3DXVECTOR3(1280.0f, 720.0f, 0.0f);	// 4つ目の頂点情報

	// rhwの設定(1.0fで固定)
	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;

	// 頂点カラーの設定
	pVtx[0].col = D3DXCOLOR(g_colorFade);
	pVtx[1].col = D3DXCOLOR(g_colorFade);
	pVtx[2].col = D3DXCOLOR(g_colorFade);
	pVtx[3].col = D3DXCOLOR(g_colorFade);

	// アンロック
	g_pVtxBuffFade->Unlock();

}
//=====================
// フェードの終了処理
//=====================
void UninitFade(void)
{
    // 頂点バッファの破棄
	if (g_pVtxBuffFade != NULL)
	{
		g_pVtxBuffFade->Release();
		g_pVtxBuffFade = NULL;
	}

}
//=====================
// フェード更新
//=====================
void UpdateFade(void)
{
	// 頂点情報のポインタ
	VERTEX_2D* pVtx;		

	if (g_fade != FADE_NONE)
	{
		if (g_fade == FADE_IN)
		{// フェードイン状態
			// 透明にしていく
			g_colorFade.a -= 0.03f;		
			if (g_colorFade.a <= 0.0f)
			{
				g_colorFade.a = 0.0f;

				// 何もしていない状態
				g_fade = FADE_NONE;		
			}

			// 頂点バッファをロックし,頂点情報へのポインタを取得
			g_pVtxBuffFade->Lock(0, 0, (void**)&pVtx, 0);

			// 頂点カラーの設定
			pVtx[0].col = D3DXCOLOR(g_colorFade);
			pVtx[1].col = D3DXCOLOR(g_colorFade);
			pVtx[2].col = D3DXCOLOR(g_colorFade);
			pVtx[3].col = D3DXCOLOR(g_colorFade);

			// アンロック
			g_pVtxBuffFade->Unlock();
		}
		else if (g_fade == FADE_OUT)
		{
			// フェードアウト状態
			g_colorFade.a += 0.03f;
			if (g_colorFade.a >= 1.0f)
			{
				g_colorFade.a = 1.0f;
				g_fade = FADE_IN;

				return;
			}

			// 頂点バッファをロックし,頂点情報へのポインタを取得
			g_pVtxBuffFade->Lock(0, 0, (void**)&pVtx, 0);

			// 頂点カラーの設定
			pVtx[0].col = D3DXCOLOR(g_colorFade);
			pVtx[1].col = D3DXCOLOR(g_colorFade);
			pVtx[2].col = D3DXCOLOR(g_colorFade);
			pVtx[3].col = D3DXCOLOR(g_colorFade);

			// アンロック
			g_pVtxBuffFade->Unlock();
		}
	}
}
//====================
// フェードの描画
//====================
void DrawFade(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, g_pVtxBuffFade, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	// テクスチャの設定
	pDevice->SetTexture(0, NULL);		

	// フェードの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);		

}
//=========================
// フェードの設定
//=========================
void SetFade(MODE modeNext)
{
	g_fade = FADE_OUT;		// フェードアウト状態
	g_modeNext = modeNext;	// 次の画面の設定
}
//========================
// フェードの取得
//========================
FADE GetFade(void)
{
	return g_fade;
}
