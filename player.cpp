//=============================
//
// �v���C���[���� [player.cpp]
// Author: Asuma Nishio
//
//=============================

//***************************
// �C���N���[�h�t�@�C���錾
//***************************
#include "input.h"
#include "player.h"
#include "camera.h"
#include "shadow.h"
#include "block.h"
#include <stdio.h>
#include <string.h>

//***************************
// �̗̓o�[�̗񋓌^�錾
//***************************
typedef enum
{
	LIFE_FRAME = 0,   // �̗̓Q�[�W�̘g
	LIFE_BAR,		  // �̗̓Q�[�W
	LIFE_MAX
}LIFE;

//***************************
// �̗̓o�[�̍\���̐錾
//***************************
typedef struct
{
	D3DXVECTOR3 pos;	// ���W
	D3DXCOLOR col;		// �F
	int nType;			// ���
}LIFEBAR;

//********************************
// �v���g�^�C�v�錾
//********************************
void LoadModel(void);			// ���f���ǂݍ��ݏ���

//*****************************
// �O���[�o���ϐ��錾
//*****************************
PLAYER g_Player;										// �v���C���[�̍\���̕ϐ�
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffLifebar = NULL;		// ���_�o�b�t�@�̃|�C���^
LPDIRECT3DTEXTURE9 g_pTextureLifebar[LIFE_MAX] = {};	// �e�N�X�`���̃|�C���^
LIFEBAR g_aLifebar[LIFE_MAX];							// �̗̓Q�[�W�̏��
bool bPad;
bool isHit;

