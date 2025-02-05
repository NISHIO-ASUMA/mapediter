//=============================
//
// �t�F�[�h���� [fade.h]
// Author:Asuma Nishio
//
//=============================
#ifndef _FADE_H_
#define _FADE_H_

//*********************************
// �C���N���[�h�t�@�C���錾
//*********************************
#include "main.h"

//**************************
// �t�F�[�h�̏�ԗ񋓌^
//**************************
typedef enum
{
	FADE_NONE = 0,			// �t�F�[�h�Ȃ��̏��
	FADE_IN,				// �t�F�[�h�C���̏��
	FADE_OUT,				// �t�F�[�h�A�E�g�̏��
	FAFE_MAX
}FADE;

//*****************************
// �v���g�^�C�v�錾
//*****************************
void InitFade(MODE modeNext);		// �t�F�[�h�̏�����
void UninitFade(void);				// �t�F�[�h�̏I��
void UpdateFade(void);				// �t�F�[�h�̍X�V
void DrawFade(void);				// �t�F�[�h�̕`��
void SetFade(MODE modeNext);		// �t�F�[�h�̐ݒ�
FADE GetFade(void);					// �t�F�[�h�̎擾

#endif