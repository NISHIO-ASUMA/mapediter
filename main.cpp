//====================================
//
// �}�b�v�G�f�B�^�[����� [main.cpp]
// Author: Asuma Nishio
//
//====================================

//****************************
// �C���N���[�h�t�@�C���錾
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
// imgui�C���N���[�h�t�@�C���錾
//*********************************
#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"

//*****************************
// �O���[�o���ϐ��錾
//*****************************
LPDIRECT3D9 g_pD3D = NULL;				// Direct3D�I�u�W�F�N�g�ւ̃|�C���^
LPDIRECT3DDEVICE9 g_pD3DDevice = NULL;	// Direct3D�f�o�C�X�ւ̃|�C���^
MODE g_mode = MODE_EDIT;				// ���݂̃��[�h
bool g_isFullscreen = false;			// �E�B���h�E��؂�ւ��邽�߂̃t���O
RECT g_windowRect;						// �E�B���h�E��؂�ւ��邽�߂̕ϐ�
LPD3DXFONT g_pFont = NULL;				// �t�H���g�ւ̃|�C���^
int g_nCountFPS = 0;					// FPS�J�E���^
int nGetNumber = 0;						// �ԍ�
int Filenamepass;						// �t�@�C����
int nBlockSet = 0;						// �z�u����u���b�N��
LPDIRECT3DTEXTURE9 g_pModelIconTexture = NULL; // ���f���A�C�R���̃e�N�X�`��

//*****************************
// imgui�O���[�o���ϐ��錾
//*****************************
static bool g_DeviceLost = false;
static UINT g_ResizeWidth = 0, g_ResizeHeight = 0;
static D3DPRESENT_PARAMETERS  g_d3dpp = {};

//******************************
// �v���g�^�C�v�錾
//******************************
void ToggleFullscreen(HWND hWnd);	// �E�B���h�E���t���X�N���[���ɂ�����@
void DrawEditkey(void);				// �G�f�B�^�[��ʂ̑���t�H���g�p
void DrawModeChange();				// ���[�h�؂�ւ�
void DrawEditMove();				// �ړ���
void DrawPlayerPos();				// �v���C���[���W
void DebugEditModelInfo();			// �z�u���f�����
void DrawCameraPos(void);			// �J�����̍��W�\��
void onWireFrame();					// ���C���[�t���[���N��
void offWireFrame();				// ���C���[�t���[���I��
void DrawNumBlock();				// �u���b�N�̃f�o�b�O�\��
void UpdateModelPosition(int modelIndex, D3DXVECTOR3 newPos); // �ʒu�X�V�֐�
void UpdateModelScale(int modelIndex, D3DXVECTOR3 newScale);  // �g�嗦�X�V�֐�
void UpdateModelMatrix(int modelIndex);		// ���[���h�}�g���b�N�X
void LoadModelIcon(LPDIRECT3DDEVICE9 pDevice, const char* filePath);

//******************************
// imgui�̃v���g�^�C�v�錾
//******************************
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void ResetDevice();
void ImguiDrawData();
void SaveToFile(const char* text); // �e�L�X�g

