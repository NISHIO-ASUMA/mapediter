//=================================================
//
// ImGuiの情報設定処理関数 [ imgui_info.h ]
// Author : Asuma Nishio
//
//==================================================
#ifndef _IMGUI_INFO_H_
#define _IMGUI_INFO_H_

//*******************************************
// インクルードファイル宣言
//*******************************************
#include "main.h"

//*******************************************
// IMGUIの種類
//*******************************************
typedef enum
{
	IMGUITYPE_NONE = 0, // 何もしない
	IMGUITYPE_NOMOVE,	// 移動させない
	IMGUITYPE_NOMOVESIZE, // サイズを拡大縮小させない
	IMGUITYPE_NOMOVEANDSIZE, // サイズ,移動ともにさせない
	IMGUITYPE_MAX
}IMGUITYPE;

//*******************************************
// グローバル変数宣言
//*******************************************
void InitImguiInfo(HWND hWnd,LPDIRECT3DDEVICE9 pDevice); // 初期化処理
void UninitImguiInfo();	// 終了処理
void NewFrameImGui(); // 開始フレーム
void StartImgui(const char* ImguiName, IMGUITYPE nType);    // 開始
void EndImgui();      // Guiブロック終了フレーム
void EndImguiFrame(); // 終了フレーム
void SetPosImgui(float PosX, float PosY); // 座標設定
void SetSizeImgui(float SizeX, float SizeY); // 大きさ
void DrawImguiInfo();	// 描画処理

void InitMallocData(); // マロック関数
void UninitMallocData(); // 終了関数 
void UpdatemallocData(int nModel); // 更新関数


#endif // !_IMGUI_INFO_H_
