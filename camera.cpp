//==========================
//
// カメラ処理 [camera.cpp]
// Author: Asuma Nishio
//
//==========================

//****************************
// インクルードファイル宣言
//****************************
#include "camera.h"
#include "input.h"
#include "player.h"
#include "mouse.h"

//*****************************
// マクロ定義
//*****************************
#define MAX_VIEWUP (3.00f) // カメラの制限
#define MAX_VIEWDOWN (0.1f) // カメラの制限

//*****************************
// プロトタイプ宣言
//*****************************
void MouseView(void);       // ゲームの時のマウスの視点移動
void MouseEditMode(void); // 編集モードの時のマウス移動

//*****************************
// グローバル変数宣言
//*****************************
Camera g_camera;			// カメラ情報
D3DXVECTOR3 zoom;			// カメラズーム
bool isrot;					// 自動旋回か否か
bool isPlayer;

//=========================
// カメラの初期化処理
//=========================
void InitCamera(void)
{
	g_camera.posV = D3DXVECTOR3(0.0f, 350.0f, -1600.0f);		 // カメラの位置
	g_camera.posR = D3DXVECTOR3(0.0f, 0.0f, 0.0f);			 // カメラの見ている位置
	g_camera.vecU = D3DXVECTOR3(0.0f, 1.0f, 0.0f);			 // 上方向ベクトル
	g_camera.rot = D3DXVECTOR3(D3DX_PI * 0.65f, 0.0f, 0.0f); // 角度
	g_camera.g_CameraMode = CAMERAMODE_NONE;				 // 初期状態

	// 座標の差分を計算
	float fRotx = g_camera.posV.x - g_camera.posR.x;
	float fRoty = g_camera.posV.y - g_camera.posR.y;
	float fRotz = g_camera.posV.z - g_camera.posR.z;

	// 視点から注視点までの距離
	g_camera.fDistance = sqrtf((fRotx * fRotx) + (fRoty * fRoty) + (fRotz * fRotz));	
	
	// グローバル変数の初期化
	zoom = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	isrot = false;
	isPlayer = false;
}
//=========================
// カメラの終了処理
//=========================
void UninitCamera(void)
{
	// 無し
}
//=========================
// カメラの更新処理
//=========================
void UpdateCamera(void)
{
	// プレイヤーを取得
	PLAYER* pPlayer = GetPlayer();

	// 現在のモードを取得
	MODE mode = GetMode();

	//編集モード中のカメラ移動
	MouseEditMode();	
#if 0
	if (GetKeyboardPress(DIK_LEFT))
	{
		// 回転
		g_camera.rot.y += 0.009f;

		// カメラの視点の情報
		g_camera.posV.x = g_camera.posR.x - sinf(g_camera.rot.x) * sinf(g_camera.rot.y) * g_camera.fDistance;
		g_camera.posV.y = g_camera.posR.y - cosf(g_camera.rot.x) * g_camera.fDistance;
		g_camera.posV.z = g_camera.posR.z - sinf(g_camera.rot.x) * cosf(g_camera.rot.y) * g_camera.fDistance;

		// 正規化
		if (g_camera.rot.y > D3DX_PI)
		{// D3DX_PIより大きくなったら
			g_camera.rot.y -= D3DX_PI * 2.0f;
		}

		// 角度の正規化
		if (g_camera.rot.y < -D3DX_PI)
		{// D3DX_PIより小さくなったら
			g_camera.rot.y += D3DX_PI * 2.0f;
		}
	}
	else if (GetKeyboardPress(DIK_RIGHT))
	{
		// 回転
		g_camera.rot.y -= 0.009f;

		// カメラの視点の情報
		g_camera.posV.x = g_camera.posR.x - sinf(g_camera.rot.x) * sinf(g_camera.rot.y) * g_camera.fDistance;
		g_camera.posV.y = g_camera.posR.y - cosf(g_camera.rot.x) * g_camera.fDistance;
		g_camera.posV.z = g_camera.posR.z - sinf(g_camera.rot.x) * cosf(g_camera.rot.y) * g_camera.fDistance;

		// 正規化
		if (g_camera.rot.y > D3DX_PI)
		{// D3DX_PIより大きくなったら
			g_camera.rot.y -= D3DX_PI * 2.0f;
		}

		// 角度の正規化
		if (g_camera.rot.y < -D3DX_PI)
		{// D3DX_PIより小さくなったら
			g_camera.rot.y += D3DX_PI * 2.0f;
		}
	}

	if (GetKeyboardPress(DIK_UP))
	{
		// 回転
		g_camera.rot.x += 0.009f;

		// 角度の正規化
		if (g_camera.rot.x < -D3DX_PI)
		{// D3DX_PIより小さくなったら
			g_camera.rot.x += D3DX_PI * 2.0f;
		}
		if (g_camera.rot.x > D3DX_PI)
		{// D3DX_PIより大きくなったら
			g_camera.rot.x -= D3DX_PI * 2.0f;
		}

		// カメラの視点の情報
		g_camera.posV.x = g_camera.posR.x - sinf(g_camera.rot.x) * sinf(g_camera.rot.y) * g_camera.fDistance;
		g_camera.posV.y = g_camera.posR.y - cosf(g_camera.rot.x) * g_camera.fDistance;
		g_camera.posV.z = g_camera.posR.z - sinf(g_camera.rot.x) * cosf(g_camera.rot.y) * g_camera.fDistance;

	}
	else if (GetKeyboardPress(DIK_DOWN))
	{
		// 回転
		g_camera.rot.x -= 0.009f;

		// 角度の正規化
		if (g_camera.rot.x < -D3DX_PI)
		{// D3DX_PIより小さくなったら
			g_camera.rot.x += D3DX_PI * 2.0f;
		}
		if (g_camera.rot.x > D3DX_PI)
		{// D3DX_PIより大きくなったら
			g_camera.rot.x -= D3DX_PI * 2.0f;
		}

		// カメラの視点の情報
		g_camera.posV.x = g_camera.posR.x - sinf(g_camera.rot.x) * sinf(g_camera.rot.y) * g_camera.fDistance;
		g_camera.posV.y = g_camera.posR.y - cosf(g_camera.rot.x) * g_camera.fDistance;
		g_camera.posV.z = g_camera.posR.z - sinf(g_camera.rot.x) * cosf(g_camera.rot.y) * g_camera.fDistance;
	}
#endif
	if (KeyboardTrigger(DIK_F10))
	{// F10キー
		// 旋回起動
		isrot = true;
	}
	else if (KeyboardTrigger(DIK_F11))
	{// F11キー
		// 旋回終了
		isrot = false;
	}

	if (KeyboardTrigger(DIK_N))
	{
		// 追従
		isPlayer = true;
	}
	else if (KeyboardTrigger(DIK_M))
	{
		// 追従
		isPlayer = false;
	}

	if (OnMouseTriggerDown(2) || KeyboardTrigger(DIK_TAB))
	{
		InitCamera();
	}

#if 1
	if (mode == MODE_PLAY)
	{// 自動旋回

		if (isrot)
		{
			g_camera.posV.z = -800.0f;
			g_camera.posV.y = 800.0f;

			g_camera.rot.y += 0.005f; // カメラの視点の情報

			g_camera.posV.x = g_camera.posR.x - sinf(g_camera.rot.x) * sinf(g_camera.rot.y) * 1800.0f;
			g_camera.posV.y = g_camera.posR.y - cosf(g_camera.rot.x) * 1800.0f;
			g_camera.posV.z = g_camera.posR.z - sinf(g_camera.rot.x) * cosf(g_camera.rot.y) * 1800.0f;
		}

	}

	// プレイ時のカメラの更新
	if (mode != MODE_EDIT && isPlayer)
	{
		// マウス視点移動
		MouseView();

		//****************************************
		//      プレイヤー追従
		//****************************************
		g_camera.posRDest.x = pPlayer->pos.x + sinf(pPlayer->rotDestPlayer.y) * 1.0f;
		g_camera.posRDest.y = pPlayer->pos.y + cosf(pPlayer->rotDestPlayer.y) * 1.0f;
		g_camera.posRDest.z = pPlayer->pos.z + cosf(pPlayer->rotDestPlayer.y) * 1.0f;

		g_camera.posVDest.x = pPlayer->pos.x - sinf(g_camera.rot.y) * g_camera.fDistance;
		g_camera.posVDest.y = pPlayer->pos.y - cosf(g_camera.rot.y) * g_camera.fDistance;
		g_camera.posVDest.z = pPlayer->pos.z - cosf(g_camera.rot.y) * g_camera.fDistance;

		g_camera.posR.x += ((g_camera.posRDest.x - g_camera.posR.x) * 0.3f);
		g_camera.posR.y += ((g_camera.posRDest.y - g_camera.posR.y) * 0.3f);
		g_camera.posR.z += ((g_camera.posRDest.z - g_camera.posR.z) * 0.3f);

		g_camera.posV.x += ((g_camera.posVDest.x - g_camera.posV.x) * 0.3f);
		g_camera.posV.z += ((g_camera.posVDest.z - g_camera.posV.z) * 0.3f);

	}

#endif

	if (g_camera.rot.x <= D3DX_PI * 0.35f)
	{// カメラの下限

		g_camera.rot.x = D3DX_PI * 0.35f;
	}
	else if (g_camera.rot.x >= D3DX_PI * 0.9f)
	{// カメラの上限

		g_camera.rot.x = D3DX_PI * 0.9f;
	}
}
//=========================
// カメラの設定処理
//=========================
void SetCamera(void)
{
	// デバイスポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// ビューマトリックスの初期化
	D3DXMatrixIdentity(&g_camera.mtxView);

	// ビューマトリックスの作成
	D3DXMatrixLookAtLH(&g_camera.mtxView,
		&g_camera.posV,
		&g_camera.posR,
		&g_camera.vecU);

	// ビューマトリックスの設定
	pDevice->SetTransform(D3DTS_VIEW, &g_camera.mtxView);

	// プロジェクションマトリックスの初期化
	D3DXMatrixIdentity(&g_camera.mtxProjection);

	// プロジェクションマトリックスの作成
	D3DXMatrixPerspectiveFovLH(&g_camera.mtxProjection,
		D3DXToRadian(45.0f),
		(float)SCREEN_WIDTH / (float)SCREEN_HEIGHT,
		10.0f,
		3000.0f);

	// プロジェクションマトリックスの設定
	pDevice->SetTransform(D3DTS_PROJECTION, &g_camera.mtxProjection);
	
}
//=========================
// カメラの取得処理
//=========================
Camera* GetCamera()
{
	return &g_camera;
}
//=================================
// カメラのための右スティック処理
//=================================
void StickCamera(void)
{
	// スティック
	XINPUT_STATE* pStick = GetJoyStickAngle();
#if 1
	if (GetJoyStick())
	{
		float RStickAngleY = pStick->Gamepad.sThumbRY;
		float RStickAngleX = pStick->Gamepad.sThumbRX;

		float DeadZone = 10920.0f;
		float fMag = sqrtf((RStickAngleX * RStickAngleX) + (RStickAngleY * RStickAngleY));

		if (fMag > DeadZone)
		{
			if (pStick->Gamepad.sThumbRX < -10920.0f)
			{
				// 角度
				g_camera.rot.y -= 0.03f;

				g_camera.posV.x = g_camera.posR.x - sinf(g_camera.rot.y) * sinf(g_camera.rot.y) * g_camera.fDistance;
				g_camera.posV.z = g_camera.posR.z - sinf(g_camera.rot.y) * cosf(g_camera.rot.y) * g_camera.fDistance;

			}
			else if (pStick->Gamepad.sThumbRX > 10920.0f)
			{
				// 角度
				g_camera.rot.y += 0.03f;

				g_camera.posV.x = g_camera.posR.x - sinf(g_camera.rot.y) * sinf(g_camera.rot.y) * g_camera.fDistance;
				g_camera.posV.z = g_camera.posR.z - sinf(g_camera.rot.y) * cosf(g_camera.rot.y) * g_camera.fDistance;

			}


			// 角度の正規化
			if (g_camera.rot.y > D3DX_PI)
			{// D3DX_PIより大きくなったら
				g_camera.rot.y -= D3DX_PI * 2.0f;
			}

			// 角度の正規化
			if (g_camera.rot.y < -D3DX_PI)
			{// D3DX_PIより小さくなったら
				g_camera.rot.y += D3DX_PI * 2.0f;
			}
		}
	}
#endif
}
//=================================
// マウスホイール処理
//=================================
void MouseWheel(int zdelta)
{
	// ローカル変数
	zoom = g_camera.posV - g_camera.posR;
	D3DXVec3Normalize(&zoom, &zoom);

	if (zdelta > 0)
	{// マウスの値が正 (奥に動かした)
		g_camera.fDistance += 10.0f;
	}
	else if (zdelta < 0)
	{// マウスの値が負 (手前に動かした)
		g_camera.fDistance -= 10.0f;
	}

	if (g_camera.fDistance <= 300.0f)
	{// 300.0f以下なら
		g_camera.fDistance = 300.0f;
	}

	// カメラの視点の情報
	g_camera.posV.x = g_camera.posR.x - sinf(g_camera.rot.x) * sinf(g_camera.rot.y) * g_camera.fDistance;
	g_camera.posV.y = g_camera.posR.y - cosf(g_camera.rot.x) * g_camera.fDistance;
	g_camera.posV.z = g_camera.posR.z - sinf(g_camera.rot.x) * cosf(g_camera.rot.y) * g_camera.fDistance;

}
//=========================
// マウスの視点
//========================
void MouseView(void)
{
	MODE mode = GetMode();

	DIMOUSESTATE mouseState;

	if (mode != MODE_PLAY && GetMouseState(&mouseState))
	{
		static POINT prevCursorPos = { (long)SCREEN_WIDTH / (long)1.5f,(long)SCREEN_HEIGHT / (long)1.5f };

		POINT cursorPos;
		GetCursorPos(&cursorPos);

		float X = (float)cursorPos.x - prevCursorPos.x;
		float Y = (float)cursorPos.y - prevCursorPos.y;

		const float mouseSensitivity = 0.00025f;

		X *= mouseSensitivity;
		Y *= mouseSensitivity;

		g_camera.rot.y += X;
		g_camera.rot.x += Y;

		if (g_camera.rot.y < -D3DX_PI)
		{
			g_camera.rot.y += D3DX_PI * 2.0f;
		}
		else if (g_camera.rot.y > D3DX_PI)
		{
			g_camera.rot.y += -D3DX_PI * 2.0f;
		}

		if (g_camera.rot.x < -D3DX_PI)
		{
			g_camera.rot.x += D3DX_PI * 2.0f;
		}
		else if (g_camera.rot.x > D3DX_PI)
		{
			g_camera.rot.x += -D3DX_PI * 2.0f;
		}

		if (g_camera.rot.x > MAX_VIEWUP)
		{
			g_camera.rot.x -= Y;
		}
		else if (g_camera.rot.x < MAX_VIEWDOWN)
		{
			g_camera.rot.x -= Y;
		}

		SetCursorPos((long)SCREEN_WIDTH / (long)1.5f, (long)SCREEN_HEIGHT / (long)1.5f);

		prevCursorPos.x = (long)SCREEN_WIDTH / (long)1.5f;
		prevCursorPos.y = (long)SCREEN_HEIGHT / (long)1.5f;

		g_camera.posR.x = g_camera.posV.x + sinf(g_camera.rot.x) * sinf(g_camera.rot.y) * g_camera.fDistance;
		g_camera.posR.y = g_camera.posV.y + cosf(g_camera.rot.x) * g_camera.fDistance;
		g_camera.posR.z = g_camera.posV.z + sinf(g_camera.rot.x) * cosf(g_camera.rot.y) * g_camera.fDistance;

	}

}
//============================
// 編集モードの時のマウス移動
//============================
void MouseEditMode(void)
{
	if (OnMousePress(RIGHT_MOUSE) && GetKeyboardPress(DIK_LSHIFT))
	{
		D3DXVECTOR2 Move = GetMouseVelocity();
		D3DXVECTOR2 MoveOld = GetMouseOldVelocity();

		D3DXVECTOR2 fAngle = Move - MoveOld;

		//回転量を更新
		g_camera.rot.y += fAngle.x * 0.01f;
		g_camera.rot.x += fAngle.y * 0.01f;

		//回転量を制限
		if (g_camera.rot.x > MAX_VIEWUP)
		{
			g_camera.rot.x -= fAngle.y * 0.01f;
		}
		else if (g_camera.rot.x < MAX_VIEWDOWN)
		{
			g_camera.rot.x -= fAngle.y * 0.01f;
		}

		//カメラ座標を更新
		g_camera.posR.x = g_camera.posV.x + sinf(g_camera.rot.x) * sinf(g_camera.rot.y) * g_camera.fDistance;
		g_camera.posR.y = g_camera.posV.y + cosf(g_camera.rot.x) * g_camera.fDistance;
		g_camera.posR.z = g_camera.posV.z + sinf(g_camera.rot.x) * cosf(g_camera.rot.y) * g_camera.fDistance;
	}
	else if (OnMousePress(LEFT_MOUSE) && GetKeyboardPress(DIK_LSHIFT))
	{
		D3DXVECTOR2 Move = GetMouseVelocity();
		D3DXVECTOR2 MoveOld = GetMouseOldVelocity();

		D3DXVECTOR2 fAngle = Move - MoveOld;

		//回転量を更新
		g_camera.rot.y += fAngle.x * 0.01f;
		g_camera.rot.x += fAngle.y * 0.01f;

		//回転量を制限
		if (g_camera.rot.x > MAX_VIEWUP)
		{
			g_camera.rot.x -= fAngle.y * 0.01f;
		}
		else if (g_camera.rot.x < MAX_VIEWDOWN)
		{
			g_camera.rot.x -= fAngle.y * 0.01f;
		}

		// カメラの視点の情報
		g_camera.posV.x = g_camera.posR.x - sinf(g_camera.rot.x) * sinf(g_camera.rot.y) * g_camera.fDistance;
		g_camera.posV.y = g_camera.posR.y - cosf(g_camera.rot.x) * g_camera.fDistance;
		g_camera.posV.z = g_camera.posR.z - sinf(g_camera.rot.x) * cosf(g_camera.rot.y) * g_camera.fDistance;
	}

	//正規化
	if (g_camera.rot.y < -D3DX_PI)
	{
		g_camera.rot.y += D3DX_PI * 2.0f;
	}
	else if (g_camera.rot.y > D3DX_PI)
	{
		g_camera.rot.y += -D3DX_PI * 2.0f;
	}
	if (g_camera.rot.x < -D3DX_PI)
	{
		g_camera.rot.x += D3DX_PI * 2.0f;
	}
	else if (g_camera.rot.x > D3DX_PI)
	{
		g_camera.rot.x += -D3DX_PI * 2.0f;
	}

}