//=========================
// �v���C���[�̏���������
//=========================
void InitPlayer(void)
{
	// �f�o�C�X�|�C���^��錾
	LPDIRECT3DDEVICE9 pDevice;

	// �f�o�C�X���擾
	pDevice = GetDevice();

	// �ϐ��̏�����
	g_Player.pos = D3DXVECTOR3(-20.0f, 0.0f, -120.0f);			// ���W
	g_Player.posOld = D3DXVECTOR3(0.0f, 0.0f, 0.0f);			// �ߋ��̍��W
	g_Player.rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);				// �p�x
	g_Player.move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);				// �ړ���
	g_Player.rotDestPlayer = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// �ڕW
	g_Player.bjump = false;										// �W�����v
	g_Player.bLanding = false;									// ���n����
	g_Player.nIdxShadow = SetShadow(D3DXVECTOR3(g_Player.pos.x,0.0f,g_Player.pos.z), g_Player.rot,D3DXVECTOR3 (1.0f,0.0f,1.0f));// �e
	g_Player.bLoopMotion = true;								// ���[�v����
	g_Player.nNumKey = 0;										// �L�[�̑���
	g_Player.nCounterMotion = 0;								// ���[�V�����J�E���^�[
	g_Player.nKey = 0;											// ���݂̃L�[No
	g_Player.nNumModel = 0;									// �p�[�c�̑���
	g_Player.bDisp = true;										// �g�p���
	g_Player.state = PLAYERSTATE_NEUTRAL;						// �������
	g_Player.bWalk = false;										// �ҋ@���
	g_Player.nLife = 10;										// �����̗�
	g_Player.Pcon = PLAYERCONDITION_NORMAL;						// �������
	bPad = false;												// �p�b�h�̏��
	isHit = true;												// ����؂�ւ��t���O

	// ���f���ǂݍ���
	LoadModel();

	// �I�t�Z�b�g���l������
	for (int nOff = 0; nOff < g_Player.nNumModel; nOff++)
	{
		g_Player.aModel[nOff].Offpos = g_Player.aModel[nOff].pos;
		g_Player.aModel[nOff].Offrot = g_Player.aModel[nOff].rot;
	}

	// ���[�J���ϐ��錾-----------------------------
	int nNumVtx;		// ���_��
	DWORD sizeFVF;		// ���_�t�H�[�}�b�g�̃T�C�Y
	BYTE* pVtxBuff;		// ���_�o�b�t�@�̃|�C���^
	//----------------------------------------------

	for (int nCntModel = 0; nCntModel < g_Player.nNumModel; nCntModel++)
	{// ���f���̍ő吔����
		// ���_���̎擾
		nNumVtx = g_Player.aModel[nCntModel].pMeshModel->GetNumVertices();

		// ���_�̃T�C�Y���擾
		sizeFVF = D3DXGetFVFVertexSize(g_Player.aModel[nCntModel].pMeshModel->GetFVF());

		// ���_�o�b�t�@�����b�N
		g_Player.aModel[nCntModel].pMeshModel->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pVtxBuff);

		for (int nCnt = 0; nCnt < nNumVtx; nCnt++)
		{// ���_������for������
			// ���_���W�̑��
			D3DXVECTOR3 Vtx = *(D3DXVECTOR3*)pVtxBuff;

			// ���_���W�̔�r
			if (Vtx.x < g_Player.Vtxmin.x)
			{// x���ŏ��l��菬����������
				// ���
				g_Player.Vtxmin.x = Vtx.x;
			}
			else if (Vtx.x > g_Player.Vtxmax.x)
			{// x���ő�l���傫��������
				// ���
				g_Player.Vtxmax.x = Vtx.x;
			}

			if (Vtx.y < g_Player.Vtxmin.y)
			{// y���ŏ��l��菬����������
				// ���
				g_Player.Vtxmin.y = Vtx.y;
			}
			else if (Vtx.y > g_Player.Vtxmax.y)
			{// y���ő�l���傫��������
				// ���
				g_Player.Vtxmax.y = Vtx.y;
			}

			if (Vtx.z < g_Player.Vtxmin.z)
			{// z���ŏ��l��菬����������
				// ���
				g_Player.Vtxmin.z = Vtx.z;
			}
			else if (Vtx.z > g_Player.Vtxmax.z)
			{// z���ő�l���傫��������
				// ���
				g_Player.Vtxmax.z = Vtx.z;
			}

			//-----------------------------------------------
			if (Vtx.x < g_Player.aModel[nCntModel].Vtxmax.x)
			{// x���ő�l���傫��
				// ���
				g_Player.aModel[nCntModel].Vtxmax.x = Vtx.x;
			}
			else if (Vtx.y < g_Player.aModel[nCntModel].Vtxmax.y)
			{
				g_Player.aModel[nCntModel].Vtxmax.y = Vtx.y;
			}
			else if (Vtx.z < g_Player.aModel[nCntModel].Vtxmax.z)
			{
				g_Player.aModel[nCntModel].Vtxmax.z = Vtx.z;
			}

			if (Vtx.x > g_Player.aModel[nCntModel].Vtxmin.x)
			{// x���ő�l���傫��
				// ���
				g_Player.aModel[nCntModel].Vtxmin.x = Vtx.x;
			}
			else if (Vtx.y > g_Player.aModel[nCntModel].Vtxmin.y)
			{
				g_Player.aModel[nCntModel].Vtxmin.y = Vtx.y;
			}
			else if (Vtx.z > g_Player.aModel[nCntModel].Vtxmin.z)
			{
				g_Player.aModel[nCntModel].Vtxmin.z = Vtx.z;
			}
			//-------------------------------------------------------
			
			// ���_�t�H�[�}�b�g�̃T�C�Y���i�߂�
			pVtxBuff += sizeFVF;
		}

		// �T�C�Y��ۑ�
		g_Player.size.x = g_Player.Vtxmax.x - g_Player.Vtxmin.x;
		g_Player.size.y = g_Player.Vtxmax.y - g_Player.Vtxmin.y;
		g_Player.size.z = g_Player.Vtxmax.z - g_Player.Vtxmin.z;

		// ���f�����Ƃ̃T�C�Y��ۑ�
		g_Player.aModel[nCntModel].size.x = g_Player.aModel[nCntModel].Vtxmax.x - g_Player.aModel[nCntModel].Vtxmin.x;
		g_Player.aModel[nCntModel].size.y = g_Player.aModel[nCntModel].Vtxmax.y - g_Player.aModel[nCntModel].Vtxmin.y;
		g_Player.aModel[nCntModel].size.z = g_Player.aModel[nCntModel].Vtxmax.z - g_Player.aModel[nCntModel].Vtxmin.z;

		// �A�����b�N
		g_Player.aModel[nCntModel].pMeshModel->UnlockVertexBuffer();
	}

	// �}�e���A���f�[�^�ւ̃|�C���^
	D3DXMATERIAL* pMat;

	for (int nCnt = 0; nCnt < g_Player.nNumModel; nCnt++)
	{
		// �}�e���A���f�[�^�ւ̃|�C���^���擾
		pMat = (D3DXMATERIAL*)g_Player.aModel[nCnt].pBuffMatModel->GetBufferPointer();

		// �}�e���A����������
		for (int nCntMat = 0; nCntMat < (int)g_Player.aModel[nCnt].dwNumMatModel; nCntMat++)
		{
			if (pMat[nCntMat].pTextureFilename != NULL)
			{
				// �e�N�X�`���̓ǂݍ���
				D3DXCreateTextureFromFile(pDevice,
					pMat[nCntMat].pTextureFilename,
					&g_Player.aModel[nCnt].pTexture[nCntMat]);
			}
		}

		// �J���[������������
		g_Player.aModel[nCnt].Diffuse = pMat->MatD3D.Diffuse;
		g_Player.aModel[nCnt].FirstDiffuse = g_Player.aModel[nCnt].Diffuse;
	}	

	// ���C�t�o�[�̏�����
	InitLifeBar();
}
//=========================
// �v���C���[�̏I������
//=========================
void UninitPlayer(void)
{
	// ���b�V���̔j��
	for (int nCnt = 0; nCnt < g_Player.nNumModel; nCnt++)
	{
		if (g_Player.aModel[nCnt].pMeshModel != NULL)
		{
			g_Player.aModel[nCnt].pMeshModel->Release();
			g_Player.aModel[nCnt].pMeshModel = NULL;
		}

		if (g_Player.aModel[nCnt].pBuffMatModel != NULL)
		{
			g_Player.aModel[nCnt].pBuffMatModel->Release();
			g_Player.aModel[nCnt].pBuffMatModel = NULL;
		}

		for (int nCntModel = 0; nCntModel < (int)g_Player.aModel[nCnt].dwNumMatModel; nCntModel++)
		{
			if (g_Player.aModel[nCnt].pTexture[nCntModel] != NULL)
			{
				g_Player.aModel[nCnt].pTexture[nCntModel]->Release();
				g_Player.aModel[nCnt].pTexture[nCntModel] = NULL;
			}
		}
	}
	
	// ���C�t�o�[�̏I��
	UninitLifeBar();

}
//=========================
// �v���C���[�̍X�V����
//=========================
void UpdatePlayer(void)
{
	// �J�����̏����擾
	Camera* pCamera = GetCamera();

	
	// �W�����v�ʂ��擾
	float fJump = ReturnJump();

	bPad = false;

	switch (g_Player.Pcon)
	{
	case PLAYERCONDITION_NORMAL:
		break;

	case PLAYERCONDITION_DAMAGE:
		g_Player.nCntState--;

		if (g_Player.nCntState <= 0)
		{
			g_Player.Pcon = PLAYERCONDTION_APPER;
			g_Player.nCntState = 80;
		}
		break;

	case PLAYERCONDTION_APPER:
	{
		g_Player.nCntState--;

		for (int nCnt = 0; nCnt < g_Player.nNumModel; nCnt++)
		{
			if (g_Player.nCntState % 5 == 0)
			{
				//�����ɂ���
				g_Player.aModel[nCnt].Diffuse.a = 0.1f;

			}
			else
			{
				//�ʏ�F
				g_Player.aModel[nCnt].Diffuse.a = 1.0f;
			}

			if (g_Player.nCntState <= 0)
			{
				//�v���C���[��ʏ��Ԃ�
				//�ʏ�F
				g_Player.aModel[nCnt].Diffuse.a = 1.0f;
				g_Player.Pcon = PLAYERCONDITION_NORMAL;
			}

		}

	}
	break;

	default:
		break;
	}	

	// �X�e�B�b�N
	Stick();

	if (bPad == false)
	{
		if (GetKeyboardPress(DIK_A) || JoypadPress(JOYKEY_LEFT))
		{// A�L�[��������

			if (GetKeyboardPress(DIK_W) || JoypadPress(JOYKEY_UP))
			{// W�L�[��������
				g_Player.move.x += sinf(pCamera->rot.y - D3DX_PI * 0.25f) * MAX_MOVE;
				g_Player.move.z += cosf(pCamera->rot.y - D3DX_PI * 0.25f) * MAX_MOVE;
				g_Player.rotDestPlayer.y = pCamera->rot.y + (D3DX_PI * 0.75f);

				g_Player.state = PLAYERSTATE_MOVE;

			}
			else if (GetKeyboardPress(DIK_S) || JoypadPress(JOYKEY_DOWN))
			{// S�L�[��������
				g_Player.move.x -= sinf(pCamera->rot.y + D3DX_PI * 0.25f) * MAX_MOVE;
				g_Player.move.z -= cosf(pCamera->rot.y + D3DX_PI * 0.25f) * MAX_MOVE;
				g_Player.rotDestPlayer.y = pCamera->rot.y + (D3DX_PI * 0.25f);

				//�@�^�C�v�؂�ւ�
				g_Player.state = PLAYERSTATE_MOVE;
			}
			else
			{// A�L�[�݂̂�������
				g_Player.move.x -= sinf(pCamera->rot.y + (D3DX_PI * 0.5f)) * MAX_MOVE;
				g_Player.move.z -= cosf(pCamera->rot.y + (D3DX_PI * 0.5f)) * MAX_MOVE;
				g_Player.rotDestPlayer.y = pCamera->rot.y + (D3DX_PI * 0.5f);

				//�@�^�C�v�؂�ւ�
				g_Player.state = PLAYERSTATE_MOVE;

			}

			// �p�x�̐��K��
			if (g_Player.rot.y > D3DX_PI)
			{// D3DX_PI���傫���Ȃ�����
				g_Player.rot.y -= D3DX_PI * 2.0f;
			}

		}
		else if (GetKeyboardPress(DIK_D) || JoypadPress(JOYKEY_RIGHT))
		{// D�L�[��������
			if (GetKeyboardPress(DIK_W) || JoypadPress(JOYKEY_UP))
			{// W�L�[��������
				g_Player.move.x += sinf(pCamera->rot.y + D3DX_PI * 0.25f) * MAX_MOVE;
				g_Player.move.z += cosf(pCamera->rot.y + D3DX_PI * 0.25f) * MAX_MOVE;
				g_Player.rotDestPlayer.y = pCamera->rot.y - (D3DX_PI * 0.75f);

				//�@�^�C�v�؂�ւ�
				g_Player.state = PLAYERSTATE_MOVE;

			}

			else if (GetKeyboardPress(DIK_S) || JoypadPress(JOYKEY_DOWN))
			{// S�L�[��������
				g_Player.move.x -= sinf(pCamera->rot.y - D3DX_PI * 0.25f) * MAX_MOVE;
				g_Player.move.z -= cosf(pCamera->rot.y - D3DX_PI * 0.25f) * MAX_MOVE;
				g_Player.rotDestPlayer.y = pCamera->rot.y - (D3DX_PI * 0.25f);

				//�@�^�C�v�؂�ւ�
				g_Player.state = PLAYERSTATE_MOVE;

			}

			else
			{// D�L�[�̂݉�����
				g_Player.move.x += sinf(pCamera->rot.y + (D3DX_PI * 0.5f)) * MAX_MOVE;
				g_Player.move.z += cosf(pCamera->rot.y + (D3DX_PI * 0.5f)) * MAX_MOVE;
				g_Player.rotDestPlayer.y = pCamera->rot.y - (D3DX_PI * 0.5f);

				//�@�^�C�v�؂�ւ�
				g_Player.state = PLAYERSTATE_MOVE;
			}

			// �p�x�̐��K��
			if (g_Player.rot.y > D3DX_PI)
			{// D3DX_PI���傫���Ȃ�����
				g_Player.rot.y -= D3DX_PI * 2.0f;
			}

		}
		else if (GetKeyboardPress(DIK_W) || JoypadPress(JOYKEY_UP))
		{// W�L�[��������
			//�@�^�C�v�؂�ւ�
			g_Player.state = PLAYERSTATE_MOVE;

			g_Player.move.x += sinf(pCamera->rot.y) * MAX_MOVE;
			g_Player.move.z += cosf(pCamera->rot.y) * MAX_MOVE;
			g_Player.rotDestPlayer.y = pCamera->rot.y - (D3DX_PI);

			// �p�x�𐳋K��
			if (g_Player.rot.y < -D3DX_PI)
			{// D3DX_PI��菬�����Ȃ�����
				g_Player.rot.y += D3DX_PI * 2.0f;
			}

		}
		else if (GetKeyboardPress(DIK_S) || JoypadPress(JOYKEY_DOWN))
		{// S�L�[��������
			//�@�^�C�v�؂�ւ�
			g_Player.state = PLAYERSTATE_MOVE;


			g_Player.move.x -= sinf(pCamera->rot.y) * MAX_MOVE;
			g_Player.move.z -= cosf(pCamera->rot.y) * MAX_MOVE;
			g_Player.rotDestPlayer.y = pCamera->rot.y;

			// �p�x�̐��K��
			if (g_Player.rot.y > D3DX_PI)
			{// D3DX_PI���傫���Ȃ�����
				g_Player.rot.y -= D3DX_PI * 2.0f;
			}

		}
		else
		{
			if (g_Player.state != PLAYERSTATE_ACTION)
			{
				//�@�^�C�v�؂�ւ�
				g_Player.state = PLAYERSTATE_NEUTRAL;
			}

		}

	}
	
	if (KeyboardTrigger(DIK_RETURN) || JoypadTrigger(JOYKEY_X))
	{// ENTER�L�[��������
		// ���[�V�����ύX
		g_Player.nKey = 0;
		g_Player.state = PLAYERSTATE_ACTION;

	}

	if (g_Player.rotDestPlayer.y - g_Player.rot.y > D3DX_PI)
	{// ����]
		// �p�x
		g_Player.rot.y = g_Player.rot.y + D3DX_PI * 2.0f;
	}
	else if (g_Player.rot.y - g_Player.rotDestPlayer.y > D3DX_PI)
	{// �E��]
		// �p�x
		g_Player.rot.y = g_Player.rot.y - D3DX_PI * 2.0f;

	}

	// ���݂̊p�x
	g_Player.rot.y += (g_Player.rotDestPlayer.y - g_Player.rot.y) * 0.25f;

	if (g_Player.bjump == false)
	{// �W�����v���Ă��Ȃ��Ē��n���Ă��Ȃ��Ƃ�
		if (KeyboardTrigger(DIK_SPACE) == true || JoypadTrigger(JOYKEY_A) == true)
		{
			g_Player.bjump = true;
			g_Player.bLanding = false;

			// �W�����v�L�[�������ꂽ
			g_Player.move.y = fJump;
		}
	}

	// �W�����v���g���Ă���
	if (g_Player.bjump == true)
	{
		// ���[�V�����W�����v
		g_Player.state = PLAYERSTATE_JUMP;

		if (g_Player.bLanding == true)
		{
			// ���n������
			g_Player.state = PLAYERSTATE_NEUTRAL;

			if (g_Player.state != PLAYERSTATE_JUMP)
			{
				g_Player.bLoopMotion = true;
			}

			g_Player.bjump = false;
		}
	}

	//�d��
	g_Player.move.y -= 0.7f;               // �d�͉��Z

	// �ړ��ʂ̌���
	g_Player.move.x += (0.0f - g_Player.move.x) * 0.5f;
	g_Player.move.z += (0.0f - g_Player.move.z) * 0.5f;

	// �O��̈ʒu��ۑ�
	g_Player.posOld = g_Player.pos;

	// �ʒu���X�V
	g_Player.pos.x += g_Player.move.x;
	g_Player.pos.z += g_Player.move.z;
	g_Player.pos.y += g_Player.move.y;


	if (isHit)
	{
		// �u���b�N�Ƃ̓����蔻��
		CollisionBlock(&g_Player.pos,
			&g_Player.posOld,
			&g_Player.move,
			&g_Player.size);
	}

	//// ���b�V���Ƃ̓����蔻��
	//CollisionMeshWall();


	if (0.0f >= g_Player.pos.y)
	{// �n�ʂ�艺
		g_Player.pos.y = 0.0f;
		g_Player.bLanding = true;
		g_Player.move.y = 0.0f;

	}

	// �e�̍��W�X�V�ݒ�
	SetPositionShadow(g_Player.nIdxShadow, D3DXVECTOR3(g_Player.pos.x,0.0f,g_Player.pos.z));

	// ���[�V�����̍X�V
	UpdateMotionPlayer();

	// ���C�t�o�[�̍X�V
	UpdateLifeBar();
}
//=========================
// �v���C���[�̕`�揈��
//=========================
void DrawPlayer(void)
{
	// ���[�h�̎擾
	MODE nMode = GetMode();

	if (nMode == MODE_GAME)
	{
		// ���C�t�o�[�̕`��
		DrawLifeBar();
	}

	// �f�o�C�X�|�C���^��錾
	LPDIRECT3DDEVICE9 pDevice;

	// �f�o�C�X���擾
	pDevice = GetDevice();

	// �v�Z�p�̃}�g���b�N�X��錾
	D3DXMATRIX mtxRot, mtxTrans, mtxScall;

	// ���݂̃}�e���A����ۑ�
	D3DMATERIAL9 matDef;

	// �}�e���A���f�[�^�ւ̃|�C���^
	D3DXMATERIAL* pMat;

	// ���[���h�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&g_Player.mtxWorld);

	// �����𔽉f
	D3DXMatrixRotationYawPitchRoll(&mtxRot, g_Player.rot.y, g_Player.rot.x, g_Player.rot.z);
	D3DXMatrixMultiply(&g_Player.mtxWorld, &g_Player.mtxWorld, &mtxRot);

	// �ʒu�𔽉f
	D3DXMatrixTranslation(&mtxTrans, g_Player.pos.x, g_Player.pos.y, g_Player.pos.z);
	D3DXMatrixMultiply(&g_Player.mtxWorld, &g_Player.mtxWorld, &mtxTrans);

	// ���[���h�}�g���b�N�X�̐ݒ�
	pDevice->SetTransform(D3DTS_WORLD, &g_Player.mtxWorld);

	// ���݂̃}�g���b�N�X�̎擾
	pDevice->GetMaterial(&matDef);

	if (g_Player.bDisp)
	{
		// �S�p�[�c����`��
		for (int nCnt = 0; nCnt < g_Player.nNumModel; nCnt++)
		{
			// ���[�J���ϐ�-------------------------------------
			D3DXMATRIX mtxWorldModel, mtxTransModel, mtxRotModel;	// ���f���v�Z�p
			D3DXMATRIX mtxParnet;									// �e�̃}�g���b�N�X

			// �p�[�c�̃��[���h�}�g���b�N�X�̏�����
			D3DXMatrixIdentity(&g_Player.aModel[nCnt].mtxWorld);

			// �p�[�c�̌����𔽉f
			D3DXMatrixRotationYawPitchRoll(&mtxRotModel, g_Player.aModel[nCnt].rot.y, g_Player.aModel[nCnt].rot.x, g_Player.aModel[nCnt].rot.z);
			D3DXMatrixMultiply(&g_Player.aModel[nCnt].mtxWorld, &g_Player.aModel[nCnt].mtxWorld, &mtxRotModel);

			// �p�[�c�̈ʒu�𔽉f
			D3DXMatrixTranslation(&mtxTransModel, g_Player.aModel[nCnt].pos.x, g_Player.aModel[nCnt].pos.y, g_Player.aModel[nCnt].pos.z);
			D3DXMatrixMultiply(&g_Player.aModel[nCnt].mtxWorld, &g_Player.aModel[nCnt].mtxWorld, &mtxTransModel);

			// �p�[�c�̐e�}�g���b�N�X��ݒ�
			if (g_Player.aModel[nCnt].nIndxModelParent != -1)
			{// �e���f��������ꍇ
				mtxParnet = g_Player.aModel[g_Player.aModel[nCnt].nIndxModelParent].mtxWorld;
			}
			else
			{// �e���f�����Ȃ��ꍇ
				mtxParnet = g_Player.mtxWorld;
			}

			// �p�[�c�̃��[���h�}�g���b�N�X�Ɛe�̃��[���h�}�g���b�N�X����Z
			D3DXMatrixMultiply(&g_Player.aModel[nCnt].mtxWorld,
				&g_Player.aModel[nCnt].mtxWorld,
				&mtxParnet);

			// ���[���h�}�g���b�N�X�̐ݒ�
			pDevice->SetTransform(D3DTS_WORLD, &g_Player.aModel[nCnt].mtxWorld);

			// �}�e���A����������
			for (int nCntMat = 0; nCntMat < (int)g_Player.aModel[nCnt].dwNumMatModel; nCntMat++)
			{
				// �}�e���A���f�[�^�ւ̃|�C���^���擾
				pMat = (D3DXMATERIAL*)g_Player.aModel[nCnt].pBuffMatModel->GetBufferPointer();

				pMat[nCntMat].MatD3D.Diffuse = g_Player.aModel[nCnt].Diffuse;

				// �}�e���A���ݒ�
				pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

				// �e�N�X�`���ݒ�
				pDevice->SetTexture(0, g_Player.aModel[nCnt].pTexture[nCntMat]);

				// ���f��(�p�[�c)�̕`��
				g_Player.aModel[nCnt].pMeshModel->DrawSubset(nCntMat);

			}

		}
		// �}�e���A����߂�
		pDevice->SetMaterial(&matDef);
	}

}
//=========================
// �v���C���[�̎擾����
//=========================
PLAYER* GetPlayer(void)
{
	return &g_Player;
}
//================================
// �v���C���[�̃��[�V�����X�V����
//================================
void UpdateMotionPlayer(void)
{
	//���f���p�[�c�̍X�V
	for (int nCnt = 0; nCnt < g_Player.nNumModel; nCnt++)
	{
		// ���̃L�[���v�Z
		int nNextKey = (g_Player.nKey + 1) % g_Player.aMotionInfo[g_Player.state].nNumKey;

		// ���E�`�F�b�N
		if (g_Player.nKey >= g_Player.aMotionInfo[g_Player.state].nNumKey)
		{// g_Player.nNumKey���傫���Ȃ� �܂���nNextKey��g_Player.nNumKey���傫���Ȃ�����

			// �L�[����߂�
			g_Player.nKey = 0;	
		}

		// �L�[��񂩂�ʒu�ƌ������Z�o
		D3DXVECTOR3 posMotion, rotMotion;

		// �������v�Z����
		posMotion.x = g_Player.aMotionInfo[g_Player.state].aKeyInfo[nNextKey].aKey[nCnt].fPosX - g_Player.aMotionInfo[g_Player.state].aKeyInfo[g_Player.nKey].aKey[nCnt].fPosX;
		posMotion.y = g_Player.aMotionInfo[g_Player.state].aKeyInfo[nNextKey].aKey[nCnt].fPosY - g_Player.aMotionInfo[g_Player.state].aKeyInfo[g_Player.nKey].aKey[nCnt].fPosY;
		posMotion.z = g_Player.aMotionInfo[g_Player.state].aKeyInfo[nNextKey].aKey[nCnt].fPosZ - g_Player.aMotionInfo[g_Player.state].aKeyInfo[g_Player.nKey].aKey[nCnt].fPosZ;

		rotMotion.x = g_Player.aMotionInfo[g_Player.state].aKeyInfo[nNextKey].aKey[nCnt].fRotX - g_Player.aMotionInfo[g_Player.state].aKeyInfo[g_Player.nKey].aKey[nCnt].fRotX;
		rotMotion.y = g_Player.aMotionInfo[g_Player.state].aKeyInfo[nNextKey].aKey[nCnt].fRotY - g_Player.aMotionInfo[g_Player.state].aKeyInfo[g_Player.nKey].aKey[nCnt].fRotY;
		rotMotion.z = g_Player.aMotionInfo[g_Player.state].aKeyInfo[nNextKey].aKey[nCnt].fRotZ - g_Player.aMotionInfo[g_Player.state].aKeyInfo[g_Player.nKey].aKey[nCnt].fRotZ;

		// ���߂�l��ۑ�����ϐ���錾
		D3DXVECTOR3 Answer, Answer1;

		// ��ԌW�����v�Z
		float fDis = (float)g_Player.nCounterMotion / g_Player.aMotionInfo[g_Player.state].aKeyInfo[g_Player.nKey].nFrame;

		Answer.x =  (g_Player.aMotionInfo[g_Player.state].aKeyInfo[g_Player.nKey].aKey[nCnt].fPosX + posMotion.x * fDis);
		Answer.y =  (g_Player.aMotionInfo[g_Player.state].aKeyInfo[g_Player.nKey].aKey[nCnt].fPosY + posMotion.y * fDis);
		Answer.z =  (g_Player.aMotionInfo[g_Player.state].aKeyInfo[g_Player.nKey].aKey[nCnt].fPosZ + posMotion.z * fDis);

		Answer1.x = (g_Player.aMotionInfo[g_Player.state].aKeyInfo[g_Player.nKey].aKey[nCnt].fRotX + rotMotion.x * fDis);
		Answer1.y = (g_Player.aMotionInfo[g_Player.state].aKeyInfo[g_Player.nKey].aKey[nCnt].fRotY + rotMotion.y * fDis);
		Answer1.z = (g_Player.aMotionInfo[g_Player.state].aKeyInfo[g_Player.nKey].aKey[nCnt].fRotZ + rotMotion.z * fDis);

		// �p�[�c�̈ʒu
		g_Player.aModel[nCnt].pos = g_Player.aModel[nCnt].Offpos + Answer;
		g_Player.aModel[nCnt].rot = g_Player.aModel[nCnt].Offrot + Answer1;
	}
	
	if (g_Player.bLoopMotion)
	{
		g_Player.nCounterMotion++;	// �C���N�������g�����ăJ�E���^�[�����Z
	}

	if (g_Player.aMotionInfo[g_Player.state].bLoop == false)
	{
		if (g_Player.nKey >= g_Player.aMotionInfo[g_Player.state].nNumKey -1)
		{
			g_Player.nKey = g_Player.aMotionInfo[g_Player.state].nNumKey - 1;
			g_Player.bLoopMotion = false;
		}
	}

	if (g_Player.state == PLAYERSTATE_ACTION && g_Player.bLoopMotion == false)
	{
		g_Player.state = PLAYERSTATE_NEUTRAL;
		g_Player.bLoopMotion = true;
		
	}

	if (g_Player.nCounterMotion >= g_Player.aMotionInfo[g_Player.state].aKeyInfo[g_Player.nKey].nFrame)
	{// �t���[�����̏���ɒB������

		g_Player.nCounterMotion = 0;	// �J�E���^�[�������l�ɖ߂�

		g_Player.nKey++;				// �L�[�����C���N�������g
	}

}

