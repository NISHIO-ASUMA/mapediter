//=============================
//
// プレイヤー処理 [player.cpp]
// Author: Asuma Nishio
//
//=============================

//***************************
// インクルードファイル宣言
//***************************
#include "input.h"
#include "player.h"
#include "camera.h"
#include "shadow.h"
#include "block.h"
#include <stdio.h>
#include <string.h>

//***************************
// 体力バーの列挙型宣言
//***************************
typedef enum
{
	LIFE_FRAME = 0,   // 体力ゲージの枠
	LIFE_BAR,		  // 体力ゲージ
	LIFE_MAX
}LIFE;

//***************************
// 体力バーの構造体宣言
//***************************
typedef struct
{
	D3DXVECTOR3 pos;	// 座標
	D3DXCOLOR col;		// 色
	int nType;			// 種類
}LIFEBAR;

//********************************
// プロトタイプ宣言
//********************************
void LoadModel(void);			// モデル読み込み処理

//*****************************
// グローバル変数宣言
//*****************************
PLAYER g_Player;										// プレイヤーの構造体変数
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffLifebar = NULL;		// 頂点バッファのポインタ
LPDIRECT3DTEXTURE9 g_pTextureLifebar[LIFE_MAX] = {};	// テクスチャのポインタ
LIFEBAR g_aLifebar[LIFE_MAX];							// 体力ゲージの情報
bool bPad;
bool isHit;

