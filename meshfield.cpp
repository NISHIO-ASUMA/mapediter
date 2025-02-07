//========================================
//
// ���b�V���t�B�[���h���� [meshfield.cpp]
// Author: Asuma Nishio
//
//========================================

//****************************
// �C���N���[�h�t�@�C���錾
//****************************
#include "meshfield.h"

//***************************
// �}�N����`
//***************************
#define MAX_XVTX (3)	// X�̒l
#define MAX_ZVTX (5)	// Z�̒l

#define MAX_ALLVTX ((MAX_XVTX + 1) * (MAX_ZVTX + 1))	// ���_��
#define POLYGON (((MAX_XVTX * MAX_ZVTX) * 2)) + (4 * (MAX_ZVTX - 1))	// �|���S����
#define MAX_INDEXBUFF (POLYGON + 2)		// �C���f�b�N�X�o�b�t�@��

#define MAXWIDTH  (1600.0f)	// �t�B�[���h��X
#define MAXHEIGHT (1600.0f)	// �t�B�[���h��Z

//*****************************
// �O���[�o���ϐ��錾
//*****************************
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffMeshField = NULL;	// ���_�o�b�t�@�̃|�C���^
LPDIRECT3DTEXTURE9 g_apTextureMeshField[2] = {};		// �e�N�X�`���̃|�C���^
LPDIRECT3DINDEXBUFFER9 g_pIdxBuffMeshField = NULL;	// �C���f�b�N�X�o�b�t�@�ւ̃|�C���^
Meshfield g_Mesh;									// �\���̕ϐ�

//==============================
// ���b�V���t�B�[���h����������
//==============================
void InitMeshField(void)
{
	// �f�o�C�X�|�C���^��錾
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// �e�N�X�`���̓ǂݍ���
	D3DXCreateTextureFromFile(pDevice,
		"data\\TEXTURE\\faver.png",
		&g_apTextureMeshField[1]);

	// �e�N�X�`���̓ǂݍ���
	D3DXCreateTextureFromFile(pDevice,
		"data\\TEXTURE\\trash.png",
		&g_apTextureMeshField[0]);

	// �ϐ��̏�����
	g_Mesh.pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_Mesh.rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	// ���[�J���ϐ��錾------------------------
	int nCntPos = 0;				// ���W�p
	float TexX = 1.0f / MAX_XVTX;	// �e�N�X�`�����W��X
	float TexZ = 1.0f / MAX_ZVTX;	// �e�N�X�`�����W��Z

	int nIdxNumber = 0;				// ���[�J���ϐ�
	int nCntIdx3 = MAX_XVTX + 1;	// X�̒l
	int nNum = 0;					// ���[�J���ϐ�
	//------------------------------------------
	
	// ���_�o�b�t�@(�O�p�`�͈ӎ����Ȃ�)
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D_MULTI) * MAX_ALLVTX,
		D3DUSAGE_WRITEONLY, 
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffMeshField,
		NULL);

	// �C���f�b�N�X�o�b�t�@�̐���
	pDevice->CreateIndexBuffer(sizeof(WORD) * MAX_INDEXBUFF,
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&g_pIdxBuffMeshField,
		NULL);

	// ���_���̃|�C���^
	VERTEX_3D_MULTI * pVtx = NULL;

	// ���_�o�b�t�@�����b�N��,���_���ւ̃|�C���^���擾
	g_pVtxBuffMeshField->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntx = 0; nCntx <= MAX_ZVTX; nCntx++)
	{
		for (int nCntz = 0; nCntz <= MAX_XVTX; nCntz++)
		{
			// ���_���̐ݒ�
			pVtx[nCntPos].pos.x = (0.0f + (MAXWIDTH / MAX_XVTX) * nCntz)  - (MAXWIDTH * 0.5f);	// ���������ɂ��炵�Ă���
			pVtx[nCntPos].pos.y = 0.0f;															// �n�ʂȂ̂�0.0f
			pVtx[nCntPos].pos.z = (MAXHEIGHT - (MAXHEIGHT / MAX_ZVTX) * nCntx) - (MAXHEIGHT * 0.5f);	// �������炷

			// �e���_�̖@��(�x�N�g��)�̐ݒ�
			pVtx[nCntPos].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);		// �@�����

			// ���_�J���[�̐ݒ�
			pVtx[nCntPos].col = D3DXCOLOR(1.0f,1.0f,1.0f, 1.0f);	// �J���[

			// �e�N�X�`�����W�̐ݒ�
			pVtx[nCntPos].tex = D3DXVECTOR2(TexX *  nCntz, TexZ * nCntx);

			// �e�N�X�`�����W�̐ݒ�
			pVtx[nCntPos].texM = D3DXVECTOR2(TexX * nCntz, TexZ * nCntx);

			nCntPos++;												// �C���N�������g����1�𑫂�
		}
	}
	
	// �A�����b�N
	g_pVtxBuffMeshField->Unlock();

	// �C���f�b�N�X�o�b�t�@�̃|�C���^
	WORD* pIdx;

	//�C���f�b�N�X�o�b�t�@�����b�N
	g_pIdxBuffMeshField->Lock(0, 0, (void**)&pIdx, 0);

	 //�C���f�b�N�X��ݒ�
	for (int nCntIdx = 0; nCntIdx < MAX_ZVTX; nCntIdx++)
	{// MAX_ZVTX�̐�������
		for (int nCntIdx2 = 0; nCntIdx2 <= MAX_XVTX; nCntIdx2++, nCntIdx3++, nNum++)
		{// MAX_XVTX�̐�������
			pIdx[nIdxNumber] = nCntIdx3;	// ��������
			pIdx[nIdxNumber + 1] = nNum;	// ��������
			nIdxNumber += 2;				// 2���i�߂�
		}

		// NOTE: �Ō�̍s�ɏk�ރ|���S�����Ȃ�
		if (nCntIdx !=  MAX_ZVTX -1)
		{
			pIdx[nIdxNumber] = nNum - 1;		// for���ɓ��邽��
			pIdx[nIdxNumber + 1] = nCntIdx3;	// ��������
			nIdxNumber += 2;
		}
	}

	// �A�����b�N
	g_pIdxBuffMeshField->Unlock();
}
//==============================
// ���b�V���t�B�[���h�I������
//==============================
void UninitMeshField(void)
{
	// �e�N�X�`���̔j��
	for (int n = 0; n < 2; n++)
	{
		if (g_apTextureMeshField[n] != NULL)
		{
			g_apTextureMeshField[n]->Release();
			g_apTextureMeshField[n] = NULL;
		}

	}

	// ���_�o�b�t�@�̔j��
	if (g_pVtxBuffMeshField != NULL)
	{
		g_pVtxBuffMeshField->Release();
		g_pVtxBuffMeshField = NULL;
	}

	// �C���f�b�N�X�o�b�t�@�̔j��
	if (g_pIdxBuffMeshField != NULL)
	{
		g_pIdxBuffMeshField->Release();
		g_pIdxBuffMeshField = NULL;
	}

}
//==============================
// ���b�V���t�B�[���h�X�V����
//==============================
void UpdateMeshField(void)
{
	// ����
}
//==============================
// ���b�V���t�B�[���h�`�揈��
//==============================
void DrawMeshField(void)
{
	// �f�o�C�X�|�C���^��錾
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	
	// �v�Z�p�̃}�g���b�N�X��錾
	D3DXMATRIX mtxRot, mtxTrans;

	// ���[���h�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&g_Mesh.mtxWorld);

	// �����𔽉f
	D3DXMatrixRotationYawPitchRoll(&mtxRot, g_Mesh.rot.y, g_Mesh.rot.x, g_Mesh.rot.z);
	D3DXMatrixMultiply(&g_Mesh.mtxWorld, &g_Mesh.mtxWorld, &mtxRot);

	// �ʒu�𔽉f
	D3DXMatrixTranslation(&mtxTrans, g_Mesh.pos.x, g_Mesh.pos.y, g_Mesh.pos.z);
	D3DXMatrixMultiply(&g_Mesh.mtxWorld, &g_Mesh.mtxWorld, &mtxTrans);

	// ���[���h�}�g���b�N�X�̐ݒ�
	pDevice->SetTransform(D3DTS_WORLD, &g_Mesh.mtxWorld);

	// ���_�o�b�t�@���f�[�^�X�g���[���ɐݒ�
	pDevice->SetStreamSource(0, g_pVtxBuffMeshField, 0, sizeof(VERTEX_3D_MULTI));

	// �C���f�b�N�X�o�b�t�@���f�[�^�X�g���[���ɐݒ�
	pDevice->SetIndices(g_pIdxBuffMeshField);

	// ���_�t�H�[�}�b�g�̐ݒ�
	pDevice->SetFVF(FVF_VERTEX_3D_MULTI);

	//�e�N�X�`���̐ݒ�
	pDevice->SetTexture(0, g_apTextureMeshField[0]);

	pDevice->SetTexture(1, g_apTextureMeshField[1]);

	// �|���S���̕`��
	pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP,
		0,
		0,
		MAX_ALLVTX,
		0,
		POLYGON);
}