//=====================================
// �v���C���[�̃��f���ǂݍ��ݏ���
//=====================================
void LoadModel(void)
{
	// �f�o�C�X�|�C���^��錾
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// ���[�J���ϐ�
	int nIdx = 0;					// �C���f�b�N�X
	int NumKey = 0;					// �L�[��
	int nCnt = 0;					// ���f���J�E���g�p
	int nNumParts = 0;				// �p�[�c���i�[�p
	int nCntMotion = 0;				// ���[�V�����J�E���g�p
	int nCntKey = 0;				// �L�[�J�E���g�p
	int nCntPosKey = 0;				// pos�J�E���g
	int nCntRotkey = 0;				// rot�J�E���g

	// �t�@�C���|�C���^��錾
	FILE* pFile;

	// �t�@�C�����J��
	pFile = fopen("data/motion.txt", "r");

	if (pFile != NULL)
	{//�@NULL ����Ȃ�
		char aString[MAX_WORD];

		while (1)
		{
			// �ǂݔ�΂�
			fscanf(pFile, "%s", &aString[0]);

			if (strcmp(&aString[0], "SCRIPT") == 0)
			{// SCRIPT��ǂݎ������
				while (1)
				{
					// �ǂݔ�΂�
					fscanf(pFile, "%s", &aString[0]);

					if (strcmp(&aString[0], "NUM_MODEL") == 0)
					{// NUM_MODEL��ǂݎ������
						fscanf(pFile, "%s", &aString[0]);

						if (strcmp(&aString[0], "=") == 0)
						{// �l����
							fscanf(pFile, "%d", &g_Player.nNumModel);
						}
					}

					else if (strcmp(&aString[0], "MODEL_FILENAME") == 0)
					{
						// MODEL_FILENAME��ǂݎ������
						fscanf(pFile, "%s", &aString[0]);

						if (strcmp(&aString[0], "=") == 0)
						{// ���
							// �������ǂݍ���
							fscanf(pFile, "%s", &aString[0]);

							const char* MODEL_FILE = {};	// ���f���t�@�C�����i�[�p�̕ϐ�

							// �ǂݍ��񂾕������ۑ�����
							MODEL_FILE = aString;

							//X�t�@�C���̓ǂݍ���
							D3DXLoadMeshFromX(MODEL_FILE,
								D3DXMESH_SYSTEMMEM,
								pDevice,
								NULL,
								&g_Player.aModel[nCnt].pBuffMatModel,
								NULL,
								&g_Player.aModel[nCnt].dwNumMatModel,
								&g_Player.aModel[nCnt].pMeshModel);

							nCnt++; // nCnt���C���N�������g
						}
					}
					else if (strcmp(&aString[0], "CHARACTERSET") == 0)
					{
						while (1)
						{
							// �������ǂݔ�΂�
							fscanf(pFile, "%s", &aString[0]);

							if (strcmp(&aString[0], "NUM_PARTS") == 0)
							{// NUM_PARTS��ǂݎ������

								fscanf(pFile, "%s", &aString[0]);

								if (strcmp(&aString[0], "=") == 0)
								{// �l����
									fscanf(pFile, "%d", &nNumParts);
								}
							}

							else if (strcmp(&aString[0], "PARTSSET") == 0)
							{
								while (1)
								{
									// �������ǂݔ�΂�
									fscanf(pFile, "%s", &aString[0]);

									if (strcmp(&aString[0], "INDEX") == 0)
									{// INDEX��ǂݎ������

										fscanf(pFile, "%s", &aString[0]);

										if (strcmp(&aString[0], "=") == 0)
										{// ���
											fscanf(pFile, "%d", &nIdx);
										}
									}

									if (strcmp(&aString[0], "PARENT") == 0)
									{// PARENT��ǂݎ������

										fscanf(pFile, "%s", &aString[0]);

										if (strcmp(&aString[0], "=") == 0)
										{// ���
											// �y�A�l���g
											fscanf(pFile, "%d", &g_Player.aModel[nIdx].nIndxModelParent);
										}
									}


									if (strcmp(&aString[0], "POS") == 0)
									{// INDEX��ǂݎ������

										fscanf(pFile, "%s", &aString[0]);

										if (strcmp(&aString[0], "=") == 0)
										{// ���W����
											fscanf(pFile, "%f", &g_Player.aModel[nIdx].pos.x);
											fscanf(pFile, "%f", &g_Player.aModel[nIdx].pos.y);
											fscanf(pFile, "%f", &g_Player.aModel[nIdx].pos.z);
										}
									}

									if (strcmp(&aString[0], "ROT") == 0)
									{// INDEX��ǂݎ������

										fscanf(pFile, "%s", &aString[0]);

										if (strcmp(&aString[0], "=") == 0)
										{// �p�x����
											fscanf(pFile, "%f", &g_Player.aModel[nIdx].rot.x);
											fscanf(pFile, "%f", &g_Player.aModel[nIdx].rot.y);
											fscanf(pFile, "%f", &g_Player.aModel[nIdx].rot.z);
										}
									}

									if (strcmp(&aString[0], "END_PARTSSET") == 0)
									{// END_PARTSSET��ǂݎ������
										// while�𔲂���
										break;
									}

								}// while����
							}
							else if (strcmp(&aString[0], "END_CHARACTERSET") == 0)
							{
								break;
							}
						}
					}
					else if (strcmp(&aString[0], "MOTIONSET") == 0)
					{// MOTIONSET��ǂݎ������


						while (1)
						{
							// ������ǂݔ�΂�
							fscanf(pFile, "%s", &aString[0]);

							if (strcmp(aString, "LOOP") == 0)
							{// LOOP ��ǂݎ������
								// ������ǂݔ�΂�
								fscanf(pFile, "%s", &aString[0]);

								if (strcmp(&aString[0], "=") == 0)
								{// = ��ǂݎ������
									// �l����
									fscanf(pFile, "%d", &g_Player.aMotionInfo[nCntMotion].bLoop);
								}
							}

							else if (strcmp(aString, "NUM_KEY") == 0)
							{// NUM_KEY��ǂݎ������
								// ������ǂݔ�΂�
								fscanf(pFile, "%s", &aString[0]);

								if (strcmp(&aString[0], "=") == 0)
								{// = ��ǂݎ������
									// �l����
									fscanf(pFile, "%d", &g_Player.aMotionInfo[nCntMotion].nNumKey);
								}

								while (nCntKey < g_Player.aMotionInfo[nCntMotion].nNumKey)
								{
									// ������ǂݔ�΂�
									fscanf(pFile, "%s", &aString[0]);

									if (strcmp(aString, "KEYSET") == 0)
									{// KEYSET��ǂݎ������


										while (1)
										{
											// ������ǂݔ�΂�
											fscanf(pFile, "%s", &aString[0]);


											if (strcmp(&aString[0], "FRAME") == 0)
											{// FRAME��ǂݎ������
												// ������ǂݔ�΂�
												fscanf(pFile, "%s", &aString[0]);

												if (strcmp(&aString[0], "=") == 0)
												{// �l����
													fscanf(pFile, "%d", &g_Player.aMotionInfo[nCntMotion].aKeyInfo[nCntKey].nFrame);
													break;
												}
											}

										}

										while (1)
										{
											// ������ǂݔ�΂�
											fscanf(pFile, "%s", &aString[0]);
											if (strcmp(&aString[0], "KEY") == 0)
											{// KEY��ǂ݂Ƃ�����
												while (1)
												{
													// ������ǂݔ�΂�
													fscanf(pFile, "%s", &aString[0]);

													if (strcmp(&aString[0], "POS") == 0)
													{// POS��ǂݎ������
														// ������ǂݔ�΂�
														fscanf(pFile, "%s", &aString[0]);

														if (strcmp(&aString[0], "=") == 0)
														{// �l����
															fscanf(pFile, "%f", &g_Player.aMotionInfo[nCntMotion].aKeyInfo[nCntKey].aKey[nCntPosKey].fPosX);
															fscanf(pFile, "%f", &g_Player.aMotionInfo[nCntMotion].aKeyInfo[nCntKey].aKey[nCntPosKey].fPosY);
															fscanf(pFile, "%f", &g_Player.aMotionInfo[nCntMotion].aKeyInfo[nCntKey].aKey[nCntPosKey].fPosZ);
															nCntPosKey++;		// �C���N�������g
														}
													}

													else if (strcmp(&aString[0], "ROT") == 0)
													{// ROT��ǂݎ������
														// ������ǂݔ�΂�
														fscanf(pFile, "%s", &aString[0]);

														if (strcmp(&aString[0], "=") == 0)
														{// �l����
															fscanf(pFile, "%f", &g_Player.aMotionInfo[nCntMotion].aKeyInfo[nCntKey].aKey[nCntRotkey].fRotX);
															fscanf(pFile, "%f", &g_Player.aMotionInfo[nCntMotion].aKeyInfo[nCntKey].aKey[nCntRotkey].fRotY);
															fscanf(pFile, "%f", &g_Player.aMotionInfo[nCntMotion].aKeyInfo[nCntKey].aKey[nCntRotkey].fRotZ);
															nCntRotkey++;		// �C���N�������g
														}
													}

													else if (strcmp(&aString[0], "END_KEY") == 0)
													{// END_KEY��ǂݎ������
														break;
													}
												}
											}
											else if (strcmp(&aString[0], "END_KEYSET") == 0)
											{// END_KEYSET��ǂݎ������
												nCntRotkey = 0;
												nCntPosKey = 0;
												nCntKey++;			// �C���N�������g
												break;
											}


										}

									}

								}
							}

							if (strcmp(&aString[0], "END_MOTIONSET") == 0)
							{// END_MOTIONSET��ǂݎ������
								nCntMotion++;		// ���[�V�����̍X�V
								nCntKey = 0;		// 0����n�߂�
								break;
							}
						}
					}

					else if (strcmp(&aString[0], "END_SCRIPT") == 0)
					{
						// �t�@�C�������
						fclose(pFile);

						break;
					}
					else
					{
						continue;
					}
				}// while����

				break;
			}
		}// while����
	}
}
//=========================
// ���C�t�o�[�̏���������
//=========================
 void InitLifeBar(void)
 {
	 // �f�o�C�X�ւ̃|�C���^
	 LPDIRECT3DDEVICE9 pDevice = GetDevice();		

	 // ���_���̃|�C���^
	 VERTEX_2D* pVtx;				

	 // �e�N�X�`���̓ǂݍ���
	 D3DXCreateTextureFromFile(pDevice,
		 "data\\TEXTURE\\ber.png",
		 &g_pTextureLifebar[0]);

	 // �e�N�X�`���̓ǂݍ���
	 D3DXCreateTextureFromFile(pDevice,
		 "data\\TEXTURE\\lifebar.png",
		 &g_pTextureLifebar[1]);

	 // �\���̂̏�����
	 for (int nCnt = 0; nCnt < LIFE_MAX; nCnt++)
	 {
		 g_aLifebar[nCnt].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);	// �J���[
		 g_aLifebar[nCnt].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// ���W
	 }

	 g_aLifebar[LIFE_FRAME].nType = LIFE_FRAME;	// �o�[�̘g
	 g_aLifebar[LIFE_BAR].nType = LIFE_BAR;		// �o�[�̃Q�[�W

	 // ���_�o�b�t�@�̐���
	 pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4 * LIFE_MAX,
		 D3DUSAGE_WRITEONLY, FVF_VERTEX_2D,
		 D3DPOOL_MANAGED,
		 &g_pVtxBuffLifebar,
		 NULL);

	 // ���_�o�b�t�@�����b�N��,���_���ւ̃|�C���^���擾
	 g_pVtxBuffLifebar->Lock(0, 0, (void**)&pVtx, 0);

	 for (int nCnt1 = 0; nCnt1 < LIFE_MAX; nCnt1++)
	 {
		 // ���_���W�̐ݒ�
		 pVtx[0].pos = D3DXVECTOR3(15.0f, 5.0f, 0.0f);							// 1�ڂ̒��_���
		 pVtx[1].pos = D3DXVECTOR3(15.0f + (g_Player.nLife * 40), 5.0f, 0.0f);	// 2�ڂ̒��_���
		 pVtx[2].pos = D3DXVECTOR3(15.0f, 55.0f, 0.0f);							// 3�ڂ̒��_���
		 pVtx[3].pos = D3DXVECTOR3(15.0f + (g_Player.nLife * 40), 55.0f, 0.0f); // 4�ڂ̒��_���

		 // rhw�̐ݒ�(1.0f�ŌŒ�)
		 pVtx[0].rhw = 1.0f;
		 pVtx[1].rhw = 1.0f;
		 pVtx[2].rhw = 1.0f;
		 pVtx[3].rhw = 1.0f;

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

		 // �|�C���^�����炷
		 pVtx += 4;
	 }

	 //�A�����b�N
	 g_pVtxBuffLifebar->Unlock();

 }
