//==========================
//
// カメラ処理 [camera.h]
// Author: Asuma Nishio
//
//==========================
#ifndef _CAMERA_H_
#define _CAMERA_H_

//****************************
// インクルードファイル宣言
//****************************
#include "main.h"

//****************************
// カメラ列挙型を定義
//****************************
typedef enum
{
	CAMERAMODE_NONE = 0,
	CAMERAMODE_MAX
}CAMERAMODE;

//****************************
// カメラ構造体を定義
//****************************
typedef struct
{
	D3DXVECTOR3 posV;			// 視点
	D3DXVECTOR3 posR;			// 注視点
	D3DXVECTOR3 vecU;			// 上方向ベクトル
	D3DXMATRIX mtxProjection;	// プロジェクションマトリックス
	D3DXMATRIX mtxView;			// ビューマトリックス
	D3DXVECTOR3 rot;			// 角度
	float fDistance;			// 視点から注視点までの距離
	D3DXVECTOR3 posVDest;		// 目的の視点
	D3DXVECTOR3 posRDest;		// 目的の注視点
	CAMERAMODE g_CameraMode;
	D3DVIEWPORT9 viewport;		// ビューポート
}Camera;

//**********************
// プロトタイプ宣言
//**********************
void InitCamera(void);		// カメラの初期化
void UninitCamera(void);	// カメラの終了
void UpdateCamera(void);	// カメラの更新
void SetCamera(void);		// カメラの設定
Camera* GetCamera();		// カメラの取得
void StickCamera(void);		// 右スティックのカメラ処理
void MouseWheel(int zdelta);

#endif