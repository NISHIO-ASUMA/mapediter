//==========================
//
// �e�̏��� [shadow.h]
// Author: Asuma Nishio
//
//==========================
#ifndef _SHADOW_H_
#define _SHADOW_H_

//***************************
// �C���N���[�h�t�@�C���錾
//***************************
#include "main.h"

//**************************
// �}�N����`
//**************************
#define MAX_SHADOW (10)			// �e�̍ő吔
#define SHADOW_SIZE (20.0f)			// �e�̃T�C�Y

//**********************
// �\���̐錾
//**********************
typedef struct
{
	D3DXVECTOR3 posShadow;		// �ʒu
	D3DXVECTOR3 rotShadow;		// ����
	D3DXMATRIX  mtxWorldShadow;	// ���[���h�}�g���b�N�X
	D3DXVECTOR3 Scal;			// �傫��
	bool bUse;					// �g�p����
}Shadow;

//**********************
// �v���g�^�C�v�錾
//**********************
void InitShadow(void);		// �e����������
void UninitShadow(void);	// �e�I������
void UpdateShadow(void);	// �e�X�V����
void DrawShadow(void);		// �e�`�揈��
int SetShadow(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 Scal);		// �e�̐ݒ�
void SetPositionShadow(int nidxSahdow, D3DXVECTOR3 pos);// �e�̍��W�X�V�ݒ�
void DeleteShadow(int nidxSahdow);// �e�̏���

#endif // !_SHADOW_H_

