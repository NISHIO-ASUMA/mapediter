//==========================
//
// ���f������ [model.h]
// Author: Asuma Nishio
//
//==========================
#ifndef _MODEL_H_
#define _MODEL_H_

//***************************
// �C���N���[�h�t�@�C���錾
//***************************
#include "main.h"

//*****************************
// �}�N����`
//*****************************
#define MAX_TETURE (10)		// �e�N�X�`����

//***************************
// �\���̐錾
//***************************
typedef struct
{
	LPD3DXMESH pMeshModel;		// ���b�V���̃|�C���^
	LPD3DXBUFFER pBuffMatModel;	// �}�e���A���̃|�C���^
	DWORD dwNumMatModel;		// �}�e���A����
	D3DXVECTOR3 pos;			// �ʒu (�I�t�Z�b�g)
	D3DXVECTOR3 rot;			// ����
	D3DXMATRIX mtxWorld;		// ���[���h�}�g���b�N�X
	int nIndxModelParent;		// �e���f���̃I�t�Z�b�g
	LPDIRECT3DTEXTURE9 pTexture[MAX_TETURE];	// �e�N�X�`���̃|�C���^
	D3DXVECTOR3 Offpos;
	D3DXVECTOR3 Offrot;
	D3DCOLORVALUE Diffuse;
	D3DCOLORVALUE FirstDiffuse;
	D3DXVECTOR3 size;
	D3DXVECTOR3 Vtxmin, Vtxmax;
}Model;

//**********************
// �v���g�^�C�v�錾
//**********************
void InitModel(void);		// ���f������������
void UninitModel(void);		// ���f���I������
void UpdateModel(void);		// ���f���X�V����
void DrawModel(void);		// ���f���`�揈��

#endif // !_MODEL_H_