//=========================
// ���C�t�o�[�̏I������
//=========================
 void UninitLifeBar(void)
 {
	 for (int nCnt = 0; nCnt < LIFE_MAX; nCnt++)
	 {
		 // �e�N�X�`���̔j��
		 if (g_pTextureLifebar[nCnt] != NULL)
		 {
			 g_pTextureLifebar[nCnt]->Release();
			 g_pTextureLifebar[nCnt] = NULL;
		 }
	 }

	 // ���_�o�b�t�@�̔j��
	 if (g_pVtxBuffLifebar != NULL)
	 {
		 g_pVtxBuffLifebar->Release();
		 g_pVtxBuffLifebar = NULL;
	 }

 }
//=========================
// ���C�t�o�[�̍X�V����
//=========================
 void UpdateLifeBar(void)
 {
	 // ���_���̃|�C���^
	 VERTEX_2D* pVtx;		

	 // ���_�o�b�t�@�����b�N��,���_���ւ̃|�C���^���擾
	 g_pVtxBuffLifebar->Lock(0, 0, (void**)&pVtx, 0);

	 for (int nCnt1 = 0; nCnt1 < LIFE_MAX; nCnt1++)
	 {
		 if (g_aLifebar[nCnt1].nType == LIFE_BAR)
		 {
			 // ���_���W�̐ݒ�
			 pVtx[0].pos = D3DXVECTOR3(15.0f, 5.0f, 0.0f);							// 1�ڂ̒��_���
			 pVtx[1].pos = D3DXVECTOR3(15.0f + (g_Player.nLife * 40), 5.0f, 0.0f);	// 2�ڂ̒��_���
			 pVtx[2].pos = D3DXVECTOR3(15.0f, 55.0f, 0.0f);							// 3�ڂ̒��_���
			 pVtx[3].pos = D3DXVECTOR3(15.0f + (g_Player.nLife * 40), 55.0f, 0.0f);	// 4�ڂ̒��_���

			 // rhw�̐ݒ�(1.0f�ŌŒ�)
			 pVtx[0].rhw = 1.0f;
			 pVtx[1].rhw = 1.0f;
			 pVtx[2].rhw = 1.0f;
			 pVtx[3].rhw = 1.0f;

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

		 }

		 // ���_��񕪐i�߂�
		 pVtx += 4;
	 }

	 // �A�����b�N
	 g_pVtxBuffLifebar->Unlock();

 }
