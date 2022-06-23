//=============================================================================
//
// �Q�[����ʏ��� [game.cpp]
// Author : GP11A132 16 ��Ńe�C
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "model.h"
#include "camera.h"
#include "input.h"
#include "sound.h"
#include "fade.h"
#include "game.h"

#include "player.h"
#include "enemy.h"
#include "meshfield.h"
#include "meshwall.h"
#include "sky.h"
#include "item.h"
#include "shadow.h"
#include "tree.h"
#include "bullet.h"
#include "circle.h"
#include "score.h"
#include "ui.h"
#include "particle.h"
#include "collision.h"
#include "result.h"
#include "debugproc.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************



//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void CheckHit(void);



//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static int	g_ViewPortType_Game = TYPE_FULL_SCREEN;

static BOOL	g_bPause = TRUE;	// �|�[�YON/OFF
static int	g_Result;			// ���U���h���`���b�N 1:clear 2:over

//=============================================================================
// ����������
//=============================================================================
HRESULT InitGame(void)
{
	g_ViewPortType_Game = TYPE_FULL_SCREEN;
	g_Result = 0;

	// �t�B�[���h�̏�����
	InitMeshField(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), 100, 100, 13.0f, 13.0f);

	// �w�i�̏�����
	InitSky();

	// ���C�g��L����	// �e�̏���������
	InitShadow();

	// �v���C���[�̏�����
	InitPlayer();

	// �G�l�~�[�̏�����
	InitEnemy();

	// �A�C�e���̏�����
	InitItem();

	// �ǂ̏�����
	InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_TOP), XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(MAP_LEFT, 0.0f, 0.0f), XMFLOAT3(0.0f, -XM_PI * 0.50f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(MAP_RIGHT, 0.0f, 0.0f), XMFLOAT3(0.0f, XM_PI * 0.50f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_DOWN), XMFLOAT3(0.0f,  XM_PI, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);

	// ��(�����p�̔�����)
	InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_TOP), XMFLOAT3(0.0f,    XM_PI, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(MAP_LEFT, 0.0f, 0.0f), XMFLOAT3(0.0f,   XM_PI * 0.50f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(MAP_RIGHT, 0.0f, 0.0f), XMFLOAT3(0.0f, -XM_PI * 0.50f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_DOWN), XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);

	// �؂𐶂₷
	InitTree();

	// �e�̏�����
	InitBullet();

	// ���@�w�̏�����
	InitCircle();

	// UI�̏�����
	InitUI();

	// �p�[�e�B�N���̏�����
	InitParticle();

	// ���U���h�̏�����
	InitResult();

	// BGM�Đ�
	PlaySound(SOUND_LABEL_BGM_map);

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitGame(void)
{
	// ���U���h�̏I������
	UninitResult();

	// �p�[�e�B�N���̏I������
	UninitParticle();

	// UI�̏I������
	UninitUI();

	// ���@�w�̏I������
	UninitCircle();

	// �e�̏I������
	UninitBullet();

	// �؂̏I������
	UninitTree();

	// �ǂ̏I������
	UninitMeshWall();

	// �n�ʂ̏I������
	UninitMeshField();

	// �w�i�̏I������
	UninitSky();

	// �G�l�~�[�̏I������
	UninitEnemy();

	// �v���C���[�̏I������
	UninitPlayer();

	// �A�C�e���̏I������
	UninitItem();

	// �e�̏I������
	UninitShadow();

}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateGame(void)
{
#ifdef _DEBUG
	if (GetKeyboardTrigger(DIK_V))
	{
		g_ViewPortType_Game = (g_ViewPortType_Game + 1) % TYPE_NONE;
		SetViewPort(g_ViewPortType_Game);
	}

	if (GetKeyboardTrigger(DIK_P))
	{
		g_bPause = g_bPause ? FALSE : TRUE;
	}


#endif

	if(g_bPause == FALSE)
		return;

	// �n�ʏ����̍X�V
	UpdateMeshField();

	// �w�i�̍X�V
	UpdateSky();

	// �v���C���[�̍X�V����
	UpdatePlayer();

	// �G�l�~�[�̍X�V����
	UpdateEnemy();

	// �A�C�e���̍X�V����
	UpdateItem();

	// �Ǐ����̍X�V
	UpdateMeshWall();

	// �؂̍X�V����
	UpdateTree();

	// �e�̍X�V����
	UpdateBullet();

	// ���@�w�̍X�V����
	UpdateCircle();

	// �p�[�e�B�N���̍X�V����
	UpdateParticle();

	// �e�̍X�V����
	UpdateShadow();

	// �����蔻�菈��
	CheckHit();

	// MAP�Ƃ̓����蔻��
	CheckField();

	// UI�̍X�V����
	UpdateUI();

	// ���U���h�̍X�V����
	UpdateResult();

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawGame0(void)
{
	// 3D�̕���`�悷�鏈��
	// �n�ʂ̕`�揈��
	DrawMeshField();

	// �w�i�̕`�揈��
	DrawSky();

	// �e�̕`�揈��
	DrawShadow();

	// �G�l�~�[�̕`�揈��
	DrawEnemy();

	// �v���C���[�̕`�揈��
	DrawPlayer();

	// �A�C�e���̕`�揈��
	DrawItem();

	// �e�̕`�揈��
	DrawBullet();

	// ���@�w�̕`�揈��
	DrawCircle();

	// �ǂ̕`�揈��
	DrawMeshWall();

	// �؂̕`�揈��
	DrawTree();

	// �p�[�e�B�N���̕`�揈��
	DrawParticle();


	// 2D�̕���`�悷�鏈��
	// Z��r�Ȃ�
	SetDepthEnable(FALSE);

	// ���C�e�B���O�𖳌�
	SetLightEnable(FALSE);

	// UI�̕`�揈��
	DrawUI();

	// ���U���h�̕`�揈��
	DrawResult();

	// ���C�e�B���O��L����
	SetLightEnable(TRUE);

	// Z��r����
	SetDepthEnable(TRUE);
}


void DrawGame(void)
{
	XMFLOAT3 pos;


#ifdef _DEBUG
	// �f�o�b�O�\��
	PrintDebugProc("ViewPortType:%d\n", g_ViewPortType_Game);

#endif

	// �v���C���[���_
	pos = GetPlayer()->pos;
	pos.y = 35.0f;			// �J����������h�����߂ɃN���A���Ă���
	SetCameraAT(pos);
	SetCamera();

	switch(g_ViewPortType_Game)
	{
	case TYPE_FULL_SCREEN:
		SetViewPort(TYPE_FULL_SCREEN);
		DrawGame0();
		break;

	case TYPE_LEFT_HALF_SCREEN:
	case TYPE_RIGHT_HALF_SCREEN:
		SetViewPort(TYPE_LEFT_HALF_SCREEN);
		DrawGame0();

		// �G�l�~�[���_
		pos = GetEnemy()->pos;
		pos.y = 0.0f;
		SetCameraAT(pos);
		SetCamera();
		SetViewPort(TYPE_RIGHT_HALF_SCREEN);
		DrawGame0();
		break;

	case TYPE_UP_HALF_SCREEN:
	case TYPE_DOWN_HALF_SCREEN:
		SetViewPort(TYPE_UP_HALF_SCREEN);
		DrawGame0();

		// �G�l�~�[���_
		pos = GetEnemy()->pos;
		pos.y = 0.0f;
		SetCameraAT(pos);
		SetCamera();
		SetViewPort(TYPE_DOWN_HALF_SCREEN);
		DrawGame0();
		break;

	}

}


//=============================================================================
// �����蔻�菈��
//=============================================================================
void CheckHit(void)
{
	ENEMY *enemy = GetEnemy();		// �G�l�~�[�̃|�C���^�[��������
	PLAYER *player = GetPlayer();	// �v���C���[�̃|�C���^�[��������
	BULLET *bullet = GetBullet();	// �e�̃|�C���^�[��������
	PARTICLE *particle = GetParticle();	// �G�̋Z�̃|�C���^�[��������
	ITEM *item = GetItem();			// �A�C�e���̃|�C���^�[��������
	
	//// �G�ƃv���C���[�L����
	//for (int i = 0; i < MAX_ENEMY; i++)
	//{
	//	//�G�̗L���t���O���`�F�b�N����
	//	if (enemy[i].use == FALSE)
	//		continue;

	//	//BC�̓����蔻��
	//	if (CollisionBC(player->pos, enemy[i].pos, player->size, enemy[i].size))
	//	{
	//		// �G�L�����N�^�[�͓|�����
	//		enemy[i].use = FALSE;
	//		ReleaseShadow(enemy[i].shadowIdx);

	//		// �X�R�A�𑫂�
	//		AddScore(100);
	//	}
	//}


	// �v���C���[�̒e�ƓG
	for (int i = 0; i < MAX_BULLET; i++)
	{
		//�e�̗L���t���O���`�F�b�N����
		if (bullet[i].use == FALSE)
			continue;

		// �G�Ɠ������Ă邩���ׂ�
		for (int j = 0; j < MAX_ENEMY; j++)
		{
			//�G�̗L���t���O���`�F�b�N����
			if (enemy[j].use == FALSE)
				continue;

			//BC�̓����蔻��
			if (CollisionBC(bullet[i].pos, enemy[j].pos, bullet[i].fWidth, enemy[j].size))
			{
				// �����������疢�g�p�ɖ߂�
				bullet[i].use = FALSE;
				ReleaseShadow(bullet[i].shadowIdx);

				// �G�L�����N�^�[�͓|�����
				enemy[j].hp -= player->atk;
				enemy[j].hit = TRUE;
				if (enemy[j].hp <= 0)
				{
					enemy[j].hp = 0;
					enemy[j].use = FALSE;
					ReleaseShadow(enemy[j].shadowIdx);
				}
				PlaySound(SOUND_LABEL_SE_hit);
			}
		}

	}

	// �G�̋Z�ƃv���C���[
	for (int i = 0; i < MAX_PARTICLE; i++)
	{
		//�e�̗L���t���O���`�F�b�N����
		if (particle[i].inUse == false)
			continue;

		// �������Ă邩���ׂ�
		for (int j = 0; j < MAX_ENEMY; j++)
		{
			//�L���t���O���`�F�b�N����
			if (player->use == FALSE)
				continue;

			if (particle[i].type == PARTICLE_TYPE_FLAME)
			{//BC�̓����蔻��
				if (CollisionBC(particle[i].pos, player->pos, particle[i].size.x, player->size))
				{
					// �����������疢�g�p�ɖ߂�
					particle[i].inUse = false;

					// �v���C���[�͓|�����
					player->hp -= enemy[j].atk;
					player->hit = TRUE;
					if (player->hp <= 0)
					{
						player->hp = 0;
						player->use = FALSE;
						ReleaseShadow(player->shadowIdx);
					}
					PlaySound(SOUND_LABEL_SE_hit);
				}
			}
		}


	}

	// �A�C�e���ƃv���C���[�L����
	for (int i = 0; i < MAX_ITEM; i++)
	{
		//�A�C�e���̗L���t���O���`�F�b�N����
		if (item[i].use == FALSE)
			continue;

		XMFLOAT3 pos = item[i].pos;
		pos.y = player->pos.y;

		//BC�̓����蔻��
		if (CollisionBC(player->pos, pos, player->size, item[i].size))
		{
			// �A�C�e���͎g����
			item[i].effect = TRUE;
			item[i].use = FALSE;
			ReleaseShadow(item[i].shadowIdx);

			player->hp += 10;

			if (player->hp > player->hp_max)
			{
				player->hp = player->hp_max;
			}
		}
	}


	// �G�l�~�[���S�����S�������ԑJ��
	int enemy_count = 0;
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (enemy[i].use == FALSE) continue;
		enemy_count++;
	}

	// �G�l�~�[���O�C�H
	if (enemy_count == 0)
	{
		g_Result = 1;
	}

	// �v���C���[�����񂾂�
	if (player->hp == 0)
	{
		g_Result = 2;
	}
}

//=============================================================================
// ���U���h���擾
//=============================================================================
int GetResult(void)
{
	return g_Result;
}