//=========================
// プレイヤーの初期化処理
//=========================
void InitPlayer(void)
{
	// デバイスポインタを宣言
	LPDIRECT3DDEVICE9 pDevice;

	// デバイスを取得
	pDevice = GetDevice();

	// 変数の初期化
	g_Player.pos = D3DXVECTOR3(-20.0f, 0.0f, -120.0f);			// 座標
	g_Player.posOld = D3DXVECTOR3(0.0f, 0.0f, 0.0f);			// 過去の座標
	g_Player.rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);				// 角度
	g_Player.move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);				// 移動量
	g_Player.rotDestPlayer = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 目標
	g_Player.bjump = false;										// ジャンプ
	g_Player.bLanding = false;									// 着地判定
	g_Player.nIdxShadow = SetShadow(D3DXVECTOR3(g_Player.pos.x,0.0f,g_Player.pos.z), g_Player.rot,D3DXVECTOR3 (1.0f,0.0f,1.0f));// 影
	g_Player.bLoopMotion = true;								// ループ判定
	g_Player.nNumKey = 0;										// キーの総数
	g_Player.nCounterMotion = 0;								// モーションカウンター
	g_Player.nKey = 0;											// 現在のキーNo
	g_Player.nNumModel = 0;									// パーツの総数
	g_Player.bDisp = true;										// 使用状態
	g_Player.state = PLAYERSTATE_NEUTRAL;						// 初期状態
	g_Player.bWalk = false;										// 待機状態
	g_Player.nLife = 10;										// 初期体力
	g_Player.Pcon = PLAYERCONDITION_NORMAL;						// 初期状態
	bPad = false;												// パッドの状態
	isHit = true;												// 判定切り替えフラグ

	// モデル読み込み
	LoadModel();

	// オフセットを考慮する
	for (int nOff = 0; nOff < g_Player.nNumModel; nOff++)
	{
		g_Player.aModel[nOff].Offpos = g_Player.aModel[nOff].pos;
		g_Player.aModel[nOff].Offrot = g_Player.aModel[nOff].rot;
	}

	// ローカル変数宣言-----------------------------
	int nNumVtx;		// 頂点数
	DWORD sizeFVF;		// 頂点フォーマットのサイズ
	BYTE* pVtxBuff;		// 頂点バッファのポインタ
	//----------------------------------------------

	for (int nCntModel = 0; nCntModel < g_Player.nNumModel; nCntModel++)
	{// モデルの最大数分回す
		// 頂点数の取得
		nNumVtx = g_Player.aModel[nCntModel].pMeshModel->GetNumVertices();

		// 頂点のサイズを取得
		sizeFVF = D3DXGetFVFVertexSize(g_Player.aModel[nCntModel].pMeshModel->GetFVF());

		// 頂点バッファをロック
		g_Player.aModel[nCntModel].pMeshModel->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pVtxBuff);

		for (int nCnt = 0; nCnt < nNumVtx; nCnt++)
		{// 頂点数分のfor文を回す
			// 頂点座標の代入
			D3DXVECTOR3 Vtx = *(D3DXVECTOR3*)pVtxBuff;

			// 頂点座標の比較
			if (Vtx.x < g_Player.Vtxmin.x)
			{// xが最小値より小さかったら
				// 代入
				g_Player.Vtxmin.x = Vtx.x;
			}
			else if (Vtx.x > g_Player.Vtxmax.x)
			{// xが最大値より大きかったら
				// 代入
				g_Player.Vtxmax.x = Vtx.x;
			}

			if (Vtx.y < g_Player.Vtxmin.y)
			{// yが最小値より小さかったら
				// 代入
				g_Player.Vtxmin.y = Vtx.y;
			}
			else if (Vtx.y > g_Player.Vtxmax.y)
			{// yが最大値より大きかったら
				// 代入
				g_Player.Vtxmax.y = Vtx.y;
			}

			if (Vtx.z < g_Player.Vtxmin.z)
			{// zが最小値より小さかったら
				// 代入
				g_Player.Vtxmin.z = Vtx.z;
			}
			else if (Vtx.z > g_Player.Vtxmax.z)
			{// zが最大値より大きかったら
				// 代入
				g_Player.Vtxmax.z = Vtx.z;
			}

			//-----------------------------------------------
			if (Vtx.x < g_Player.aModel[nCntModel].Vtxmax.x)
			{// xが最大値より大きい
				// 代入
				g_Player.aModel[nCntModel].Vtxmax.x = Vtx.x;
			}
			else if (Vtx.y < g_Player.aModel[nCntModel].Vtxmax.y)
			{
				g_Player.aModel[nCntModel].Vtxmax.y = Vtx.y;
			}
			else if (Vtx.z < g_Player.aModel[nCntModel].Vtxmax.z)
			{
				g_Player.aModel[nCntModel].Vtxmax.z = Vtx.z;
			}

			if (Vtx.x > g_Player.aModel[nCntModel].Vtxmin.x)
			{// xが最大値より大きい
				// 代入
				g_Player.aModel[nCntModel].Vtxmin.x = Vtx.x;
			}
			else if (Vtx.y > g_Player.aModel[nCntModel].Vtxmin.y)
			{
				g_Player.aModel[nCntModel].Vtxmin.y = Vtx.y;
			}
			else if (Vtx.z > g_Player.aModel[nCntModel].Vtxmin.z)
			{
				g_Player.aModel[nCntModel].Vtxmin.z = Vtx.z;
			}
			//-------------------------------------------------------
			
			// 頂点フォーマットのサイズ分進める
			pVtxBuff += sizeFVF;
		}

		// サイズを保存
		g_Player.size.x = g_Player.Vtxmax.x - g_Player.Vtxmin.x;
		g_Player.size.y = g_Player.Vtxmax.y - g_Player.Vtxmin.y;
		g_Player.size.z = g_Player.Vtxmax.z - g_Player.Vtxmin.z;

		// モデルごとのサイズを保存
		g_Player.aModel[nCntModel].size.x = g_Player.aModel[nCntModel].Vtxmax.x - g_Player.aModel[nCntModel].Vtxmin.x;
		g_Player.aModel[nCntModel].size.y = g_Player.aModel[nCntModel].Vtxmax.y - g_Player.aModel[nCntModel].Vtxmin.y;
		g_Player.aModel[nCntModel].size.z = g_Player.aModel[nCntModel].Vtxmax.z - g_Player.aModel[nCntModel].Vtxmin.z;

		// アンロック
		g_Player.aModel[nCntModel].pMeshModel->UnlockVertexBuffer();
	}

	// マテリアルデータへのポインタ
	D3DXMATERIAL* pMat;

	for (int nCnt = 0; nCnt < g_Player.nNumModel; nCnt++)
	{
		// マテリアルデータへのポインタを取得
		pMat = (D3DXMATERIAL*)g_Player.aModel[nCnt].pBuffMatModel->GetBufferPointer();

		// マテリアル数だけ回す
		for (int nCntMat = 0; nCntMat < (int)g_Player.aModel[nCnt].dwNumMatModel; nCntMat++)
		{
			if (pMat[nCntMat].pTextureFilename != NULL)
			{
				// テクスチャの読み込み
				D3DXCreateTextureFromFile(pDevice,
					pMat[nCntMat].pTextureFilename,
					&g_Player.aModel[nCnt].pTexture[nCntMat]);
			}
		}

		// カラーを初期化する
		g_Player.aModel[nCnt].Diffuse = pMat->MatD3D.Diffuse;
		g_Player.aModel[nCnt].FirstDiffuse = g_Player.aModel[nCnt].Diffuse;
	}	

	// ライフバーの初期化
	InitLifeBar();
}
//=========================
// プレイヤーの終了処理
//=========================
void UninitPlayer(void)
{
	// メッシュの破棄
	for (int nCnt = 0; nCnt < g_Player.nNumModel; nCnt++)
	{
		if (g_Player.aModel[nCnt].pMeshModel != NULL)
		{
			g_Player.aModel[nCnt].pMeshModel->Release();
			g_Player.aModel[nCnt].pMeshModel = NULL;
		}

		if (g_Player.aModel[nCnt].pBuffMatModel != NULL)
		{
			g_Player.aModel[nCnt].pBuffMatModel->Release();
			g_Player.aModel[nCnt].pBuffMatModel = NULL;
		}

		for (int nCntModel = 0; nCntModel < (int)g_Player.aModel[nCnt].dwNumMatModel; nCntModel++)
		{
			if (g_Player.aModel[nCnt].pTexture[nCntModel] != NULL)
			{
				g_Player.aModel[nCnt].pTexture[nCntModel]->Release();
				g_Player.aModel[nCnt].pTexture[nCntModel] = NULL;
			}
		}
	}
	
	// ライフバーの終了
	UninitLifeBar();

}
//=========================
// プレイヤーの更新処理
//=========================
void UpdatePlayer(void)
{
	// カメラの情報を取得
	Camera* pCamera = GetCamera();

	
	// ジャンプ量を取得
	float fJump = ReturnJump();

	bPad = false;

	switch (g_Player.Pcon)
	{
	case PLAYERCONDITION_NORMAL:
		break;

	case PLAYERCONDITION_DAMAGE:
		g_Player.nCntState--;

		if (g_Player.nCntState <= 0)
		{
			g_Player.Pcon = PLAYERCONDTION_APPER;
			g_Player.nCntState = 80;
		}
		break;

	case PLAYERCONDTION_APPER:
	{
		g_Player.nCntState--;

		for (int nCnt = 0; nCnt < g_Player.nNumModel; nCnt++)
		{
			if (g_Player.nCntState % 5 == 0)
			{
				//透明にする
				g_Player.aModel[nCnt].Diffuse.a = 0.1f;

			}
			else
			{
				//通常色
				g_Player.aModel[nCnt].Diffuse.a = 1.0f;
			}

			if (g_Player.nCntState <= 0)
			{
				//プレイヤーを通常状態に
				//通常色
				g_Player.aModel[nCnt].Diffuse.a = 1.0f;
				g_Player.Pcon = PLAYERCONDITION_NORMAL;
			}

		}

	}
	break;

	default:
		break;
	}	

	// スティック
	Stick();

	if (bPad == false)
	{
		if (GetKeyboardPress(DIK_A) || JoypadPress(JOYKEY_LEFT))
		{// Aキーを押した

			if (GetKeyboardPress(DIK_W) || JoypadPress(JOYKEY_UP))
			{// Wキーを押した
				g_Player.move.x += sinf(pCamera->rot.y - D3DX_PI * 0.25f) * MAX_MOVE;
				g_Player.move.z += cosf(pCamera->rot.y - D3DX_PI * 0.25f) * MAX_MOVE;
				g_Player.rotDestPlayer.y = pCamera->rot.y + (D3DX_PI * 0.75f);

				g_Player.state = PLAYERSTATE_MOVE;

			}
			else if (GetKeyboardPress(DIK_S) || JoypadPress(JOYKEY_DOWN))
			{// Sキーを押した
				g_Player.move.x -= sinf(pCamera->rot.y + D3DX_PI * 0.25f) * MAX_MOVE;
				g_Player.move.z -= cosf(pCamera->rot.y + D3DX_PI * 0.25f) * MAX_MOVE;
				g_Player.rotDestPlayer.y = pCamera->rot.y + (D3DX_PI * 0.25f);

				//　タイプ切り替え
				g_Player.state = PLAYERSTATE_MOVE;
			}
			else
			{// Aキーのみを押した
				g_Player.move.x -= sinf(pCamera->rot.y + (D3DX_PI * 0.5f)) * MAX_MOVE;
				g_Player.move.z -= cosf(pCamera->rot.y + (D3DX_PI * 0.5f)) * MAX_MOVE;
				g_Player.rotDestPlayer.y = pCamera->rot.y + (D3DX_PI * 0.5f);

				//　タイプ切り替え
				g_Player.state = PLAYERSTATE_MOVE;

			}

			// 角度の正規化
			if (g_Player.rot.y > D3DX_PI)
			{// D3DX_PIより大きくなったら
				g_Player.rot.y -= D3DX_PI * 2.0f;
			}

		}
		else if (GetKeyboardPress(DIK_D) || JoypadPress(JOYKEY_RIGHT))
		{// Dキーを押した
			if (GetKeyboardPress(DIK_W) || JoypadPress(JOYKEY_UP))
			{// Wキーを押した
				g_Player.move.x += sinf(pCamera->rot.y + D3DX_PI * 0.25f) * MAX_MOVE;
				g_Player.move.z += cosf(pCamera->rot.y + D3DX_PI * 0.25f) * MAX_MOVE;
				g_Player.rotDestPlayer.y = pCamera->rot.y - (D3DX_PI * 0.75f);

				//　タイプ切り替え
				g_Player.state = PLAYERSTATE_MOVE;

			}

			else if (GetKeyboardPress(DIK_S) || JoypadPress(JOYKEY_DOWN))
			{// Sキーを押した
				g_Player.move.x -= sinf(pCamera->rot.y - D3DX_PI * 0.25f) * MAX_MOVE;
				g_Player.move.z -= cosf(pCamera->rot.y - D3DX_PI * 0.25f) * MAX_MOVE;
				g_Player.rotDestPlayer.y = pCamera->rot.y - (D3DX_PI * 0.25f);

				//　タイプ切り替え
				g_Player.state = PLAYERSTATE_MOVE;

			}

			else
			{// Dキーのみ押した
				g_Player.move.x += sinf(pCamera->rot.y + (D3DX_PI * 0.5f)) * MAX_MOVE;
				g_Player.move.z += cosf(pCamera->rot.y + (D3DX_PI * 0.5f)) * MAX_MOVE;
				g_Player.rotDestPlayer.y = pCamera->rot.y - (D3DX_PI * 0.5f);

				//　タイプ切り替え
				g_Player.state = PLAYERSTATE_MOVE;
			}

			// 角度の正規化
			if (g_Player.rot.y > D3DX_PI)
			{// D3DX_PIより大きくなったら
				g_Player.rot.y -= D3DX_PI * 2.0f;
			}

		}
		else if (GetKeyboardPress(DIK_W) || JoypadPress(JOYKEY_UP))
		{// Wキーを押した
			//　タイプ切り替え
			g_Player.state = PLAYERSTATE_MOVE;

			g_Player.move.x += sinf(pCamera->rot.y) * MAX_MOVE;
			g_Player.move.z += cosf(pCamera->rot.y) * MAX_MOVE;
			g_Player.rotDestPlayer.y = pCamera->rot.y - (D3DX_PI);

			// 角度を正規化
			if (g_Player.rot.y < -D3DX_PI)
			{// D3DX_PIより小さくなったら
				g_Player.rot.y += D3DX_PI * 2.0f;
			}

		}
		else if (GetKeyboardPress(DIK_S) || JoypadPress(JOYKEY_DOWN))
		{// Sキーを押した
			//　タイプ切り替え
			g_Player.state = PLAYERSTATE_MOVE;


			g_Player.move.x -= sinf(pCamera->rot.y) * MAX_MOVE;
			g_Player.move.z -= cosf(pCamera->rot.y) * MAX_MOVE;
			g_Player.rotDestPlayer.y = pCamera->rot.y;

			// 角度の正規化
			if (g_Player.rot.y > D3DX_PI)
			{// D3DX_PIより大きくなったら
				g_Player.rot.y -= D3DX_PI * 2.0f;
			}

		}
		else
		{
			if (g_Player.state != PLAYERSTATE_ACTION)
			{
				//　タイプ切り替え
				g_Player.state = PLAYERSTATE_NEUTRAL;
			}

		}

	}
	
	if (KeyboardTrigger(DIK_RETURN) || JoypadTrigger(JOYKEY_X))
	{// ENTERキーを押した
		// モーション変更
		g_Player.nKey = 0;
		g_Player.state = PLAYERSTATE_ACTION;

	}

	if (g_Player.rotDestPlayer.y - g_Player.rot.y > D3DX_PI)
	{// 左回転
		// 角度
		g_Player.rot.y = g_Player.rot.y + D3DX_PI * 2.0f;
	}
	else if (g_Player.rot.y - g_Player.rotDestPlayer.y > D3DX_PI)
	{// 右回転
		// 角度
		g_Player.rot.y = g_Player.rot.y - D3DX_PI * 2.0f;

	}

	// 現在の角度
	g_Player.rot.y += (g_Player.rotDestPlayer.y - g_Player.rot.y) * 0.25f;

	if (g_Player.bjump == false)
	{// ジャンプしていなくて着地していないとき
		if (KeyboardTrigger(DIK_SPACE) == true || JoypadTrigger(JOYKEY_A) == true)
		{
			g_Player.bjump = true;
			g_Player.bLanding = false;

			// ジャンプキーが押された
			g_Player.move.y = fJump;
		}
	}

	// ジャンプが使われている
	if (g_Player.bjump == true)
	{
		// モーションジャンプ
		g_Player.state = PLAYERSTATE_JUMP;

		if (g_Player.bLanding == true)
		{
			// 着地したら
			g_Player.state = PLAYERSTATE_NEUTRAL;

			if (g_Player.state != PLAYERSTATE_JUMP)
			{
				g_Player.bLoopMotion = true;
			}

			g_Player.bjump = false;
		}
	}

	//重力
	g_Player.move.y -= 0.7f;               // 重力加算

	// 移動量の減衰
	g_Player.move.x += (0.0f - g_Player.move.x) * 0.5f;
	g_Player.move.z += (0.0f - g_Player.move.z) * 0.5f;

	// 前回の位置を保存
	g_Player.posOld = g_Player.pos;

	// 位置を更新
	g_Player.pos.x += g_Player.move.x;
	g_Player.pos.z += g_Player.move.z;
	g_Player.pos.y += g_Player.move.y;


	if (isHit)
	{
		// ブロックとの当たり判定
		CollisionBlock(&g_Player.pos,
			&g_Player.posOld,
			&g_Player.move,
			&g_Player.size);
	}

	//// メッシュとの当たり判定
	//CollisionMeshWall();


	if (0.0f >= g_Player.pos.y)
	{// 地面より下
		g_Player.pos.y = 0.0f;
		g_Player.bLanding = true;
		g_Player.move.y = 0.0f;

	}

	// 影の座標更新設定
	SetPositionShadow(g_Player.nIdxShadow, D3DXVECTOR3(g_Player.pos.x,0.0f,g_Player.pos.z));

	// モーションの更新
	UpdateMotionPlayer();

	// ライフバーの更新
	UpdateLifeBar();
}
//=========================
// プレイヤーの描画処理
//=========================
void DrawPlayer(void)
{
	// モードの取得
	MODE nMode = GetMode();

	if (nMode == MODE_GAME)
	{
		// ライフバーの描画
		DrawLifeBar();
	}

	// デバイスポインタを宣言
	LPDIRECT3DDEVICE9 pDevice;

	// デバイスを取得
	pDevice = GetDevice();

	// 計算用のマトリックスを宣言
	D3DXMATRIX mtxRot, mtxTrans, mtxScall;

	// 現在のマテリアルを保存
	D3DMATERIAL9 matDef;

	// マテリアルデータへのポインタ
	D3DXMATERIAL* pMat;

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&g_Player.mtxWorld);

	// 向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, g_Player.rot.y, g_Player.rot.x, g_Player.rot.z);
	D3DXMatrixMultiply(&g_Player.mtxWorld, &g_Player.mtxWorld, &mtxRot);

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans, g_Player.pos.x, g_Player.pos.y, g_Player.pos.z);
	D3DXMatrixMultiply(&g_Player.mtxWorld, &g_Player.mtxWorld, &mtxTrans);

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &g_Player.mtxWorld);

	// 現在のマトリックスの取得
	pDevice->GetMaterial(&matDef);

	if (g_Player.bDisp)
	{
		// 全パーツ分を描画
		for (int nCnt = 0; nCnt < g_Player.nNumModel; nCnt++)
		{
			// ローカル変数-------------------------------------
			D3DXMATRIX mtxWorldModel, mtxTransModel, mtxRotModel;	// モデル計算用
			D3DXMATRIX mtxParnet;									// 親のマトリックス

			// パーツのワールドマトリックスの初期化
			D3DXMatrixIdentity(&g_Player.aModel[nCnt].mtxWorld);

			// パーツの向きを反映
			D3DXMatrixRotationYawPitchRoll(&mtxRotModel, g_Player.aModel[nCnt].rot.y, g_Player.aModel[nCnt].rot.x, g_Player.aModel[nCnt].rot.z);
			D3DXMatrixMultiply(&g_Player.aModel[nCnt].mtxWorld, &g_Player.aModel[nCnt].mtxWorld, &mtxRotModel);

			// パーツの位置を反映
			D3DXMatrixTranslation(&mtxTransModel, g_Player.aModel[nCnt].pos.x, g_Player.aModel[nCnt].pos.y, g_Player.aModel[nCnt].pos.z);
			D3DXMatrixMultiply(&g_Player.aModel[nCnt].mtxWorld, &g_Player.aModel[nCnt].mtxWorld, &mtxTransModel);

			// パーツの親マトリックスを設定
			if (g_Player.aModel[nCnt].nIndxModelParent != -1)
			{// 親モデルがある場合
				mtxParnet = g_Player.aModel[g_Player.aModel[nCnt].nIndxModelParent].mtxWorld;
			}
			else
			{// 親モデルがない場合
				mtxParnet = g_Player.mtxWorld;
			}

			// パーツのワールドマトリックスと親のワールドマトリックスを乗算
			D3DXMatrixMultiply(&g_Player.aModel[nCnt].mtxWorld,
				&g_Player.aModel[nCnt].mtxWorld,
				&mtxParnet);

			// ワールドマトリックスの設定
			pDevice->SetTransform(D3DTS_WORLD, &g_Player.aModel[nCnt].mtxWorld);

			// マテリアル数だけ回す
			for (int nCntMat = 0; nCntMat < (int)g_Player.aModel[nCnt].dwNumMatModel; nCntMat++)
			{
				// マテリアルデータへのポインタを取得
				pMat = (D3DXMATERIAL*)g_Player.aModel[nCnt].pBuffMatModel->GetBufferPointer();

				pMat[nCntMat].MatD3D.Diffuse = g_Player.aModel[nCnt].Diffuse;

				// マテリアル設定
				pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

				// テクスチャ設定
				pDevice->SetTexture(0, g_Player.aModel[nCnt].pTexture[nCntMat]);

				// モデル(パーツ)の描画
				g_Player.aModel[nCnt].pMeshModel->DrawSubset(nCntMat);

			}

		}
		// マテリアルを戻す
		pDevice->SetMaterial(&matDef);
	}

}
//=========================
// プレイヤーの取得処理
//=========================
PLAYER* GetPlayer(void)
{
	return &g_Player;
}
//================================
// プレイヤーのモーション更新処理
//================================
void UpdateMotionPlayer(void)
{
	//モデルパーツの更新
	for (int nCnt = 0; nCnt < g_Player.nNumModel; nCnt++)
	{
		// 次のキーを計算
		int nNextKey = (g_Player.nKey + 1) % g_Player.aMotionInfo[g_Player.state].nNumKey;

		// 境界チェック
		if (g_Player.nKey >= g_Player.aMotionInfo[g_Player.state].nNumKey)
		{// g_Player.nNumKeyより大きくなる またはnNextKeyがg_Player.nNumKeyより大きくなった時

			// キー数を戻す
			g_Player.nKey = 0;	
		}

		// キー情報から位置と向きを算出
		D3DXVECTOR3 posMotion, rotMotion;

		// 差分を計算する
		posMotion.x = g_Player.aMotionInfo[g_Player.state].aKeyInfo[nNextKey].aKey[nCnt].fPosX - g_Player.aMotionInfo[g_Player.state].aKeyInfo[g_Player.nKey].aKey[nCnt].fPosX;
		posMotion.y = g_Player.aMotionInfo[g_Player.state].aKeyInfo[nNextKey].aKey[nCnt].fPosY - g_Player.aMotionInfo[g_Player.state].aKeyInfo[g_Player.nKey].aKey[nCnt].fPosY;
		posMotion.z = g_Player.aMotionInfo[g_Player.state].aKeyInfo[nNextKey].aKey[nCnt].fPosZ - g_Player.aMotionInfo[g_Player.state].aKeyInfo[g_Player.nKey].aKey[nCnt].fPosZ;

		rotMotion.x = g_Player.aMotionInfo[g_Player.state].aKeyInfo[nNextKey].aKey[nCnt].fRotX - g_Player.aMotionInfo[g_Player.state].aKeyInfo[g_Player.nKey].aKey[nCnt].fRotX;
		rotMotion.y = g_Player.aMotionInfo[g_Player.state].aKeyInfo[nNextKey].aKey[nCnt].fRotY - g_Player.aMotionInfo[g_Player.state].aKeyInfo[g_Player.nKey].aKey[nCnt].fRotY;
		rotMotion.z = g_Player.aMotionInfo[g_Player.state].aKeyInfo[nNextKey].aKey[nCnt].fRotZ - g_Player.aMotionInfo[g_Player.state].aKeyInfo[g_Player.nKey].aKey[nCnt].fRotZ;

		// 求める値を保存する変数を宣言
		D3DXVECTOR3 Answer, Answer1;

		// 補間係数を計算
		float fDis = (float)g_Player.nCounterMotion / g_Player.aMotionInfo[g_Player.state].aKeyInfo[g_Player.nKey].nFrame;

		Answer.x =  (g_Player.aMotionInfo[g_Player.state].aKeyInfo[g_Player.nKey].aKey[nCnt].fPosX + posMotion.x * fDis);
		Answer.y =  (g_Player.aMotionInfo[g_Player.state].aKeyInfo[g_Player.nKey].aKey[nCnt].fPosY + posMotion.y * fDis);
		Answer.z =  (g_Player.aMotionInfo[g_Player.state].aKeyInfo[g_Player.nKey].aKey[nCnt].fPosZ + posMotion.z * fDis);

		Answer1.x = (g_Player.aMotionInfo[g_Player.state].aKeyInfo[g_Player.nKey].aKey[nCnt].fRotX + rotMotion.x * fDis);
		Answer1.y = (g_Player.aMotionInfo[g_Player.state].aKeyInfo[g_Player.nKey].aKey[nCnt].fRotY + rotMotion.y * fDis);
		Answer1.z = (g_Player.aMotionInfo[g_Player.state].aKeyInfo[g_Player.nKey].aKey[nCnt].fRotZ + rotMotion.z * fDis);

		// パーツの位置
		g_Player.aModel[nCnt].pos = g_Player.aModel[nCnt].Offpos + Answer;
		g_Player.aModel[nCnt].rot = g_Player.aModel[nCnt].Offrot + Answer1;
	}
	
	if (g_Player.bLoopMotion)
	{
		g_Player.nCounterMotion++;	// インクリメントをしてカウンターを加算
	}

	if (g_Player.aMotionInfo[g_Player.state].bLoop == false)
	{
		if (g_Player.nKey >= g_Player.aMotionInfo[g_Player.state].nNumKey -1)
		{
			g_Player.nKey = g_Player.aMotionInfo[g_Player.state].nNumKey - 1;
			g_Player.bLoopMotion = false;
		}
	}

	if (g_Player.state == PLAYERSTATE_ACTION && g_Player.bLoopMotion == false)
	{
		g_Player.state = PLAYERSTATE_NEUTRAL;
		g_Player.bLoopMotion = true;
		
	}

	if (g_Player.nCounterMotion >= g_Player.aMotionInfo[g_Player.state].aKeyInfo[g_Player.nKey].nFrame)
	{// フレーム数の上限に達したら

		g_Player.nCounterMotion = 0;	// カウンターを初期値に戻す

		g_Player.nKey++;				// キー数をインクリメント
	}

}

