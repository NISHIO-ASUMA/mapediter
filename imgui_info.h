//=================================================
//
// ImGui�̏��ݒ菈���֐� [ imgui_info.h ]
// Author : Asuma Nishio
//
//==================================================
#ifndef _IMGUI_INFO_H_
#define _IMGUI_INFO_H_

//*******************************************
// �C���N���[�h�t�@�C���錾
//*******************************************
#include "main.h"

//*******************************************
// IMGUI�̎��
//*******************************************
typedef enum
{
	IMGUITYPE_NONE = 0, // �������Ȃ�
	IMGUITYPE_NOMOVE,	// �ړ������Ȃ�
	IMGUITYPE_NOMOVESIZE, // �T�C�Y���g��k�������Ȃ�
	IMGUITYPE_NOMOVEANDSIZE, // �T�C�Y,�ړ��Ƃ��ɂ����Ȃ�
	IMGUITYPE_MAX
}IMGUITYPE;

//*******************************************
// �O���[�o���ϐ��錾
//*******************************************
void InitImguiInfo(HWND hWnd,LPDIRECT3DDEVICE9 pDevice); // ����������
void UninitImguiInfo();	// �I������
void NewFrameImGui(); // �J�n�t���[��
void StartImgui(const char* ImguiName, IMGUITYPE nType);    // �J�n
void EndImgui();      // Gui�u���b�N�I���t���[��
void EndImguiFrame(); // �I���t���[��
void SetPosImgui(float PosX, float PosY); // ���W�ݒ�
void SetSizeImgui(float SizeX, float SizeY); // �傫��
void DrawImguiInfo();	// �`�揈��

void InitMallocData(); // �}���b�N�֐�
void UninitMallocData(); // �I���֐� 
void UpdatemallocData(int nModel); // �X�V�֐�


#endif // !_IMGUI_INFO_H_
