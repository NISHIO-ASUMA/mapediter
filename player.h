//==========================
//
// プレイヤー処理 [player.h]
// Author: Asuma Nishio
//
//==========================
#ifndef _PLAYER_H_
#define _PLAYER_H_

//***************************
// インクルードファイル宣言
//***************************
#include "main.h"
#include "model.h"

//***************************
// マクロ定義
//***************************
#define MAX_MOVE (3.0f)				// 移動量
#define MAX_MODEL (15)				// パーツ数
#define MAX_WORD (256)				// 文字数
#define MAX_LIFEBARSIZE_X (400)		// ライフバーの横幅
#define MAX_LIFEBARSIZE_Y (20)		// ライフバーの高さ

//***************************
// Xファイル列挙型宣言
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
// モーション列挙型宣言
//***************************
typedef enum
{
	MOTIONTYPE_NEUTRAL = 0,	// ニュートラル
	MOTIONTYPE_MOVE,		// 移動
	MOTIONTYPE_ACTION,		// アクション
	MOTIONTYPE_JUMP,		// ジャンプ
	MOTIONTYPE_LANDING,		// 着地
	MOTIONTYPE_MAX
}MOTIONTYPE;

//***************************
// キー構造体宣言
//***************************
typedef struct
{
	float fPosX;	// 位置X
	float fPosY;	// 位置Y
	float fPosZ;	// 位置Z
	float fRotX;	// 角度X
	float fRotY;	// 角度Y
	float fRotZ;	// 角度Z
}KEY;

//***************************
// キー情報の構造体宣言
//***************************
typedef struct
{
	int nFrame;				// フレーム数
	KEY aKey[MAX_MODEL];	// 各パーツのキーの要素
}KEY_INFO;

//***************************
// モーション情報の構造体宣言
//***************************
typedef struct
{
	bool bLoop;						// ループするかしないか
	int nNumKey;					// キーの総数
	KEY_INFO aKeyInfo[MAX_MODEL];	// キー情報
	MOTIONTYPE Motiontype;			// モーションの種類
}MOTION_INFO;

//******************************
// プレイヤーのモーション列挙型
//******************************
typedef enum
{
	PLAYERSTATE_NEUTRAL = 0,// ニュートラル
	PLAYERSTATE_MOVE,		// 移動
	PLAYERSTATE_JUMP,		// ジャンプ
	PLAYERSTATE_ACTION,		// アクション
	PLAYERSTATE_LANDING,	// 着地
	PLAYERSTATE_MAX
}PLAYERSTATE;

//******************************
// プレイヤーの状態列挙型
//******************************
typedef enum
{
	PLAYERCONDITION_NORMAL = 0, // 通常状態
	PLAYERCONDITION_DAMAGE,		// ダメージ状態
	PLAYERCONDTION_DEATH,		// 死亡状態
	PLAYERCONDTION_APPER,		// 無敵状態
	PLAYERCONDITION_MOVE,		// 移動
	PLAYERCONDITION_ACTION,		// 攻撃
	PLAYERCONDITION_JUMP,		// ジャンプ
	PLAYERCONDITION_MAX
}PLAYERCONDITION;

//***************************
// プレイヤー構造体宣言
//***************************
typedef struct
{
	D3DXVECTOR3 pos;			// 座標
	D3DXVECTOR3 posOld;			// 前回の位置
	D3DXVECTOR3 rot;			// 角度
	D3DXVECTOR3 move;			// 移動量
	D3DXVECTOR3 rotDestPlayer;	// 目的の角度
	D3DXVECTOR3 size;			// サイズ
	D3DXVECTOR3 Vtxmax, Vtxmin;	// 最大,最小
	D3DXMATRIX mtxWorld;		// ワールドマトリックス
	int nIdxShadow;				// 影のインデックス
	bool bjump;					// ジャンプ
	bool bLanding;				// 着地判定
	Model aModel[MAX_MODEL];	// モデルパーツ
	int nNumModel;				// モデルの総数
	MOTION_INFO aMotionInfo[PLAYERSTATE_MAX];	// モーション情報
	int nNumMotion;				// モーションの総数
	bool bLoopMotion;			// ループするかどうか
	int nNumKey;				// キーの総数
	int nKey;					// 現在のキーNo
	int nCounterMotion;			// モーションのカウンター
	bool bDisp;					// プレイヤーの使用状態
	PLAYERSTATE state;			// プレイヤーのモーション状態
	PLAYERCONDITION Pcon;		// プレイヤーの状態
	bool bWalk;					// 歩いているか
	int nLife;					// 体力
	int nCntState;				// カウント用の変数
	bool isFinishmotion;		// 現在のモーションが終了しているかどうか
	bool isBlendmotion;			// ブレンドが有効か
	MOTIONTYPE motiontypeblend; // ブレンドの種類
	bool isLopmotionblend;		// ブレンドモーションループしているか
	int nNumkeyblend;			// ブレンドのキー数
	int nKeyBlend;				// ブレンドの現在キー
	int nNextKeyBlend;			// ブレンドの次のキー 
	int nCntMotionBlend;		// モーションブレンドカウンター
	int nFrameBlend;			// ブレンドのフレーム数
	int nCntBlend;				// ブレンドのカウンター
}PLAYER;

//**********************
// プロトタイプ宣言
//**********************
void InitPlayer(void);		    // プレイヤー初期化処理
void UninitPlayer(void);	    // プレイヤー終了処理
void UpdatePlayer(void);	    // プレイヤー更新処理
void DrawPlayer(void);		    // プレイヤー描画処理
PLAYER* GetPlayer(void);	    // プレイヤーの取得
void UpdateMotionPlayer(void);	// モーションの更新
void HitPlayer(int nDamage);	// ダメージ処理

void InitLifeBar(void);			// ライフバーの初期化
void UninitLifeBar(void);	    // ライフバー終了処理
void UpdateLifeBar(void);	    // ライフバー更新処理
void DrawLifeBar(void);		    // ライフバー描画処理

void Stick(void);
bool ReturnHit();

#endif // !_PLAYER_H_

