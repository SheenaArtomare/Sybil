//=============================================================================
//
// �G�l�~�[���f������ [enemy.cpp]
// Author : GP11A132 16 ��Ńe�C
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "model.h"
#include "enemy.h"
#include "shadow.h"
#include "light.h"
#include "player.h"
#include "debugproc.h"
#include "sound.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_ENEMY				"data/MODEL/E_body.obj"			// �ǂݍ��ރ��f����
#define	MODEL_ENEMY_HEAD		"data/MODEL/E_head.obj"			// �ǂݍ��ރ��f����
#define	MODEL_ENEMY_ARM_LEFT	"data/MODEL/E_arm_left.obj"		// �ǂݍ��ރ��f����
#define	MODEL_ENEMY_ARM_RIGHT	"data/MODEL/E_arm_right.obj"	// �ǂݍ��ރ��f����
#define	MODEL_ENEMY_LEG_LEFT	"data/MODEL/E_leg_left.obj"		// �ǂݍ��ރ��f����
#define	MODEL_ENEMY_LEG_RIGHT	"data/MODEL/E_leg_right.obj"	// �ǂݍ��ރ��f����
#define	MODEL_ENEMY_TAIL		"data/MODEL/E_tail.obj"			// �ǂݍ��ރ��f����
#define	MODEL_ENEMY_EAR_LEFT	"data/MODEL/E_ear_left.obj"		// �ǂݍ��ރ��f����
#define	MODEL_ENEMY_EAR_RIGHT	"data/MODEL/E_ear_right.obj"	// �ǂݍ��ރ��f����

#define	VALUE_MOVE			(1.7f)						// �ړ���
#define	VALUE_ROTATE		(XM_PI * 0.02f)				// ��]��

#define ENEMY_SHADOW_SIZE	(0.7f)						// �e�̑傫��
#define ENEMY_OFFSET_Y		(19.0f)						// �G�l�~�[�̑��������킹��

#define ENEMY_PARTS_MAX		(8)							// �G�l�~�[�̃p�[�c�̐�


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ENEMY			g_Enemy[MAX_ENEMY];				// �G�l�~�[

static ENEMY			g_Parts[ENEMY_PARTS_MAX];		// �G�l�~�[�̃p�[�c�p

static BOOL				g_Load = FALSE;

static int				g_cd;							// �U���̃N�[���_�E��
static int				g_sound;						// SE��炷
static int				g_fuchi;						// �����t���O
static int				fuchi_keep;						// ���������΂炭�ێ�


static INTERPOLATION_DATA move_tbl[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(   0.0f, ENEMY_OFFSET_Y,  20.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(ENEMY_SHADOW_SIZE, ENEMY_SHADOW_SIZE, ENEMY_SHADOW_SIZE), 60*2 },
	{ XMFLOAT3(-200.0f, ENEMY_OFFSET_Y,  20.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(ENEMY_SHADOW_SIZE, ENEMY_SHADOW_SIZE, ENEMY_SHADOW_SIZE), 60*1 },
	{ XMFLOAT3(-200.0f, ENEMY_OFFSET_Y, 200.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(ENEMY_SHADOW_SIZE, ENEMY_SHADOW_SIZE, ENEMY_SHADOW_SIZE), 60*0.5f },
	{ XMFLOAT3(   0.0f, ENEMY_OFFSET_Y,  20.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(ENEMY_SHADOW_SIZE, ENEMY_SHADOW_SIZE, ENEMY_SHADOW_SIZE), 60*2 },

};

static INTERPOLATION_DATA move_tbl_none[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),      XMFLOAT3(1.0f, 1.0f, 1.0f), 0 },

};

static INTERPOLATION_DATA move_tbl_arm[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),             XMFLOAT3(1.0f, 1.0f, 1.0f), 10 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, XM_PI / 4, 0.0f),        XMFLOAT3(1.0f, 1.0f, 1.0f), 10 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),             XMFLOAT3(1.0f, 1.0f, 1.0f), 10 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, (-1)*(XM_PI / 4), 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 10 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),             XMFLOAT3(1.0f, 1.0f, 1.0f), 10 },

};

