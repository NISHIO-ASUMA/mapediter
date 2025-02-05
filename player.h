//==========================
//
// �v���C���[���� [player.h]
// Author: Asuma Nishio
//
//==========================
#ifndef _PLAYER_H_
#define _PLAYER_H_

//***************************
// �C���N���[�h�t�@�C���錾
//***************************
#include "main.h"
#include "model.h"

//***************************
// �}�N����`
//***************************
#define MAX_MOVE (3.0f)				// �ړ���
#define MAX_MODEL (15)				// �p�[�c��
#define MAX_WORD (256)				// ������
#define MAX_LIFEBARSIZE_X (400)		// ���C�t�o�[�̉���
#define MAX_LIFEBARSIZE_Y (20)		// ���C�t�o�[�̍���

//***************************
// X�t�@�C���񋓌^�錾
//***************************
static const char* MODEL_FILE[MAX_MODEL] =
{
	"data\\MODEL\\00_body.x",
	"data\\MODEL\\01_face.x",
	"data\\MODEL\\02_armRight.x",
	"data\\MODEL\\03_armfrontRight.x",
	"data\\MODEL\\04_handRight.x",
	"data\\MODEL\\05_armLeft.x",
	"data\\MODEL\\06_armfrontLeft.x",
	"data\\MODEL\\07_handLeft.x",
	"data\\MODEL\\08_Rightmomo.x",
	"data\\MODEL\\09_RightLeg.x",
	"data\\MODEL\\10_Rightfoot.x",
	"data\\MODEL\\11_Leftmomo.x",
	"data\\MODEL\\12_Leftleg.x",
	"data\\MODEL\\13_Leftfoot.x",
	"data\\MODEL\\14_futa000.x",
};
//***************************
// ���[�V�����񋓌^�錾
//***************************
typedef enum
{
	MOTIONTYPE_NEUTRAL = 0,	// �j���[�g����
	MOTIONTYPE_MOVE,		// �ړ�
	MOTIONTYPE_ACTION,		// �A�N�V����
	MOTIONTYPE_JUMP,		// �W�����v
	MOTIONTYPE_LANDING,		// ���n
	MOTIONTYPE_MAX
}MOTIONTYPE;

//***************************
// �L�[�\���̐錾
//***************************
typedef struct
{
	float fPosX;	// �ʒuX
	float fPosY;	// �ʒuY
	float fPosZ;	// �ʒuZ
	float fRotX;	// �p�xX
	float fRotY;	// �p�xY
	float fRotZ;	// �p�xZ
}KEY;

//***************************
// �L�[���̍\���̐錾
//***************************
typedef struct
{
	int nFrame;				// �t���[����
	KEY aKey[MAX_MODEL];	// �e�p�[�c�̃L�[�̗v�f
}KEY_INFO;

//***************************
// ���[�V�������̍\���̐錾
//***************************
typedef struct
{
	bool bLoop;						// ���[�v���邩���Ȃ���
	int nNumKey;					// �L�[�̑���
	KEY_INFO aKeyInfo[MAX_MODEL];	// �L�[���
	MOTIONTYPE Motiontype;			// ���[�V�����̎��
}MOTION_INFO;

//******************************
// �v���C���[�̃��[�V�����񋓌^
//******************************
typedef enum
{
	PLAYERSTATE_NEUTRAL = 0,// �j���[�g����
	PLAYERSTATE_MOVE,		// �ړ�
	PLAYERSTATE_JUMP,		// �W�����v
	PLAYERSTATE_ACTION,		// �A�N�V����
	PLAYERSTATE_LANDING,	// ���n
	PLAYERSTATE_MAX
}PLAYERSTATE;

//******************************
// �v���C���[�̏�ԗ񋓌^
//******************************
typedef enum
{
	PLAYERCONDITION_NORMAL = 0, // �ʏ���
	PLAYERCONDITION_DAMAGE,		// �_���[�W���
	PLAYERCONDTION_DEATH,		// ���S���
	PLAYERCONDTION_APPER,		// ���G���
	PLAYERCONDITION_MOVE,		// �ړ�
	PLAYERCONDITION_ACTION,		// �U��
	PLAYERCONDITION_JUMP,		// �W�����v
	PLAYERCONDITION_MAX
}PLAYERCONDITION;

//***************************
// �v���C���[�\���̐錾
//***************************
typedef struct
{
	D3DXVECTOR3 pos;			// ���W
	D3DXVECTOR3 posOld;			// �O��̈ʒu
	D3DXVECTOR3 rot;			// �p�x
	D3DXVECTOR3 move;			// �ړ���
	D3DXVECTOR3 rotDestPlayer;	// �ړI�̊p�x
	D3DXVECTOR3 size;			// �T�C�Y
	D3DXVECTOR3 Vtxmax, Vtxmin;	// �ő�,�ŏ�
	D3DXMATRIX mtxWorld;		// ���[���h�}�g���b�N�X
	int nIdxShadow;				// �e�̃C���f�b�N�X
	bool bjump;					// �W�����v
	bool bLanding;				// ���n����
	Model aModel[MAX_MODEL];	// ���f���p�[�c
	int nNumModel;				// ���f���̑���
	MOTION_INFO aMotionInfo[PLAYERSTATE_MAX];	// ���[�V�������
	int nNumMotion;				// ���[�V�����̑���
	bool bLoopMotion;			// ���[�v���邩�ǂ���
	int nNumKey;				// �L�[�̑���
	int nKey;					// ���݂̃L�[No
	int nCounterMotion;			// ���[�V�����̃J�E���^�[
	bool bDisp;					// �v���C���[�̎g�p���
	PLAYERSTATE state;			// �v���C���[�̃��[�V�������
	PLAYERCONDITION Pcon;		// �v���C���[�̏��
	bool bWalk;					// �����Ă��邩
	int nLife;					// �̗�
	int nCntState;				// �J�E���g�p�̕ϐ�
	bool isFinishmotion;		// ���݂̃��[�V�������I�����Ă��邩�ǂ���
	bool isBlendmotion;			// �u�����h���L����
	MOTIONTYPE motiontypeblend; // �u�����h�̎��
	bool isLopmotionblend;		// �u�����h���[�V�������[�v���Ă��邩
	int nNumkeyblend;			// �u�����h�̃L�[��
	int nKeyBlend;				// �u�����h�̌��݃L�[
	int nNextKeyBlend;			// �u�����h�̎��̃L�[ 
	int nCntMotionBlend;		// ���[�V�����u�����h�J�E���^�[
	int nFrameBlend;			// �u�����h�̃t���[����
	int nCntBlend;				// �u�����h�̃J�E���^�[
}PLAYER;

//**********************
// �v���g�^�C�v�錾
//**********************
void InitPlayer(void);		    // �v���C���[����������
void UninitPlayer(void);	    // �v���C���[�I������
void UpdatePlayer(void);	    // �v���C���[�X�V����
void DrawPlayer(void);		    // �v���C���[�`�揈��
PLAYER* GetPlayer(void);	    // �v���C���[�̎擾
void UpdateMotionPlayer(void);	// ���[�V�����̍X�V
void HitPlayer(int nDamage);	// �_���[�W����

void InitLifeBar(void);			// ���C�t�o�[�̏�����
void UninitLifeBar(void);	    // ���C�t�o�[�I������
void UpdateLifeBar(void);	    // ���C�t�o�[�X�V����
void DrawLifeBar(void);		    // ���C�t�o�[�`�揈��

void Stick(void);
bool ReturnHit();

#endif // !_PLAYER_H_

