//======================================
//
// ���b�V���t�B�[���h���� [mashfield.h]
// Author: Asuma Nishio
//
//======================================
#ifndef _MESHFIELD_H_
#define _MESHFIELD_H_

//***************************
// �C���N���[�h�t�@�C���錾
//***************************
#include "main.h"

//***************************
// �\���̐錾
//***************************
typedef struct
{
	D3DXVECTOR3 pos;		// �ʒu
	D3DXVECTOR3 rot;		// ����
	D3DXMATRIX  mtxWorld;	// ���[���h�}�g���b�N�X
}Meshfield;

//**********************
// �v���g�^�C�v�錾
//**********************
void InitMeshField(void);	// �|���S������������
void UninitMeshField(void);	// �|���S���I������
void UpdateMeshField(void);	// �|���S���X�V����
void DrawMeshField(void);	// �|���S���`�揈��

#endif // !_POLYGON_H_