//=========================
// ���C�t�o�[�̕`�揈��
//=========================
 void DrawLifeBar(void)
 {
	 // �f�o�C�X�ւ̃|�C���^
	 LPDIRECT3DDEVICE9 pDevice = GetDevice();		

	 // ���_�o�b�t�@���f�[�^�X�g���[���ɐݒ�
	 pDevice->SetStreamSource(0, g_pVtxBuffLifebar, 0, sizeof(VERTEX_2D));

	 // ���_�t�H�[�}�b�g�̐ݒ�
	 pDevice->SetFVF(FVF_VERTEX_2D);

	 for (int nCnt = 0; nCnt < LIFE_MAX; nCnt++)
	 {
		 // �e�N�X�`���̐ݒ�
		 pDevice->SetTexture(0, g_pTextureLifebar[nCnt]);

		 // �|���S���`��
		 pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, nCnt * 4, 2);
	 }
 }
 //======================
 // �X�e�B�b�N
 //======================
 void Stick(void)
 {
	 // �X�e�B�b�N
	 XINPUT_STATE* pStick = GetJoyStickAngle();

	 Camera* pCamera = GetCamera();

	 if (GetJoyStick())
	 {
		 float LStickAngleY = pStick->Gamepad.sThumbLY;
		 float LStickAngleX = pStick->Gamepad.sThumbLX;

		 float DeadZone = 10920;
		 float fMag = sqrtf((LStickAngleX * LStickAngleX) + (LStickAngleY * LStickAngleY));


		 if (fMag > DeadZone)
		 {
			 bPad = true;
			 float normalizeX = (LStickAngleX / fMag);
			 float normalizeY = (LStickAngleY / fMag);


			 float MoveX = normalizeX * cosf(-pCamera->rot.y) - normalizeY * sinf(-pCamera->rot.y);
			 float MoveZ = normalizeX * sinf(-pCamera->rot.y) + normalizeY * cosf(-pCamera->rot.y);

			 g_Player.move.x += MoveX * MAX_MOVE;
			 g_Player.move.z += MoveZ * MAX_MOVE;

			 g_Player.rotDestPlayer.y = atan2f(-MoveX, -MoveZ);

			 g_Player.state = PLAYERSTATE_MOVE;
		 }
	 }
 }
 //==========================
 // �����蔻���Ԃ�
 //==========================
 bool ReturnHit()
 {
	 return isHit;
 }