//=====================================
// プレイヤーのモデル読み込み処理
//=====================================
void LoadModel(void)
{
	// デバイスポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// ローカル変数
	int nIdx = 0;					// インデックス
	int NumKey = 0;					// キー数
	int nCnt = 0;					// モデルカウント用
	int nNumParts = 0;				// パーツ数格納用
	int nCntMotion = 0;				// モーションカウント用
	int nCntKey = 0;				// キーカウント用
	int nCntPosKey = 0;				// posカウント
	int nCntRotkey = 0;				// rotカウント

	// ファイルポインタを宣言
	FILE* pFile;

	// ファイルを開く
	pFile = fopen("data/motion.txt", "r");

	if (pFile != NULL)
	{//　NULL じゃない
		char aString[MAX_WORD];

		while (1)
		{
			// 読み飛ばし
			fscanf(pFile, "%s", &aString[0]);

			if (strcmp(&aString[0], "SCRIPT") == 0)
			{// SCRIPTを読み取ったら
				while (1)
				{
					// 読み飛ばし
					fscanf(pFile, "%s", &aString[0]);

					if (strcmp(&aString[0], "NUM_MODEL") == 0)
					{// NUM_MODELを読み取ったら
						fscanf(pFile, "%s", &aString[0]);

						if (strcmp(&aString[0], "=") == 0)
						{// 値を代入
							fscanf(pFile, "%d", &g_Player.nNumModel);
						}
					}

					else if (strcmp(&aString[0], "MODEL_FILENAME") == 0)
					{
						// MODEL_FILENAMEを読み取ったら
						fscanf(pFile, "%s", &aString[0]);

						if (strcmp(&aString[0], "=") == 0)
						{// 代入
							// 文字列を読み込む
							fscanf(pFile, "%s", &aString[0]);

							const char* MODEL_FILE = {};	// モデルファイル名格納用の変数

							// 読み込んだ文字列を保存する
							MODEL_FILE = aString;

							//Xファイルの読み込み
							D3DXLoadMeshFromX(MODEL_FILE,
								D3DXMESH_SYSTEMMEM,
								pDevice,
								NULL,
								&g_Player.aModel[nCnt].pBuffMatModel,
								NULL,
								&g_Player.aModel[nCnt].dwNumMatModel,
								&g_Player.aModel[nCnt].pMeshModel);

							nCnt++; // nCntをインクリメント
						}
					}
					else if (strcmp(&aString[0], "CHARACTERSET") == 0)
					{
						while (1)
						{
							// 文字列を読み飛ばす
							fscanf(pFile, "%s", &aString[0]);

							if (strcmp(&aString[0], "NUM_PARTS") == 0)
							{// NUM_PARTSを読み取ったら

								fscanf(pFile, "%s", &aString[0]);

								if (strcmp(&aString[0], "=") == 0)
								{// 値を代入
									fscanf(pFile, "%d", &nNumParts);
								}
							}

							else if (strcmp(&aString[0], "PARTSSET") == 0)
							{
								while (1)
								{
									// 文字列を読み飛ばす
									fscanf(pFile, "%s", &aString[0]);

									if (strcmp(&aString[0], "INDEX") == 0)
									{// INDEXを読み取ったら

										fscanf(pFile, "%s", &aString[0]);

										if (strcmp(&aString[0], "=") == 0)
										{// 代入
											fscanf(pFile, "%d", &nIdx);
										}
									}

									if (strcmp(&aString[0], "PARENT") == 0)
									{// PARENTを読み取ったら

										fscanf(pFile, "%s", &aString[0]);

										if (strcmp(&aString[0], "=") == 0)
										{// 代入
											// ペアネント
											fscanf(pFile, "%d", &g_Player.aModel[nIdx].nIndxModelParent);
										}
									}


									if (strcmp(&aString[0], "POS") == 0)
									{// INDEXを読み取ったら

										fscanf(pFile, "%s", &aString[0]);

										if (strcmp(&aString[0], "=") == 0)
										{// 座標を代入
											fscanf(pFile, "%f", &g_Player.aModel[nIdx].pos.x);
											fscanf(pFile, "%f", &g_Player.aModel[nIdx].pos.y);
											fscanf(pFile, "%f", &g_Player.aModel[nIdx].pos.z);
										}
									}

									if (strcmp(&aString[0], "ROT") == 0)
									{// INDEXを読み取ったら

										fscanf(pFile, "%s", &aString[0]);

										if (strcmp(&aString[0], "=") == 0)
										{// 角度を代入
											fscanf(pFile, "%f", &g_Player.aModel[nIdx].rot.x);
											fscanf(pFile, "%f", &g_Player.aModel[nIdx].rot.y);
											fscanf(pFile, "%f", &g_Player.aModel[nIdx].rot.z);
										}
									}

									if (strcmp(&aString[0], "END_PARTSSET") == 0)
									{// END_PARTSSETを読み取ったら
										// whileを抜ける
										break;
									}

								}// while文末
							}
							else if (strcmp(&aString[0], "END_CHARACTERSET") == 0)
							{
								break;
							}
						}
					}
					else if (strcmp(&aString[0], "MOTIONSET") == 0)
					{// MOTIONSETを読み取ったら


						while (1)
						{
							// 文字を読み飛ばす
							fscanf(pFile, "%s", &aString[0]);

							if (strcmp(aString, "LOOP") == 0)
							{// LOOP を読み取ったら
								// 文字を読み飛ばす
								fscanf(pFile, "%s", &aString[0]);

								if (strcmp(&aString[0], "=") == 0)
								{// = を読み取ったら
									// 値を代入
									fscanf(pFile, "%d", &g_Player.aMotionInfo[nCntMotion].bLoop);
								}
							}

							else if (strcmp(aString, "NUM_KEY") == 0)
							{// NUM_KEYを読み取ったら
								// 文字を読み飛ばす
								fscanf(pFile, "%s", &aString[0]);

								if (strcmp(&aString[0], "=") == 0)
								{// = を読み取ったら
									// 値を代入
									fscanf(pFile, "%d", &g_Player.aMotionInfo[nCntMotion].nNumKey);
								}

								while (nCntKey < g_Player.aMotionInfo[nCntMotion].nNumKey)
								{
									// 文字を読み飛ばす
									fscanf(pFile, "%s", &aString[0]);

									if (strcmp(aString, "KEYSET") == 0)
									{// KEYSETを読み取ったら


										while (1)
										{
											// 文字を読み飛ばす
											fscanf(pFile, "%s", &aString[0]);


											if (strcmp(&aString[0], "FRAME") == 0)
											{// FRAMEを読み取ったら
												// 文字を読み飛ばす
												fscanf(pFile, "%s", &aString[0]);

												if (strcmp(&aString[0], "=") == 0)
												{// 値を代入
													fscanf(pFile, "%d", &g_Player.aMotionInfo[nCntMotion].aKeyInfo[nCntKey].nFrame);
													break;
												}
											}

										}

										while (1)
										{
											// 文字を読み飛ばす
											fscanf(pFile, "%s", &aString[0]);
											if (strcmp(&aString[0], "KEY") == 0)
											{// KEYを読みとったら
												while (1)
												{
													// 文字を読み飛ばす
													fscanf(pFile, "%s", &aString[0]);

													if (strcmp(&aString[0], "POS") == 0)
													{// POSを読み取ったら
														// 文字を読み飛ばす
														fscanf(pFile, "%s", &aString[0]);

														if (strcmp(&aString[0], "=") == 0)
														{// 値を代入
															fscanf(pFile, "%f", &g_Player.aMotionInfo[nCntMotion].aKeyInfo[nCntKey].aKey[nCntPosKey].fPosX);
															fscanf(pFile, "%f", &g_Player.aMotionInfo[nCntMotion].aKeyInfo[nCntKey].aKey[nCntPosKey].fPosY);
															fscanf(pFile, "%f", &g_Player.aMotionInfo[nCntMotion].aKeyInfo[nCntKey].aKey[nCntPosKey].fPosZ);
															nCntPosKey++;		// インクリメント
														}
													}

													else if (strcmp(&aString[0], "ROT") == 0)
													{// ROTを読み取ったら
														// 文字を読み飛ばす
														fscanf(pFile, "%s", &aString[0]);

														if (strcmp(&aString[0], "=") == 0)
														{// 値を代入
															fscanf(pFile, "%f", &g_Player.aMotionInfo[nCntMotion].aKeyInfo[nCntKey].aKey[nCntRotkey].fRotX);
															fscanf(pFile, "%f", &g_Player.aMotionInfo[nCntMotion].aKeyInfo[nCntKey].aKey[nCntRotkey].fRotY);
															fscanf(pFile, "%f", &g_Player.aMotionInfo[nCntMotion].aKeyInfo[nCntKey].aKey[nCntRotkey].fRotZ);
															nCntRotkey++;		// インクリメント
														}
													}

													else if (strcmp(&aString[0], "END_KEY") == 0)
													{// END_KEYを読み取ったら
														break;
													}
												}
											}
											else if (strcmp(&aString[0], "END_KEYSET") == 0)
											{// END_KEYSETを読み取ったら
												nCntRotkey = 0;
												nCntPosKey = 0;
												nCntKey++;			// インクリメント
												break;
											}


										}

									}

								}
							}

							if (strcmp(&aString[0], "END_MOTIONSET") == 0)
							{// END_MOTIONSETを読み取ったら
								nCntMotion++;		// モーションの更新
								nCntKey = 0;		// 0から始める
								break;
							}
						}
					}

					else if (strcmp(&aString[0], "END_SCRIPT") == 0)
					{
						// ファイルを閉じる
						fclose(pFile);

						break;
					}
					else
					{
						continue;
					}
				}// while文末

				break;
			}
		}// while文末
	}
}
//=========================
// ライフバーの初期化処理
//=========================
 void InitLifeBar(void)
 {
	 // デバイスへのポインタ
	 LPDIRECT3DDEVICE9 pDevice = GetDevice();		

	 // 頂点情報のポインタ
	 VERTEX_2D* pVtx;				

	 // テクスチャの読み込み
	 D3DXCreateTextureFromFile(pDevice,
		 "data\\TEXTURE\\ber.png",
		 &g_pTextureLifebar[0]);

	 // テクスチャの読み込み
	 D3DXCreateTextureFromFile(pDevice,
		 "data\\TEXTURE\\lifebar.png",
		 &g_pTextureLifebar[1]);

	 // 構造体の初期化
	 for (int nCnt = 0; nCnt < LIFE_MAX; nCnt++)
	 {
		 g_aLifebar[nCnt].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);	// カラー
		 g_aLifebar[nCnt].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 座標
	 }

	 g_aLifebar[LIFE_FRAME].nType = LIFE_FRAME;	// バーの枠
	 g_aLifebar[LIFE_BAR].nType = LIFE_BAR;		// バーのゲージ

	 // 頂点バッファの生成
	 pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4 * LIFE_MAX,
		 D3DUSAGE_WRITEONLY, FVF_VERTEX_2D,
		 D3DPOOL_MANAGED,
		 &g_pVtxBuffLifebar,
		 NULL);

	 // 頂点バッファをロックし,頂点情報へのポインタを取得
	 g_pVtxBuffLifebar->Lock(0, 0, (void**)&pVtx, 0);

	 for (int nCnt1 = 0; nCnt1 < LIFE_MAX; nCnt1++)
	 {
		 // 頂点座標の設定
		 pVtx[0].pos = D3DXVECTOR3(15.0f, 5.0f, 0.0f);							// 1つ目の頂点情報
		 pVtx[1].pos = D3DXVECTOR3(15.0f + (g_Player.nLife * 40), 5.0f, 0.0f);	// 2つ目の頂点情報
		 pVtx[2].pos = D3DXVECTOR3(15.0f, 55.0f, 0.0f);							// 3つ目の頂点情報
		 pVtx[3].pos = D3DXVECTOR3(15.0f + (g_Player.nLife * 40), 55.0f, 0.0f); // 4つ目の頂点情報

		 // rhwの設定(1.0fで固定)
		 pVtx[0].rhw = 1.0f;
		 pVtx[1].rhw = 1.0f;
		 pVtx[2].rhw = 1.0f;
		 pVtx[3].rhw = 1.0f;

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

		 // ポインタをずらす
		 pVtx += 4;
	 }

	 //アンロック
	 g_pVtxBuffLifebar->Unlock();

 }
