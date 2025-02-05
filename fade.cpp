//=============================
// 
// �t�F�[�h���� [fade.cpp]
// Author:Asuma Nishio
//
//=============================

//**************************
// �C���N���[�h�t�@�C���錾
//**************************
#include "fade.h"

//***************************
// �O���[�o���ϐ��錾
//***************************
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffFade = NULL;	// ���_�o�b�t�@�̃|�C���^
FADE g_fade;			// �t�F�[�h�̏��
MODE g_modeNext;		// ���̉��
D3DXCOLOR g_colorFade;	// �|���S���̐F

//====================
// �t�F�[�h������
//====================
void InitFade(MODE modeNext)
{
	g_fade = FADE_IN;
	g_modeNext = modeNext;
	g_colorFade = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);	// ���|���S��

	LPDIRECT3DDEVICE9 pDevice;		// �f�o�C�X�ւ̃|�C���^

	// �f�o�C�X�̎擾
	pDevice = GetDevice();

	VERTEX_2D* pVtx;// ���_���̃|�C���^

	// ���_�o�b�t�@�̐���
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffFade,
		NULL);

	// ���_�o�b�t�@�����b�N��,���_���ւ̃|�C���^���擾
	g_pVtxBuffFade->Lock(0, 0, (void**)&pVtx, 0);

	// ���_���W�̐ݒ�
	pVtx[0].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 1�ڂ̒��_���
	pVtx[1].pos = D3DXVECTOR3(1280.0f, 0.0f, 0.0f);		// 2�ڂ̒��_���
	pVtx[2].pos = D3DXVECTOR3(0.0f, 720.0f, 0.0f);		// 3�ڂ̒��_���
	pVtx[3].pos = D3DXVECTOR3(1280.0f, 720.0f, 0.0f);	// 4�ڂ̒��_���

	// rhw�̐ݒ�(1.0f�ŌŒ�)
	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;

	// ���_�J���[�̐ݒ�
	pVtx[0].col = D3DXCOLOR(g_colorFade);
	pVtx[1].col = D3DXCOLOR(g_colorFade);
	pVtx[2].col = D3DXCOLOR(g_colorFade);
	pVtx[3].col = D3DXCOLOR(g_colorFade);

	// �A�����b�N
	g_pVtxBuffFade->Unlock();

}
//=====================
// �t�F�[�h�̏I������
//=====================
void UninitFade(void)
{
    // ���_�o�b�t�@�̔j��
	if (g_pVtxBuffFade != NULL)
	{
		g_pVtxBuffFade->Release();
		g_pVtxBuffFade = NULL;
	}

}
//=====================
// �t�F�[�h�X�V
//=====================
void UpdateFade(void)
{
	// ���_���̃|�C���^
	VERTEX_2D* pVtx;		

	if (g_fade != FADE_NONE)
	{
		if (g_fade == FADE_IN)
		{// �t�F�[�h�C�����
			// �����ɂ��Ă���
			g_colorFade.a -= 0.03f;		
			if (g_colorFade.a <= 0.0f)
			{
				g_colorFade.a = 0.0f;

				// �������Ă��Ȃ����
				g_fade = FADE_NONE;		
			}

			// ���_�o�b�t�@�����b�N��,���_���ւ̃|�C���^���擾
			g_pVtxBuffFade->Lock(0, 0, (void**)&pVtx, 0);

			// ���_�J���[�̐ݒ�
			pVtx[0].col = D3DXCOLOR(g_colorFade);
			pVtx[1].col = D3DXCOLOR(g_colorFade);
			pVtx[2].col = D3DXCOLOR(g_colorFade);
			pVtx[3].col = D3DXCOLOR(g_colorFade);

			// �A�����b�N
			g_pVtxBuffFade->Unlock();
		}
		else if (g_fade == FADE_OUT)
		{
			// �t�F�[�h�A�E�g���
			g_colorFade.a += 0.03f;
			if (g_colorFade.a >= 1.0f)
			{
				g_colorFade.a = 1.0f;
				g_fade = FADE_IN;

				return;
			}

			// ���_�o�b�t�@�����b�N��,���_���ւ̃|�C���^���擾
			g_pVtxBuffFade->Lock(0, 0, (void**)&pVtx, 0);

			// ���_�J���[�̐ݒ�
			pVtx[0].col = D3DXCOLOR(g_colorFade);
			pVtx[1].col = D3DXCOLOR(g_colorFade);
			pVtx[2].col = D3DXCOLOR(g_colorFade);
			pVtx[3].col = D3DXCOLOR(g_colorFade);

			// �A�����b�N
			g_pVtxBuffFade->Unlock();
		}
	}
}
//====================
// �t�F�[�h�̕`��
//====================
void DrawFade(void)
{
	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// ���_�o�b�t�@���f�[�^�X�g���[���ɐݒ�
	pDevice->SetStreamSource(0, g_pVtxBuffFade, 0, sizeof(VERTEX_2D));

	// ���_�t�H�[�}�b�g�̐ݒ�
	pDevice->SetFVF(FVF_VERTEX_2D);

	// �e�N�X�`���̐ݒ�
	pDevice->SetTexture(0, NULL);		

	// �t�F�[�h�̕`��
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);		

}
//=========================
// �t�F�[�h�̐ݒ�
//=========================
void SetFade(MODE modeNext)
{
	g_fade = FADE_OUT;		// �t�F�[�h�A�E�g���
	g_modeNext = modeNext;	// ���̉�ʂ̐ݒ�
}
//========================
// �t�F�[�h�̎擾
//========================
FADE GetFade(void)
{
	return g_fade;
}
