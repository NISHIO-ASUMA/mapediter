//===================================
//
// �}�b�v�G�f�B�^�[���� [mapedit.cpp]
// Author: Asuma Nishio
//
//===================================

//****************************
// �C���N���[�h�t�@�C���錾
//****************************
#include "mapedit.h"
#include "camera.h"
#include "meshfield.h"
#include "light.h"
#include "input.h"
#include <stdio.h>
#include "block.h"

//*************************************
// �t�@�C�����񋓌^�錾
//*************************************
typedef enum
{
	FILEPASS_0 = 0,	// �����t�@�C���p�X
	FILEPASS_1,		// 2��
	FILEPASS_2,		// 3��
	FILEPASS_MAX
}FILEPASS;

//*************************************
// �����o���t�@�C���p�X��ݒ�
//*************************************
const char* WRITE_FILENAME[FILEPASS_MAX] =
{
	"data\\stage000.bin",	// �������
	"data\\stage001.bin",	// 2�Ԗ�
	"data\\stage002.bin",	// 3�Ԗ�
};

//****************************
// �O���[�o���ϐ��錾
//****************************
EDITMODEL g_ModelInfo[MAX_TEXTURE];	 // �u���b�N�̏���ۑ�
MAPMODELINFO g_MapEdit[MAX_EDITOBJ]; // �z�u���̏��
int g_nTypeNum;						 // �u���b�N�̎�ސ���ۑ�
int g_Edit;							 // �z�u�����J�E���g
float g_fspeed;						 // �ړ���
float g_jump;						 // �W�����v
int g_CurrentFilepass;				 // �t�@�C���p�X�̃C���f�b�N�X
bool isLoad;						 // ���[�h�������ǂ���

//****************************
// �v���g�^�C�v�錾
//****************************
void InitEditinfo(); // �\���̏�����

