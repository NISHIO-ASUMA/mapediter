//=================================================
//
// ImGuiの情報設定処理関数 [ imgui_info.cpp ]
// Author : Asuma Nishio
//
//==================================================

//*******************************************
// インクルードファイル宣言
//*******************************************
#include "imgui_info.h"
#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include "mapedit.h"
#include "player.h"
#include "camera.h"
#include "block.h"
#include <stdio.h>
#include "fade.h"

//*******************************************
// グローバル変数宣言
//*******************************************

//===========================================
// Guiの初期化関数
//===========================================
void InitImguiInfo(HWND hWnd, LPDIRECT3DDEVICE9 pDevice)
{
	// 初期化関数
	InitMallocData();

	// ====== ImGui 初期化 ======
	IMGUI_CHECKVERSION(); // ImGui のバージョンチェック
	ImGui::CreateContext(); // ImGui コンテキスト作成
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // キーボード入力有効化
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // ゲームパッド入力有効化

	ImGui::StyleColorsClassic(); // テーマ適用

	// ====== フォント設定 ======
	// カスタムフォントの追加 (フォントパスを変更してください)
	ImFont* myFont = io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/consola.ttf", 15.0f);

	if (myFont == nullptr) 
	{// フォントがNULLなら
		printf("フォントのロードに失敗しました\n");
	}

	// フォントをリビルド
	ImGui::GetIO().Fonts->Build();

	// ImGui のバックエンド初期化（Win32 & DirectX9）
	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX9_Init(pDevice);
}
//===========================================
// Guiの終了関数
//===========================================
void UninitImguiInfo()
{
	//=======================================
	// ドロップダウンメニュー用終了処理
	//========================================
	UninitMallocData();

	// Imguiの破棄
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
//===========================================
// Guiの座標設定関数
//===========================================
void SetPosImgui(float PosX,float PosY)
{
	ImGui::SetNextWindowPos(ImVec2(PosX, PosY), ImGuiCond_Always);
}
//===========================================
// Guiのサイズ設定関数
//===========================================
void SetSizeImgui(float SizeX, float SizeY)
{
	ImGui::SetNextWindowSize(ImVec2(SizeX, SizeY), ImGuiCond_Always);
}
//===========================================
// Guiの描画関数
//===========================================
void DrawImguiInfo()
{
	//================================
	//  情報取得関係
	//================================
	PLAYER* pPlayer = GetPlayer(); // プレイヤー
	MODE nMode = GetMode();	// 現在のモード取得

	float fspeed = ReturnSpeed(); // 配置速度
	int nType = ReturnType(); // 種類数
	int nModel = ReturnEdit(); // 配置カウント

	MAPMODELINFO* pEdit = MapInfo(); // 配置時の情報
	EDITMODEL* pModelEdit = GetBlockInfo(pEdit[nModel].mapedit.nType); // モデル情報
	Camera* pCamera = GetCamera(); // カメラ
	int Filenamepass = Filepass(); // ファイルパスを取得

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
	NewFrameImGui();

	if (nMode == MODE_EDIT)
	{
		//==============================
		// 1個目の ImGui ウィンドウ
		//==============================
		// 大きさ,サイズ設定
		SetPosImgui(0.0f, 110.0f);
		SetSizeImgui(250.0f, 560.0f);

		// 描画
		StartImgui("Edit_Info", IMGUITYPE_NOMOVEANDSIZE);

		// モデル情報
		if (ImGui::CollapsingHeader("Model Info"))
		{

			ImGui::Text("SetBlock Num: %d / 256", ReturnEdit()); // 配置数
			ImGui::Text("FilePass: %s", aStFile); // ファイルパス

			// モデルの位置を変更可能にする
			ImGui::Text("Model Position"); // 座標

			ImGui::SetNextItemWidth(100.0f);

			if (ImGui::InputFloat("Pos X", &pEdit[nModel].mapedit.pos.x, 20.0f, 1000.0f, "%.2f"))
			{
				UpdateModelPosition(nModel, pEdit[nModel].mapedit.pos); // モデルの位置更新関数を呼ぶ
			}

			ImGui::SetNextItemWidth(100.0f);

			if (ImGui::InputFloat("Pos Y", &pEdit[nModel].mapedit.pos.y, 20.0f, 1000.0f, "%.2f"))
			{
				UpdateModelPosition(nModel, pEdit[nModel].mapedit.pos); // モデルの位置更新関数を呼ぶ

			}

			ImGui::SetNextItemWidth(100.0f);

			if (ImGui::InputFloat("Pos Z", &pEdit[nModel].mapedit.pos.z, 20.0f, 1000.0f, "%.2f"))
			{
				UpdateModelPosition(nModel, pEdit[nModel].mapedit.pos); // モデルの位置更新関数を呼ぶ
			}

			// モデルのスケールを変更可能にする
			ImGui::Text("Model Scale");

			ImGui::SetNextItemWidth(100.0f);

			if (ImGui::InputFloat("Scale X", &pEdit[nModel].mapedit.Scal.x, 0.1f, 2.0f, "%.2f"))
			{
				UpdateModelScale(nModel, pEdit[nModel].mapedit.Scal); // 拡大率変更
			}

			ImGui::SetNextItemWidth(100.0f);

			if (ImGui::InputFloat("Scale Y", &pEdit[nModel].mapedit.Scal.y, 0.1f, 2.0f, "%.2f"))
			{
				UpdateModelScale(nModel, pEdit[nModel].mapedit.Scal); // 拡大率変更

			}

			ImGui::SetNextItemWidth(100.0f);

			if (ImGui::InputFloat("Scale Z", &pEdit[nModel].mapedit.Scal.z, 0.1f, 2.0f, "%.2f"))
			{
				UpdateModelScale(nModel, pEdit[nModel].mapedit.Scal); // 拡大率変更
			}

		}

		// ファイル情報
		if (ImGui::CollapsingHeader("Save & Load"))
		{
			static bool saveSuccess = false; // 成功フラグ
			static bool saveError = false;   // 失敗フラグ
			static float saveMessageTimer = 0.0f; // メッセージ表示用タイマー
			static float saveMessageDuration = 2.0f; // 2秒間表示

			// 赤いボタン
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 0.6f)); // 赤色

			if (ImGui::Button("Save File"))
			{

				if (SaveEdit())  // SaveEdit() の戻り値で成否を判断
				{
					saveSuccess = true;
					saveError = false;
					saveMessageTimer = ImGui::GetTime(); // 現在の時間を記録
				}
				else
				{
					saveSuccess = false;
					saveError = true;
					saveMessageTimer = ImGui::GetTime(); // 失敗時も記録
				}
			}

			// ボタンの設定を戻す
			ImGui::PopStyleColor(1); // 1つのカラー設定をリセット

			// メッセージの表示（一定時間後に消す）
			if (saveSuccess)
			{
				if (ImGui::GetTime() - saveMessageTimer < saveMessageDuration)
				{
					// 書き出し成功
					ImGui::TextColored(ImVec4(0, 1, 0, 1), "Save Successful!");
				}
				else
				{
					saveSuccess = false; // 時間が経過したら非表示
				}
			}
			else if (saveError)
			{
				if (ImGui::GetTime() - saveMessageTimer < saveMessageDuration)
				{
					// 書き出し失敗
					ImGui::TextColored(ImVec4(1, 0, 0, 1), "Save Failed!");
				}
				else
				{
					saveError = false; // 時間が経過したら非表示
				}
			}

			// リロードか否か
			static bool isReload = false;
			static bool isload = isReload;

			if (ImGui::Button("ReloadFile") && !isReload)
			{
				// 再読み込み処理
				ReloadEdit();
				isReload = true; // フラグをtrue
			}

		}

		// 終了関数
		ImGui::End();
		

		//==============================
		// 2個目の ImGui ウィンドウ
		//==============================

		// モード切替データ
		ImguiDrawData();

		//==============================
		// 3個目の ImGui ウィンドウ
		//==============================
		// 大きさ,サイズ設定
		SetPosImgui(1000.0f, 0.0f);
		SetSizeImgui(280.0f, 200.0f);

		// インスペクターの描画
		StartImgui("Inspector", IMGUITYPE_NOMOVEANDSIZE);

		if (ImGui::CollapsingHeader("Player_Info"))
		{
			// プレイヤー情報
			ImGui::Text("POS : %.2f,%.2f,%.2f", pPlayer->pos.x, pPlayer->pos.y, pPlayer->pos.z); // 現在の座標
			ImGui::Text("ROT : %.2f,%.2f,%.2f", pPlayer->rot.x, pPlayer->rot.y, pPlayer->rot.z); // 現在の角度
		}

		// 3個目のウィンドウ終了
		ImGui::End();

		// レンダリング処理
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
	}
}
//===========================================
// Guiの開始関数
//===========================================
void StartImgui(const char* ImguiName, IMGUITYPE nType)
{
	if (ImguiName != NULL)
	{
		switch (nType)
		{
		case IMGUITYPE_NONE:
			// シンプルスタート
			ImGui::Begin(ImguiName);
			break;
		case IMGUITYPE_NOMOVE:
			// 移動させない時
			ImGui::Begin(ImguiName, NULL, ImGuiWindowFlags_NoMove);
			break;
		case IMGUITYPE_NOMOVESIZE:
			// 拡大させない時
			ImGui::Begin(ImguiName, NULL, ImGuiWindowFlags_NoResize);
			break;
		case IMGUITYPE_NOMOVEANDSIZE:
			// 移動も拡大もさせない
			ImGui::Begin(ImguiName, NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
			break;
		default:
			break;
		}
	}

}
//===========================================
// Guiのフレーム開始関数
//===========================================
void NewFrameImGui()
{
	// フレーム開始
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}
//===========================================
// Guiのブロック終了関数
//===========================================
void EndImgui()
{
	// 終了関数
	ImGui::End();

	// レンダリング処理
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}
//===========================================
// Guiのフレーム終了関数
//===========================================
void EndImguiFrame()
{
	// フレーム終了
	ImGui::EndFrame();
}
//===========================================
// メモリの動的確保処理初期化関数
//===========================================
void InitMallocData()
{
	//// 種類数
	//g_nTypeNum = ReturnType();
	//int nData = g_nTypeNum;

	//// メモリの確保用変数
	//items = (char**)malloc(nData * sizeof(char*));

	//// 配列に代入
	//items[0] = strdup("block000.x");
	//items[1] = strdup("block001.x");
	//items[2] = strdup("block002.x");
	//items[3] = strdup("body.x");
}
//===========================================n
// メモリの動的確保処理終了関数
//===========================================
void UninitMallocData()
{
	//// メモリの解放 (これしないとヤバい！！！メモリリークになってしまう...)
	//for (int nCnt = 0; nCnt < g_n; nCnt++)
	//{
	//	free(items[nCnt]);
	//}
	//free(items);
}
//===========================================
// メモリの動的確保処理更新関数
//===========================================
void UpdatemallocData(int nModel)
{
	//// 取得
	//MAPMODELINFO* pEdit = MapInfo(); // 配置時の情報

	//// 現在選択されているモデルの種類
	//int& currentItem = pEdit[nModel].mapedit.nType;

	//if (ImGui::Combo("Type", &currentItem, (const char**)items, g_n))
	//{
	//	// ここに、選択されたときの処理を書くよ！
	//	printf("Selected: %s\n", items[currentItem]);
	//}
	
}
