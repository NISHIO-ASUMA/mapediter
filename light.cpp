//==========================
//
// ライト処理 [light.cpp]
// Author: Asuma Nishio
//
//==========================

//****************************
// インクルードファイル宣言
//****************************
#include "light.h"

//*****************************
// グローバル変数宣言
//*****************************
D3DLIGHT9 g_light[MAX_LIGHT];		// ライト情報
D3DXVECTOR3 g_vecDir[MAX_LIGHT];	// ライトの数

//=========================
//　カメラの初期化処理
//=========================
void InitLight(void)
{
	// デバイスポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// ライトのクリア処理(ゼロメモリー関数)
	ZeroMemory(&g_light, sizeof(g_light));

	// ライトの種類を設定
	for (int nCnt = 0; nCnt < MAX_LIGHT; nCnt++)
	{
		// ライトの方向ベクトル
		g_vecDir[nCnt];

		g_light[nCnt].Type = D3DLIGHT_DIRECTIONAL;	// 平行光源

		// ライトの拡散光
		g_light[nCnt].Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	}

	// ライトの方向設定
	g_vecDir[0] = D3DXVECTOR3(0.2f, -0.8f, -0.4f);
	g_vecDir[1] = D3DXVECTOR3(-0.6f, 0.1f, 0.4f);
	g_vecDir[2] = D3DXVECTOR3(-0.2f, 0.6f, 0.6f);

	for (int nCnt1 = 0; nCnt1 < MAX_LIGHT; nCnt1++)
	{// 正規化する

		D3DXVec3Normalize(&g_vecDir[nCnt1], &g_vecDir[nCnt1]); // ベクトルの大きさを1にする

		g_light[nCnt1].Direction = g_vecDir[nCnt1];

		// ライトの設定
		pDevice->SetLight(nCnt1, &g_light[nCnt1]);

		// ライトを有効化
		pDevice->LightEnable(nCnt1, TRUE);

	}
}
//=========================
//　ライトの終了処理
//=========================
void UninitLight(void)
{
	// 一旦無しで
}
//=========================
//　ライトの更新処理
//=========================
void UpdateLight(void)
{
	// 一旦無しで
}

