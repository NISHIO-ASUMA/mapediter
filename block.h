//==========================
//
// �u���b�N���� [block.h]
// Author: Asuma Nishio
//
//==========================
#ifndef _BLOCK_H_
#define _BLOCK_H_

//***************************
// �C���N���[�h�t�@�C���錾
//***************************
#include "main.h"
#include "mapedit.h"

//********************************
// �u���b�N�̃��f���̍\���̐錾
//********************************
typedef struct
{
	LPD3DXMESH pMesh;		// ���b�V���̃|�C���^
	LPD3DXBUFFER pBuffMat;	// �}�e���A���̃|�C���^
	DWORD dwNumMat;			// �}�e���A����
	LPDIRECT3DTEXTURE9 pTexture[MAX_TEXTURE]; // �e�N�X�`���̃|�C���^
	D3DXVECTOR3 Vtxmax;		// �ő�l��ێ�����ϐ�
	D3DXVECTOR3 Vtxmin;		// �ŏ��l��ێ�����ϐ�
	D3DXVECTOR3 size;		// �T�C�Y���
	char FileName[64];      // ������
}BlockModel;

//***************************
// �u���b�N�̍\���̐錾
//***************************
typedef struct
{
	D3DXVECTOR3 pos; // ���W
	D3DXVECTOR3 rot; // �p�x
	D3DXMATRIX mtxWorld; // ���[���h�}�g���b�N�X
	D3DXVECTOR3 move;	// �ړ���
	int  nType;			// ���
	D3DXVECTOR3 Scal;	// �g�嗦
}Block;

//***********************************
// �u���b�N���f���ۑ��p�̍\���̐錾
//***********************************
typedef struct
{
	bool bUse;		// �g�p��Ԃ��ǂ���
	Block aBlock;   // �\���̏��
}BlockInfo;

//**********************
// �v���g�^�C�v�錾
//**********************
void InitBlock(void);		// �u���b�N����������
void UninitBlock(void);		// �u���b�N�I������
void UpdateBlock(void);		// �u���b�N�X�V����
void DrawBlock(void);		// �u���b�N�`�揈��
void SetBlock(D3DXVECTOR3 pos, int nType, D3DXVECTOR3 Scal); // �u���b�N�̐ݒ菈��
void CollisionBlock(D3DXVECTOR3 *pPos,D3DXVECTOR3 *pPosOld,D3DXVECTOR3 *pMove,D3DXVECTOR3 *pSize); // �����蔻��
BlockInfo* GetBlock();		// ���̎擾
void LoadBlock();			// �ǂݍ��ݏ���

#endif // !_BLOCK_H_