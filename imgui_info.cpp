//=================================================
//
// ImGui�̏��ݒ菈���֐� [ imgui_info.cpp ]
// Author : Asuma Nishio
//
//==================================================

//*******************************************
// �C���N���[�h�t�@�C���錾
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
// �O���[�o���ϐ��錾
//*******************************************

//===========================================
// Gui�̏������֐�
//===========================================
void InitImguiInfo(HWND hWnd, LPDIRECT3DDEVICE9 pDevice)
{
	// �������֐�
	InitMallocData();

	// ====== ImGui ������ ======
	IMGUI_CHECKVERSION(); // ImGui �̃o�[�W�����`�F�b�N
	ImGui::CreateContext(); // ImGui �R���e�L�X�g�쐬
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // �L�[�{�[�h���͗L����
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // �Q�[���p�b�h���͗L����

	ImGui::StyleColorsClassic(); // �e�[�}�K�p

	// ====== �t�H���g�ݒ� ======
	// �J�X�^���t�H���g�̒ǉ� (�t�H���g�p�X��ύX���Ă�������)
	ImFont* myFont = io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/consola.ttf", 15.0f);

	if (myFont == nullptr) 
	{// �t�H���g��NULL�Ȃ�
		printf("�t�H���g�̃��[�h�Ɏ��s���܂���\n");
	}

	// �t�H���g�����r���h
	ImGui::GetIO().Fonts->Build();

	// ImGui �̃o�b�N�G���h�������iWin32 & DirectX9�j
	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX9_Init(pDevice);
}
//===========================================
// Gui�̏I���֐�
//===========================================
void UninitImguiInfo()
{
	//=======================================
	// �h���b�v�_�E�����j���[�p�I������
	//========================================
	UninitMallocData();

	// Imgui�̔j��
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
//===========================================
// Gui�̍��W�ݒ�֐�
//===========================================
void SetPosImgui(float PosX,float PosY)
{
	ImGui::SetNextWindowPos(ImVec2(PosX, PosY), ImGuiCond_Always);
}
//===========================================
// Gui�̃T�C�Y�ݒ�֐�
//===========================================
void SetSizeImgui(float SizeX, float SizeY)
{
	ImGui::SetNextWindowSize(ImVec2(SizeX, SizeY), ImGuiCond_Always);
}
//===========================================
// Gui�̕`��֐�
//===========================================
void DrawImguiInfo()
{
	//================================
	//  ���擾�֌W
	//================================
	PLAYER* pPlayer = GetPlayer(); // �v���C���[
	MODE nMode = GetMode();	// ���݂̃��[�h�擾

	float fspeed = ReturnSpeed(); // �z�u���x
	int nType = ReturnType(); // ��ސ�
	int nModel = ReturnEdit(); // �z�u�J�E���g

	MAPMODELINFO* pEdit = MapInfo(); // �z�u���̏��
	EDITMODEL* pModelEdit = GetBlockInfo(pEdit[nModel].mapedit.nType); // ���f�����
	Camera* pCamera = GetCamera(); // �J����
	int Filenamepass = Filepass(); // �t�@�C���p�X���擾

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
	NewFrameImGui();

	if (nMode == MODE_EDIT)
	{
		//==============================
		// 1�ڂ� ImGui �E�B���h�E
		//==============================
		// �傫��,�T�C�Y�ݒ�
		SetPosImgui(0.0f, 110.0f);
		SetSizeImgui(250.0f, 560.0f);

		// �`��
		StartImgui("Edit_Info", IMGUITYPE_NOMOVEANDSIZE);

		// ���f�����
		if (ImGui::CollapsingHeader("Model Info"))
		{

			ImGui::Text("SetBlock Num: %d / 256", ReturnEdit()); // �z�u��
			ImGui::Text("FilePass: %s", aStFile); // �t�@�C���p�X

			// ���f���̈ʒu��ύX�\�ɂ���
			ImGui::Text("Model Position"); // ���W

			ImGui::SetNextItemWidth(100.0f);

			if (ImGui::InputFloat("Pos X", &pEdit[nModel].mapedit.pos.x, 20.0f, 1000.0f, "%.2f"))
			{
				UpdateModelPosition(nModel, pEdit[nModel].mapedit.pos); // ���f���̈ʒu�X�V�֐����Ă�
			}

			ImGui::SetNextItemWidth(100.0f);

			if (ImGui::InputFloat("Pos Y", &pEdit[nModel].mapedit.pos.y, 20.0f, 1000.0f, "%.2f"))
			{
				UpdateModelPosition(nModel, pEdit[nModel].mapedit.pos); // ���f���̈ʒu�X�V�֐����Ă�

			}

			ImGui::SetNextItemWidth(100.0f);

			if (ImGui::InputFloat("Pos Z", &pEdit[nModel].mapedit.pos.z, 20.0f, 1000.0f, "%.2f"))
			{
				UpdateModelPosition(nModel, pEdit[nModel].mapedit.pos); // ���f���̈ʒu�X�V�֐����Ă�
			}

			// ���f���̃X�P�[����ύX�\�ɂ���
			ImGui::Text("Model Scale");

			ImGui::SetNextItemWidth(100.0f);

			if (ImGui::InputFloat("Scale X", &pEdit[nModel].mapedit.Scal.x, 0.1f, 2.0f, "%.2f"))
			{
				UpdateModelScale(nModel, pEdit[nModel].mapedit.Scal); // �g�嗦�ύX
			}

			ImGui::SetNextItemWidth(100.0f);

			if (ImGui::InputFloat("Scale Y", &pEdit[nModel].mapedit.Scal.y, 0.1f, 2.0f, "%.2f"))
			{
				UpdateModelScale(nModel, pEdit[nModel].mapedit.Scal); // �g�嗦�ύX

			}

			ImGui::SetNextItemWidth(100.0f);

			if (ImGui::InputFloat("Scale Z", &pEdit[nModel].mapedit.Scal.z, 0.1f, 2.0f, "%.2f"))
			{
				UpdateModelScale(nModel, pEdit[nModel].mapedit.Scal); // �g�嗦�ύX
			}

		}

		// �t�@�C�����
		if (ImGui::CollapsingHeader("Save & Load"))
		{
			static bool saveSuccess = false; // �����t���O
			static bool saveError = false;   // ���s�t���O
			static float saveMessageTimer = 0.0f; // ���b�Z�[�W�\���p�^�C�}�[
			static float saveMessageDuration = 2.0f; // 2�b�ԕ\��

			// �Ԃ��{�^��
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 0.6f)); // �ԐF

			if (ImGui::Button("Save File"))
			{

				if (SaveEdit())  // SaveEdit() �̖߂�l�Ő��ۂ𔻒f
				{
					saveSuccess = true;
					saveError = false;
					saveMessageTimer = ImGui::GetTime(); // ���݂̎��Ԃ��L�^
				}
				else
				{
					saveSuccess = false;
					saveError = true;
					saveMessageTimer = ImGui::GetTime(); // ���s�����L�^
				}
			}

			// �{�^���̐ݒ��߂�
			ImGui::PopStyleColor(1); // 1�̃J���[�ݒ�����Z�b�g

			// ���b�Z�[�W�̕\���i��莞�Ԍ�ɏ����j
			if (saveSuccess)
			{
				if (ImGui::GetTime() - saveMessageTimer < saveMessageDuration)
				{
					// �����o������
					ImGui::TextColored(ImVec4(0, 1, 0, 1), "Save Successful!");
				}
				else
				{
					saveSuccess = false; // ���Ԃ��o�߂������\��
				}
			}
			else if (saveError)
			{
				if (ImGui::GetTime() - saveMessageTimer < saveMessageDuration)
				{
					// �����o�����s
					ImGui::TextColored(ImVec4(1, 0, 0, 1), "Save Failed!");
				}
				else
				{
					saveError = false; // ���Ԃ��o�߂������\��
				}
			}

			// �����[�h���ۂ�
			static bool isReload = false;
			static bool isload = isReload;

			if (ImGui::Button("ReloadFile") && !isReload)
			{
				// �ēǂݍ��ݏ���
				ReloadEdit();
				isReload = true; // �t���O��true
			}

		}

		// �I���֐�
		ImGui::End();
		

		//==============================
		// 2�ڂ� ImGui �E�B���h�E
		//==============================

		// ���[�h�ؑփf�[�^
		ImguiDrawData();

		//==============================
		// 3�ڂ� ImGui �E�B���h�E
		//==============================
		// �傫��,�T�C�Y�ݒ�
		SetPosImgui(1000.0f, 0.0f);
		SetSizeImgui(280.0f, 200.0f);

		// �C���X�y�N�^�[�̕`��
		StartImgui("Inspector", IMGUITYPE_NOMOVEANDSIZE);

		if (ImGui::CollapsingHeader("Player_Info"))
		{
			// �v���C���[���
			ImGui::Text("POS : %.2f,%.2f,%.2f", pPlayer->pos.x, pPlayer->pos.y, pPlayer->pos.z); // ���݂̍��W
			ImGui::Text("ROT : %.2f,%.2f,%.2f", pPlayer->rot.x, pPlayer->rot.y, pPlayer->rot.z); // ���݂̊p�x
		}

		// 3�ڂ̃E�B���h�E�I��
		ImGui::End();

		// �����_�����O����
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
	}
}
//===========================================
// Gui�̊J�n�֐�
//===========================================
void StartImgui(const char* ImguiName, IMGUITYPE nType)
{
	if (ImguiName != NULL)
	{
		switch (nType)
		{
		case IMGUITYPE_NONE:
			// �V���v���X�^�[�g
			ImGui::Begin(ImguiName);
			break;
		case IMGUITYPE_NOMOVE:
			// �ړ������Ȃ���
			ImGui::Begin(ImguiName, NULL, ImGuiWindowFlags_NoMove);
			break;
		case IMGUITYPE_NOMOVESIZE:
			// �g�傳���Ȃ���
			ImGui::Begin(ImguiName, NULL, ImGuiWindowFlags_NoResize);
			break;
		case IMGUITYPE_NOMOVEANDSIZE:
			// �ړ����g��������Ȃ�
			ImGui::Begin(ImguiName, NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
			break;
		default:
			break;
		}
	}

}
//===========================================
// Gui�̃t���[���J�n�֐�
//===========================================
void NewFrameImGui()
{
	// �t���[���J�n
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}
//===========================================
// Gui�̃u���b�N�I���֐�
//===========================================
void EndImgui()
{
	// �I���֐�
	ImGui::End();

	// �����_�����O����
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}
//===========================================
// Gui�̃t���[���I���֐�
//===========================================
void EndImguiFrame()
{
	// �t���[���I��
	ImGui::EndFrame();
}
//===========================================
// �������̓��I�m�ۏ����������֐�
//===========================================
void InitMallocData()
{
	//// ��ސ�
	//g_nTypeNum = ReturnType();
	//int nData = g_nTypeNum;

	//// �������̊m�ۗp�ϐ�
	//items = (char**)malloc(nData * sizeof(char*));

	//// �z��ɑ��
	//items[0] = strdup("block000.x");
	//items[1] = strdup("block001.x");
	//items[2] = strdup("block002.x");
	//items[3] = strdup("body.x");
}
//===========================================n
// �������̓��I�m�ۏ����I���֐�
//===========================================
void UninitMallocData()
{
	//// �������̉�� (���ꂵ�Ȃ��ƃ��o���I�I�I���������[�N�ɂȂ��Ă��܂�...)
	//for (int nCnt = 0; nCnt < g_n; nCnt++)
	//{
	//	free(items[nCnt]);
	//}
	//free(items);
}
//===========================================
// �������̓��I�m�ۏ����X�V�֐�
//===========================================
void UpdatemallocData(int nModel)
{
	//// �擾
	//MAPMODELINFO* pEdit = MapInfo(); // �z�u���̏��

	//// ���ݑI������Ă��郂�f���̎��
	//int& currentItem = pEdit[nModel].mapedit.nType;

	//if (ImGui::Combo("Type", &currentItem, (const char**)items, g_n))
	//{
	//	// �����ɁA�I�����ꂽ�Ƃ��̏�����������I
	//	printf("Selected: %s\n", items[currentItem]);
	//}
	
}
