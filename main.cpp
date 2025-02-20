//====================================
//
// マップエディター試作版 [main.cpp]
// Author: Asuma Nishio
//
//====================================

//****************************
// インクルードファイル宣言
//****************************
#include "main.h"
#include <time.h>
#include <stdlib.h>
#include <crtdbg.h>
#include <stdio.h>
#include "camera.h"
#include "light.h"
#include "input.h"
#include "shadow.h"
#include "mapedit.h"
#include "meshfield.h"
#include "player.h"
#include "block.h"
#include "mouse.h"

//*********************************
// imguiインクルードファイル宣言
//*********************************
#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"

//*****************************
// グローバル変数宣言
//*****************************
LPDIRECT3D9 g_pD3D = NULL;				// Direct3Dオブジェクトへのポインタ
LPDIRECT3DDEVICE9 g_pD3DDevice = NULL;	// Direct3Dデバイスへのポインタ
MODE g_mode = MODE_EDIT;				// 現在のモード
bool g_isFullscreen = false;			// ウィンドウを切り替えるためのフラグ
RECT g_windowRect;						// ウィンドウを切り替えるための変数
LPD3DXFONT g_pFont = NULL;				// フォントへのポインタ
int g_nCountFPS = 0;					// FPSカウンタ
int nGetNumber = 0;						// 番号
int Filenamepass;						// ファイル名
int nBlockSet = 0;						// 配置するブロック数
LPDIRECT3DTEXTURE9 g_pModelIconTexture = NULL; // モデルアイコンのテクスチャ

//*****************************
// imguiグローバル変数宣言
//*****************************
static bool g_DeviceLost = false;
static UINT g_ResizeWidth = 0, g_ResizeHeight = 0;
static D3DPRESENT_PARAMETERS  g_d3dpp = {};

//******************************
// プロトタイプ宣言
//******************************
void ToggleFullscreen(HWND hWnd);	// ウィンドウをフルスクリーンにする方法
void DrawEditkey(void);				// エディター画面の操作フォント用
void DrawModeChange();				// モード切り替え
void DrawEditMove();				// 移動量
void DrawPlayerPos();				// プレイヤー座標
void DebugEditModelInfo();			// 配置モデル情報
void DrawCameraPos(void);			// カメラの座標表示
void onWireFrame();					// ワイヤーフレーム起動
void offWireFrame();				// ワイヤーフレーム終了
void DrawNumBlock();				// ブロックのデバッグ表示
void UpdateModelPosition(int modelIndex, D3DXVECTOR3 newPos); // 位置更新関数
void UpdateModelScale(int modelIndex, D3DXVECTOR3 newScale);  // 拡大率更新関数
void UpdateModelMatrix(int modelIndex);		// ワールドマトリックス
void LoadModelIcon(LPDIRECT3DDEVICE9 pDevice, const char* filePath);

//******************************
// imguiのプロトタイプ宣言
//******************************
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void ResetDevice();
void ImguiDrawData();
void SaveToFile(const char* text); // テキスト