//===============================
// ���C���֐�
//===============================
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_  HINSTANCE hInstancePrev, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	// ���������[�N���m�p�̃t���O
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF);		

	DWORD dwCurrentTime;			// ���ݎ���
	DWORD dwExecLastTime;			// �I������

	WNDCLASSEX wcex =
	{
		sizeof(WNDCLASSEX),				 // WNDCLASSEX�̃������T�C�Y
		CS_CLASSDC,						 // �E�C���h�E�̃X�^�C��
		WindowProc,						 // �E�C���h�E�v���V�[�W��
		0,								 // 0�ɂ���
		0,								 // 0�ɂ���
		hInstance,						 // �C���X�^���X�n���h��
		LoadIcon(NULL,IDI_APPLICATION),  // �^�X�N�o�[�A�C�R��
		LoadCursor(NULL,IDC_ARROW),      // �}�E�X�J�[�\��
		(HBRUSH)(COLOR_WINDOW + 1),      // �N���C�A���g�̈�̔w�i�F
		NULL,                            // ���j���[�o�[
		CLASS_NAME,                      // �E�C���h�E�N���X�̖��O
		LoadIcon(NULL,IDI_APPLICATION),  // �t�@�C���A�C�R��
	};


	HWND hWnd = {};  // �E�C���h�E�n���h��(���ʎq)
	MSG msg;    //���b�Z�[�W���i�[����ϐ�

	// ��ʃT�C�Y�̍\��
	RECT rect = { 0,0,SCREEN_WIDTH,SCREEN_HEIGHT };

	// �t���X�N���[�����[�h
	BOOL bWindow = FALSE;

	// �E�C���h�E�N���X�̓o�^
	RegisterClassEx(&wcex);

	// �N���C�A���g�̈�
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

	// �E�C���h�E�𐶐�
	hWnd = CreateWindowEx(0,		// �g���E�C���h�E�X�^�C��
		CLASS_NAME,					// �E�C���h�E�N���X�̖��O
		WINDOW_NAME,				// �E�C���h�E�̖��O
		WS_OVERLAPPEDWINDOW,		// �E�C���h�E�X�^�C��
		CW_USEDEFAULT,				// �E�C���h�E�̍����X���W
		CW_USEDEFAULT,				// �E�C���h�E�̍����Y���W
		(rect.right - rect.left),   // �E�C���h�E��
		(rect.bottom - rect.top),   // �E�C���h�E�̍���
		NULL,						// NULL
		NULL,						// NULL
		hInstance,					// �C���X�^���X�n���h��
		NULL);						// �E�C���h�E�쐬�f�[�^


	// ����������
	if (FAILED(Init(hInstance, hWnd, TRUE)))
	{
		// ���������������s�����Ƃ�
		return -1;
	}

	// ����\�������Ă�
	timeBeginPeriod(1);
	dwCurrentTime = 0;					// ������
	dwExecLastTime = timeGetTime();		// ���ݎ�����ۑ�

	// �E�C���h�E�̕\��
	ShowWindow(hWnd, nCmdShow); // �E�C���h�E�̕\����Ԃ̐ݒ�
	UpdateWindow(hWnd);				    // �N���C�A���g�̈�̍X�V

	// ������
	DWORD dwFrameCount;					// �t���[���J�E���g
	DWORD dwFPSLastTime;				// �Ō��FPS���v����������

	dwFrameCount = 0;					// �t���[���J�E���g��������
	dwFPSLastTime = timeGetTime();		// ���݂̎������擾

	// ���b�Z�[�W���[�v
	while (1)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) != 0)
		{
			// Windows�̏���
			if (msg.message == WM_QUIT)
			{
				// WM_QUIT���b�Z�[�W���󂯎�����烋�[�v������
				break;
			}
			else
			{
				// ���b�Z�[�W�ݒ�
				TranslateMessage(&msg);	// ���z�L�[���b�Z�[�W�𕶎����b�Z�[�W�֕ϊ�

				DispatchMessage(&msg);	// �E�C���h�E�v���V�[�W���փ��b�Z�[�W�𑗏o
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
		{// DirectX�̏���
			dwCurrentTime = timeGetTime();		// ���ݎ����̎擾
			if ((dwCurrentTime - dwFPSLastTime) >= 500)
			{
				// 0.5�b�o��
				// FPS���v��
				g_nCountFPS = (dwFrameCount * 1000) / (dwCurrentTime - dwFPSLastTime);

				dwFPSLastTime = dwCurrentTime;	// �ۑ�

				dwFrameCount = 0;				// 0�ɖ߂�

			}

			if ((dwCurrentTime - dwExecLastTime) >= (1000 / 60))
			{
				// 60/1�o��
				dwExecLastTime = dwCurrentTime;	// �����J�n�̎���

				dwFrameCount++;					// �t���[���J�E���g�����Z

				// �X�V����
				Update();

				// �`��
				ImguiDrawData();

				// ImGui�t���[���̏I��
				ImGui::EndFrame();

				// �`�揈��
				Draw();

			}
		}

	}

	// �I������
	Uninit();

	// ����\��߂�
	timeEndPeriod(1);

	// �E�C���h�E�N���X�̓o�^������
	UnregisterClass(CLASS_NAME, wcex.hInstance);

	// ���b�Z�[�W��Ԃ�
	return(int)msg.wParam;
}

// IMGUI�̃n���h���[�錾
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

