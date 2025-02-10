//===================================
//
// �}�b�v�G�f�B�^�[���� [mapedit.h]
// Author: Asuma Nishio
//
//===================================
#ifndef _MAPEDIT_H_
#define _MAPEDIT_H_

//****************************
// �C���N���[�h�t�@�C���錾
//****************************
#include "main.h"

//****************************
// �}�N����`
//****************************
#define MAX_EDITOBJ (256)	// �ő�z�u��
#define EDIT_MOVE   (20.0f) // �ړ���(�z�u��)
#define MAX_TEXTURE (256)   // �ő�e�N�X�`����

//*************************************
// �}�b�v�G�f�B�^�[���f���\���̐錾
//*************************************
typedef struct
{
	LPD3DXMESH pMesh;		// ���b�V���̃|�C���^
	LPD3DXBUFFER pBuffMat;	// �}�e���A���̃|�C���^
	DWORD dwNumMat;			// �}�e���A����
	LPDIRECT3DTEXTURE9 pTexture[MAX_TEXTURE]; // �e�N�X�`���̃|�C���^
	D3DXVECTOR3 Vtxmax;	    // �ő�l��ێ�����ϐ�
	D3DXVECTOR3 Vtxmin;	    // �ŏ��l��ێ�����ϐ�
	D3DXVECTOR3 size;	    // �T�C�Y���
	char FileName[64];      // ������
}EDITMODEL;

//****************************
// �}�b�v�G�f�B�^�[�\���̐錾
//****************************
typedef struct
{
	D3DXVECTOR3 pos; // ���W
	D3DXVECTOR3 rot; // �p�x
	D3DXMATRIX mtxWorld; // ���[���h�}�g���b�N�X
	D3DXVECTOR3 move;	 // �ړ���
	int nType;		     // ���
	D3DXVECTOR3 Scal;	 // �g�嗦
}MapEdit;

//***********************************
// �}�b�v�G�f�B�^�[�z�u�p�\���̐錾
//***********************************
typedef struct
{
	bool isputblock;     // �z�u�������ǂ���
	bool bUse;		     // �g�p��Ԃ��ǂ���
	MapEdit mapedit;	 // �\���̕ϐ�
}MAPMODELINFO;

//****************************
// �v���g�^�C�v�錾
//****************************
void InitMapEdit();		// ����������
void UninitMapEdit();	// �I������
void UpdateMapEdit();	// �X�V����
void DrawMapEdit();		// �`�揈��
void SaveEdit();		// �ۑ�����
void ReloadEdit();		// �ēǂݍ��ݏ���
void LoadXFile();		// �O������̓ǂݍ���
EDITMODEL* GetBlockInfo(int type); // �G�f�B�^�[���f�����擾
MapEdit* getMap();		// �\���̏��擾
int ReturnEdit();		// �z�u�����
float ReturnSpeed();	// �ړ�
int ReturnType();		// ��ސ�
float ReturnJump();		// �W�����v��
int Filepass();			// �t�@�C���p�X��Ԃ�
bool IsGetLoad();		// �����[�h������
MAPMODELINFO* MapInfo();// �z�u���̏󋵎擾

#endif