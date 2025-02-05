//=============================
//
// ���f������ [model.cpp]
// Author: Asuma Nishio
//
//=============================

//****************************
// �C���N���[�h�t�@�C���錾
//****************************
#include "model.h"
#include "input.h"


//*****************************
// �O���[�o���ϐ��錾
//*****************************
Model g_Model;

//=========================
//�@���f������������
//=========================
void InitModel(void)
{
	//// �f�o�C�X�|�C���^��錾
	//LPDIRECT3DDEVICE9 pDevice;

	//// �f�o�C�X���擾
	//pDevice = GetDevice();

	//// �ϐ��̏�����
	//g_Model.pos = D3DXVECTOR3 (0.0f,0.0f,0.0f);
	//g_Model.rot = D3DXVECTOR3 (0.0f,0.0f,0.0f);

	//// X�t�@�C���̓ǂݍ���
	//D3DXLoadMeshFromX("data\\MODEL\\block.x",
	//	D3DXMESH_SYSTEMMEM,
	//	pDevice,
	//	NULL,
	//	&g_Model.pBuffMatMode,
	//	NULL,
	//	&g_Model.dwNumMatModel,
	//	&g_Model.pMeshModel);

	//// �}�e���A���f�[�^�ւ̃|�C���^
	//D3DXMATERIAL* pMat;

	//// �}�e���A���f�[�^�ւ̃|�C���^���擾
	//pMat = (D3DXMATERIAL*)g_Model.pBuffMatMode->GetBufferPointer();

	//// �}�e���A����������
	//for (int nCntMat = 0; nCntMat < (int)g_Model.dwNumMatModel; nCntMat++)
	//{
	//	if (pMat[nCntMat].pTextureFilename != NULL)
	//	{
	//		// �e�N�X�`���̓ǂݍ���
	//		D3DXCreateTextureFromFile(pDevice,
	//			pMat[nCntMat].pTextureFilename,
	//			&g_apTextureModel[nCntMat]);

	//	}
	//}

}
//=========================
//�@���f���I������
//=========================
void UninitModel(void)
{
	//// ���b�V���̔j��
	//if (g_Model.pMeshModel != NULL)
	//{
	//	g_Model.pMeshModel->Release();
	//	g_Model.pMeshModel = NULL;
	//}

	//// �}�e���A���̔j��
	//if (g_Model.pBuffMatMode != NULL)
	//{
	//	g_Model.pBuffMatMode->Release();
	//	g_Model.pBuffMatMode = NULL;
	//}

	//// �e�N�X�`���̔j��
	//for (int nCnt = 0; nCnt < MAX_TETURE; nCnt++)
	//{
	//	if (g_apTextureModel[nCnt] != NULL)
	//	{
	//		g_apTextureModel[nCnt]->Release();
	//		g_apTextureModel[nCnt] = NULL;
	//	}
	//}

}
//=========================
//�@���f���X�V����
//=========================
void UpdateModel(void)
{
	// ��U������

}
//=========================
//�@���f���`�揈��
//=========================
void DrawModel(void)
{
	//// �f�o�C�X�|�C���^��錾
	//LPDIRECT3DDEVICE9 pDevice = GetDevice();

	//// �v�Z�p�̃}�g���b�N�X��錾
	//D3DXMATRIX mtxRot, mtxTrans;

	//// ���݂̃}�e���A����ۑ�
	//D3DMATERIAL9 matDef;

	//// �}�e���A���f�[�^�ւ̃|�C���^
	//D3DXMATERIAL* pMat;

	//// ���[���h�}�g���b�N�X�̏�����
	//D3DXMatrixIdentity(&g_Model.mtxWorld);

	//// �����𔽉f
	//D3DXMatrixRotationYawPitchRoll(&mtxRot, g_Model.rot.y, g_Model.rot.x, g_Model.rot.z);
	//D3DXMatrixMultiply(&g_Model.mtxWorld, &g_Model.mtxWorld, &mtxRot);

	//// �ʒu�𔽉f
	//D3DXMatrixTranslation(&mtxTrans, g_Model.pos.x, g_Model.pos.y, g_Model.pos.z);
	//D3DXMatrixMultiply(&g_Model.mtxWorld, &g_Model.mtxWorld, &mtxTrans);

	//// ���[���h�}�g���b�N�X�̐ݒ�
	//pDevice->SetTransform(D3DTS_WORLD, &g_Model.mtxWorld);

	//// ���݂̃}�g���b�N�X�̎擾
	//pDevice->GetMaterial(&matDef);

	//// �}�e���A���f�[�^�ւ̃|�C���^���擾
	//pMat = (D3DXMATERIAL*)g_Model.pBuffMatMode->GetBufferPointer();

	//// �}�e���A����������
	//for (int nCntMat = 0; nCntMat < (int)g_Model.dwNumMatModel; nCntMat++)
	//{
	//	// �}�e���A���ݒ�
	//	pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

	//	// �e�N�X�`���ݒ�
	//	pDevice->SetTexture(0, g_apTextureModel[nCntMat]);

	//	// ���f��(�p�[�c)�̕`��
	//	g_Model.pMeshModel->DrawSubset(nCntMat);

	//}

	//// �}�e���A����߂�
	//pDevice->SetMaterial(&matDef);
}