//=======================================
// �E�C���h�E�v���V�[�W��
//=======================================
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	const RECT rect = { 0,0,SCREEN_WIDTH,SCREEN_HEIGHT }; // �E�C���h�E�̗̈�
	int nID;

	// ImGui �̃C�x���g����
	if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
	{
		return true;
	}
	switch (uMsg)
	{
	case WM_ACTIVATE:	// �A�N�e�B�u���F1�@��A�N�e�B�u���F0
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

		g_ResizeWidth = (UINT)LOWORD(lParam); // ����
		g_ResizeHeight = (UINT)HIWORD(lParam);// ����
		break;

	case WM_DESTROY: // �E�C���h�E�j�����b�Z�[�W
		// WM_QUIT���b�Z�[�W�𑗂�
		PostQuitMessage(0); // �E�C���h�E�̔j��
		break;

	case WM_KEYDOWN: // �L�[�����̃��b�Z�[�W
		switch (wParam)
		{
		case VK_ESCAPE:	// [ESC]�L�[�������ꂽ

			nID = MessageBox(hWnd, "�I�����܂���?", "�I�����b�Z�[�W", MB_YESNO);
			if (nID == IDYES)
			{
				DestroyWindow(hWnd); // �E�C���h�E�j�����b�Z�[�W
			}
			else
			{
				return 0;			 // �Ԃ�
			}

			break;
		}
		break;

	case WM_MOUSEWHEEL: // �}�E�X�̃z�C�[������

		// ���[�J���ϐ�
		int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);

		// �z�C�[�����
		MouseWheel(zDelta);
		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam); // ����̏������J��Ԃ�
}
//=========================
// ����������
//=========================
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	D3DDISPLAYMODE d3ddm;			// �f�B�X�v���C���[�h
	D3DPRESENT_PARAMETERS d3dpp;	// �v���[���e�[�V�����p�����[�^

	// Direct3D�I�u�W�F�N�g�̐���
	g_pD3D = Direct3DCreate9(D3D_SDK_VERSION);

	if (g_pD3D == NULL)
	{
		return E_FAIL;
	}
	// ���݂̃f�B�X�v���C���[�h���擾
	if (FAILED(g_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm)))
	{
		return E_FAIL;
	}

	// �p�����[�^�[��0�N���A
	ZeroMemory(&d3dpp, sizeof(d3dpp));

	d3dpp.BackBufferWidth = SCREEN_WIDTH;						// �Q�[����ʃT�C�Y(��)
	d3dpp.BackBufferHeight = SCREEN_HEIGHT;						// �Q�[����ʃT�C�Y(����)
	d3dpp.BackBufferFormat = d3ddm.Format;						// �o�b�N�o�b�t�@�̌`��
	d3dpp.BackBufferCount = 1;									// �o�b�N�o�b�t�@�̐�
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;					// �_�u���o�b�t�@�̐؂�ւ�(�f���M���ɓ���)
	d3dpp.EnableAutoDepthStencil = TRUE;						// �f�v�X�o�b�t�@�ƃX�e���V���o�b�t�@���쐬
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;					// �f�v�X�o�b�t�@�Ƃ���16bit���g�p
	d3dpp.Windowed = bWindow;									// �E�C���h�E���[�h
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;	// ���t���b�V���V���[�g
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;	// �C���^�[�o��

	// Direct3D�f�o�C�X�̐���(�`�揈���ƒ��_�������n�[�h�E�F�A�ōs��)
	if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&d3dpp,
		&g_pD3DDevice)))
	{
		// Direct3D�f�o�C�X�̐���(�`�揈���̓n�[�h�E�F�A,���_������CPU�ōs��)
		if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			hWnd,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING,
			&d3dpp,
			&g_pD3DDevice)))
		{
			// Direct3D�f�o�C�X�̐���(�`�揈��,���_������CPU�ōs��)
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

	// �����_�[�X�e�[�g�̐ݒ�
	g_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	g_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	g_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	g_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	// �T���v���[�X�e�[�g�̐ݒ�
	g_pD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	g_pD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	g_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	g_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

	// �e�N�X�`���X�e�[�W�`���[�g�̐ݒ�
	g_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	g_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	g_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);

	//==============================
	// ����������
	//==============================
	if (FAILED(InitKeyboard(hInstance, hWnd)))
	{// �L�[�{�[�h
		return E_FAIL;
	}
	if (FAILED(InitJoypad()))
	{// �p�b�h
		return E_FAIL;
	}

	// �f�o�b�O�\���p�̃t�H���g��ݒ�
	D3DXCreateFont(g_pD3DDevice, 18, 0, 0, 0,
		FALSE,
		DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH,
		"Terminal", &g_pFont);

	// �}�E�X������
	InitMouse(hInstance,hWnd);

	// ���C�g
	InitLight();

	// �J����
	InitCamera();

	// �e
	InitShadow();

	// ���b�V���̕`��
	InitMeshField();

	// �G�f�B�^�[
	InitMapEdit();

	// �u���b�N
	InitBlock();

	// �v���C���[
	InitPlayer();

	// ====== ImGui ������ ======
	IMGUI_CHECKVERSION(); // ImGui �̃o�[�W�����`�F�b�N
	ImGui::CreateContext(); // ImGui �R���e�L�X�g�쐬
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // �L�[�{�[�h���͗L����
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // �Q�[���p�b�h���͗L����

	ImGui::StyleColorsLight(); // �e�[�}�K�p

	// ImGui �̃o�b�N�G���h�������iWin32 & DirectX9�j
	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX9_Init(g_pD3DDevice);

	return S_OK; // ���ʂ�Ԃ�

}
//==================
// �I������
//==================
void Uninit(void)
{
	//==================
	// �e��I������
	//==================

	// �L�[�{�[�h
	UninitKeyboard();

	// �}�E�X
	UninitMouse();

	// �p�b�h
	UninitJoypad();

	// ���C�g
	UninitLight();

	// �e
	UninitShadow();

	// ���b�V���̏I��
	UninitMeshField();

	// �G�f�B�^�[
	UninitMapEdit();

	// �u���b�N
	UninitBlock();

	// �v���C���[
	UninitPlayer();

	// �e�N�X�`���̔j��
	if (g_pModelIconTexture)
	{
		g_pModelIconTexture->Release();
		g_pModelIconTexture = nullptr;
	}

	// �f�o�b�O�\���p�̃t�H���g�̔j��
	if (g_pFont != NULL)
	{
		g_pFont->Release();
		g_pFont = NULL;
	}

	// Direct3D�f�o�C�X�̔j��
	if (g_pD3DDevice != NULL)
	{
		g_pD3DDevice->Release();
		g_pD3DDevice = NULL;
	}

	// Direct3D�I�u�W�F�N�g�̔j��
	if (g_pD3D != NULL)
	{
		g_pD3D->Release();
		g_pD3D = NULL;
	}

	// Imgui�̔j��
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
//===================
// �X�V����
//===================
void Update(void)
{
	//===========================
	// �e��I�u�W�F�N�g�X�V����
	//===========================
	// �L�[�{�[�h�̍X�V����
	UpdateKeyboard();

	// �}�E�X�̍X�V����
	UpdateMouse();

	// �Q�[���p�b�h�̍X�V����
	UpdateJoypad();

	// �J�����̍X�V
	UpdateCamera();

	// �e�̍X�V����
	UpdateShadow();

	// �u���b�N�̍X�V����
	UpdateBlock();

	if (g_mode == MODE_PLAY)
	{// �v���C���̂�
		// �v���C���[�̍X�V
		UpdatePlayer();
	}

	if (KeyboardTrigger(DIK_F3))
	{// F3�L�[
		// ���C���[�t���[���N��
		onWireFrame();
	}
	else if (KeyboardTrigger(DIK_F4))
	{// F4�L�[
		// ���C���[�t���[������
		offWireFrame();
	}

	if (g_mode == MODE_EDIT)
	{// �ҏW���[�h�̎�
		// �X�V
		UpdateMapEdit();
	}
}
//===================
// �`�揈��
//===================
void Draw(void)
{
	// �J�����擾
	Camera* pCamera = GetCamera();

	// ��ʃN���A(�o�b�N�o�b�t�@&Z�o�b�t�@�̃N���A)
	g_pD3DDevice->Clear(0,
		NULL,
		(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER),
		D3DCOLOR_RGBA(0, 0, 0, 0), 1.0f, 0);

	// �`��J�n
	if (SUCCEEDED(g_pD3DDevice->BeginScene()))
	{
		// �`�搬����
		//================
		// �`�揈��
		//================
			
		// �J�������Z�b�g
		SetCamera();

		// ���b�V���̕`��
		DrawMeshField();


		if (g_mode == MODE_EDIT)
		{
			// �G�f�B�^�[���
			DrawMapEdit();
		}

		if (g_mode == MODE_PLAY)
		{
			// �v���C���[�̕`��
			DrawPlayer();

			// �e�̕`��
			DrawShadow();

			// �u���b�N�̕`��
			DrawBlock();

			// �v���C���[���W�̕`��
			DrawPlayerPos();
		}

		//===========================
		// �e��t�H���g�̕`��
		//===========================
		// �L���v�V����
		//DrawFPS();

		//// �J�����\��
		//DrawCameraPos();

		//// �e��L�[�̎��
		// DrawEditkey();

		//// �z�u���̕\��
		//DrawNumBlock();

		//// ���[�h�ؑ֕\��
		//DrawModeChange();

		//// �e�L�X�g�ǂݍ��ݏ��̕\��
		//DrawEditMove();

		//// �G�f�B�^�[���f�����̕\��
		//DebugEditModelInfo();
		
		// Gui�E�B���h�E
		ImguiDrawData();

		// �`��I��
		g_pD3DDevice->EndScene();
	}

	// �o�b�N�o�b�t�@�ƃt�����g�o�b�t�@�̓���ւ�
	g_pD3DDevice->Present(NULL, NULL, NULL, NULL);

}
//======================
// FPS�̕\��
//======================
void DrawFPS(void)
{
	RECT rect = { 0,0,SCREEN_WIDTH,SCREEN_HEIGHT };
	char aString[256];

	//������ɑ��
	wsprintf(&aString[0], "//************************\n// �V���[�g�J�b�g�L�[���\n//************************\n");

	//�e�L�X�g�̕`��
	g_pFont->DrawText(NULL, &aString[0], -1, &rect, DT_LEFT, D3DCOLOR_RGBA(255, 255, 255, 255));
}
//==============================
// �G�f�B�b�g��ʂ̕\��
//==============================
void DrawEditkey(void)
{
	// ���[�J���ϐ�
	RECT rect5 = { 0,120,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT rect6 = { 0,60,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT rect7 = { 0,80,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT rect8 = { 0,100,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT rect10 = { 0,40,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT rect11 = { 0,140,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT rect12 = { 0,160,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT rect13 = { 0,180,SCREEN_WIDTH,SCREEN_HEIGHT };

	// ������
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

	// �t�@�C���p�X���擾
	Filenamepass = Filepass();

	int nModel = ReturnEdit(); // �z�u�J�E���g
	MAPMODELINFO* pEdit = MapInfo(); // �z�u���̏��

	switch (Filenamepass)
	{
	case 0:
		strcpy(aStFile, "data/stage000.bin"); // �����t�@�C��
		break;

	case 1:
		strcpy(aStFile, "data/stage001.bin"); // 2�Ԗڂ̃t�@�C��
		break;

	case 2:
		strcpy(aStFile, "data/stage002.bin"); // 3�Ԗڂ̃t�@�C��
		break;

	default:
		break;
	}

	// ������ɑ��
	if (pEdit[nModel].mapedit.bCollision)
	{// �L���Ȃ�Ȃ�
		strcpy(aStColl, "�L��");
	}
	else if (!pEdit[nModel].mapedit.bCollision)
	{// �����Ȃ�
		strcpy(aStColl, "����");
	}


	// �������ݒ�
	wsprintf(&aString5[0], "[ F7 ] : �t�@�C�������o��  ***< %s >*** \n",&aStFile[0]);
	wsprintf(&aString6[0], "[ F1 ] / [ F2 ] : ���[�h�؂�ւ� \n");
	wsprintf(&aString7[0], "[ F3 ] / [ F4 ] : ���C���[�t���[���؂�ւ�\n");
	wsprintf(&aString8[0], "[ F5 ] / [ F6 ] : �����蔻��`��؂�ւ� ( �v���C���[�h�� )\n");
	wsprintf(&aString10[0]," CollisionBlock����  [ %s ]\n",&aStColl[0]);
	wsprintf(&aString11[0],"[ F8 ] : �����o���t�@�C���p�X��؂�ւ� \n");
	wsprintf(&aString12[0],"[ F9 ] : �z�u���̍ēǂݍ��� \n");
	wsprintf(&aString13[0],"[ F10 ] / [ F11 ] : ��������؂�ւ� ( �v���C���[�h�� ) \n");

	// �t�H���g�̕\��
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
// �J�����\��
//========================
void DrawCameraPos(void)
{
	// ���[�J���ϐ�
	RECT rect = { 0,600,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT rect1 = { 0,620,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT rect2 = { 0,520,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT rect3 = { 0,580,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT rect4 = { 0,640,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT rect5 = { 0,660,SCREEN_WIDTH,SCREEN_HEIGHT };

	// ������
	char aString[256];
	char aString1[256];
	char aString2[256];
	char aString3[256];
	char aString4[64];
	char aString5[64];

	// �J�����̎擾
	Camera* pCamera = GetCamera();

	// ������ɑ��
	wsprintf(&aString2[0], "//************************\n// �J�������\n//************************\n");
	sprintf(&aString[0],   "[ �J������rot.y ]: < %.2f >\n", pCamera->rot.y);
	sprintf(&aString1[0],  "[ �J������rot.x ]: < %.2f >\n", pCamera->rot.x);
	wsprintf(&aString3[0], "[ ���_ / �����_ ] : < �}�E�X���� >\n");
	wsprintf(&aString4[0], "[ �Y�[�� ] :   < �}�E�X�z�C�[�� >\n");
	wsprintf(&aString5[0], "[ ���Z�b�g ] : < �}�E�X�z�C�[���������� >\n");

	// �e�L�X�g�`��
	g_pFont->DrawText(NULL, &aString[0], -1, &rect, DT_LEFT, D3DCOLOR_RGBA(255, 255, 255, 255));
	g_pFont->DrawText(NULL, &aString1[0], -1, &rect1, DT_LEFT, D3DCOLOR_RGBA(255, 255, 255, 255));
	g_pFont->DrawText(NULL, &aString2[0], -1, &rect2, DT_LEFT, D3DCOLOR_RGBA(255, 255, 255, 255));
	g_pFont->DrawText(NULL, &aString3[0], -1, &rect3, DT_LEFT, D3DCOLOR_RGBA(255, 255, 255, 255));
	g_pFont->DrawText(NULL, &aString4[0], -1, &rect4, DT_LEFT, D3DCOLOR_RGBA(255, 255, 255, 255));
	g_pFont->DrawText(NULL, &aString5[0], -1, &rect5, DT_LEFT, D3DCOLOR_RGBA(255, 255, 255, 255));

}
//============================
// �G�f�B�^�[���f���֌W�̕\��
//============================
void DebugEditModelInfo()
{
	// �擾�֌W
	int nModel = ReturnEdit(); // �z�u�J�E���g
	MAPMODELINFO* pEdit = MapInfo(); // �z�u���̏��
	EDITMODEL* pModelEdit = GetBlockInfo(pEdit[nModel].mapedit.nType); // ���f�����

	// ���[�J���ϐ�
	RECT rect = { 0,220,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT rect1 = { 0,320,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT rect2 = { 0,340,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT rect3 = { 0,360,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT rect4 = { 0,400,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT rect5 = { 0,420,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT rect6 = { 0,440,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT rect7 = { 0,480,SCREEN_WIDTH,SCREEN_HEIGHT };

	// ������
	char aString[256];
	char aString1[256];
	char aString2[256];
	char aString3[256];
	char aString4[256];
	char aString5[256];
	char aString6[256];
	char aString7[256];

	// ������ɑ��
	wsprintf(&aString[0], "//************************\n// �z�u���郂�f�����\n//************************\n");
	sprintf(&aString1[0], "[ ���f����X���W ] : < %.2f >\n", pEdit[nModel].mapedit.pos.x);
	sprintf(&aString2[0], "[ ���f����Y���W ] : < %.2f >\n", pEdit[nModel].mapedit.pos.y);
	sprintf(&aString3[0], "[ ���f����Z���W ] : < %.2f >\n", pEdit[nModel].mapedit.pos.z);
	sprintf(&aString4[0], "[ ���f����X�̊g�嗦 ] : < %.2f > \n", pEdit[nModel].mapedit.Scal.x);
	sprintf(&aString5[0], "[ ���f����Y�̊g�嗦 ] : < %.2f >\n", pEdit[nModel].mapedit.Scal.y);
	sprintf(&aString6[0], "[ ���f����Z�̊g�嗦 ] : < %.2f >\n", pEdit[nModel].mapedit.Scal.z);
	sprintf(&aString7[0], "[ �I�𒆂̃��f���̎�� ] : < %s > \n", pModelEdit->FileName);

	// �e�L�X�g�`��
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
// ���݂̔z�u�ԍ���\��
//============================
void DrawNumBlock()
{
	// �擾
	nBlockSet = ReturnEdit();

	// ���[�J���ϐ�
	RECT rect = { 0,280,SCREEN_WIDTH,SCREEN_HEIGHT };
	char aString[256];

	// ������ɑ��
	wsprintf(&aString[0], "[ �z�u�������f���� ] < %d / 256 >\n", nBlockSet);

	// �e�L�X�g�`��
	g_pFont->DrawText(NULL, &aString[0], -1, &rect, DT_LEFT, D3DCOLOR_RGBA(255, 255, 255, 255));
}
//==============================
// ���[�h�ؑփt�H���g�̕\��
//==============================
void DrawModeChange()
{
	// �擾
	MODE nMode = GetMode();

	// ���[�J���ϐ�
	RECT rect = { 0,0,SCREEN_WIDTH,SCREEN_HEIGHT };

	char aString[256];
	char aString1[256];

	if (nMode == MODE_EDIT)
	{// �ҏW���[�h�Ȃ�
		strcpy(aString1, "�ҏW���[�h");
	}
	else if (nMode == MODE_PLAY)
	{// �v���C���[�h�Ȃ�
		strcpy(aString1, "�v���C���[�h");
	}

	// ������ɑ��
	wsprintf(&aString[0], "���݂̃��[�h < %s >\n", &aString1[0]);

	// �t�H���g�̕\��
	g_pFont->DrawText(NULL, &aString[0], -1, &rect, DT_RIGHT, D3DCOLOR_RGBA(255, 255, 255, 255));

}
//============================
// �ǂݍ��݊֌W�̕\��
//============================
void DrawEditMove()
{
	// �f�[�^�̎擾
	float fspeed = ReturnSpeed();
	int nType = ReturnType();
	float fjumpmove = ReturnJump();

	// ���[�J���ϐ�
	RECT rect = { 0,100,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT rect1 = { 0,120,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT rect2 = { 0,140,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT rect3 = { 0,160,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT rect4 = { 0,80,SCREEN_WIDTH,SCREEN_HEIGHT };

	// ������
	char aString[128];
	char aString1[128];
	char aString2[128];
	char aString3[128];
	char aString4[128];

	// ������ɑ��
	sprintf(&aString[0], "[ �z�u���̈ړ��� ] : <   %.2f  >\n", fspeed);
	sprintf(&aString1[0], "[ �ǂݍ��ގ�ސ� ] : < %d / 256 >\n", nType);
	wsprintf(&aString2[0], "[ ���[�V���� ] : < motion.txt >\n");
	sprintf(&aString3[0], "[ �W�����v�̐��l ] : <   %.2f  >\n", fjumpmove);
	wsprintf(&aString4[0], "----- �ǂݍ��ݏ�� [ model.txt ] -----\n");


	// �e�L�X�g�`��
	g_pFont->DrawText(NULL, &aString[0], -1, &rect, DT_RIGHT, D3DCOLOR_RGBA(255, 255, 255, 255));
	g_pFont->DrawText(NULL, &aString1[0], -1, &rect1, DT_RIGHT, D3DCOLOR_RGBA(255, 255, 255, 255));
	g_pFont->DrawText(NULL, &aString2[0], -1, &rect2, DT_RIGHT, D3DCOLOR_RGBA(255, 255, 255, 255));
	g_pFont->DrawText(NULL, &aString3[0], -1, &rect3, DT_RIGHT, D3DCOLOR_RGBA(255, 255, 255, 255));
	g_pFont->DrawText(NULL, &aString4[0], -1, &rect4, DT_RIGHT, D3DCOLOR_RGBA(255, 255, 255, 255));

}
//==============================
// �v���C���[���W�\��
//==============================
void DrawPlayerPos()
{
	// �v���C���[���擾
	PLAYER* pPlayer = GetPlayer();

	// ���[�J���ϐ�
	RECT rect = { 0,200,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT rect1 = { 0,220,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT rect2 = { 0,240,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT rect3 = { 0,260,SCREEN_WIDTH,SCREEN_HEIGHT };

	// ������
	char aString[128];
	char aString1[128];
	char aString2[128];
	char aString3[128];

	// ������ɑ��
	wsprintf(&aString[0], "------ [ �v���C���[��� ] ------\n");
	sprintf(&aString1[0], "�v���C���[pos.x  : <  %.2f  >\n", pPlayer->pos.x);
	sprintf(&aString2[0], "�v���C���[pos.y  : <   %.2f  >\n", pPlayer->pos.y);
	sprintf(&aString3[0], "�v���C���[pos.z  : <  %.2f  >\n", pPlayer->pos.z);

	// �e�L�X�g�`��
	g_pFont->DrawText(NULL, &aString[0], -1, &rect, DT_RIGHT, D3DCOLOR_RGBA(255, 255, 255, 255));
	g_pFont->DrawText(NULL, &aString1[0], -1, &rect1, DT_RIGHT, D3DCOLOR_RGBA(255, 255, 255, 255));
	g_pFont->DrawText(NULL, &aString2[0], -1, &rect2, DT_RIGHT, D3DCOLOR_RGBA(255, 255, 255, 255));
	g_pFont->DrawText(NULL, &aString3[0], -1, &rect3, DT_RIGHT, D3DCOLOR_RGBA(255, 255, 255, 255));

}
//======================
// �f�o�C�X�̎擾
//======================
LPDIRECT3DDEVICE9 GetDevice(void)
{
	return g_pD3DDevice;
}
//====================
// ���[�h�̎擾
//====================
MODE GetMode(void)
{
	return g_mode; // ���݂̃��[�h��Ԃ�
}
//=====================
// ���C���[�t���[��ON
//=====================
void onWireFrame()
{
	g_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
}
//=====================
// ���C���[�t���[��OFF
//=====================
void offWireFrame() 
{
	g_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
}
//=================================================
// �E�B���h�E���t���X�N���[���ɕς��鏈��
//=================================================
void ToggleFullscreen(HWND hWnd)
{
	// ���݂̃E�B���h�E�X�^�C�����擾
	DWORD dwStyle = GetWindowLong(hWnd, GWL_STYLE);

	if (g_isFullscreen)
	{
		// �E�B���h�E���[�h�ɐ؂�ւ�
		SetWindowLong(hWnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPos(hWnd, HWND_TOP, g_windowRect.left, g_windowRect.top,
			g_windowRect.right - g_windowRect.left, g_windowRect.bottom - g_windowRect.top,
			SWP_FRAMECHANGED | SWP_NOACTIVATE);
		ShowWindow(hWnd, SW_NORMAL);
	}
	else
	{
		// �t���X�N���[�����[�h�ɐ؂�ւ�
		GetWindowRect(hWnd, &g_windowRect);
		SetWindowLong(hWnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
		SetWindowPos(hWnd, HWND_TOP, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
			SWP_FRAMECHANGED | SWP_NOACTIVATE);
		ShowWindow(hWnd, SW_MAXIMIZE);
	}

	g_isFullscreen = !g_isFullscreen;
}

//==========================================
// imgui�E�B���h�E�̐�������
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
// imgui�E�B���h�E�̏I������
//==========================================
void CleanupDeviceD3D()
{
	if (g_pD3DDevice) { g_pD3DDevice->Release(); g_pD3DDevice = nullptr; }
	if (g_pD3D) { g_pD3D->Release(); g_pD3D = nullptr; }
}
//==========================================
// imgui�E�B���h�E�̍ď���������
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
// imgui�E�B���h�E�̕`�揈��
//==========================================
void ImguiDrawData()
{
	//================================
	//  ���擾�֌W
	//================================
	PLAYER* pPlayer = GetPlayer(); // �v���C���[
	MODE nMode = GetMode(); // ���݂̃��[�h

	float fspeed = ReturnSpeed(); // �z�u���x
	int nType = ReturnType(); // ��ސ�
	int nModel = ReturnEdit(); // �z�u�J�E���g

	MAPMODELINFO* pEdit = MapInfo(); // �z�u���̏��
	EDITMODEL* pModelEdit = GetBlockInfo(pEdit[nModel].mapedit.nType); // ���f�����
	Camera* pCamera = GetCamera(); // �J����
	Filenamepass = Filepass(); // �t�@�C���p�X���擾

	// ������
	char aStFile[256];

	// �t�@�C���p�X
	switch (Filenamepass)
	{
	case 0:
		strcpy(aStFile, "data/stage000.bin"); // �����t�@�C��
		break;
	case 1:
		strcpy(aStFile, "data/stage001.bin"); // 2�Ԗڂ̃t�@�C��
		break;
	case 2:
		strcpy(aStFile, "data/stage002.bin"); // 3�Ԗڂ̃t�@�C��
		break;
	default:
		break;
	}

	// �t���[���J�n
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	//==============================
	// ���C���� ImGui �E�B���h�E
	//==============================
	// �T�C�Y,�ꏊ�Œ�
	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(300, 720), ImGuiCond_Always); 

	// �V�[���J�n
	ImGui::Begin("Debug & Mode Selector", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

	// FPS �\��
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
		1000.0f / (float)g_nCountFPS, (float)g_nCountFPS);

	// ���[�h�I��
	if (ImGui::CollapsingHeader("Mode Selector"))
	{
		bool isEditMode = (g_mode == MODE_EDIT);

		if (ImGui::Checkbox("Edit Mode", &isEditMode))
		{

			g_mode = isEditMode ? MODE_EDIT : MODE_PLAY;

			if (g_mode == MODE_PLAY)
			{
				LoadBlock(); // ���f���ǂݍ���
			}
		}

		// LoadModelIcon(g_pD3DDevice, "data/TEXTURE/gold.jpg");
		ImGui::Text("Now Mode: %s", (g_mode == MODE_EDIT) ? "EDIT" : "PLAY");
	}

	// ���f�����
	if (ImGui::CollapsingHeader("Model Info"))
	{
		//static char aSt[128] = "";

		//ImGui::InputText("Enter text", aSt, sizeof(aSt));

		//if (ImGui::Button("Save to File")) 
		//{// �����o��
		//	SaveToFile(aSt);
		//}		

		//// ���f���̃T���l�C����\��
		//if (g_pModelIconTexture)
		//{
		//	ImGui::Text("Model Icon:");
		//	ImGui::Image((ImTextureID)g_pModelIconTexture, ImVec2(64, 64)); // 64x64�s�N�Z���ŕ`��
		//}

		ImGui::Text("SetBlock Num: %d / 256", ReturnEdit()); // �z�u��
		ImGui::Text("FilePass: %s", aStFile); // �t�@�C���p�X

		// ���f���̈ʒu��ύX�\�ɂ���
		ImGui::Text("Model Position"); // ���W

		if (ImGui::InputFloat("Pos X", &pEdit[nModel].mapedit.pos.x, 20.0f, 1000.0f, "%.2f"))
		{
			UpdateModelPosition(nModel, pEdit[nModel].mapedit.pos); // ���f���̈ʒu�X�V�֐����Ă�
		}
		if (ImGui::InputFloat("Pos Y", &pEdit[nModel].mapedit.pos.y, 20.0f, 1000.0f, "%.2f"))
		{
			UpdateModelPosition(nModel, pEdit[nModel].mapedit.pos); // ���f���̈ʒu�X�V�֐����Ă�

		}
		if (ImGui::InputFloat("Pos Z", &pEdit[nModel].mapedit.pos.z, 20.0f, 1000.0f, "%.2f"))
		{
			UpdateModelPosition(nModel, pEdit[nModel].mapedit.pos); // ���f���̈ʒu�X�V�֐����Ă�

		}

		// ���f���̃X�P�[����ύX�\�ɂ���
		ImGui::Text("Model Scale");

		if (ImGui::InputFloat("Scale X", &pEdit[nModel].mapedit.Scal.x, 0.1f, 2.0f, "%.2f"))
		{
			UpdateModelScale(nModel, pEdit[nModel].mapedit.Scal); // �g�嗦�ύX
		}
		if (ImGui::InputFloat("Scale Y", &pEdit[nModel].mapedit.Scal.y, 0.1f, 2.0f, "%.2f"))
		{
			UpdateModelScale(nModel, pEdit[nModel].mapedit.Scal); // �g�嗦�ύX

		}
		if (ImGui::InputFloat("Scale Z", &pEdit[nModel].mapedit.Scal.z, 0.1f, 2.0f, "%.2f"))
		{
			UpdateModelScale(nModel, pEdit[nModel].mapedit.Scal); // �g�嗦�ύX

		}
	}

	// ���
	if (ImGui::CollapsingHeader("Block Info"))
	{
		ImGui::Text("AllBlockType: %d / 256", ReturnType());
	}

	ImGui::End();

	// �����_�����O����
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}
//=======================================
// ���W�X�V�p�֐�
//=======================================
void UpdateModelPosition(int modelIndex, D3DXVECTOR3 newPos)
{
	MAPMODELINFO* pEdit = MapInfo();

	// ���W��������
	pEdit[modelIndex].mapedit.pos = newPos;

	// �������f���̃��[���h�}�g���N�X���X�V����K�v������ꍇ
	UpdateModelMatrix(modelIndex);
}
//=======================================
// �g�嗦�X�V�p�֐�
//=======================================
void UpdateModelScale(int modelIndex, D3DXVECTOR3 newScale)
{
	MAPMODELINFO* pEdit = MapInfo();

	// �g�嗦����
	pEdit[modelIndex].mapedit.Scal = newScale;

	// �g�嗦����
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

	// ���[���h�}�g���N�X�X�V
	UpdateModelMatrix(modelIndex);
}
//=======================================
// ���[���h�}�g���b�N�X�X�V�p�֐�
//=======================================
void UpdateModelMatrix(int modelIndex)
{
	MAPMODELINFO* pEdit = MapInfo();

	// D3DXMatrixTransformation ���g���ă��[���h�}�g���N�X���X�V
	D3DXMATRIX matScale, matTranslate, matWorld;

	// �g�嗦
	D3DXMatrixScaling(&matScale,
		pEdit[modelIndex].mapedit.Scal.x,
		pEdit[modelIndex].mapedit.Scal.y,
		pEdit[modelIndex].mapedit.Scal.z);

	// �ʒu
	D3DXMatrixTranslation(&matTranslate,
		pEdit[modelIndex].mapedit.pos.x,
		pEdit[modelIndex].mapedit.pos.y,
		pEdit[modelIndex].mapedit.pos.z);

	// ���[���h�}�g���N�X�̍X�V
	matWorld = matScale * matTranslate;

	// �C���f�b�N�X�ԍ��̃��[���h�}�g���b�N�X
	pEdit[modelIndex].mapedit.mtxWorld = matWorld;
}
//==============================
// ���f���A�C�R����ǂݍ��ފ֐�
//==============================
void LoadModelIcon(LPDIRECT3DDEVICE9 pDevice, const char* filePath)
{
	// �e�N�X�`���̓ǂݍ���
	if (FAILED(D3DXCreateTextureFromFile(pDevice, filePath, &g_pModelIconTexture)))
	{
		MessageBoxA(0, "���f���A�C�R���̓ǂݍ��݂Ɏ��s���܂���", "Error", MB_OK);
	}
}
void SaveToFile(const char* text) 
{
	FILE* file = fopen("data\\test.txt", "w"); // "w" ���[�h�ŏ�������
	if (file)
	{
		fprintf(file, "%s", text);
		fclose(file);
	}
}