//============================
// �}�b�v�G�f�B�^�[����������
//============================
void InitMapEdit()
{
	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// �J�����̏�����
	InitCamera();

	// ���C�g�̏�����
	InitLight();

	// ���b�V���t�B�[���h�̏�����
	InitMeshField();

	// �u���b�N���̓ǂݍ���
	LoadXFile();

	// �\���̏��
	InitEditinfo();

	// �O���[�o���ϐ��̏�����
	g_Edit = 0; // �z�u���̃J�E���g
	g_MapEdit[0].bUse = true;	// �g�p���
	g_CurrentFilepass = FILEPASS_0;	// �����t�@�C���p�X
	isLoad = false;	// ���[�h���
}
//============================
// �}�b�v�G�f�B�^�[�I������
//============================
void UninitMapEdit()
{
	// ���b�V���t�B�[���h�̏I��
	UninitMeshField();

	// ���C�g�̏I��
	UninitLight();

	// �e�N�X�`���̔j��
	for (int i = 0; i < MAX_TEXTURE; i++)
	{
		for (int nCntMat = 0; nCntMat < (int)g_ModelInfo[i].dwNumMat; nCntMat++)
		{
			if (g_ModelInfo[i].pTexture[nCntMat] != NULL)
			{
				g_ModelInfo[i].pTexture[nCntMat]->Release();
				g_ModelInfo[i].pTexture[nCntMat] = NULL;
			}
		}

		// ���b�V���̔j��
		if (g_ModelInfo[i].pMesh != NULL)
		{
			g_ModelInfo[i].pMesh->Release();
			g_ModelInfo[i].pMesh = NULL;
		}

		// �o�b�t�@�̔j��
		if (g_ModelInfo[i].pBuffMat != NULL)
		{
			g_ModelInfo[i].pBuffMat->Release();
			g_ModelInfo[i].pBuffMat = NULL;
		}
	}
}
//============================
// �}�b�v�G�f�B�^�[�X�V����
//============================
void UpdateMapEdit()
{
	// �J�������擾
	Camera* pCamera = GetCamera();

	// ���[�h���擾
	MODE nMode = GetMode();

	// �J�����̍X�V
	UpdateCamera();

	if (nMode == MODE_EDIT)
	{// MODE���ҏW���[�h�Ȃ�

		//	�����o������
		if (KeyboardTrigger(DIK_F7))
		{// F7�L�[��������
			// �ۑ�����
			SaveEdit();
		}

		if (KeyboardTrigger(DIK_F8))
		{// F8�L�[��������
			// �t�@�C���p�X��؂�ւ�
			g_CurrentFilepass = (g_CurrentFilepass + 1) % FILEPASS_MAX;
		}

		if (KeyboardTrigger(DIK_F9))
		{// F9�L�[��������
			// �ēǂݍ���
			ReloadEdit();
			isLoad = true;
		}

		// �ړ�����
		if (GetKeyboardPress(DIK_A))
		{// A�L�[��������

			g_MapEdit[g_Edit].mapedit.move.x -= sinf(pCamera->rot.y + (D3DX_PI * 0.5f)) * g_fspeed;
			g_MapEdit[g_Edit].mapedit.move.z -= cosf(pCamera->rot.y + (D3DX_PI * 0.5f)) * g_fspeed;

			// �p�x�̐��K��
			if (g_MapEdit[g_Edit].mapedit.rot.y > D3DX_PI)
			{// D3DX_PI���傫���Ȃ�����
				g_MapEdit[g_Edit].mapedit.rot.y -= D3DX_PI * 2.0f;
			}
		}
		else if (GetKeyboardPress(DIK_D))
		{// D�L�[��������
			// D�L�[�̂݉�����
			g_MapEdit[g_Edit].mapedit.move.x += sinf(pCamera->rot.y + (D3DX_PI * 0.5f)) * g_fspeed;
			g_MapEdit[g_Edit].mapedit.move.z += cosf(pCamera->rot.y + (D3DX_PI * 0.5f)) * g_fspeed;

			// �p�x�̐��K��
			if (g_MapEdit[g_Edit].mapedit.rot.y > D3DX_PI)
			{// D3DX_PI���傫���Ȃ�����
				g_MapEdit[g_Edit].mapedit.rot.y -= D3DX_PI * 2.0f;
			}

		}
		else if (GetKeyboardPress(DIK_W))
		{// W�L�[��������

			g_MapEdit[g_Edit].mapedit.move.x += sinf(pCamera->rot.y) * g_fspeed;
			g_MapEdit[g_Edit].mapedit.move.z += cosf(pCamera->rot.y) * g_fspeed;

			// �p�x�𐳋K��
			if (g_MapEdit[g_Edit].mapedit.rot.y < -D3DX_PI)
			{// D3DX_PI��菬�����Ȃ�����
				g_MapEdit[g_Edit].mapedit.rot.y += D3DX_PI * 2.0f;
			}

		}
		else if (GetKeyboardPress(DIK_S))
		{// S�L�[��������

			g_MapEdit[g_Edit].mapedit.move.x -= sinf(pCamera->rot.y) * g_fspeed;
			g_MapEdit[g_Edit].mapedit.move.z -= cosf(pCamera->rot.y) * g_fspeed;

			// �p�x�̐��K��
			if (g_MapEdit[g_Edit].mapedit.rot.y > D3DX_PI)
			{// D3DX_PI���傫���Ȃ�����
				g_MapEdit[g_Edit].mapedit.rot.y -= D3DX_PI * 2.0f;
			}
		}

		if (KeyboardTrigger(DIK_UP))
		{// ��L�[��������
			if (g_MapEdit[g_Edit].mapedit.nType < g_nTypeNum - 1)
			{
				// �ő吔��菬�����Ƃ�
				g_MapEdit[g_Edit].mapedit.nType++;
			}
		}
		else if (KeyboardTrigger(DIK_DOWN))
		{// ���L�[��������
			if (g_MapEdit[g_Edit].mapedit.nType > 0)
			{
				// 1���傫���Ƃ�
				g_MapEdit[g_Edit].mapedit.nType--;
			}
		}

		if (KeyboardTrigger(DIK_T))
		{// T�L�[����������
			// ���W���X�V
			g_MapEdit[g_Edit].mapedit.pos.y += 10.0f;
		}
		else if (KeyboardTrigger(DIK_G))
		{// G�L�[����������
			// ���W���X�V
			g_MapEdit[g_Edit].mapedit.pos.y -= 10.0f;
		}

		if (KeyboardTrigger(DIK_H))
		{// H�L�[�������ꂽ
			// �g�嗦�̕ύX
			g_MapEdit[g_Edit].mapedit.Scal.x -= 0.1f;

			if (g_MapEdit[g_Edit].mapedit.Scal.x <= 0.1f)
			{// ������ݒ�
				g_MapEdit[g_Edit].mapedit.Scal.x = 0.1f;
			}

		}
		else if (KeyboardTrigger(DIK_Y))
		{// Y�L�[�������ꂽ
			// �g�嗦�̕ύX
			g_MapEdit[g_Edit].mapedit.Scal.x += 0.1f;

			if (g_MapEdit[g_Edit].mapedit.Scal.x >= 2.0f)
			{// �����ݒ�
				g_MapEdit[g_Edit].mapedit.Scal.x = 2.0f;
			}
		}

		if (KeyboardTrigger(DIK_J))
		{// J�L�[�������ꂽ
			// �g�嗦�̕ύX
			g_MapEdit[g_Edit].mapedit.Scal.y -= 0.1f;

			if (g_MapEdit[g_Edit].mapedit.Scal.y <= 0.1f)
			{// ������ݒ�
				g_MapEdit[g_Edit].mapedit.Scal.y = 0.1f;
			}

		}
		else if (KeyboardTrigger(DIK_U))
		{// U�L�[�������ꂽ
			// �g�嗦�̕ύX
			g_MapEdit[g_Edit].mapedit.Scal.y += 0.1f;

			if (g_MapEdit[g_Edit].mapedit.Scal.y >= 2.0f)
			{// �����ݒ�
				g_MapEdit[g_Edit].mapedit.Scal.y = 2.0f;
			}

		}

		if (KeyboardTrigger(DIK_I))
		{// I�L�[�������ꂽ
			// �g�嗦�̕ύX
			g_MapEdit[g_Edit].mapedit.Scal.z += 0.1f;

			if (g_MapEdit[g_Edit].mapedit.Scal.z >= 2.0f)
			{// �����ݒ�
				g_MapEdit[g_Edit].mapedit.Scal.z = 2.0f;
			}

		}
		else if (KeyboardTrigger(DIK_K))
		{// K�L�[�������ꂽ
			// �g�嗦�̕ύX
			g_MapEdit[g_Edit].mapedit.Scal.z -= 0.1f;

			if (g_MapEdit[g_Edit].mapedit.Scal.z <= 0.1f)
			{// ������ݒ�
				g_MapEdit[g_Edit].mapedit.Scal.z = 0.1f;
			}

		}

		if (KeyboardTrigger(DIK_RETURN))
		{// ENTER�L�[�������ꂽ
			// ���̓G�̈ʒu��z�u�����G�Ɠ����ɂ���
			g_MapEdit[g_Edit + 1].mapedit.pos = g_MapEdit[g_Edit].mapedit.pos;

			// �^�C�v�𓯂��ɂ���
			g_MapEdit[g_Edit + 1].mapedit.nType = g_MapEdit[g_Edit].mapedit.nType;

			// �g�p����ɂ���
			g_MapEdit[g_Edit + 1].bUse = true;

			// �u������������
			g_MapEdit[g_Edit].isputblock = true;

			// �C���N�������g
			g_Edit++;

			// �u���������������
			g_MapEdit[g_Edit].isputblock = false;
		}

		if (g_Edit > 0)
		{// 0�ȏ�Ȃ�
			if (KeyboardTrigger(DIK_BACKSPACE))
			{// BACKSPACE�L�[�������ꂽ

				// ���g�p����ɂ���
				g_MapEdit[g_Edit].bUse = false;

				// �u��������
				g_MapEdit[g_Edit].isputblock = false;

				// �f�N�������g
				g_Edit--;
			}
		}


		// �ړ��ʂ̌���
		g_MapEdit[g_Edit].mapedit.move.x += (0.0f - g_MapEdit[g_Edit].mapedit.move.x) * 0.5f;
		g_MapEdit[g_Edit].mapedit.move.z += (0.0f - g_MapEdit[g_Edit].mapedit.move.z) * 0.5f;

		// �ړ��ʂ̍X�V
		g_MapEdit[g_Edit].mapedit.pos += g_MapEdit[g_Edit].mapedit.move;
	}
}
//============================
// �}�b�v�G�f�B�^�[�`�揈��
//============================
void DrawMapEdit()
{
	// �f�o�C�X�|�C���^��錾
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// ���݂̃��[�h���擾
	MODE nMode = GetMode();

	// �J�������Z�b�g
	SetCamera();

	// ���b�V���t�B�[���h�̕`��
	DrawMeshField();

	// �v�Z�p�̃}�g���b�N�X��錾
	D3DXMATRIX mtxRot, mtxTrans, mtxScal;

	// ���݂̃}�e���A����ۑ�
	D3DMATERIAL9 matDef;

	// �}�e���A���f�[�^�ւ̃|�C���^
	D3DXMATERIAL* pMat;

	for (int nCnt = 0; nCnt < MAX_EDITOBJ; nCnt++)
	{
		// �^�C�v��ۑ�
		int nType = g_MapEdit[nCnt].mapedit.nType;

		if (((nMode == MODE_PLAY) && !g_MapEdit[nCnt].isputblock) || !g_MapEdit[nCnt].bUse)
		{// �v���C���[�h ���� isputblock�����g�p �܂��� bUse�����g�p
			continue;
		}

		// ���[���h�}�g���b�N�X�̏�����
		D3DXMatrixIdentity(&g_MapEdit[nCnt].mapedit.mtxWorld);

		// �傫���𔽉f
		D3DXMatrixScaling(&mtxScal, g_MapEdit[nCnt].mapedit.Scal.x, g_MapEdit[nCnt].mapedit.Scal.y, g_MapEdit[nCnt].mapedit.Scal.z);
		D3DXMatrixMultiply(&g_MapEdit[nCnt].mapedit.mtxWorld, &g_MapEdit[nCnt].mapedit.mtxWorld, &mtxScal);

		// �����𔽉f
		D3DXMatrixRotationYawPitchRoll(&mtxRot, g_MapEdit[nCnt].mapedit.rot.y, g_MapEdit[nCnt].mapedit.rot.x, g_MapEdit[nCnt].mapedit.rot.z);
		D3DXMatrixMultiply(&g_MapEdit[nCnt].mapedit.mtxWorld, &g_MapEdit[nCnt].mapedit.mtxWorld, &mtxRot);

		// �ʒu�𔽉f
		D3DXMatrixTranslation(&mtxTrans, g_MapEdit[nCnt].mapedit.pos.x, g_MapEdit[nCnt].mapedit.pos.y, g_MapEdit[nCnt].mapedit.pos.z);
		D3DXMatrixMultiply(&g_MapEdit[nCnt].mapedit.mtxWorld, &g_MapEdit[nCnt].mapedit.mtxWorld, &mtxTrans);

		// ���[���h�}�g���b�N�X�̐ݒ�
		pDevice->SetTransform(D3DTS_WORLD, &g_MapEdit[nCnt].mapedit.mtxWorld);

		// ���݂̃}�g���b�N�X�̎擾
		pDevice->GetMaterial(&matDef);

		// �}�e���A����������
		for (int nCntMat = 0; nCntMat < (int)g_ModelInfo[nType].dwNumMat; nCntMat++)
		{
			// �}�e���A���f�[�^�ւ̃|�C���^���擾
			pMat = (D3DXMATERIAL*)g_ModelInfo[nType].pBuffMat->GetBufferPointer();

			// �}�e���A���ݒ�
			pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

			// �e�N�X�`���ݒ�
			pDevice->SetTexture(0, g_ModelInfo[nType].pTexture[nCntMat]);

			// �u���b�N(�p�[�c)�̕`��
			g_ModelInfo[nType].pMesh->DrawSubset(nCntMat);
		}

		// �}�e���A����߂�
		pDevice->SetMaterial(&matDef);
	}
}
//========================
// ��������
//========================
void SaveEdit(void)
{
	// �t�@�C���|�C���^��錾
	FILE* pFile;

	// �I�����ꂽ�t�@�C���p�X�ŏ����o��
	pFile = fopen(WRITE_FILENAME[g_CurrentFilepass], "wb");

	if (pFile != NULL)
	{// �t�@�C�����J������

		// �����o���`��錾
		fwrite(&g_Edit, sizeof(int), 1, pFile);

		// �z�u��������������
		for (int nCnt = 0; nCnt < g_Edit; nCnt++)
		{
			if (g_MapEdit[nCnt].isputblock)
			{// ���������o��

				// �ړ��ʂ�������
				g_MapEdit[nCnt].mapedit.move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

				// �����o��
				fwrite(&g_MapEdit[nCnt].mapedit, sizeof(MapEdit), 1, pFile);
			}
		}

		// �t�@�C�������
		fclose(pFile);
	}
	else
	{// �t�@�C�����J���Ȃ�������
		// ���b�Z�[�WBOX�̕\��
		MessageBox(NULL, "�J���܂���", "�G���[", MB_OK);

		return;
	}
}
//========================
// �ēǂݍ���
//========================
void ReloadEdit(void)
{
	// �t�@�C���|�C���^��錾
	FILE* pFile;

	// �t�@�C���J��
	pFile = fopen(WRITE_FILENAME[g_CurrentFilepass], "rb");

	// �t�@�C�����J������
	if (pFile != NULL)
	{
		// ������
		InitEditinfo();

		// �ǂݍ���
		fread(&g_Edit, sizeof(int), 1, pFile);

		for (int nCnt = 0; nCnt < g_Edit; nCnt++)
		{// �z�u��������������
			// �ǂݍ��ݏ���
			fread(&g_MapEdit[nCnt].mapedit, sizeof(MapEdit), 1, pFile);

			// �g�p����ɂ���
			g_MapEdit[nCnt].bUse = true;
			g_MapEdit[nCnt].isputblock = true;
		}

		// �g�p����
		g_MapEdit[g_Edit].bUse = true;
		g_MapEdit[g_Edit].isputblock = false;

		// �����false��
		isLoad = false;

		// �t�@�C�������
		fclose(pFile);
	}
	else
	{// �J���Ȃ�������
		return;
	}
}
//===============================
// X�t�@�C���̓ǂݍ��ݏ���
//===============================
void LoadXFile()
{
	// �f�o�C�X�|�C���^��錾
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// �ϐ��̏�����
	g_nTypeNum = 0; // ��ސ�
	g_fspeed = 0.0f;// �ړ���
	g_jump = 0.0f;	// �W�����v��

	// �t�@�C���|�C���^��錾
	FILE* pFile;

	// �J���t�@�C����
	pFile = fopen("data\\model.txt", "r");

	if (pFile != NULL)
	{// �J������

		// ���[�J���ϐ�-------------------------------
		char aString[256] = {};	// ������
		char aGomi[3] = {};		// �C�R�[����������
		int nCnt = 0;
		int nData = 0;
		//---------------------------------------------

		while (1)
		{
			// nData�ɑ������
			nData = fscanf(pFile, "%s", &aString[0]);

			if (strcmp(&aString[0],"BLOCKTYPE") == 0)
			{// BLOCKTYPE��ǂݎ����
				// �ǂݔ�΂�
				fscanf(pFile, "%s", &aGomi[0]);

				//��ނ̐���ǂݍ���
				fscanf(pFile, "%d", &g_nTypeNum);

			}

			else if (strcmp(&aString[0], "MOVE") == 0)
			{// MOVE��ǂ�

				// �ǂݔ�΂�
				fscanf(pFile, "%s", &aGomi[0]);

				// ��ނ̐���ǂݍ���
				fscanf(pFile, "%f", &g_fspeed);
			}

			else if (strcmp(&aString[0], "JUMP") == 0)
			{// JUMP��ǂ�

				// �ǂݔ�΂�
				fscanf(pFile, "%s", &aGomi[0]);

				// �W�����v�ʂ̐���ǂݍ���
				fscanf(pFile, "%f", &g_jump);
			}

			else if (strcmp(&aString[0],"MODEL_FILENAME") == 0)
			{// MODEL_FILENAME��ǂݎ������

				// =���΂�
				fscanf(pFile, "%s", &aGomi[0]);

				// �������ǂݍ���
				fscanf(pFile, "%s", &aString[0]);

				// ���f���t�@�C�����i�[�p�̕ϐ�
				char* filename[256] = {};	

				// �ǂݍ��񂾕������ۑ�����
				filename[0] = aString;

				// �t�@�C�������R�s�[
				strcpy(&g_ModelInfo[nCnt].FileName[0], filename[0]);

				// X�t�@�C���̓ǂݍ���
				D3DXLoadMeshFromX(filename[0],
					D3DXMESH_SYSTEMMEM,
					pDevice,
					NULL,
					&g_ModelInfo[nCnt].pBuffMat,
					NULL,
					&g_ModelInfo[nCnt].dwNumMat,
					&g_ModelInfo[nCnt].pMesh);
				
				// �}�e���A���f�[�^�ւ̃|�C���^
				D3DXMATERIAL* pMat;

				// �}�e���A���f�[�^�ւ̃|�C���^���擾
				pMat = (D3DXMATERIAL*)g_ModelInfo[nCnt].pBuffMat->GetBufferPointer();

				// �}�e���A����������
				for (int nCntMat = 0; nCntMat < (int)g_ModelInfo[nCnt].dwNumMat; nCntMat++)
				{
					if (pMat[nCntMat].pTextureFilename != NULL)
					{
						// �e�N�X�`���̓ǂݍ���
						D3DXCreateTextureFromFile(pDevice,
							pMat[nCntMat].pTextureFilename,
							&g_ModelInfo[nCnt].pTexture[nCntMat]);
					}
				}

				// ���[�J���ϐ��錾-----------------------------
				int nNumVtx;		// ���_��
				DWORD sizeFVF;		// ���_�t�H�[�}�b�g�̃T�C�Y
				BYTE* pVtxBuff;		// ���_�o�b�t�@�̃|�C���^
				//----------------------------------------------

				// ���_���̎擾
				nNumVtx = g_ModelInfo[nCnt].pMesh->GetNumVertices();

				// ���_�̃T�C�Y���擾
				sizeFVF = D3DXGetFVFVertexSize(g_ModelInfo[nCnt].pMesh->GetFVF());

				// ���_�o�b�t�@�����b�N
				g_ModelInfo[nCnt].pMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pVtxBuff);

				// ���_������
				for (int nCntEnemy = 0; nCntEnemy < nNumVtx; nCntEnemy++)
				{
					// ���_���W�̑��
					D3DXVECTOR3 Vtx = *(D3DXVECTOR3*)pVtxBuff;

					// ���_���W�̔�r
					if (Vtx.x > g_ModelInfo[nCnt].Vtxmax.x)
					{// x���ő�l���傫��������
						// ���
						g_ModelInfo[nCnt].Vtxmax.x = Vtx.x;
					}
					if (Vtx.y > g_ModelInfo[nCnt].Vtxmax.y)
					{// y���ő�l���傫��������
						// ���
						g_ModelInfo[nCnt].Vtxmax.y = Vtx.y;
					}
					if (Vtx.z > g_ModelInfo[nCnt].Vtxmax.z)
					{// z���ő�l���傫��������
						// ���
						g_ModelInfo[nCnt].Vtxmax.z = Vtx.z;
					}

					if (Vtx.x < g_ModelInfo[nCnt].Vtxmin.x)
					{// x���ŏ��l��菬����������
						// ���
						g_ModelInfo[nCnt].Vtxmin.x = Vtx.x;
					}
					if (Vtx.y < g_ModelInfo[nCnt].Vtxmin.y)
					{// y���ŏ��l��菬����������
						// ���
						g_ModelInfo[nCnt].Vtxmin.y = Vtx.y;
					}
					if (Vtx.z < g_ModelInfo[nCnt].Vtxmin.z)
					{// z���ŏ��l��菬����������
						// ���
						g_ModelInfo[nCnt].Vtxmin.z = Vtx.z;
					}

					// ���_�t�H�[�}�b�g�̃T�C�Y���i�߂�
					pVtxBuff += sizeFVF;
				}

				// �T�C�Y����
				g_ModelInfo[nCnt].size.x = g_ModelInfo[nCnt].Vtxmax.x - g_ModelInfo[nCnt].Vtxmin.x;
				g_ModelInfo[nCnt].size.y = g_ModelInfo[nCnt].Vtxmax.y - g_ModelInfo[nCnt].Vtxmin.y;
				g_ModelInfo[nCnt].size.z = g_ModelInfo[nCnt].Vtxmax.z - g_ModelInfo[nCnt].Vtxmin.z;

				// �A�����b�N
				g_ModelInfo[nCnt].pMesh->UnlockVertexBuffer();

				// nCnt���C���N�������g
				nCnt++;

			}

			if (nData == EOF)
			{// EOF��ǂݎ��
				break;
			}
		}
	}
	else
	{// �J���Ȃ�������
		return;
	}

	// �t�@�C�������
	fclose(pFile);

}
//===============================
// �G�f�B�^�[���f���z�u���̏��
//===============================
MAPMODELINFO* MapInfo()
{
	return &g_MapEdit[0];
}
//===============================
// �G�f�B�^�[���f�����̎擾
//===============================
EDITMODEL* GetBlockInfo(int type)
{
	return &g_ModelInfo[type];
}
//===============================
// �G�f�B�^�[���̎擾
//===============================
MapEdit* getMap()
{
	return &g_MapEdit[0].mapedit;
}
//================================
// �z�u�����̎擾
//================================
int ReturnEdit()
{
	return g_Edit;
}
//================================
// �z�u���ړ��ʂ̎擾
//================================
float ReturnSpeed()
{
	return g_fspeed;
}
//================================
// �ݒ肵����ސ��̎擾
//================================
int ReturnType()
{
	return g_nTypeNum;
}
//================================
// �ݒ肵���W�����v�ʂ̎擾
//================================
float ReturnJump()
{
	return g_jump;
}
//================================
// �t�@�C���p�X�i���o�[���擾
//================================
int Filepass()
{
	return g_CurrentFilepass;
}
//================================
// ������擾
//================================
bool IsGetLoad()
{
	return isLoad;
}
//================================
// �\���̏���������
//================================
void InitEditinfo()
{
	// �\���̕ϐ��̏�����
	for (int nCnt = 0; nCnt < MAX_EDITOBJ; nCnt++)
	{
		g_MapEdit[nCnt].mapedit.pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);  // ���W
		g_MapEdit[nCnt].mapedit.move = D3DXVECTOR3(0.0f, 0.0f, 0.0f); // �ړ���
		g_MapEdit[nCnt].bUse = false;						          // ���g�p���
		g_MapEdit[nCnt].mapedit.rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);  // �p�x
		g_MapEdit[nCnt].mapedit.nType = 0;							  // ���
		g_MapEdit[nCnt].isputblock = false;					          // �z�u���ꂽ��
		g_MapEdit[nCnt].mapedit.Scal = D3DXVECTOR3(1.0f, 1.0f, 1.0f); // �g�嗦
	}
}