//=========================
// ライフバーの終了処理
//=========================
 void UninitLifeBar(void)
 {
	 for (int nCnt = 0; nCnt < LIFE_MAX; nCnt++)
	 {
		 // テクスチャの破棄
		 if (g_pTextureLifebar[nCnt] != NULL)
		 {
			 g_pTextureLifebar[nCnt]->Release();
			 g_pTextureLifebar[nCnt] = NULL;
		 }
	 }

	 // 頂点バッファの破棄
	 if (g_pVtxBuffLifebar != NULL)
	 {
		 g_pVtxBuffLifebar->Release();
		 g_pVtxBuffLifebar = NULL;
	 }

 }
//=========================
// ライフバーの更新処理
//=========================
 void UpdateLifeBar(void)
 {
	 // 頂点情報のポインタ
	 VERTEX_2D* pVtx;		

	 // 頂点バッファをロックし,頂点情報へのポインタを取得
	 g_pVtxBuffLifebar->Lock(0, 0, (void**)&pVtx, 0);

	 for (int nCnt1 = 0; nCnt1 < LIFE_MAX; nCnt1++)
	 {
		 if (g_aLifebar[nCnt1].nType == LIFE_BAR)
		 {
			 // 頂点座標の設定
			 pVtx[0].pos = D3DXVECTOR3(15.0f, 5.0f, 0.0f);							// 1つ目の頂点情報
			 pVtx[1].pos = D3DXVECTOR3(15.0f + (g_Player.nLife * 40), 5.0f, 0.0f);	// 2つ目の頂点情報
			 pVtx[2].pos = D3DXVECTOR3(15.0f, 55.0f, 0.0f);							// 3つ目の頂点情報
			 pVtx[3].pos = D3DXVECTOR3(15.0f + (g_Player.nLife * 40), 55.0f, 0.0f);	// 4つ目の頂点情報

			 // rhwの設定(1.0fで固定)
			 pVtx[0].rhw = 1.0f;
			 pVtx[1].rhw = 1.0f;
			 pVtx[2].rhw = 1.0f;
			 pVtx[3].rhw = 1.0f;

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

		 }

		 // 頂点情報分進める
		 pVtx += 4;
	 }

	 // アンロック
	 g_pVtxBuffLifebar->Unlock();

 }
