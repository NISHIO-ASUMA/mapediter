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

//*********************************
// 特殊コード
//*********************************
#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include "mouse.h"

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

static bool g_DeviceLost = false;
static UINT g_ResizeWidth = 0, g_ResizeHeight = 0;
static D3DPRESENT_PARAMETERS  g_d3dpp = {};

//******************************
// プロトタイプ宣言
//******************************
void ToggleFullscreen(HWND hWnd);		//　ウィンドウをフルスクリーンにする方法
void DrawEditkey(void);					// エディター画面の操作フォント用
void DrawModeChange();					// モード切り替え
void ScalBlock(void);					// 拡大率
void DrawEditMove();					// 移動量
void DrawPlayerPos();					// プレイヤー座標
void DebugEditModelInfo();				// 配置モデル情報

//******************************
// imgui
//******************************
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void ResetDevice();

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
//===============================
// メイン関数
//===============================
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_  HINSTANCE hInstancePrev, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF);		// メモリリーク検知用のフラグ


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
		NULL,
		NULL,
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
	ShowWindow(hWnd, SW_SHOWMAXIMIZED); // ウインドウの表示状態の設定
	UpdateWindow(hWnd);				    // クライアント領域の更新


	// 初期化
	DWORD dwFrameCount;					// フレームカウント
	DWORD dwFPSLastTime;				// 最後にFPSを計測した時刻

	dwFrameCount = 0;
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
			if (!GetMessage(&msg, NULL, 0, 0)) {
				break;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
			
		}

		else
		{

			// DirectXの処理
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
//=======================================
// ウインドウプロシージャ
//=======================================
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	const RECT rect = { 0,0,SCREEN_WIDTH,SCREEN_HEIGHT }; // ウインドウの領域
	int nID;

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
			return 0;
		g_ResizeWidth = (UINT)LOWORD(lParam); // Queue resize
		g_ResizeHeight = (UINT)HIWORD(lParam);
		break;

	case WM_DESTROY: // ウインドウ破棄メッセージ
		// WM_QUITメッセージを送る
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:				 // キー押下のメッセージ
		switch (wParam)
		{
		case VK_ESCAPE:				 // [ESC]キーが押された
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

		case VK_F11:
			ToggleFullscreen(hWnd);	 // F11でフルスクリーン
			break;
		}
		break;

	case WM_MOUSEWHEEL:
		// ローカル
		int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
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
	// デバイスのプレゼンテーションのパラメーターを設定
	ZeroMemory(&d3dpp, sizeof(d3dpp));//パラメーターの0クリア

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

	// メッシュ
	InitMeshField();

	// エディター
	InitMapEdit();

	// ブロック
	InitBlock();

	// プレイヤー
	InitPlayer();

	return S_OK; // 結果を返す
#if 0

//==============================
// 	   (IMGUIのセットアップ)
//==============================


	// Setup Dear ImGui context　(セットアップ)
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Platform/Renderer backends　(プラっとフォーム/バックエンド環境)
	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX9_Init(g_pD3DDevice);

	//================
	// 各種初期化処理
	//================

		// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	// デモウィンドウの出現
	if (show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);
	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
	{
		static float f = 0.0f;
		static int counter = 0;

		ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

		ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
		ImGui::Checkbox("Another Window", &show_another_window);

		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

		if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
		ImGui::End();
	}

	// 3. Show another simple window.
	if (show_another_window)
	{
		ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui::Text("Hello from another window!");
		if (ImGui::Button("Close Me"))
			show_another_window = false;
		ImGui::End();
	}


	// Our state　(変数の指定)
	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	// Start the Dear ImGui frame (IMGUIのフレーム開始)
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// メインループの下に追加
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

	//================
// 終了処理に追加
//=================
// Cleanup 破棄
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();


#endif

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

	// メッシュ
	UninitMeshField();

	// エディター
	UninitMapEdit();

	// ブロック
	UninitBlock();

	// プレイヤー
	UninitPlayer();

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

}
//===================
// 更新処理
//===================
void Update(void)
{

	//===========================
	// 各種オブジェクト更新処理
	//===========================
	UpdateKeyboard();

	UpdateMouse();

	UpdateJoypad();

	// カメラの更新
	UpdateCamera();

	UpdateShadow();

	UpdateMeshField();

	UpdateBlock();

	// ワイヤーフレーム関係
	if (KeyboardTrigger(DIK_F3))
	{
		onWireFrame();
	}
	else if (KeyboardTrigger(DIK_F4))
	{
		offWireFrame();
	}

	if (KeyboardTrigger(DIK_F1))
	{
		g_mode = MODE_PLAY;

		LoadBlock();

	}
	else if (KeyboardTrigger(DIK_F2))
	{
		g_mode = MODE_EDIT;
	}

	if (g_mode == MODE_EDIT)
	{
		// 更新
		UpdateMapEdit();
	}

	if (g_mode == MODE_PLAY)
	{
		UpdatePlayer();
	}
}
//===================
// 描画処理
//===================
void Draw(void)
{
	// カメラ取得
	Camera* pCamera = GetCamera();

	//画面クリア(バックバッファ&Zバッファのクリア)
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
			
		SetCamera();

		DrawMeshField();

		if (g_mode == MODE_EDIT)
		{
			// エディタ
			DrawMapEdit();

		}

		if (g_mode == MODE_PLAY)
		{
			DrawPlayer();

			DrawShadow();

			DrawBlock();

			DrawPlayerPos();
		}

		//===========================
		// 各種フォントの描画
		//===========================
		// キャプション
		DrawFPS();

		// カメラ
		DrawCameraPos();

		// 各種キーの種類
		DrawEditkey();

		DrawNumBlock();

		DrawModeChange();

		ScalBlock();

		DrawEditMove();

		DebugEditModelInfo();

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
	wsprintf(&aString[0], "************************ 試作モデルビューワー ****************\n");

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
	RECT rect6 = { 0,100,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT rect7 = { 0,80,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT rect8 = { 0,140,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT rect10 = { 0,40,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT rect11 = { 0,140,SCREEN_WIDTH,SCREEN_HEIGHT };

	char aString5[128];
	char aString6[128];
	char aString7[128];
	char aString8[128];
	char aString10[128];
	char aString11[128];
	char aStFile[64];

	// ファイルパスを取得
	Filenamepass = Filepass();

	switch (Filenamepass)
	{
	case 0:
		strcpy(aStFile, "data/stage000.bin");
		break;

	case 1:
		strcpy(aStFile, "data/stage001.bin");
		break;

	case 2:
		strcpy(aStFile, "data/stage002.bin");
		break;

	default:
		break;
	}

	wsprintf(&aString5[0], "ファイルに書き出し [ F7 ] ***< %s >*** \n",&aStFile[0]);
	wsprintf(&aString6[0], "モード切り替え [ F1 ] / [ F2 ]\n");
	wsprintf(&aString7[0], "[ 特殊コマンド ]---------------------------------------\n");
	wsprintf(&aString8[0], "[ 読み込み情報 ]--------------\n");
	wsprintf(&aString10[0], " CollisionBlock判定  [ 有効 ]\n");
	wsprintf(&aString11[0], "ファイルパスを切り替え [ F9 ]\n");

	g_pFont->DrawText(NULL, &aString5[0], -1, &rect5, DT_LEFT, D3DCOLOR_RGBA(255, 255, 255, 255));
	g_pFont->DrawText(NULL, &aString6[0], -1, &rect6, DT_LEFT, D3DCOLOR_RGBA(255, 255, 255, 255));
	g_pFont->DrawText(NULL, &aString7[0], -1, &rect7, DT_LEFT, D3DCOLOR_RGBA(255, 255, 255, 255));
	g_pFont->DrawText(NULL, &aString8[0], -1, &rect8, DT_RIGHT, D3DCOLOR_RGBA(255, 255, 255, 255));
	g_pFont->DrawText(NULL, &aString10[0], -1, &rect10, DT_RIGHT, D3DCOLOR_RGBA(255, 255, 255, 255));
	g_pFont->DrawText(NULL, &aString11[0], -1, &rect11, DT_LEFT, D3DCOLOR_RGBA(255, 255, 255, 255));

}
//========================
// カメラ表示
//========================
void DrawCameraPos(void)
{
	// ローカル変数
	RECT rect = { 0,540,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT rect1 = { 0,560,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT rect2 = { 0,500,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT rect3 = { 0,520,SCREEN_WIDTH,SCREEN_HEIGHT };

	char aString[256];
	char aString1[256];
	char aString2[256];
	char aString3[256];

	// 取得
	Camera* pCamera = GetCamera();

	// 文字列に代入
	wsprintf(&aString2[0], "[ カメラ関係 ]---------------------------------------\n");
	sprintf(&aString[0],   "カメラのrot.y:%.2f\n", pCamera->rot.y);
	sprintf(&aString1[0],  "カメラのrot.x:%.2f\n", pCamera->rot.x);
	wsprintf(&aString3[0], "視点操作 [ マウスクリックしながら上下左右移動 ]\n");

	// テキスト描画
	g_pFont->DrawText(NULL, &aString[0], -1, &rect, DT_LEFT, D3DCOLOR_RGBA(255, 255, 255, 255));
	g_pFont->DrawText(NULL, &aString1[0], -1, &rect1, DT_LEFT, D3DCOLOR_RGBA(255, 255, 255, 255));
	g_pFont->DrawText(NULL, &aString2[0], -1, &rect2, DT_LEFT, D3DCOLOR_RGBA(255, 255, 255, 255));
	g_pFont->DrawText(NULL, &aString3[0], -1, &rect3, DT_LEFT, D3DCOLOR_RGBA(255, 255, 255, 255));

}
//======================
//デバイスの取得
//======================
LPDIRECT3DDEVICE9 GetDevice(void)
{
	return g_pD3DDevice;
}
//====================
//モードの取得
//====================
MODE GetMode(void)
{
	return g_mode; // 変数を返す
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
	RECT rect = { 0,200,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT rect1 = { 0,220,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT rect2 = { 0,240,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT rect3 = { 0,260,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT rect4 = { 0,300,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT rect5 = { 0,320,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT rect6 = { 0,340,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT rect7 = { 0,380,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT rect8 = { 0,400,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT rect9 = { 0,420,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT rect10 = { 0,460,SCREEN_WIDTH,SCREEN_HEIGHT };

	// 文字列
	char aString[256];
	char aString1[256];
	char aString2[256];
	char aString3[256];
	char aString4[256];
	char aString5[256];
	char aString6[256];
	char aString7[256];
	char aString8[256];
	char aString9[256];
	char aString10[256];

	// 文字列に代入
	wsprintf(&aString[0], "[ 配置する情報 ]-------------------\n");
	sprintf(&aString1[0], "[ 選択中のモデルのX座標 ] : %.2f\n", pEdit[nModel].mapedit.pos.x);
	sprintf(&aString2[0], "[ 選択中のモデルのY座標 ] : %.2f\n", pEdit[nModel].mapedit.pos.y);
	sprintf(&aString3[0], "[ 選択中のモデルのZ座標 ] : %.2f\n", pEdit[nModel].mapedit.pos.z);
	sprintf(&aString4[0], "[ 選択中のモデルのXの拡大率 ] : %.2f\n", pEdit[nModel].mapedit.Scal.x);
	sprintf(&aString5[0], "[ 選択中のモデルのYの拡大率 ] : %.2f\n", pEdit[nModel].mapedit.Scal.y);
	sprintf(&aString6[0], "[ 選択中のモデルのZの拡大率 ] : %.2f\n", pEdit[nModel].mapedit.Scal.z);
	sprintf(&aString7[0], "[ 選択中のモデルのXの回転値 ] : %.2f\n", pEdit[nModel].mapedit.rot.x);
	sprintf(&aString8[0], "[ 選択中のモデルのYの回転値 ] : %.2f\n", pEdit[nModel].mapedit.rot.y);
	sprintf(&aString9[0], "[ 選択中のモデルのZの回転値 ] : %.2f\n", pEdit[nModel].mapedit.rot.z);
	sprintf(&aString10[0],"[ 選択中のモデルの種類 ] : %s \n", pModelEdit->FileName);

	// テキスト描画
	g_pFont->DrawText(NULL, &aString[0], -1, &rect, DT_LEFT, D3DCOLOR_RGBA(255, 255, 255, 255));
	g_pFont->DrawText(NULL, &aString1[0], -1, &rect1, DT_LEFT, D3DCOLOR_RGBA(255, 255, 255, 255));
	g_pFont->DrawText(NULL, &aString2[0], -1, &rect2, DT_LEFT, D3DCOLOR_RGBA(255, 255, 255, 255));
	g_pFont->DrawText(NULL, &aString3[0], -1, &rect3, DT_LEFT, D3DCOLOR_RGBA(255, 255, 255, 255));
	g_pFont->DrawText(NULL, &aString4[0], -1, &rect4, DT_LEFT, D3DCOLOR_RGBA(255, 255, 255, 255));
	g_pFont->DrawText(NULL, &aString5[0], -1, &rect5, DT_LEFT, D3DCOLOR_RGBA(255, 255, 255, 255));
	g_pFont->DrawText(NULL, &aString6[0], -1, &rect6, DT_LEFT, D3DCOLOR_RGBA(255, 255, 255, 255));
	g_pFont->DrawText(NULL, &aString7[0], -1, &rect7, DT_LEFT, D3DCOLOR_RGBA(255, 255, 255, 255));
	g_pFont->DrawText(NULL, &aString8[0], -1, &rect8, DT_LEFT, D3DCOLOR_RGBA(255, 255, 255, 255));
	g_pFont->DrawText(NULL, &aString9[0], -1, &rect9, DT_LEFT, D3DCOLOR_RGBA(255, 255, 255, 255));
	g_pFont->DrawText(NULL, &aString10[0], -1, &rect10, DT_LEFT, D3DCOLOR_RGBA(255, 255, 255, 255));

}
//============================
// 現在の配置番号を表示
//============================
void DrawNumBlock()
{
	// ローカル変数
	int nData = 0;

	// 取得
	nData = ReturnEdit();

	// ローカル変数
	RECT rect = { 0,40,SCREEN_WIDTH,SCREEN_HEIGHT };
	char aString[256];

	// 文字列に代入
	wsprintf(&aString[0], "配置した数 < %d / 256 >\n", nData);
	
	// テキスト描画
	g_pFont->DrawText(NULL, &aString[0], -1, &rect, DT_LEFT, D3DCOLOR_RGBA(255, 255, 255, 255));
}
//============================
// 拡大率の表示
//============================
void ScalBlock(void)
{
#if 0
	// ローカル変数
	RECT rect = { 0,440,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT rect1 = { 0,460,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT rect2 = { 0,480,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT rect4 = { 0,500,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT rect5 = { 0,520,SCREEN_WIDTH,SCREEN_HEIGHT };

	// 文字列を設定
	char aString[256];
	char aString1[256];
	char aString2[256];
	char aString4[64];
	char aString5[128];

	// 文字列に代入する
	wsprintf(&aString[0],  "X方向への拡大率増減 [ Y / H ]\n");
	wsprintf(&aString1[0], "Y方向への拡大率増減 [ U / J ]\n");
	wsprintf(&aString2[0], "Z方向への拡大率増減 [ I / K ]\n");
	wsprintf(&aString4[0], "Y座標増減 [ T / G ]\n");
	wsprintf(&aString5[0], "消去 [ BACKSPACE ] (0以上の時のみ)\n");

	// 表示
	g_pFont->DrawText(NULL, &aString[0], -1, &rect, DT_LEFT, D3DCOLOR_RGBA(255, 255, 255, 255));
	g_pFont->DrawText(NULL, &aString1[0], -1, &rect1, DT_LEFT, D3DCOLOR_RGBA(255, 255, 255, 255));
	g_pFont->DrawText(NULL, &aString2[0], -1, &rect2, DT_LEFT, D3DCOLOR_RGBA(255, 255, 255, 255));
	g_pFont->DrawText(NULL, &aString4[0], -1, &rect4, DT_LEFT, D3DCOLOR_RGBA(255, 255, 255, 255));
	g_pFont->DrawText(NULL, &aString5[0], -1, &rect5, DT_LEFT, D3DCOLOR_RGBA(255, 255, 255, 255));
#endif
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

	// 表示
	g_pFont->DrawText(NULL, &aString[0], -1, &rect, DT_RIGHT, D3DCOLOR_RGBA(255, 255,255, 255));

}
//============================
// 読み込み関係の表示
//============================
void DrawEditMove()
{
	// 取得
	float fspeed = ReturnSpeed();
	int nType = ReturnType();
	float fjumpmove = ReturnJump();

	// ローカル変数
	RECT rect = { 0,160,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT rect1 = { 0,180,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT rect2 = { 0,200,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT rect3 = { 0,220,SCREEN_WIDTH,SCREEN_HEIGHT };

	// 文字列
	char aString[128];
	char aString1[128];
	char aString2[128];
	char aString3[128];

	// 文字列に代入
	sprintf(&aString[0], "設定した移動量 < %.2f>\n", fspeed);
	sprintf(&aString1[0],"設定した種類数 < %d / 256 >\n", nType);
	wsprintf(&aString2[0],"読み込むモーションファイル < data/motion.txt >\n");
	sprintf(&aString3[0], "設定したジャンプ量 < %.2f >\n", fjumpmove);

	// テキスト描画
	g_pFont->DrawText(NULL, &aString[0], -1, &rect, DT_RIGHT, D3DCOLOR_RGBA(255, 255, 255, 255));
	g_pFont->DrawText(NULL, &aString1[0], -1, &rect1, DT_RIGHT, D3DCOLOR_RGBA(255, 255, 255, 255));
	g_pFont->DrawText(NULL, &aString2[0], -1, &rect2, DT_RIGHT, D3DCOLOR_RGBA(255, 255, 255, 255));
	g_pFont->DrawText(NULL, &aString3[0], -1, &rect3, DT_RIGHT, D3DCOLOR_RGBA(255, 255, 255, 255));

}
//==============================
// プレイヤー座標表示
//==============================
void DrawPlayerPos()
{
	// プレイヤーを取得
	PLAYER* pPlayer = GetPlayer();

	// ローカル変数
	RECT rect =  { 0,60,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT rect1 = { 0,80,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT rect2 = { 0,100,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT rect3 = { 0,120,SCREEN_WIDTH,SCREEN_HEIGHT };

	// 文字列
	char aString[128];
	char aString1[128];
	char aString2[128];
	char aString3[128];

	// 文字列に代入
	wsprintf(&aString[0], "[ プレイヤー情報 ]--------------------\n");
	sprintf(&aString1[0], "プレイヤーpos.x < %.2f >\n", pPlayer->pos.x);
	sprintf(&aString2[0], "プレイヤーpos.y < %.2f >\n", pPlayer->pos.y);
	sprintf(&aString3[0], "プレイヤーpos.z < %.2f >\n", pPlayer->pos.z);

	// テキスト描画
	g_pFont->DrawText(NULL, &aString[0], -1, &rect, DT_RIGHT, D3DCOLOR_RGBA(255, 255, 255, 255));
	g_pFont->DrawText(NULL, &aString1[0], -1, &rect1, DT_RIGHT, D3DCOLOR_RGBA(255, 255, 255, 255));
	g_pFont->DrawText(NULL, &aString2[0], -1, &rect2, DT_RIGHT, D3DCOLOR_RGBA(255, 255, 255, 255));
	g_pFont->DrawText(NULL, &aString3[0], -1, &rect3, DT_RIGHT, D3DCOLOR_RGBA(255, 255, 255, 255));

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