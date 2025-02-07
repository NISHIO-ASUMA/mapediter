//===================================
//
// マップエディター処理 [mapedit.cpp]
// Author: Asuma Nishio
//
//===================================

//****************************
// インクルードファイル宣言
//****************************
#include "mapedit.h"
#include "camera.h"
#include "meshfield.h"
#include "light.h"
#include "input.h"
#include <stdio.h>
#include "block.h"

//*************************************
// ファイル名列挙型宣言
//*************************************
typedef enum
{
	FILEPASS_0 = 0,	// 初期ファイルパス
	FILEPASS_1,		// 2個目
	FILEPASS_2,		// 3個目
	FILEPASS_MAX
}FILEPASS;

//*************************************
// 書き出すファイルパスを設定
//*************************************
const char* WRITE_FILENAME[FILEPASS_MAX] =
{
	"data\\stage000.bin",	// 初期状態
	"data\\stage001.bin",	// 2番目
	"data\\stage002.bin",	// 3番目
};

//****************************
// グローバル変数宣言
//****************************
EDITMODEL g_ModelInfo[MAX_TEXTURE];	 // ブロックの情報を保存
MAPMODELINFO g_MapEdit[MAX_EDITOBJ]; // 配置時の情報
int g_nTypeNum;						 // ブロックの種類数を保存
int g_Edit;							 // 配置数をカウント
float g_fspeed;						 // 移動量
float g_jump;						 // ジャンプ
int g_CurrentFilepass;				 // ファイルパスのインデックス
bool isLoad;						 // ロードしたかどうか

//****************************
// プロトタイプ宣言
//****************************
void InitEditinfo(); // 構造体初期化

