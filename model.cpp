//=============================
//
// モデル処理 [model.cpp]
// Author: Asuma Nishio
//
//=============================

//****************************
// インクルードファイル宣言
//****************************
#include "model.h"
#include "input.h"


//*****************************
// グローバル変数宣言
//*****************************
Model g_Model;

//=========================
//　モデル初期化処理
//=========================
void InitModel(void)
{
	//// デバイスポインタを宣言
	//LPDIRECT3DDEVICE9 pDevice;

	//// デバイスを取得
	//pDevice = GetDevice();

	//// 変数の初期化
	//g_Model.pos = D3DXVECTOR3 (0.0f,0.0f,0.0f);
	//g_Model.rot = D3DXVECTOR3 (0.0f,0.0f,0.0f);

	//// Xファイルの読み込み
	//D3DXLoadMeshFromX("data\\MODEL\\block.x",
	//	D3DXMESH_SYSTEMMEM,
	//	pDevice,
	//	NULL,
	//	&g_Model.pBuffMatMode,
	//	NULL,
	//	&g_Model.dwNumMatModel,
	//	&g_Model.pMeshModel);

	//// マテリアルデータへのポインタ
	//D3DXMATERIAL* pMat;

	//// マテリアルデータへのポインタを取得
	//pMat = (D3DXMATERIAL*)g_Model.pBuffMatMode->GetBufferPointer();

	//// マテリアル数だけ回す
	//for (int nCntMat = 0; nCntMat < (int)g_Model.dwNumMatModel; nCntMat++)
	//{
	//	if (pMat[nCntMat].pTextureFilename != NULL)
	//	{
	//		// テクスチャの読み込み
	//		D3DXCreateTextureFromFile(pDevice,
	//			pMat[nCntMat].pTextureFilename,
	//			&g_apTextureModel[nCntMat]);

	//	}
	//}

}
//=========================
//　モデル終了処理
//=========================
void UninitModel(void)
{
	//// メッシュの破棄
	//if (g_Model.pMeshModel != NULL)
	//{
	//	g_Model.pMeshModel->Release();
	//	g_Model.pMeshModel = NULL;
	//}

	//// マテリアルの破棄
	//if (g_Model.pBuffMatMode != NULL)
	//{
	//	g_Model.pBuffMatMode->Release();
	//	g_Model.pBuffMatMode = NULL;
	//}

	//// テクスチャの破棄
	//for (int nCnt = 0; nCnt < MAX_TETURE; nCnt++)
	//{
	//	if (g_apTextureModel[nCnt] != NULL)
	//	{
	//		g_apTextureModel[nCnt]->Release();
	//		g_apTextureModel[nCnt] = NULL;
	//	}
	//}

}
//=========================
//　モデル更新処理
//=========================
void UpdateModel(void)
{
	// 一旦無しで

}
//=========================
//　モデル描画処理
//=========================
void DrawModel(void)
{
	//// デバイスポインタを宣言
	//LPDIRECT3DDEVICE9 pDevice = GetDevice();

	//// 計算用のマトリックスを宣言
	//D3DXMATRIX mtxRot, mtxTrans;

	//// 現在のマテリアルを保存
	//D3DMATERIAL9 matDef;

	//// マテリアルデータへのポインタ
	//D3DXMATERIAL* pMat;

	//// ワールドマトリックスの初期化
	//D3DXMatrixIdentity(&g_Model.mtxWorld);

	//// 向きを反映
	//D3DXMatrixRotationYawPitchRoll(&mtxRot, g_Model.rot.y, g_Model.rot.x, g_Model.rot.z);
	//D3DXMatrixMultiply(&g_Model.mtxWorld, &g_Model.mtxWorld, &mtxRot);

	//// 位置を反映
	//D3DXMatrixTranslation(&mtxTrans, g_Model.pos.x, g_Model.pos.y, g_Model.pos.z);
	//D3DXMatrixMultiply(&g_Model.mtxWorld, &g_Model.mtxWorld, &mtxTrans);

	//// ワールドマトリックスの設定
	//pDevice->SetTransform(D3DTS_WORLD, &g_Model.mtxWorld);

	//// 現在のマトリックスの取得
	//pDevice->GetMaterial(&matDef);

	//// マテリアルデータへのポインタを取得
	//pMat = (D3DXMATERIAL*)g_Model.pBuffMatMode->GetBufferPointer();

	//// マテリアル数だけ回す
	//for (int nCntMat = 0; nCntMat < (int)g_Model.dwNumMatModel; nCntMat++)
	//{
	//	// マテリアル設定
	//	pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

	//	// テクスチャ設定
	//	pDevice->SetTexture(0, g_apTextureModel[nCntMat]);

	//	// モデル(パーツ)の描画
	//	g_Model.pMeshModel->DrawSubset(nCntMat);

	//}

	//// マテリアルを戻す
	//pDevice->SetMaterial(&matDef);
}