//===============================
// メイン関数
//===============================
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_  HINSTANCE hInstancePrev, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	// メモリリーク検知用のフラグ
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF);		

	DWORD dwCurrentTime;			// 現在時刻
	DWORD dwExecLastTime;			// 終了時刻

	WNDCLASSEX wcex =
	{
		sizeof(WNDCLASSEX),				 // WNDCLASSEXのメモリサイズ
		CS_CLASSDC,						 // ウインドウのスタイル
		WindowProc,						 // ウインドウプロシージャ
		0,								 // 0にする
		0,								 // 0にする
		hInstance,						 // インスタンスハンドル
		LoadIcon(NULL,IDI_APPLICATION),  // タスクバーアイコン
		LoadCursor(NULL,IDC_ARROW),      // マウスカーソル
		(HBRUSH)(COLOR_WINDOW + 1),      // クライアント領域の背景色
		NULL,                            // メニューバー
		CLASS_NAME,                      // ウインドウクラスの名前
		LoadIcon(NULL,IDI_APPLICATION),  // ファイルアイコン
	};


	HWND hWnd = {};  // ウインドウハンドル(識別子)
	MSG msg;    //メッセージを格納する変数

	// 画面サイズの構造
	RECT rect = { 0,0,SCREEN_WIDTH,SCREEN_HEIGHT };

	// フルスクリーンモード
	BOOL bWindow = FALSE;

	// ウインドウクラスの登録
	RegisterClassEx(&wcex);

	// クライアント領域
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

	// ウインドウを生成
	hWnd = CreateWindowEx(0,		// 拡張ウインドウスタイル
		CLASS_NAME,					// ウインドウクラスの名前
		WINDOW_NAME,				// ウインドウの名前
		WS_OVERLAPPEDWINDOW,		// ウインドウスタイル
		CW_USEDEFAULT,				// ウインドウの左上のX座標
		CW_USEDEFAULT,				// ウインドウの左上のY座標
		(rect.right - rect.left),   // ウインドウ幅
		(rect.bottom - rect.top),   // ウインドウの高さ
		NULL,						// NULL
		NULL,						// NULL
		hInstance,					// インスタンスハンドル
		NULL);						// ウインドウ作成データ


	// 初期化処理
	if (FAILED(Init(hInstance, hWnd, TRUE)))
	{
		// 初期化処理が失敗したとき
		return -1;
	}

	// 分解能をせってい
	timeBeginPeriod(1);
	dwCurrentTime = 0;					// 初期化
	dwExecLastTime = timeGetTime();		// 現在時刻を保存

	// ウインドウの表示
	ShowWindow(hWnd, nCmdShow); // ウインドウの表示状態の設定
	UpdateWindow(hWnd);				    // クライアント領域の更新

	// 初期化
	DWORD dwFrameCount;					// フレームカウント
	DWORD dwFPSLastTime;				// 最後にFPSを計測した時刻

	dwFrameCount = 0;					// フレームカウントを初期化
	dwFPSLastTime = timeGetTime();		// 現在の時刻を取得

	// メッセージループ
	while (1)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) != 0)
		{
			// Windowsの処理
			if (msg.message == WM_QUIT)
			{
				// WM_QUITメッセージを受け取ったらループ抜ける
				break;
			}
			else
			{
				// メッセージ設定
				TranslateMessage(&msg);	// 仮想キーメッセージを文字メッセージへ変換

				DispatchMessage(&msg);	// ウインドウプロシージャへメッセージを送出
			}
		}

		if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
			if (!GetMessage(&msg, NULL, 0, 0)) 
			{
				break;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
			
		}
		else
		{// DirectXの処理
			dwCurrentTime = timeGetTime();		// 現在時刻の取得
			if ((dwCurrentTime - dwFPSLastTime) >= 500)
			{
				// 0.5秒経過
				// FPSを計測
				g_nCountFPS = (dwFrameCount * 1000) / (dwCurrentTime - dwFPSLastTime);

				dwFPSLastTime = dwCurrentTime;	// 保存

				dwFrameCount = 0;				// 0に戻す

			}

			if ((dwCurrentTime - dwExecLastTime) >= (1000 / 60))
			{
				// 60/1経過
				dwExecLastTime = dwCurrentTime;	// 処理開始の時刻

				dwFrameCount++;					// フレームカウントを加算

				// 更新処理
				Update();

				// 描画
				ImguiDrawData();

				// ImGuiフレームの終了
				ImGui::EndFrame();

				// 描画処理
				Draw();

			}
		}

	}

	// 終了処理
	Uninit();

	// 分解能を戻す
	timeEndPeriod(1);

	// ウインドウクラスの登録を解除
	UnregisterClass(CLASS_NAME, wcex.hInstance);

	// メッセージを返す
	return(int)msg.wParam;
}

// IMGUIのハンドラー宣言
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