//============================
// マップエディター初期化処理
//============================
void InitMapEdit()
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// カメラの初期化
	InitCamera();

	// ライトの初期化
	InitLight();

	// メッシュフィールドの初期化
	InitMeshField();

	// ブロック情報の読み込み
	LoadXFile();

	// 構造体情報
	InitEditinfo();

	// グローバル変数の初期化
	g_Edit = 0; // 配置数のカウント
	g_MapEdit[0].bUse = true;	// 使用状態
	g_CurrentFilepass = FILEPASS_0;	// 初期ファイルパス
	isLoad = false;	// ロード状態
}
//============================
// マップエディター終了処理
//============================
void UninitMapEdit()
{
	// メッシュフィールドの終了
	UninitMeshField();

	// ライトの終了
	UninitLight();

	// テクスチャの破棄
	for (int i = 0; i < MAX_TEXTURE; i++)
	{
		for (int nCntMat = 0; nCntMat < (int)g_ModelInfo[i].dwNumMat; nCntMat++)
		{
			if (g_ModelInfo[i].pTexture[nCntMat] != NULL)
			{
				g_ModelInfo[i].pTexture[nCntMat]->Release();
				g_ModelInfo[i].pTexture[nCntMat] = NULL;
			}
		}

		// メッシュの破棄
		if (g_ModelInfo[i].pMesh != NULL)
		{
			g_ModelInfo[i].pMesh->Release();
			g_ModelInfo[i].pMesh = NULL;
		}

		// バッファの破棄
		if (g_ModelInfo[i].pBuffMat != NULL)
		{
			g_ModelInfo[i].pBuffMat->Release();
			g_ModelInfo[i].pBuffMat = NULL;
		}
	}
}
//============================
// マップエディター更新処理
//============================
void UpdateMapEdit()
{
	// カメラを取得
	Camera* pCamera = GetCamera();

	// モードを取得
	MODE nMode = GetMode();

	// カメラの更新
	UpdateCamera();

	if (nMode == MODE_EDIT)
	{// MODEが編集モードなら

		//	書き出し処理
		if (KeyboardTrigger(DIK_F7))
		{// F7キーを押した
			// 保存する
			SaveEdit();
		}

		if (KeyboardTrigger(DIK_F8))
		{// F8キーを押した
			// ファイルパスを切り替え
			g_CurrentFilepass = (g_CurrentFilepass + 1) % FILEPASS_MAX;
		}

		if (KeyboardTrigger(DIK_F9))
		{// F9キーを押した
			// 再読み込み
			ReloadEdit();
			isLoad = true;
		}

		// 移動処理
		if (GetKeyboardPress(DIK_A))
		{// Aキーを押した

			g_MapEdit[g_Edit].mapedit.move.x -= sinf(pCamera->rot.y + (D3DX_PI * 0.5f)) * g_fspeed;
			g_MapEdit[g_Edit].mapedit.move.z -= cosf(pCamera->rot.y + (D3DX_PI * 0.5f)) * g_fspeed;

			// 角度の正規化
			if (g_MapEdit[g_Edit].mapedit.rot.y > D3DX_PI)
			{// D3DX_PIより大きくなったら
				g_MapEdit[g_Edit].mapedit.rot.y -= D3DX_PI * 2.0f;
			}
		}
		else if (GetKeyboardPress(DIK_D))
		{// Dキーを押した
			// Dキーのみ押した
			g_MapEdit[g_Edit].mapedit.move.x += sinf(pCamera->rot.y + (D3DX_PI * 0.5f)) * g_fspeed;
			g_MapEdit[g_Edit].mapedit.move.z += cosf(pCamera->rot.y + (D3DX_PI * 0.5f)) * g_fspeed;

			// 角度の正規化
			if (g_MapEdit[g_Edit].mapedit.rot.y > D3DX_PI)
			{// D3DX_PIより大きくなったら
				g_MapEdit[g_Edit].mapedit.rot.y -= D3DX_PI * 2.0f;
			}

		}
		else if (GetKeyboardPress(DIK_W))
		{// Wキーを押した

			g_MapEdit[g_Edit].mapedit.move.x += sinf(pCamera->rot.y) * g_fspeed;
			g_MapEdit[g_Edit].mapedit.move.z += cosf(pCamera->rot.y) * g_fspeed;

			// 角度を正規化
			if (g_MapEdit[g_Edit].mapedit.rot.y < -D3DX_PI)
			{// D3DX_PIより小さくなったら
				g_MapEdit[g_Edit].mapedit.rot.y += D3DX_PI * 2.0f;
			}

		}
		else if (GetKeyboardPress(DIK_S))
		{// Sキーを押した

			g_MapEdit[g_Edit].mapedit.move.x -= sinf(pCamera->rot.y) * g_fspeed;
			g_MapEdit[g_Edit].mapedit.move.z -= cosf(pCamera->rot.y) * g_fspeed;

			// 角度の正規化
			if (g_MapEdit[g_Edit].mapedit.rot.y > D3DX_PI)
			{// D3DX_PIより大きくなったら
				g_MapEdit[g_Edit].mapedit.rot.y -= D3DX_PI * 2.0f;
			}
		}

		if (KeyboardTrigger(DIK_UP))
		{// 上キーを押した
			if (g_MapEdit[g_Edit].mapedit.nType < g_nTypeNum - 1)
			{
				// 最大数より小さいとき
				g_MapEdit[g_Edit].mapedit.nType++;
			}
		}
		else if (KeyboardTrigger(DIK_DOWN))
		{// 下キーを押した
			if (g_MapEdit[g_Edit].mapedit.nType > 0)
			{
				// 1より大きいとき
				g_MapEdit[g_Edit].mapedit.nType--;
			}
		}

		if (KeyboardTrigger(DIK_T))
		{// Tキーを押したら
			// 座標を更新
			g_MapEdit[g_Edit].mapedit.pos.y += 10.0f;
		}
		else if (KeyboardTrigger(DIK_G))
		{// Gキーを押したら
			// 座標を更新
			g_MapEdit[g_Edit].mapedit.pos.y -= 10.0f;
		}

		if (KeyboardTrigger(DIK_H))
		{// Hキーが押された
			// 拡大率の変更
			g_MapEdit[g_Edit].mapedit.Scal.x -= 0.1f;

			if (g_MapEdit[g_Edit].mapedit.Scal.x <= 0.1f)
			{// 下限を設定
				g_MapEdit[g_Edit].mapedit.Scal.x = 0.1f;
			}

		}
		else if (KeyboardTrigger(DIK_Y))
		{// Yキーが押された
			// 拡大率の変更
			g_MapEdit[g_Edit].mapedit.Scal.x += 0.1f;

			if (g_MapEdit[g_Edit].mapedit.Scal.x >= 2.0f)
			{// 上限を設定
				g_MapEdit[g_Edit].mapedit.Scal.x = 2.0f;
			}
		}

		if (KeyboardTrigger(DIK_J))
		{// Jキーが押された
			// 拡大率の変更
			g_MapEdit[g_Edit].mapedit.Scal.y -= 0.1f;

			if (g_MapEdit[g_Edit].mapedit.Scal.y <= 0.1f)
			{// 下限を設定
				g_MapEdit[g_Edit].mapedit.Scal.y = 0.1f;
			}

		}
		else if (KeyboardTrigger(DIK_U))
		{// Uキーが押された
			// 拡大率の変更
			g_MapEdit[g_Edit].mapedit.Scal.y += 0.1f;

			if (g_MapEdit[g_Edit].mapedit.Scal.y >= 2.0f)
			{// 上限を設定
				g_MapEdit[g_Edit].mapedit.Scal.y = 2.0f;
			}

		}

		if (KeyboardTrigger(DIK_I))
		{// Iキーが押された
			// 拡大率の変更
			g_MapEdit[g_Edit].mapedit.Scal.z += 0.1f;

			if (g_MapEdit[g_Edit].mapedit.Scal.z >= 2.0f)
			{// 上限を設定
				g_MapEdit[g_Edit].mapedit.Scal.z = 2.0f;
			}

		}
		else if (KeyboardTrigger(DIK_K))
		{// Kキーが押された
			// 拡大率の変更
			g_MapEdit[g_Edit].mapedit.Scal.z -= 0.1f;

			if (g_MapEdit[g_Edit].mapedit.Scal.z <= 0.1f)
			{// 下限を設定
				g_MapEdit[g_Edit].mapedit.Scal.z = 0.1f;
			}

		}

		if (KeyboardTrigger(DIK_RETURN))
		{// ENTERキーが押された
			// 次の敵の位置を配置した敵と同じにする
			g_MapEdit[g_Edit + 1].mapedit.pos = g_MapEdit[g_Edit].mapedit.pos;

			// タイプを同じにする
			g_MapEdit[g_Edit + 1].mapedit.nType = g_MapEdit[g_Edit].mapedit.nType;

			// 使用判定にする
			g_MapEdit[g_Edit + 1].bUse = true;

			// 置いた判定を取る
			g_MapEdit[g_Edit].isputblock = true;

			// インクリメント
			g_Edit++;

			// 置いた判定を初期化
			g_MapEdit[g_Edit].isputblock = false;
		}

		if (g_Edit > 0)
		{// 0以上なら
			if (KeyboardTrigger(DIK_BACKSPACE))
			{// BACKSPACEキーが押された

				// 未使用判定にする
				g_MapEdit[g_Edit].bUse = false;

				// 置いた判定
				g_MapEdit[g_Edit].isputblock = false;

				// デクリメント
				g_Edit--;
			}
		}


		// 移動量の減衰
		g_MapEdit[g_Edit].mapedit.move.x += (0.0f - g_MapEdit[g_Edit].mapedit.move.x) * 0.5f;
		g_MapEdit[g_Edit].mapedit.move.z += (0.0f - g_MapEdit[g_Edit].mapedit.move.z) * 0.5f;

		// 移動量の更新
		g_MapEdit[g_Edit].mapedit.pos += g_MapEdit[g_Edit].mapedit.move;
	}
}
//============================
// マップエディター描画処理
//============================
void DrawMapEdit()
{
	// デバイスポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// 現在のモードを取得
	MODE nMode = GetMode();

	// カメラをセット
	SetCamera();

	// メッシュフィールドの描画
	DrawMeshField();

	// 計算用のマトリックスを宣言
	D3DXMATRIX mtxRot, mtxTrans, mtxScal;

	// 現在のマテリアルを保存
	D3DMATERIAL9 matDef;

	// マテリアルデータへのポインタ
	D3DXMATERIAL* pMat;

	for (int nCnt = 0; nCnt < MAX_EDITOBJ; nCnt++)
	{
		// タイプを保存
		int nType = g_MapEdit[nCnt].mapedit.nType;

		if (((nMode == MODE_PLAY) && !g_MapEdit[nCnt].isputblock) || !g_MapEdit[nCnt].bUse)
		{// プレイモード かつ isputblockが未使用 または bUseが未使用
			continue;
		}

		// ワールドマトリックスの初期化
		D3DXMatrixIdentity(&g_MapEdit[nCnt].mapedit.mtxWorld);

		// 大きさを反映
		D3DXMatrixScaling(&mtxScal, g_MapEdit[nCnt].mapedit.Scal.x, g_MapEdit[nCnt].mapedit.Scal.y, g_MapEdit[nCnt].mapedit.Scal.z);
		D3DXMatrixMultiply(&g_MapEdit[nCnt].mapedit.mtxWorld, &g_MapEdit[nCnt].mapedit.mtxWorld, &mtxScal);

		// 向きを反映
		D3DXMatrixRotationYawPitchRoll(&mtxRot, g_MapEdit[nCnt].mapedit.rot.y, g_MapEdit[nCnt].mapedit.rot.x, g_MapEdit[nCnt].mapedit.rot.z);
		D3DXMatrixMultiply(&g_MapEdit[nCnt].mapedit.mtxWorld, &g_MapEdit[nCnt].mapedit.mtxWorld, &mtxRot);

		// 位置を反映
		D3DXMatrixTranslation(&mtxTrans, g_MapEdit[nCnt].mapedit.pos.x, g_MapEdit[nCnt].mapedit.pos.y, g_MapEdit[nCnt].mapedit.pos.z);
		D3DXMatrixMultiply(&g_MapEdit[nCnt].mapedit.mtxWorld, &g_MapEdit[nCnt].mapedit.mtxWorld, &mtxTrans);

		// ワールドマトリックスの設定
		pDevice->SetTransform(D3DTS_WORLD, &g_MapEdit[nCnt].mapedit.mtxWorld);

		// 現在のマトリックスの取得
		pDevice->GetMaterial(&matDef);

		// マテリアル数だけ回す
		for (int nCntMat = 0; nCntMat < (int)g_ModelInfo[nType].dwNumMat; nCntMat++)
		{
			// マテリアルデータへのポインタを取得
			pMat = (D3DXMATERIAL*)g_ModelInfo[nType].pBuffMat->GetBufferPointer();

			// マテリアル設定
			pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

			// テクスチャ設定
			pDevice->SetTexture(0, g_ModelInfo[nType].pTexture[nCntMat]);

			// ブロック(パーツ)の描画
			g_ModelInfo[nType].pMesh->DrawSubset(nCntMat);
		}

		// マテリアルを戻す
		pDevice->SetMaterial(&matDef);
	}
}
//========================
// 書きだし
//========================
void SaveEdit(void)
{
	// ファイルポインタを宣言
	FILE* pFile;

	// 選択されたファイルパスで書き出し
	pFile = fopen(WRITE_FILENAME[g_CurrentFilepass], "wb");

	if (pFile != NULL)
	{// ファイルが開けたら

		// 書き出し形を宣言
		fwrite(&g_Edit, sizeof(int), 1, pFile);

		// 配置した数だけ周回
		for (int nCnt = 0; nCnt < g_Edit; nCnt++)
		{
			if (g_MapEdit[nCnt].isputblock)
			{// 情報を書き出す

				// 移動量を初期化
				g_MapEdit[nCnt].mapedit.move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

				// 書き出す
				fwrite(&g_MapEdit[nCnt].mapedit, sizeof(MapEdit), 1, pFile);
			}
		}

		// ファイルを閉じる
		fclose(pFile);
	}
	else
	{// ファイルが開けなかった時
		// メッセージBOXの表示
		MessageBox(NULL, "開けません", "エラー", MB_OK);

		return;
	}
}
//========================
// 再読み込み
//========================
void ReloadEdit(void)
{
	// ファイルポインタを宣言
	FILE* pFile;

	// ファイル開く
	pFile = fopen(WRITE_FILENAME[g_CurrentFilepass], "rb");

	// ファイルが開けたら
	if (pFile != NULL)
	{
		// 初期化
		InitEditinfo();

		// 読み込む
		fread(&g_Edit, sizeof(int), 1, pFile);

		for (int nCnt = 0; nCnt < g_Edit; nCnt++)
		{// 配置した数だけ周回
			// 読み込み処理
			fread(&g_MapEdit[nCnt].mapedit, sizeof(MapEdit), 1, pFile);

			// 使用判定にする
			g_MapEdit[nCnt].bUse = true;
			g_MapEdit[nCnt].isputblock = true;
		}

		// 使用判定
		g_MapEdit[g_Edit].bUse = true;
		g_MapEdit[g_Edit].isputblock = false;

		// 判定をfalseに
		isLoad = false;

		// ファイルを閉じる
		fclose(pFile);
	}
	else
	{// 開けなかったら
		return;
	}
}
//===============================
// Xファイルの読み込み処理
//===============================
void LoadXFile()
{
	// デバイスポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// 変数の初期化
	g_nTypeNum = 0; // 種類数
	g_fspeed = 0.0f;// 移動量
	g_jump = 0.0f;	// ジャンプ量

	// ファイルポインタを宣言
	FILE* pFile;

	// 開くファイル名
	pFile = fopen("data\\model.txt", "r");

	if (pFile != NULL)
	{// 開けたら

		// ローカル変数-------------------------------
		char aString[256] = {};	// 文字列
		char aGomi[3] = {};		// イコールを代入する
		int nCnt = 0;
		int nData = 0;
		//---------------------------------------------

		while (1)
		{
			// nDataに代入する
			nData = fscanf(pFile, "%s", &aString[0]);

			if (strcmp(&aString[0],"BLOCKTYPE") == 0)
			{// BLOCKTYPEを読み取った
				// 読み飛ばす
				fscanf(pFile, "%s", &aGomi[0]);

				//種類の数を読み込む
				fscanf(pFile, "%d", &g_nTypeNum);

			}

			else if (strcmp(&aString[0], "MOVE") == 0)
			{// MOVEを読む

				// 読み飛ばす
				fscanf(pFile, "%s", &aGomi[0]);

				// 種類の数を読み込む
				fscanf(pFile, "%f", &g_fspeed);
			}

			else if (strcmp(&aString[0], "JUMP") == 0)
			{// JUMPを読む

				// 読み飛ばす
				fscanf(pFile, "%s", &aGomi[0]);

				// ジャンプ量の数を読み込む
				fscanf(pFile, "%f", &g_jump);
			}

			else if (strcmp(&aString[0],"MODEL_FILENAME") == 0)
			{// MODEL_FILENAMEを読み取ったら

				// =を飛ばす
				fscanf(pFile, "%s", &aGomi[0]);

				// 文字列を読み込む
				fscanf(pFile, "%s", &aString[0]);

				// モデルファイル名格納用の変数
				char* filename[256] = {};	

				// 読み込んだ文字列を保存する
				filename[0] = aString;

				// ファイル名をコピー
				strcpy(&g_ModelInfo[nCnt].FileName[0], filename[0]);

				// Xファイルの読み込み
				D3DXLoadMeshFromX(filename[0],
					D3DXMESH_SYSTEMMEM,
					pDevice,
					NULL,
					&g_ModelInfo[nCnt].pBuffMat,
					NULL,
					&g_ModelInfo[nCnt].dwNumMat,
					&g_ModelInfo[nCnt].pMesh);
				
				// マテリアルデータへのポインタ
				D3DXMATERIAL* pMat;

				// マテリアルデータへのポインタを取得
				pMat = (D3DXMATERIAL*)g_ModelInfo[nCnt].pBuffMat->GetBufferPointer();

				// マテリアル数だけ回す
				for (int nCntMat = 0; nCntMat < (int)g_ModelInfo[nCnt].dwNumMat; nCntMat++)
				{
					if (pMat[nCntMat].pTextureFilename != NULL)
					{
						// テクスチャの読み込み
						D3DXCreateTextureFromFile(pDevice,
							pMat[nCntMat].pTextureFilename,
							&g_ModelInfo[nCnt].pTexture[nCntMat]);
					}
				}

				// ローカル変数宣言-----------------------------
				int nNumVtx;		// 頂点数
				DWORD sizeFVF;		// 頂点フォーマットのサイズ
				BYTE* pVtxBuff;		// 頂点バッファのポインタ
				//----------------------------------------------

				// 頂点数の取得
				nNumVtx = g_ModelInfo[nCnt].pMesh->GetNumVertices();

				// 頂点のサイズを取得
				sizeFVF = D3DXGetFVFVertexSize(g_ModelInfo[nCnt].pMesh->GetFVF());

				// 頂点バッファをロック
				g_ModelInfo[nCnt].pMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pVtxBuff);

				// 頂点数分回す
				for (int nCntEnemy = 0; nCntEnemy < nNumVtx; nCntEnemy++)
				{
					// 頂点座標の代入
					D3DXVECTOR3 Vtx = *(D3DXVECTOR3*)pVtxBuff;

					// 頂点座標の比較
					if (Vtx.x > g_ModelInfo[nCnt].Vtxmax.x)
					{// xが最大値より大きかったら
						// 代入
						g_ModelInfo[nCnt].Vtxmax.x = Vtx.x;
					}
					if (Vtx.y > g_ModelInfo[nCnt].Vtxmax.y)
					{// yが最大値より大きかったら
						// 代入
						g_ModelInfo[nCnt].Vtxmax.y = Vtx.y;
					}
					if (Vtx.z > g_ModelInfo[nCnt].Vtxmax.z)
					{// zが最大値より大きかったら
						// 代入
						g_ModelInfo[nCnt].Vtxmax.z = Vtx.z;
					}

					if (Vtx.x < g_ModelInfo[nCnt].Vtxmin.x)
					{// xが最小値より小さかったら
						// 代入
						g_ModelInfo[nCnt].Vtxmin.x = Vtx.x;
					}
					if (Vtx.y < g_ModelInfo[nCnt].Vtxmin.y)
					{// yが最小値より小さかったら
						// 代入
						g_ModelInfo[nCnt].Vtxmin.y = Vtx.y;
					}
					if (Vtx.z < g_ModelInfo[nCnt].Vtxmin.z)
					{// zが最小値より小さかったら
						// 代入
						g_ModelInfo[nCnt].Vtxmin.z = Vtx.z;
					}

					// 頂点フォーマットのサイズ分進める
					pVtxBuff += sizeFVF;
				}

				// サイズを代入
				g_ModelInfo[nCnt].size.x = g_ModelInfo[nCnt].Vtxmax.x - g_ModelInfo[nCnt].Vtxmin.x;
				g_ModelInfo[nCnt].size.y = g_ModelInfo[nCnt].Vtxmax.y - g_ModelInfo[nCnt].Vtxmin.y;
				g_ModelInfo[nCnt].size.z = g_ModelInfo[nCnt].Vtxmax.z - g_ModelInfo[nCnt].Vtxmin.z;

				// アンロック
				g_ModelInfo[nCnt].pMesh->UnlockVertexBuffer();

				// nCntをインクリメント
				nCnt++;

			}

			if (nData == EOF)
			{// EOFを読み取る
				break;
			}
		}
	}
	else
	{// 開けなかったら
		return;
	}

	// ファイルを閉じる
	fclose(pFile);

}
//===============================
// エディターモデル配置時の情報
//===============================
MAPMODELINFO* MapInfo()
{
	return &g_MapEdit[0];
}
//===============================
// エディターモデル情報の取得
//===============================
EDITMODEL* GetBlockInfo(int type)
{
	return &g_ModelInfo[type];
}
//===============================
// エディター情報の取得
//===============================
MapEdit* getMap()
{
	return &g_MapEdit[0].mapedit;
}
//================================
// 配置数情報の取得
//================================
int ReturnEdit()
{
	return g_Edit;
}
//================================
// 配置時移動量の取得
//================================
float ReturnSpeed()
{
	return g_fspeed;
}
//================================
// 設定した種類数の取得
//================================
int ReturnType()
{
	return g_nTypeNum;
}
//================================
// 設定したジャンプ量の取得
//================================
float ReturnJump()
{
	return g_jump;
}
//================================
// ファイルパスナンバーを取得
//================================
int Filepass()
{
	return g_CurrentFilepass;
}
//================================
// 判定を取得
//================================
bool IsGetLoad()
{
	return isLoad;
}
//================================
// 構造体初期化処理
//================================
void InitEditinfo()
{
	// 構造体変数の初期化
	for (int nCnt = 0; nCnt < MAX_EDITOBJ; nCnt++)
	{
		g_MapEdit[nCnt].mapedit.pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);  // 座標
		g_MapEdit[nCnt].mapedit.move = D3DXVECTOR3(0.0f, 0.0f, 0.0f); // 移動量
		g_MapEdit[nCnt].bUse = false;						          // 未使用状態
		g_MapEdit[nCnt].mapedit.rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);  // 角度
		g_MapEdit[nCnt].mapedit.nType = 0;							  // 種類
		g_MapEdit[nCnt].isputblock = false;					          // 配置されたか
		g_MapEdit[nCnt].mapedit.Scal = D3DXVECTOR3(1.0f, 1.0f, 1.0f); // 拡大率
	}
}