static INTERPOLATION_DATA move_tbl_leg_left[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),             XMFLOAT3(1.0f, 1.0f, 1.0f), 10 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(XM_PI / 4, 0.0f, 0.0f),        XMFLOAT3(1.0f, 1.0f, 1.0f), 10 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),             XMFLOAT3(1.0f, 1.0f, 1.0f), 10 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3((-1)*(XM_PI / 4), 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 10 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),             XMFLOAT3(1.0f, 1.0f, 1.0f), 10 },

};

static INTERPOLATION_DATA move_tbl_leg_right[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),             XMFLOAT3(1.0f, 1.0f, 1.0f), 10 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3((-1)*(XM_PI / 4), 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 10 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),             XMFLOAT3(1.0f, 1.0f, 1.0f), 10 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(XM_PI / 4, 0.0f, 0.0f),        XMFLOAT3(1.0f, 1.0f, 1.0f), 10 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),             XMFLOAT3(1.0f, 1.0f, 1.0f), 10 },

};

static INTERPOLATION_DATA move_tbl_tail[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),             XMFLOAT3(1.0f, 1.0f, 1.0f), 10 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, XM_PI / 8, XM_PI / 8),        XMFLOAT3(1.0f, 1.0f, 1.0f), 10 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),             XMFLOAT3(1.0f, 1.0f, 1.0f), 10 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, (-1)*(XM_PI / 8), (-1)*(XM_PI / 8)), XMFLOAT3(1.0f, 1.0f, 1.0f), 10 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),             XMFLOAT3(1.0f, 1.0f, 1.0f), 10 },

};

static INTERPOLATION_DATA move_tbl_ear_left[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),             XMFLOAT3(1.0f, 1.0f, 1.0f), 10 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3((-1)*(XM_PI / 32),0.0f, XM_PI / 32), XMFLOAT3(1.0f, 1.0f, 1.0f), 10 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),             XMFLOAT3(1.0f, 1.0f, 1.0f), 10 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f,(-1)*(XM_PI / 32)),        XMFLOAT3(1.0f, 1.0f, 1.0f), 10 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),             XMFLOAT3(1.0f, 1.0f, 1.0f), 10 },

};