//=======================================
// ウインドウプロシージャ
//=======================================
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	const RECT rect = { 0,0,SCREEN_WIDTH,SCREEN_HEIGHT }; // ウインドウの領域
	int nID;

	// ImGui のイベント処理
	if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
	{
		return true;
	}
	switch (uMsg)
	{
	case WM_ACTIVATE:	// アクティブ時：1　非アクティブ時：0
		return 0L;

	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;

	case WM_SIZE:
		if (wParam == SIZE_MINIMIZED)
		{
			return 0;
		}

		g_ResizeWidth = (UINT)LOWORD(lParam); // 横幅
		g_ResizeHeight = (UINT)HIWORD(lParam);// 高さ
		break;

	case WM_DESTROY: // ウインドウ破棄メッセージ
		// WM_QUITメッセージを送る
		PostQuitMessage(0); // ウインドウの破棄
		break;

	case WM_KEYDOWN: // キー押下のメッセージ
		switch (wParam)
		{
		case VK_ESCAPE:	// [ESC]キーが押された

			nID = MessageBox(hWnd, "終了しますか?", "終了メッセージ", MB_YESNO);
			if (nID == IDYES)
			{
				DestroyWindow(hWnd); // ウインドウ破棄メッセージ
			}
			else
			{
				return 0;			 // 返す
			}

			break;
		}
		break;

	case WM_MOUSEWHEEL: // マウスのホイール判定

		// ローカル変数
		int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);

		// ホイール情報
		MouseWheel(zDelta);
		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam); // 既定の処理を繰り返す
}
//=========================
// 初期化処理
//=========================
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	D3DDISPLAYMODE d3ddm;			// ディスプレイモード
	D3DPRESENT_PARAMETERS d3dpp;	// プレゼンテーションパラメータ

	// Direct3Dオブジェクトの生成
	g_pD3D = Direct3DCreate9(D3D_SDK_VERSION);

	if (g_pD3D == NULL)
	{
		return E_FAIL;
	}
	// 現在のディスプレイモードを取得
	if (FAILED(g_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm)))
	{
		return E_FAIL;
	}

	// パラメーターの0クリア
	ZeroMemory(&d3dpp, sizeof(d3dpp));

	d3dpp.BackBufferWidth = SCREEN_WIDTH;						// ゲーム画面サイズ(幅)
	d3dpp.BackBufferHeight = SCREEN_HEIGHT;						// ゲーム画面サイズ(高さ)
	d3dpp.BackBufferFormat = d3ddm.Format;						// バックバッファの形式
	d3dpp.BackBufferCount = 1;									// バックバッファの数
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;					// ダブルバッファの切り替え(映像信号に同期)
	d3dpp.EnableAutoDepthStencil = TRUE;						// デプスバッファとステンシルバッファを作成
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;					// デプスバッファとして16bitを使用
	d3dpp.Windowed = bWindow;									// ウインドウモード
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;	// リフレッシュシレート
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;	// インターバル

	// Direct3Dデバイスの生成(描画処理と頂点処理をハードウェアで行う)
	if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&d3dpp,
		&g_pD3DDevice)))
	{
		// Direct3Dデバイスの生成(描画処理はハードウェア,頂点処理をCPUで行う)
		if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			hWnd,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING,
			&d3dpp,
			&g_pD3DDevice)))
		{
			// Direct3Dデバイスの生成(描画処理,頂点処理をCPUで行う)
			if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
				D3DDEVTYPE_REF,
				hWnd,
				D3DCREATE_SOFTWARE_VERTEXPROCESSING,
				&d3dpp,
				&g_pD3DDevice)))
			{
				return E_FAIL;
			}
		}
	}

	// レンダーステートの設定
	g_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	g_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	g_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	g_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	// サンプラーステートの設定
	g_pD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	g_pD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	g_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	g_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

	// テクスチャステージチャートの設定
	g_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	g_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	g_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);

	//==============================
	// 初期化処理
	//==============================
	if (FAILED(InitKeyboard(hInstance, hWnd)))
	{// キーボード
		return E_FAIL;
	}
	if (FAILED(InitJoypad()))
	{// パッド
		return E_FAIL;
	}

	// デバッグ表示用のフォントを設定
	D3DXCreateFont(g_pD3DDevice, 18, 0, 0, 0,
		FALSE,
		DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH,
		"Terminal", &g_pFont);

	// マウス初期化
	InitMouse(hInstance,hWnd);

	// ライト
	InitLight();

	// カメラ
	InitCamera();

	// 影
	InitShadow();

	// メッシュの描画
	InitMeshField();

	// エディター
	InitMapEdit();

	// ブロック
	InitBlock();

	// プレイヤー
	InitPlayer();

	// ====== ImGui 初期化 ======
	IMGUI_CHECKVERSION(); // ImGui のバージョンチェック
	ImGui::CreateContext(); // ImGui コンテキスト作成
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // キーボード入力有効化
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // ゲームパッド入力有効化

	ImGui::StyleColorsLight(); // テーマ適用

	// ImGui のバックエンド初期化（Win32 & DirectX9）
	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX9_Init(g_pD3DDevice);

	return S_OK; // 結果を返す

}
//==================
// 終了処理
//==================
void Uninit(void)
{
	//==================
	// 各種終了処理
	//==================

	// キーボード
	UninitKeyboard();

	// マウス
	UninitMouse();

	// パッド
	UninitJoypad();

	// ライト
	UninitLight();

	// 影
	UninitShadow();

	// メッシュの終了
	UninitMeshField();

	// エディター
	UninitMapEdit();

	// ブロック
	UninitBlock();

	// プレイヤー
	UninitPlayer();

	// テクスチャの破棄
	if (g_pModelIconTexture)
	{
		g_pModelIconTexture->Release();
		g_pModelIconTexture = nullptr;
	}

	// デバッグ表示用のフォントの破棄
	if (g_pFont != NULL)
	{
		g_pFont->Release();
		g_pFont = NULL;
	}

	// Direct3Dデバイスの破棄
	if (g_pD3DDevice != NULL)
	{
		g_pD3DDevice->Release();
		g_pD3DDevice = NULL;
	}

	// Direct3Dオブジェクトの破棄
	if (g_pD3D != NULL)
	{
		g_pD3D->Release();
		g_pD3D = NULL;
	}

	// Imguiの破棄
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
//===================
// 更新処理
//===================
void Update(void)
{
	//===========================
	// 各種オブジェクト更新処理
	//===========================
	// キーボードの更新処理
	UpdateKeyboard();

	// マウスの更新処理
	UpdateMouse();

	// ゲームパッドの更新処理
	UpdateJoypad();

	// カメラの更新
	UpdateCamera();

	// 影の更新処理
	UpdateShadow();

	// ブロックの更新処理
	UpdateBlock();

	if (g_mode == MODE_PLAY)
	{// プレイ時のみ
		// プレイヤーの更新
		UpdatePlayer();
	}

	if (KeyboardTrigger(DIK_F3))
	{// F3キー
		// ワイヤーフレーム起動
		onWireFrame();
	}
	else if (KeyboardTrigger(DIK_F4))
	{// F4キー
		// ワイヤーフレーム消去
		offWireFrame();
	}

	if (g_mode == MODE_EDIT)
	{// 編集モードの時
		// 更新
		UpdateMapEdit();
	}
}
//===================
// 描画処理
//===================
void Draw(void)
{
	// カメラ取得
	Camera* pCamera = GetCamera();

	// 画面クリア(バックバッファ&Zバッファのクリア)
	g_pD3DDevice->Clear(0,
		NULL,
		(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER),
		D3DCOLOR_RGBA(0, 0, 0, 0), 1.0f, 0);

	// 描画開始
	if (SUCCEEDED(g_pD3DDevice->BeginScene()))
	{
		// 描画成功時
		//================
		// 描画処理
		//================
			
		// カメラをセット
		SetCamera();

		// メッシュの描画
		DrawMeshField();


		if (g_mode == MODE_EDIT)
		{
			// エディター画面
			DrawMapEdit();
		}

		if (g_mode == MODE_PLAY)
		{
			// プレイヤーの描画
			DrawPlayer();

			// 影の描画
			DrawShadow();

			// ブロックの描画
			DrawBlock();

			// プレイヤー座標の描画
			DrawPlayerPos();
		}

		//===========================
		// 各種フォントの描画
		//===========================
		// キャプション
		//DrawFPS();

		//// カメラ表示
		//DrawCameraPos();

		//// 各種キーの種類
		// DrawEditkey();

		//// 配置数の表示
		//DrawNumBlock();

		//// モード切替表示
		//DrawModeChange();

		//// テキスト読み込み情報の表示
		//DrawEditMove();

		//// エディターモデル情報の表示
		//DebugEditModelInfo();
		
		// Guiウィンドウ
		ImguiDrawData();

		// 描画終了
		g_pD3DDevice->EndScene();
	}

	// バックバッファとフロントバッファの入れ替え
	g_pD3DDevice->Present(NULL, NULL, NULL, NULL);

}
//======================
// FPSの表示
//======================
void DrawFPS(void)
{
	RECT rect = { 0,0,SCREEN_WIDTH,SCREEN_HEIGHT };
	char aString[256];

	//文字列に代入
	wsprintf(&aString[0], "//************************\n// ショートカットキー情報\n//************************\n");

	//テキストの描画
	g_pFont->DrawText(NULL, &aString[0], -1, &rect, DT_LEFT, D3DCOLOR_RGBA(255, 255, 255, 255));
}
//==============================
// エディット画面の表示
//==============================
void DrawEditkey(void)
{
	// ローカル変数
	RECT rect5 = { 0,120,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT rect6 = { 0,60,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT rect7 = { 0,80,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT rect8 = { 0,100,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT rect10 = { 0,40,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT rect11 = { 0,140,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT rect12 = { 0,160,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT rect13 = { 0,180,SCREEN_WIDTH,SCREEN_HEIGHT };

	// 文字列
	char aString5[128];
	char aString6[128];
	char aString7[128];
	char aString8[128];
	char aString10[128];
	char aString11[128];
	char aString12[128];
	char aString13[128];
	char aStFile[64];
	char aStColl[64];

	// ファイルパスを取得
	Filenamepass = Filepass();

	int nModel = ReturnEdit(); // 配置カウント
	MAPMODELINFO* pEdit = MapInfo(); // 配置時の情報

	switch (Filenamepass)
	{
	case 0:
		strcpy(aStFile, "data/stage000.bin"); // 初期ファイル
		break;

	case 1:
		strcpy(aStFile, "data/stage001.bin"); // 2番目のファイル
		break;

	case 2:
		strcpy(aStFile, "data/stage002.bin"); // 3番目のファイル
		break;

	default:
		break;
	}

	// 文字列に代入
	if (pEdit[nModel].mapedit.bCollision)
	{// 有効ならなら
		strcpy(aStColl, "有効");
	}
	else if (!pEdit[nModel].mapedit.bCollision)
	{// 無効なら
		strcpy(aStColl, "無効");
	}


	// 文字列を設定
	wsprintf(&aString5[0], "[ F7 ] : ファイル書き出し  ***< %s >*** \n",&aStFile[0]);
	wsprintf(&aString6[0], "[ F1 ] / [ F2 ] : モード切り替え \n");
	wsprintf(&aString7[0], "[ F3 ] / [ F4 ] : ワイヤーフレーム切り替え\n");
	wsprintf(&aString8[0], "[ F5 ] / [ F6 ] : 当たり判定描画切り替え ( プレイモード時 )\n");
	wsprintf(&aString10[0]," CollisionBlock判定  [ %s ]\n",&aStColl[0]);
	wsprintf(&aString11[0],"[ F8 ] : 書き出すファイルパスを切り替え \n");
	wsprintf(&aString12[0],"[ F9 ] : 配置情報の再読み込み \n");
	wsprintf(&aString13[0],"[ F10 ] / [ F11 ] : 自動旋回切り替え ( プレイモード時 ) \n");

	// フォントの表示
	g_pFont->DrawText(NULL, &aString5[0], -1, &rect5, DT_LEFT, D3DCOLOR_RGBA(255, 255, 255, 255));
	g_pFont->DrawText(NULL, &aString6[0], -1, &rect6, DT_LEFT, D3DCOLOR_RGBA(255, 255, 255, 255));
	g_pFont->DrawText(NULL, &aString7[0], -1, &rect7, DT_LEFT, D3DCOLOR_RGBA(255, 255, 255, 255));
	g_pFont->DrawText(NULL, &aString8[0], -1, &rect8, DT_LEFT, D3DCOLOR_RGBA(255, 255, 255, 255));
	g_pFont->DrawText(NULL, &aString10[0], -1, &rect10, DT_RIGHT, D3DCOLOR_RGBA(255, 255, 255, 255));
	g_pFont->DrawText(NULL, &aString11[0], -1, &rect11, DT_LEFT, D3DCOLOR_RGBA(255, 255, 255, 255));
	g_pFont->DrawText(NULL, &aString12[0], -1, &rect12, DT_LEFT, D3DCOLOR_RGBA(255, 255, 255, 255));
	g_pFont->DrawText(NULL, &aString13[0], -1, &rect13, DT_LEFT, D3DCOLOR_RGBA(255, 255, 255, 255));

}
//========================
// カメラ表示
//========================
void DrawCameraPos(void)
{
	// ローカル変数
	RECT rect = { 0,600,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT rect1 = { 0,620,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT rect2 = { 0,520,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT rect3 = { 0,580,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT rect4 = { 0,640,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT rect5 = { 0,660,SCREEN_WIDTH,SCREEN_HEIGHT };

	// 文字列
	char aString[256];
	char aString1[256];
	char aString2[256];
	char aString3[256];
	char aString4[64];
	char aString5[64];

	// カメラの取得
	Camera* pCamera = GetCamera();

	// 文字列に代入
	wsprintf(&aString2[0], "//************************\n// カメラ情報\n//************************\n");
	sprintf(&aString[0],   "[ カメラのrot.y ]: < %.2f >\n", pCamera->rot.y);
	sprintf(&aString1[0],  "[ カメラのrot.x ]: < %.2f >\n", pCamera->rot.x);
	wsprintf(&aString3[0], "[ 視点 / 注視点 ] : < マウス操作 >\n");
	wsprintf(&aString4[0], "[ ズーム ] :   < マウスホイール >\n");
	wsprintf(&aString5[0], "[ リセット ] : < マウスホイール押し込み >\n");

	// テキスト描画
	g_pFont->DrawText(NULL, &aString[0], -1, &rect, DT_LEFT, D3DCOLOR_RGBA(255, 255, 255, 255));
	g_pFont->DrawText(NULL, &aString1[0], -1, &rect1, DT_LEFT, D3DCOLOR_RGBA(255, 255, 255, 255));
	g_pFont->DrawText(NULL, &aString2[0], -1, &rect2, DT_LEFT, D3DCOLOR_RGBA(255, 255, 255, 255));
	g_pFont->DrawText(NULL, &aString3[0], -1, &rect3, DT_LEFT, D3DCOLOR_RGBA(255, 255, 255, 255));
	g_pFont->DrawText(NULL, &aString4[0], -1, &rect4, DT_LEFT, D3DCOLOR_RGBA(255, 255, 255, 255));
	g_pFont->DrawText(NULL, &aString5[0], -1, &rect5, DT_LEFT, D3DCOLOR_RGBA(255, 255, 255, 255));

}
//============================
// エディターモデル関係の表示
//============================
void DebugEditModelInfo()
{
	// 取得関係
	int nModel = ReturnEdit(); // 配置カウント
	MAPMODELINFO* pEdit = MapInfo(); // 配置時の情報
	EDITMODEL* pModelEdit = GetBlockInfo(pEdit[nModel].mapedit.nType); // モデル情報

	// ローカル変数
	RECT rect = { 0,220,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT rect1 = { 0,320,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT rect2 = { 0,340,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT rect3 = { 0,360,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT rect4 = { 0,400,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT rect5 = { 0,420,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT rect6 = { 0,440,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT rect7 = { 0,480,SCREEN_WIDTH,SCREEN_HEIGHT };

	// 文字列
	char aString[256];
	char aString1[256];
	char aString2[256];
	char aString3[256];
	char aString4[256];
	char aString5[256];
	char aString6[256];
	char aString7[256];

	// 文字列に代入
	wsprintf(&aString[0], "//************************\n// 配置するモデル情報\n//************************\n");
	sprintf(&aString1[0], "[ モデルのX座標 ] : < %.2f >\n", pEdit[nModel].mapedit.pos.x);
	sprintf(&aString2[0], "[ モデルのY座標 ] : < %.2f >\n", pEdit[nModel].mapedit.pos.y);
	sprintf(&aString3[0], "[ モデルのZ座標 ] : < %.2f >\n", pEdit[nModel].mapedit.pos.z);
	sprintf(&aString4[0], "[ モデルのXの拡大率 ] : < %.2f > \n", pEdit[nModel].mapedit.Scal.x);
	sprintf(&aString5[0], "[ モデルのYの拡大率 ] : < %.2f >\n", pEdit[nModel].mapedit.Scal.y);
	sprintf(&aString6[0], "[ モデルのZの拡大率 ] : < %.2f >\n", pEdit[nModel].mapedit.Scal.z);
	sprintf(&aString7[0], "[ 選択中のモデルの種類 ] : < %s > \n", pModelEdit->FileName);

	// テキスト描画
	g_pFont->DrawText(NULL, &aString[0], -1, &rect, DT_LEFT, D3DCOLOR_RGBA(255, 255, 255, 255));
	g_pFont->DrawText(NULL, &aString1[0], -1, &rect1, DT_LEFT, D3DCOLOR_RGBA(255, 255, 255, 255));
	g_pFont->DrawText(NULL, &aString2[0], -1, &rect2, DT_LEFT, D3DCOLOR_RGBA(255, 255, 255, 255));
	g_pFont->DrawText(NULL, &aString3[0], -1, &rect3, DT_LEFT, D3DCOLOR_RGBA(255, 255, 255, 255));
	g_pFont->DrawText(NULL, &aString4[0], -1, &rect4, DT_LEFT, D3DCOLOR_RGBA(255, 255, 255, 255));
	g_pFont->DrawText(NULL, &aString5[0], -1, &rect5, DT_LEFT, D3DCOLOR_RGBA(255, 255, 255, 255));
	g_pFont->DrawText(NULL, &aString6[0], -1, &rect6, DT_LEFT, D3DCOLOR_RGBA(255, 255, 255, 255));
	g_pFont->DrawText(NULL, &aString7[0], -1, &rect7, DT_LEFT, D3DCOLOR_RGBA(255, 255, 255, 255));
}
//============================
// 現在の配置番号を表示
//============================
void DrawNumBlock()
{
	// 取得
	nBlockSet = ReturnEdit();

	// ローカル変数
	RECT rect = { 0,280,SCREEN_WIDTH,SCREEN_HEIGHT };
	char aString[256];

	// 文字列に代入
	wsprintf(&aString[0], "[ 配置したモデル数 ] < %d / 256 >\n", nBlockSet);

	// テキスト描画
	g_pFont->DrawText(NULL, &aString[0], -1, &rect, DT_LEFT, D3DCOLOR_RGBA(255, 255, 255, 255));
}
//==============================
// モード切替フォントの表示
//==============================
void DrawModeChange()
{
	// 取得
	MODE nMode = GetMode();

	// ローカル変数
	RECT rect = { 0,0,SCREEN_WIDTH,SCREEN_HEIGHT };

	char aString[256];
	char aString1[256];

	if (nMode == MODE_EDIT)
	{// 編集モードなら
		strcpy(aString1, "編集モード");
	}
	else if (nMode == MODE_PLAY)
	{// プレイモードなら
		strcpy(aString1, "プレイモード");
	}

	// 文字列に代入
	wsprintf(&aString[0], "現在のモード < %s >\n", &aString1[0]);

	// フォントの表示
	g_pFont->DrawText(NULL, &aString[0], -1, &rect, DT_RIGHT, D3DCOLOR_RGBA(255, 255, 255, 255));

}
//============================
// 読み込み関係の表示
//============================
void DrawEditMove()
{
	// データの取得
	float fspeed = ReturnSpeed();
	int nType = ReturnType();
	float fjumpmove = ReturnJump();

	// ローカル変数
	RECT rect = { 0,100,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT rect1 = { 0,120,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT rect2 = { 0,140,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT rect3 = { 0,160,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT rect4 = { 0,80,SCREEN_WIDTH,SCREEN_HEIGHT };

	// 文字列
	char aString[128];
	char aString1[128];
	char aString2[128];
	char aString3[128];
	char aString4[128];

	// 文字列に代入
	sprintf(&aString[0], "[ 配置時の移動量 ] : <   %.2f  >\n", fspeed);
	sprintf(&aString1[0], "[ 読み込む種類数 ] : < %d / 256 >\n", nType);
	wsprintf(&aString2[0], "[ モーション ] : < motion.txt >\n");
	sprintf(&aString3[0], "[ ジャンプの数値 ] : <   %.2f  >\n", fjumpmove);
	wsprintf(&aString4[0], "----- 読み込み情報 [ model.txt ] -----\n");


	// テキスト描画
	g_pFont->DrawText(NULL, &aString[0], -1, &rect, DT_RIGHT, D3DCOLOR_RGBA(255, 255, 255, 255));
	g_pFont->DrawText(NULL, &aString1[0], -1, &rect1, DT_RIGHT, D3DCOLOR_RGBA(255, 255, 255, 255));
	g_pFont->DrawText(NULL, &aString2[0], -1, &rect2, DT_RIGHT, D3DCOLOR_RGBA(255, 255, 255, 255));
	g_pFont->DrawText(NULL, &aString3[0], -1, &rect3, DT_RIGHT, D3DCOLOR_RGBA(255, 255, 255, 255));
	g_pFont->DrawText(NULL, &aString4[0], -1, &rect4, DT_RIGHT, D3DCOLOR_RGBA(255, 255, 255, 255));

}
//==============================
// プレイヤー座標表示
//==============================
void DrawPlayerPos()
{
	// プレイヤーを取得
	PLAYER* pPlayer = GetPlayer();

	// ローカル変数
	RECT rect = { 0,200,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT rect1 = { 0,220,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT rect2 = { 0,240,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT rect3 = { 0,260,SCREEN_WIDTH,SCREEN_HEIGHT };

	// 文字列
	char aString[128];
	char aString1[128];
	char aString2[128];
	char aString3[128];

	// 文字列に代入
	wsprintf(&aString[0], "------ [ プレイヤー情報 ] ------\n");
	sprintf(&aString1[0], "プレイヤーpos.x  : <  %.2f  >\n", pPlayer->pos.x);
	sprintf(&aString2[0], "プレイヤーpos.y  : <   %.2f  >\n", pPlayer->pos.y);
	sprintf(&aString3[0], "プレイヤーpos.z  : <  %.2f  >\n", pPlayer->pos.z);

	// テキスト描画
	g_pFont->DrawText(NULL, &aString[0], -1, &rect, DT_RIGHT, D3DCOLOR_RGBA(255, 255, 255, 255));
	g_pFont->DrawText(NULL, &aString1[0], -1, &rect1, DT_RIGHT, D3DCOLOR_RGBA(255, 255, 255, 255));
	g_pFont->DrawText(NULL, &aString2[0], -1, &rect2, DT_RIGHT, D3DCOLOR_RGBA(255, 255, 255, 255));
	g_pFont->DrawText(NULL, &aString3[0], -1, &rect3, DT_RIGHT, D3DCOLOR_RGBA(255, 255, 255, 255));

}
//======================
// デバイスの取得
//======================
LPDIRECT3DDEVICE9 GetDevice(void)
{
	return g_pD3DDevice;
}
//====================
// モードの取得
//====================
MODE GetMode(void)
{
	return g_mode; // 現在のモードを返す
}
//=====================
// ワイヤーフレームON
//=====================
void onWireFrame()
{
	g_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
}
//=====================
// ワイヤーフレームOFF
//=====================
void offWireFrame() 
{
	g_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
}
//=================================================
// ウィンドウをフルスクリーンに変える処理
//=================================================
void ToggleFullscreen(HWND hWnd)
{
	// 現在のウィンドウスタイルを取得
	DWORD dwStyle = GetWindowLong(hWnd, GWL_STYLE);

	if (g_isFullscreen)
	{
		// ウィンドウモードに切り替え
		SetWindowLong(hWnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPos(hWnd, HWND_TOP, g_windowRect.left, g_windowRect.top,
			g_windowRect.right - g_windowRect.left, g_windowRect.bottom - g_windowRect.top,
			SWP_FRAMECHANGED | SWP_NOACTIVATE);
		ShowWindow(hWnd, SW_NORMAL);
	}
	else
	{
		// フルスクリーンモードに切り替え
		GetWindowRect(hWnd, &g_windowRect);
		SetWindowLong(hWnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
		SetWindowPos(hWnd, HWND_TOP, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
			SWP_FRAMECHANGED | SWP_NOACTIVATE);
		ShowWindow(hWnd, SW_MAXIMIZE);
	}

	g_isFullscreen = !g_isFullscreen;
}

//==========================================
// imguiウィンドウの生成処理
//==========================================
bool CreateDeviceD3D(HWND hWnd)
{
	if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == nullptr)
		return false;

	// Create the D3DDevice
	ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
	g_d3dpp.Windowed = TRUE;
	g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN; // Need to use an explicit format with alpha if needing per-pixel alpha composition.
	g_d3dpp.EnableAutoDepthStencil = TRUE;
	g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;           // Present with vsync
	//g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;   // Present without vsync, maximum unthrottled framerate
	if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pD3DDevice) < 0)
		return false;

	return true;
}
//==========================================
// imguiウィンドウの終了処理
//==========================================
void CleanupDeviceD3D()
{
	if (g_pD3DDevice) { g_pD3DDevice->Release(); g_pD3DDevice = nullptr; }
	if (g_pD3D) { g_pD3D->Release(); g_pD3D = nullptr; }
}
//==========================================
// imguiウィンドウの再初期化処理
//==========================================
void ResetDevice()
{
	ImGui_ImplDX9_InvalidateDeviceObjects();
	HRESULT hr = g_pD3DDevice->Reset(&g_d3dpp);
	if (hr == D3DERR_INVALIDCALL)
		IM_ASSERT(0);
	ImGui_ImplDX9_CreateDeviceObjects();
}
//==========================================
// imguiウィンドウの描画処理
//==========================================
void ImguiDrawData()
{
	//================================
	//  情報取得関係
	//================================
	PLAYER* pPlayer = GetPlayer(); // プレイヤー
	MODE nMode = GetMode(); // 現在のモード

	float fspeed = ReturnSpeed(); // 配置速度
	int nType = ReturnType(); // 種類数
	int nModel = ReturnEdit(); // 配置カウント

	MAPMODELINFO* pEdit = MapInfo(); // 配置時の情報
	EDITMODEL* pModelEdit = GetBlockInfo(pEdit[nModel].mapedit.nType); // モデル情報
	Camera* pCamera = GetCamera(); // カメラ
	Filenamepass = Filepass(); // ファイルパスを取得

	// 文字列
	char aStFile[256];

	// ファイルパス
	switch (Filenamepass)
	{
	case 0:
		strcpy(aStFile, "data/stage000.bin"); // 初期ファイル
		break;
	case 1:
		strcpy(aStFile, "data/stage001.bin"); // 2番目のファイル
		break;
	case 2:
		strcpy(aStFile, "data/stage002.bin"); // 3番目のファイル
		break;
	default:
		break;
	}

	// フレーム開始
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	//==============================
	// メインの ImGui ウィンドウ
	//==============================
	// サイズ,場所固定
	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(300, 720), ImGuiCond_Always); 

	// シーン開始
	ImGui::Begin("Debug & Mode Selector", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

	// FPS 表示
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
		1000.0f / (float)g_nCountFPS, (float)g_nCountFPS);

	// モード選択
	if (ImGui::CollapsingHeader("Mode Selector"))
	{
		bool isEditMode = (g_mode == MODE_EDIT);

		if (ImGui::Checkbox("Edit Mode", &isEditMode))
		{

			g_mode = isEditMode ? MODE_EDIT : MODE_PLAY;

			if (g_mode == MODE_PLAY)
			{
				LoadBlock(); // モデル読み込み
			}
		}

		// LoadModelIcon(g_pD3DDevice, "data/TEXTURE/gold.jpg");
		ImGui::Text("Now Mode: %s", (g_mode == MODE_EDIT) ? "EDIT" : "PLAY");
	}

	// モデル情報
	if (ImGui::CollapsingHeader("Model Info"))
	{
		//static char aSt[128] = "";

		//ImGui::InputText("Enter text", aSt, sizeof(aSt));

		//if (ImGui::Button("Save to File")) 
		//{// 書き出し
		//	SaveToFile(aSt);
		//}		

		//// モデルのサムネイルを表示
		//if (g_pModelIconTexture)
		//{
		//	ImGui::Text("Model Icon:");
		//	ImGui::Image((ImTextureID)g_pModelIconTexture, ImVec2(64, 64)); // 64x64ピクセルで描画
		//}

		ImGui::Text("SetBlock Num: %d / 256", ReturnEdit()); // 配置数
		ImGui::Text("FilePass: %s", aStFile); // ファイルパス

		// モデルの位置を変更可能にする
		ImGui::Text("Model Position"); // 座標

		if (ImGui::InputFloat("Pos X", &pEdit[nModel].mapedit.pos.x, 20.0f, 1000.0f, "%.2f"))
		{
			UpdateModelPosition(nModel, pEdit[nModel].mapedit.pos); // モデルの位置更新関数を呼ぶ
		}
		if (ImGui::InputFloat("Pos Y", &pEdit[nModel].mapedit.pos.y, 20.0f, 1000.0f, "%.2f"))
		{
			UpdateModelPosition(nModel, pEdit[nModel].mapedit.pos); // モデルの位置更新関数を呼ぶ

		}
		if (ImGui::InputFloat("Pos Z", &pEdit[nModel].mapedit.pos.z, 20.0f, 1000.0f, "%.2f"))
		{
			UpdateModelPosition(nModel, pEdit[nModel].mapedit.pos); // モデルの位置更新関数を呼ぶ

		}

		// モデルのスケールを変更可能にする
		ImGui::Text("Model Scale");

		if (ImGui::InputFloat("Scale X", &pEdit[nModel].mapedit.Scal.x, 0.1f, 2.0f, "%.2f"))
		{
			UpdateModelScale(nModel, pEdit[nModel].mapedit.Scal); // 拡大率変更
		}
		if (ImGui::InputFloat("Scale Y", &pEdit[nModel].mapedit.Scal.y, 0.1f, 2.0f, "%.2f"))
		{
			UpdateModelScale(nModel, pEdit[nModel].mapedit.Scal); // 拡大率変更

		}
		if (ImGui::InputFloat("Scale Z", &pEdit[nModel].mapedit.Scal.z, 0.1f, 2.0f, "%.2f"))
		{
			UpdateModelScale(nModel, pEdit[nModel].mapedit.Scal); // 拡大率変更

		}
	}

	// 種類
	if (ImGui::CollapsingHeader("Block Info"))
	{
		ImGui::Text("AllBlockType: %d / 256", ReturnType());
	}

	ImGui::End();

	// レンダリング処理
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}
//=======================================
// 座標更新用関数
//=======================================
void UpdateModelPosition(int modelIndex, D3DXVECTOR3 newPos)
{
	MAPMODELINFO* pEdit = MapInfo();

	// 座標を代入する
	pEdit[modelIndex].mapedit.pos = newPos;

	// もしモデルのワールドマトリクスを更新する必要がある場合
	UpdateModelMatrix(modelIndex);
}
//=======================================
// 拡大率更新用関数
//=======================================
void UpdateModelScale(int modelIndex, D3DXVECTOR3 newScale)
{
	MAPMODELINFO* pEdit = MapInfo();

	// 拡大率を代入
	pEdit[modelIndex].mapedit.Scal = newScale;

	// 拡大率制限
	if (pEdit[modelIndex].mapedit.Scal.x >= 2.0f)
	{// X
		pEdit[modelIndex].mapedit.Scal.x = 2.0f;
	}
	if (pEdit[modelIndex].mapedit.Scal.y >= 2.0f)
	{// Y								   
		pEdit[modelIndex].mapedit.Scal.y = 2.0f;
	}
	if (pEdit[modelIndex].mapedit.Scal.z >= 2.0f)
	{// Z							   
		pEdit[modelIndex].mapedit.Scal.z = 2.0f;
	}

	// ワールドマトリクス更新
	UpdateModelMatrix(modelIndex);
}
//=======================================
// ワールドマトリックス更新用関数
//=======================================
void UpdateModelMatrix(int modelIndex)
{
	MAPMODELINFO* pEdit = MapInfo();

	// D3DXMatrixTransformation を使ってワールドマトリクスを更新
	D3DXMATRIX matScale, matTranslate, matWorld;

	// 拡大率
	D3DXMatrixScaling(&matScale,
		pEdit[modelIndex].mapedit.Scal.x,
		pEdit[modelIndex].mapedit.Scal.y,
		pEdit[modelIndex].mapedit.Scal.z);

	// 位置
	D3DXMatrixTranslation(&matTranslate,
		pEdit[modelIndex].mapedit.pos.x,
		pEdit[modelIndex].mapedit.pos.y,
		pEdit[modelIndex].mapedit.pos.z);

	// ワールドマトリクスの更新
	matWorld = matScale * matTranslate;

	// インデックス番号のワールドマトリックス
	pEdit[modelIndex].mapedit.mtxWorld = matWorld;
}
//==============================
// モデルアイコンを読み込む関数
//==============================
void LoadModelIcon(LPDIRECT3DDEVICE9 pDevice, const char* filePath)
{
	// テクスチャの読み込み
	if (FAILED(D3DXCreateTextureFromFile(pDevice, filePath, &g_pModelIconTexture)))
	{
		MessageBoxA(0, "モデルアイコンの読み込みに失敗しました", "Error", MB_OK);
	}
}
void SaveToFile(const char* text) 
{
	FILE* file = fopen("data\\test.txt", "w"); // "w" モードで書き込み
	if (file)
	{
		fprintf(file, "%s", text);
		fclose(file);
	}
}
