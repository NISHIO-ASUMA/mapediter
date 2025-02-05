//=============================
//
// �e�̏��� [shadow.cpp]
// Author: Asuma Nishio
//
//=============================

//****************************
// �C���N���[�h�t�@�C���錾
//****************************
#include "shadow.h"

//*****************************
// �O���[�o���ϐ��錾
//*****************************
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffShadow = NULL;// ���_�o�b�t�@�̃|�C���^
LPDIRECT3DTEXTURE9 g_apTextureShadow = NULL;	// �e�N�X�`���̃|�C���^
Shadow g_Shadow[MAX_SHADOW];					// �\���̕ϐ�

//=========================
//�@�e����������
//=========================
void InitShadow(void)
{
	// �f�o�C�X�|�C���^��錾
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// �e�N�X�`���̓ǂݍ���
	D3DXCreateTextureFromFile(pDevice,
		"data\\TEXTURE\\shadow000.jpg",
		&g_apTextureShadow);

	for (int nCnt = 0; nCnt < MAX_SHADOW; nCnt++)
	{// �O���[�o���ϐ��̏�����
		g_Shadow[nCnt].bUse = false;							  // ���g�p����
		g_Shadow[nCnt].posShadow = D3DXVECTOR3(0.0f, 0.5f, 0.0f); // ���W
		g_Shadow[nCnt].rotShadow = D3DXVECTOR3(0.0f, 0.0f, 0.0f); // �p�x
		g_Shadow[nCnt].Scal = D3DXVECTOR3(0.0f, 0.5f, 0.0f);	  // �{��
	}

	// ���_�o�b�t�@�̐���
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * 4 * MAX_SHADOW,
		D3DUSAGE_WRITEONLY, FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffShadow,
		NULL);

	// ���_���̃|�C���^
	VERTEX_3D* pVtx = NULL;

	// ���_�o�b�t�@�����b�N��,���_���ւ̃|�C���^���擾
	g_pVtxBuffShadow->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntShadow = 0; nCntShadow < MAX_SHADOW;nCntShadow++)
	{
		// ���_���W�̐ݒ�
		pVtx[0].pos.x = -SHADOW_SIZE;
		pVtx[0].pos.y = 0.5f;  
		pVtx[0].pos.z = SHADOW_SIZE;
														  
		pVtx[1].pos.x = SHADOW_SIZE;
		pVtx[1].pos.y = 0.5f;  
		pVtx[1].pos.z = SHADOW_SIZE;
														   
		pVtx[2].pos.x = -SHADOW_SIZE;
		pVtx[2].pos.y = 0.5f;  
		pVtx[2].pos.z = -SHADOW_SIZE;
														   
		pVtx[3].pos.x = SHADOW_SIZE;
		pVtx[3].pos.y = 0.5f;  
		pVtx[3].pos.z = -SHADOW_SIZE;
		
		// �e���_�̖@��(�x�N�g��)�̐ݒ�
		pVtx[0].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);		// 1�ڂ̖@�����
		pVtx[1].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);		// 2�ڂ̖@�����
		pVtx[2].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);		// 3�ڂ̖@�����
		pVtx[3].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);		// 4�ڂ̖@�����

		// ���_�J���[�̐ݒ�
		pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

		// �e�N�X�`�����W�̐ݒ�
		pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
		pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

		pVtx += 4;
	}

	// �A�����b�N
	g_pVtxBuffShadow->Unlock();

}
//=========================
//�@�e�I������
//=========================
void UninitShadow(void)
{
	// �e�N�X�`���̔j��
	if (g_apTextureShadow != NULL)
	{
		g_apTextureShadow->Release();
		g_apTextureShadow = NULL;
	}

	// ���_�o�b�t�@�̔j��
	if (g_pVtxBuffShadow != NULL)
	{			  
		g_pVtxBuffShadow->Release();
		g_pVtxBuffShadow = NULL;
	}

}
//=========================
//�@�e�X�V����
//=========================
void UpdateShadow(void)
{
	// �Ȃ�
}
//=========================
//�@�e�`�揈��
//=========================
void DrawShadow(void)
{
	// �f�o�C�X�|�C���^��錾
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// ���Z�����̐ݒ�
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_REVSUBTRACT);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	// �v�Z�p�̃}�g���b�N�X��錾
	D3DXMATRIX mtxRot, mtxTrans,mtxScall;

	for (int nCnt = 0; nCnt < MAX_SHADOW; nCnt++)
	{
		if (g_Shadow[nCnt].bUse)
		{// �g���Ă���Ƃ�

			// ���[���h�}�g���b�N�X�̏�����
			D3DXMatrixIdentity(&g_Shadow[nCnt].mtxWorldShadow);

			// �{����ݒ�
			D3DXMatrixScaling(&mtxScall, g_Shadow[nCnt].Scal.x, 0.5f, g_Shadow[nCnt].Scal.z);
			D3DXMatrixMultiply(&g_Shadow[nCnt].mtxWorldShadow, &g_Shadow[nCnt].mtxWorldShadow, &mtxScall);

			// �����𔽉f
			D3DXMatrixRotationYawPitchRoll(&mtxRot, g_Shadow[nCnt].rotShadow.y, g_Shadow[nCnt].rotShadow.x, g_Shadow[nCnt].rotShadow.z);
			D3DXMatrixMultiply(&g_Shadow[nCnt].mtxWorldShadow, &g_Shadow[nCnt].mtxWorldShadow, &mtxRot);

			// �ʒu�𔽉f
			D3DXMatrixTranslation(&mtxTrans, g_Shadow[nCnt].posShadow.x, g_Shadow[nCnt].posShadow.y, g_Shadow[nCnt].posShadow.z);
			D3DXMatrixMultiply(&g_Shadow[nCnt].mtxWorldShadow, &g_Shadow[nCnt].mtxWorldShadow, &mtxTrans);

			// ���[���h�}�g���b�N�X�̐ݒ�
			pDevice->SetTransform(D3DTS_WORLD, &g_Shadow[nCnt].mtxWorldShadow);

			// ���_�o�b�t�@���f�[�^�X�g���[���ɐݒ�
			pDevice->SetStreamSource(0, g_pVtxBuffShadow, 0, sizeof(VERTEX_3D));

			// ���_�t�H�[�}�b�g�̐ݒ�
			pDevice->SetFVF(FVF_VERTEX_3D);

			//�e�N�X�`���̐ݒ�
			pDevice->SetTexture(0, g_apTextureShadow);

			// �|���S���̕`��
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, nCnt * 4,2);

		}
	}

	// �ݒ��߂�
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}
//=========================
//�@�e�̐ݒ菈��
//=========================
int SetShadow(D3DXVECTOR3 pos, D3DXVECTOR3 rot,D3DXVECTOR3 Scal)
{
	// ���[�J���ϐ�
	int nCntShadow;

	for (nCntShadow = 0; nCntShadow < MAX_SHADOW; nCntShadow++)
	{
		if (g_Shadow[nCntShadow].bUse)
		{// �g�p��ԂȂ�
			continue;
		}

		g_Shadow[nCntShadow].posShadow = pos;	// ���W
		g_Shadow[nCntShadow].rotShadow = rot;	// �p�x
		g_Shadow[nCntShadow].Scal = Scal;		// �傫��
		g_Shadow[nCntShadow].bUse = true;		// �g�p����

		break;
		
	}
	return nCntShadow;		// �e�̔ԍ�(index)��Ԃ�
}
//=============================
// �e�̈ʒu�̍X�V����
//=============================
void SetPositionShadow(int nidxSahdow, D3DXVECTOR3 pos)
{
	// �����Ŏw�肳�ꂽ�ԍ��̉e��pos��ݒ�
	g_Shadow[nidxSahdow].posShadow = pos;

}
//=============================
// �e����������
//=============================
void DeleteShadow(int nidxSahdow)
{
	g_Shadow[nidxSahdow].bUse = false;
}