//=============================================================================
// ����������
//=============================================================================
HRESULT InitEnemy(void)
{
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		LoadModel(MODEL_ENEMY, &g_Enemy[i].model);
		g_Enemy[i].load = TRUE;

		g_Enemy[i].pos = XMFLOAT3(120, ENEMY_OFFSET_Y, -500.0f);
		g_Enemy[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Enemy[i].scl = XMFLOAT3(0.7f, 0.7f, 0.7f);

		g_Enemy[i].spd = 0.0f;			// �ړ��X�s�[�h�N���A
		g_Enemy[i].size = ENEMY_SIZE;	// �����蔻��̑傫��

		// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		GetModelDiffuse(&g_Enemy[0].model, &g_Enemy[0].diffuse[0]);

		XMFLOAT3 pos = g_Enemy[i].pos;
		pos.y -= (ENEMY_OFFSET_Y - 0.1f);
		g_Enemy[i].shadowIdx = CreateShadow(pos, ENEMY_SHADOW_SIZE, ENEMY_SHADOW_SIZE);

		g_Enemy[i].move_time = 0.0f;	// ���`��ԗp�̃^�C�}�[���N���A
		g_Enemy[i].tbl_adr = NULL;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Enemy[i].tbl_size = 0;		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

		g_Enemy[i].use = TRUE;			// TRUE:�����Ă�
		g_Enemy[i].hit = FALSE;
		g_Enemy[i].hp = g_Enemy[i].hp_max = 100;
		g_Enemy[i].atk = 2;
		g_Enemy[i].skill = 1;

		// �K�w�A�j���[�V�����p�̏���������
		g_Enemy[i].parent = NULL;			// �{�́i�e�j�Ȃ̂�NULL������
	}


	// 0�Ԃ������`��Ԃœ������Ă݂�
	g_Enemy[0].move_time = 0.0f;		// ���`��ԗp�̃^�C�}�[���N���A
	g_Enemy[0].tbl_adr = move_tbl;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Enemy[0].tbl_size = sizeof(move_tbl) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g



	// �p�[�c�̏�����
	for (int i = 0; i < ENEMY_PARTS_MAX; i++)
	{
		g_Parts[i].use = FALSE;

		// �ʒu�E��]�E�X�P�[���̏����ݒ�
		g_Parts[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Parts[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Parts[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

		// �e�q�֌W
		g_Parts[i].parent = &g_Enemy[0];		// �� �����ɐe�̃A�h���X������
		//	g_Parts[�r].parent= &g_Enemy[0];		// �r��������e�͖{�́i�v���C���[�j
		//	g_Parts[��].parent= &g_Paerts[�r];	// �w���r�̎q���������ꍇ�̗�

		// �K�w�A�j���[�V�����p�̃����o�[�ϐ��̏�����
		g_Parts[i].tbl_adr = NULL;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i].move_time = 0.0f;	// ���s���Ԃ��N���A
		g_Parts[i].tbl_size = 0;		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

		// �p�[�c�̓ǂݍ��݂͂܂����Ă��Ȃ�
		g_Parts[i].load = 0;
	}

	g_Parts[0].use = TRUE;
	g_Parts[0].parent = &g_Enemy[0];		// �e���Z�b�g
	g_Parts[0].load = 1;
	LoadModel(MODEL_ENEMY_HEAD, &g_Parts[0].model);

	g_Parts[1].use = TRUE;
	g_Parts[1].parent = &g_Enemy[0];		// �e���Z�b�g
	g_Parts[1].load = 1;
	LoadModel(MODEL_ENEMY_ARM_LEFT, &g_Parts[1].model);

	g_Parts[2].use = TRUE;
	g_Parts[2].parent = &g_Enemy[0];		// �e���Z�b�g
	g_Parts[2].load = 1;
	LoadModel(MODEL_ENEMY_ARM_RIGHT, &g_Parts[2].model);

	g_Parts[3].use = TRUE;
	g_Parts[3].parent = &g_Enemy[0];		// �e���Z�b�g
	g_Parts[3].load = 1;
	LoadModel(MODEL_ENEMY_LEG_LEFT, &g_Parts[3].model);

	g_Parts[4].use = TRUE;
	g_Parts[4].parent = &g_Enemy[0];		// �e���Z�b�g
	g_Parts[4].load = 1;
	LoadModel(MODEL_ENEMY_LEG_RIGHT, &g_Parts[4].model);

	g_Parts[5].use = TRUE;
	g_Parts[5].parent = &g_Enemy[0];		// �e���Z�b�g
	g_Parts[5].load = 1;
	LoadModel(MODEL_ENEMY_TAIL, &g_Parts[5].model);

	g_Parts[6].use = TRUE;
	g_Parts[6].parent = &g_Enemy[0];		// �e���Z�b�g
	g_Parts[6].load = 1;
	LoadModel(MODEL_ENEMY_EAR_LEFT, &g_Parts[6].model);

	g_Parts[7].use = TRUE;
	g_Parts[7].parent = &g_Enemy[0];		// �e���Z�b�g
	g_Parts[7].load = 1;
	LoadModel(MODEL_ENEMY_EAR_RIGHT, &g_Parts[7].model);

	g_cd = 0;
	g_sound = 1;
	g_fuchi = 0;
	fuchi_keep = 0;
	
	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitEnemy(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].load)
		{
			UnloadModel(&g_Enemy[i].model);
			g_Enemy[i].load = FALSE;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateEnemy(void)
{
	//return;

	int mode = GetMode();

	if (mode == MODE_TUTORIAL)
	{
		g_Enemy[0].pos = XMFLOAT3(-150, ENEMY_OFFSET_Y, 10);
	
		// �e���G�l�~�[�̈ʒu�ɍ��킹��
		XMFLOAT3 pos = g_Enemy[0].pos;
		pos.y -= (ENEMY_OFFSET_Y - 0.1f);
		SetPositionShadow(g_Enemy[0].shadowIdx, pos);
	}

	if (mode == MODE_GAME)
	{
		// �G�l�~�[�𓮂����ꍇ�́A�e�����킹�ē���������Y��Ȃ��悤�ɂˁI
		for (int i = 0; i < MAX_ENEMY; i++)
		{
			PLAYER *player = GetPlayer();		// �v���C���[�̏����擾
			g_Enemy[i].spd = 0.0f;

			if (g_Enemy[i].use == TRUE && player[0].use == TRUE)			// ���̃G�l�~�[���g���Ă���H
			{									// Yes

				// �z�[�~���O����
				XMFLOAT3 pos_homing = player->pos;
				XMFLOAT3 temp = XMFLOAT3(g_Enemy[i].pos.x - pos_homing.x, 0.0f, g_Enemy[i].pos.z - pos_homing.z);
				float lenSq = (temp.x * temp.x) + (temp.z * temp.z);

				if (g_Enemy[i].skill > 0 && g_cd == 0)
				{
					g_Enemy[i].spd = VALUE_MOVE;

					pos_homing.x -= g_Enemy[i].pos.x;
					pos_homing.z -= g_Enemy[i].pos.z;

					float angle = atan2f(pos_homing.z, pos_homing.x) + XM_PI;		// ���̍������g���Ċp�x�����߂Ă���
					g_Enemy[i].rot.y = g_Enemy[i].dir - angle + XM_PI / 2;

					// ���͂̂����������փv���C���[���������Ĉړ�������
					g_Enemy[i].pos.x -= cosf(angle) * g_Enemy[i].spd;
					g_Enemy[i].pos.z -= sinf(angle) * g_Enemy[i].spd;

					//XMVECTOR pos1 = XMLoadFloat3(&player->pos);
					//XMVECTOR pos2 = XMLoadFloat3(&g_Enemy[i].pos);

					//pos2 += (pos1 - pos2) * 0.01f;

					//XMStoreFloat3(&g_Enemy[i].pos, pos2);

				}

				g_Enemy[i].skill++;
				g_sound++;

				// �v���C���[�ɋߕt������U��������
				if (lenSq <= 30000.0f && g_Enemy[i].use == TRUE && g_cd == 0)
				{
					g_Enemy[i].skill = 0;
					g_sound = 0;
					g_cd++;

					if (g_sound == 0)
					{
						PlaySound(SOUND_LABEL_SE_enemyskill_b);
					}
				}

				if (g_cd > 0)
				{
					g_cd++;

					if (g_cd > 100)
					{
						g_cd = 0;
					}
				}


				//if (g_Enemy[i].tbl_adr != NULL)	// ���`��Ԃ����s����H
				//{								// ���`��Ԃ̏���
				//	// �ړ�����
				//	int		index = (int)g_Enemy[i].move_time;
				//	float	time = g_Enemy[i].move_time - index;
				//	int		size = g_Enemy[i].tbl_size;

				//	float dt = 1.0f / g_Enemy[i].tbl_adr[index].frame;	// 1�t���[���Ői�߂鎞��
				//	g_Enemy[i].move_time += dt;							// �A�j���[�V�����̍��v���Ԃɑ���

				//	if (index > (size - 2))	// �S�[�����I�[�o�[���Ă�����A�ŏ��֖߂�
				//	{
				//		g_Enemy[i].move_time = 0.0f;
				//		index = 0;
				//	}

				//	// ���W�����߂�	X = StartX + (EndX - StartX) * ���̎���
				//	XMVECTOR p1 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 1].pos);	// ���̏ꏊ
				//	XMVECTOR p0 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 0].pos);	// ���݂̏ꏊ
				//	XMVECTOR vec = p1 - p0;
				//	XMStoreFloat3(&g_Enemy[i].pos, p0 + vec * time);

				//	// ��]�����߂�	R = StartX + (EndX - StartX) * ���̎���
				//	XMVECTOR r1 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 1].rot);	// ���̊p�x
				//	XMVECTOR r0 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 0].rot);	// ���݂̊p�x
				//	XMVECTOR rot = r1 - r0;
				//	XMStoreFloat3(&g_Enemy[i].rot, r0 + rot * time);

				//	// scale�����߂� S = StartX + (EndX - StartX) * ���̎���
				//	XMVECTOR s1 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 1].scl);	// ����Scale
				//	XMVECTOR s0 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 0].scl);	// ���݂�Scale
				//	XMVECTOR scl = s1 - s0;
				//	XMStoreFloat3(&g_Enemy[i].scl, s0 + scl * time);

				//}

				// �e���G�l�~�[�̈ʒu�ɍ��킹��
				XMFLOAT3 pos = g_Enemy[i].pos;
				pos.y -= (ENEMY_OFFSET_Y - 0.1f);
				SetPositionShadow(g_Enemy[i].shadowIdx, pos);


				// �K�w�A�j���[�V����
				for (int j = 0; j < ENEMY_PARTS_MAX; j++)
				{
					g_Parts[j].tbl_adr = move_tbl_none;	// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
					g_Parts[j].tbl_size = sizeof(move_tbl_none) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g


					if (g_Enemy[i].spd > 0.0f)
					{
						g_Parts[1].tbl_adr = move_tbl_arm;	// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
						g_Parts[1].tbl_size = sizeof(move_tbl_arm) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

						g_Parts[2].tbl_adr = move_tbl_arm;	// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
						g_Parts[2].tbl_size = sizeof(move_tbl_arm) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

						g_Parts[3].tbl_adr = move_tbl_leg_left;	// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
						g_Parts[3].tbl_size = sizeof(move_tbl_leg_left) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

						g_Parts[4].tbl_adr = move_tbl_leg_right;	// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
						g_Parts[4].tbl_size = sizeof(move_tbl_leg_right) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

						g_Parts[5].tbl_adr = move_tbl_tail;	// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
						g_Parts[5].tbl_size = sizeof(move_tbl_tail) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

						g_Parts[6].tbl_adr = move_tbl_ear_left;	// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
						g_Parts[6].tbl_size = sizeof(move_tbl_ear_left) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

						g_Parts[7].tbl_adr = move_tbl_ear_left;	// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
						g_Parts[7].tbl_size = sizeof(move_tbl_ear_left) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

					}

					// �g���Ă���Ȃ珈������
					if ((g_Parts[j].use == TRUE) && (g_Parts[j].tbl_adr != NULL))
					{
						// �ړ�����
						int		index = (int)g_Parts[j].move_time;
						float	time = g_Parts[j].move_time - index;
						int		size = g_Parts[j].tbl_size;

						float dt = 1.0f / g_Parts[j].tbl_adr[index].frame;	// 1�t���[���Ői�߂鎞��
						g_Parts[j].move_time += dt;					// �A�j���[�V�����̍��v���Ԃɑ���

						if (index > (size - 2))	// �S�[�����I�[�o�[���Ă�����A�ŏ��֖߂�
						{
							g_Parts[j].move_time = 0.0f;
							index = 0;
						}

						// ���W�����߂�	X = StartX + (EndX - StartX) * ���̎���
						XMVECTOR p1 = XMLoadFloat3(&g_Parts[j].tbl_adr[index + 1].pos);	// ���̏ꏊ
						XMVECTOR p0 = XMLoadFloat3(&g_Parts[j].tbl_adr[index + 0].pos);	// ���݂̏ꏊ
						XMVECTOR vec = p1 - p0;
						XMStoreFloat3(&g_Parts[j].pos, p0 + vec * time);

						// ��]�����߂�	R = StartX + (EndX - StartX) * ���̎���
						XMVECTOR r1 = XMLoadFloat3(&g_Parts[j].tbl_adr[index + 1].rot);	// ���̊p�x
						XMVECTOR r0 = XMLoadFloat3(&g_Parts[j].tbl_adr[index + 0].rot);	// ���݂̊p�x
						XMVECTOR rot = r1 - r0;
						XMStoreFloat3(&g_Parts[j].rot, r0 + rot * time);

						// scale�����߂� S = StartX + (EndX - StartX) * ���̎���
						XMVECTOR s1 = XMLoadFloat3(&g_Parts[j].tbl_adr[index + 1].scl);	// ����Scale
						XMVECTOR s0 = XMLoadFloat3(&g_Parts[j].tbl_adr[index + 0].scl);	// ���݂�Scale
						XMVECTOR scl = s1 - s0;
						XMStoreFloat3(&g_Parts[j].scl, s0 + scl * time);

					}
				}
			}

			// �����𐧌�
			if (g_Enemy[i].hit == TRUE)
			{
				g_fuchi = 1;
				fuchi_keep++;

				if (fuchi_keep == 20)
				{
					g_Enemy[i].hit = FALSE;
					fuchi_keep = 0;
					g_fuchi = 0;
				}
			}


#ifdef _DEBUG	// �f�o�b�O����\������
			PrintDebugProc("enmey hp:%d\n", g_Enemy[i].hp);
#endif



		}
	}

	
	// �|�C���g���C�g�̃e�X�g
	{
		LIGHT *light = GetLightData(2);
		XMFLOAT3 pos = g_Enemy[0].pos;
		pos.y += 20.0f;

		light->Position = pos;
		light->Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		light->Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		light->Type = LIGHT_TYPE_POINT;
		light->Enable = TRUE;
		SetLightData(2, light);
	}


}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawEnemy(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// �J�����O����
	SetCullingMode(CULL_MODE_NONE);

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].use == FALSE) continue;

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_Enemy[i].scl.x, g_Enemy[i].scl.y, g_Enemy[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_Enemy[i].rot.x, g_Enemy[i].rot.y + XM_PI, g_Enemy[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Enemy[i].pos.x, g_Enemy[i].pos.y, g_Enemy[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Enemy[i].mtxWorld, mtxWorld);

		//�����̐ݒ�
		SetFuchi(g_fuchi);

		// ���f���`��
		DrawModel(&g_Enemy[i].model);
	


		// �p�[�c�̊K�w�A�j���[�V����
		for (int j = 0; j < ENEMY_PARTS_MAX; j++)
		{
			// ���[���h�}�g���b�N�X�̏�����
			mtxWorld = XMMatrixIdentity();

			// �X�P�[���𔽉f
			mtxScl = XMMatrixScaling(g_Parts[j].scl.x, g_Parts[j].scl.y, g_Parts[j].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// ��]�𔽉f
			mtxRot = XMMatrixRotationRollPitchYaw(g_Parts[j].rot.x, g_Parts[j].rot.y, g_Parts[j].rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			// �ړ��𔽉f
			mtxTranslate = XMMatrixTranslation(g_Parts[j].pos.x, g_Parts[j].pos.y, g_Parts[j].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			if (g_Parts[j].parent != NULL)	// �q����������e�ƌ�������
			{
				mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(&g_Parts[j].parent->mtxWorld));
				// ��
				// g_Enemy.mtxWorld���w���Ă���
			}

			XMStoreFloat4x4(&g_Parts[j].mtxWorld, mtxWorld);

			// �g���Ă���Ȃ珈������B�����܂ŏ������Ă��闝�R�͑��̃p�[�c�����̃p�[�c���Q�Ƃ��Ă���\�������邩��B
			if (g_Parts[j].use == FALSE) continue;

			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&mtxWorld);


			// ���f���`��
			DrawModel(&g_Parts[j].model);

		}
	}

	//�����̐ݒ�
	SetFuchi(0);

	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_BACK);
}

//=============================================================================
// �G�l�~�[�̎擾
//=============================================================================
ENEMY *GetEnemy()
{
	return &g_Enemy[0];
}