//=========================
// ライフバーの描画処理
//=========================
 void DrawLifeBar(void)
 {
	 // デバイスへのポインタ
	 LPDIRECT3DDEVICE9 pDevice = GetDevice();		

	 // 頂点バッファをデータストリームに設定
	 pDevice->SetStreamSource(0, g_pVtxBuffLifebar, 0, sizeof(VERTEX_2D));

	 // 頂点フォーマットの設定
	 pDevice->SetFVF(FVF_VERTEX_2D);

	 for (int nCnt = 0; nCnt < LIFE_MAX; nCnt++)
	 {
		 // テクスチャの設定
		 pDevice->SetTexture(0, g_pTextureLifebar[nCnt]);

		 // ポリゴン描画
		 pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, nCnt * 4, 2);
	 }
 }
 //======================
 // スティック
 //======================
 void Stick(void)
 {
	 // スティック
	 XINPUT_STATE* pStick = GetJoyStickAngle();

	 Camera* pCamera = GetCamera();

	 if (GetJoyStick())
	 {
		 float LStickAngleY = pStick->Gamepad.sThumbLY;
		 float LStickAngleX = pStick->Gamepad.sThumbLX;

		 float DeadZone = 10920;
		 float fMag = sqrtf((LStickAngleX * LStickAngleX) + (LStickAngleY * LStickAngleY));


		 if (fMag > DeadZone)
		 {
			 bPad = true;
			 float normalizeX = (LStickAngleX / fMag);
			 float normalizeY = (LStickAngleY / fMag);


			 float MoveX = normalizeX * cosf(-pCamera->rot.y) - normalizeY * sinf(-pCamera->rot.y);
			 float MoveZ = normalizeX * sinf(-pCamera->rot.y) + normalizeY * cosf(-pCamera->rot.y);

			 g_Player.move.x += MoveX * MAX_MOVE;
			 g_Player.move.z += MoveZ * MAX_MOVE;

			 g_Player.rotDestPlayer.y = atan2f(-MoveX, -MoveZ);

			 g_Player.state = PLAYERSTATE_MOVE;
		 }
	 }
 }
 //==========================
 // 当たり判定を返す
 //==========================
 bool ReturnHit()
 {
	 return isHit;
 }