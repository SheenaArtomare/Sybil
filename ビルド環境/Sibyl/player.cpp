//=============================================================================
//
// ���f������ [player.cpp]
// Author : GP11A132 16 ��Ńe�C
//
//=============================================================================
#include "main.h"
#include "input.h"
#include "camera.h"
#include "debugproc.h"
#include "model.h"
#include "player.h"
#include "shadow.h"
#include "light.h"
#include "bullet.h"
#include "meshfield.h"
#include "sound.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_PLAYER			"data/MODEL/P_body.obj"			// �ǂݍ��ރ��f����
#define	MODEL_PLAYER_HEAD		"data/MODEL/P_head.obj"			// �ǂݍ��ރ��f����
#define	MODEL_PLAYER_ARM_LEFT	"data/MODEL/P_arm_left.obj"		// �ǂݍ��ރ��f����
#define	MODEL_PLAYER_ARM_RIGHT	"data/MODEL/P_arm_right.obj"		// �ǂݍ��ރ��f����
#define	MODEL_PLAYER_LEG_LEFT	"data/MODEL/P_leg_left.obj"		// �ǂݍ��ރ��f����
#define	MODEL_PLAYER_LEG_RIGHT	"data/MODEL/P_leg_right.obj"		// �ǂݍ��ރ��f����


#define	VALUE_MOVE			(2.0f)							// �ړ���
#define	VALUE_ROTATE		(XM_PI * 0.02f)					// ��]��

#define PLAYER_SHADOW_SIZE	(0.5f)							// �e�̑傫��
#define PLAYER_OFFSET_Y		(14.0f)							// �v���C���[�̑��������킹��

#define PLAYER_PARTS_MAX	(5)								// �v���C���[�̃p�[�c�̐�


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static PLAYER		g_Player;						// �v���C���[

static PLAYER		g_Parts[PLAYER_PARTS_MAX];		// �v���C���[�̃p�[�c�p

static BOOL			g_Load = FALSE;

static int			g_fuchi;						// �����t���O
static int			fuchi_keep;						// ���������΂炭�ێ�

// �v���C���[�̊K�w�A�j���[�V�����f�[�^

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


//=============================================================================
// ����������
//=============================================================================
HRESULT InitPlayer(void)
{
	LoadModel(MODEL_PLAYER, &g_Player.model);
	g_Player.load = TRUE;

	g_Player.pos = { 0.0f, PLAYER_OFFSET_Y, 0.0f };
	g_Player.pos_old = g_Player.pos;
	g_Player.rot = { 0.0f, 0.0f, 0.0f };
	g_Player.scl = { 0.5f, 0.5f, 0.5f };

	g_Player.spd = 0.0f;			// �ړ��X�s�[�h�N���A
	g_Player.size = PLAYER_SIZE;	// �����蔻��̑傫��

	g_Player.use = TRUE;
	g_Player.hit = FALSE;
	g_Player.isATK = FALSE;
	g_Player.hp = g_Player.hp_max = 100;
	g_Player.atk = 10;

	// �����Ńv���C���[�p�̉e���쐬���Ă���
	XMFLOAT3 pos = g_Player.pos;
	pos.y -= (PLAYER_OFFSET_Y - 0.1f);
	g_Player.shadowIdx = CreateShadow(pos, PLAYER_SHADOW_SIZE, PLAYER_SHADOW_SIZE);
	//          ��
	//        ���̃����o�[�ϐ������������e��Index�ԍ�

	// �K�w�A�j���[�V�����p�̏���������
	g_Player.parent = NULL;			// �{�́i�e�j�Ȃ̂�NULL������

	// �p�[�c�̏�����
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		g_Parts[i].use = FALSE;

		// �ʒu�E��]�E�X�P�[���̏����ݒ�
		g_Parts[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Parts[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Parts[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

		// �e�q�֌W
		g_Parts[i].parent = &g_Player;		// �� �����ɐe�̃A�h���X������
	//	g_Parts[�r].parent= &g_Player;		// �r��������e�͖{�́i�v���C���[�j
	//	g_Parts[��].parent= &g_Paerts[�r];	// �w���r�̎q���������ꍇ�̗�

		// �K�w�A�j���[�V�����p�̃����o�[�ϐ��̏�����
		g_Parts[i].tbl_adr = NULL;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i].move_time = 0.0f;	// ���s���Ԃ��N���A
		g_Parts[i].tbl_size = 0;		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

		// �p�[�c�̓ǂݍ��݂͂܂����Ă��Ȃ�
		g_Parts[i].load = 0;
	}

	g_Parts[0].use = TRUE;
	g_Parts[0].parent   = &g_Player;		// �e���Z�b�g
	g_Parts[0].load = 1;
	LoadModel(MODEL_PLAYER_HEAD, &g_Parts[0].model);

	g_Parts[1].use = TRUE;
	g_Parts[1].parent   = &g_Player;		// �e���Z�b�g
	g_Parts[1].load = 1;
	LoadModel(MODEL_PLAYER_ARM_LEFT, &g_Parts[1].model);

	g_Parts[2].use = TRUE;
	g_Parts[2].parent = &g_Player;		// �e���Z�b�g
	g_Parts[2].load = 1;
	LoadModel(MODEL_PLAYER_ARM_RIGHT, &g_Parts[2].model);

	g_Parts[3].use = TRUE;
	g_Parts[3].parent = &g_Player;		// �e���Z�b�g
	g_Parts[3].load = 1;
	LoadModel(MODEL_PLAYER_LEG_LEFT, &g_Parts[3].model);

	g_Parts[4].use = TRUE;
	g_Parts[4].parent = &g_Player;		// �e���Z�b�g
	g_Parts[4].load = 1;
	LoadModel(MODEL_PLAYER_LEG_RIGHT, &g_Parts[4].model);

	g_fuchi = 0;
	fuchi_keep = 0;
	
	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitPlayer(void)
{
	if (g_Load == FALSE) return;

	// ���f���̉������
	if (g_Player.load)
	{
		UnloadModel(&g_Player.model);
		g_Player.load = FALSE;
	}

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdatePlayer(void)
{
	g_Player.spd = 0.0f;
	g_Player.pos_old = g_Player.pos;

	CAMERA *cam = GetCamera();

	if (g_Player.use == TRUE)
	{
		// �U�����ĂȂ��Ȃ�
		if (g_Player.isATK == FALSE)
		{// �ړ��������Ⴄ
			if (GetKeyboardPress(DIK_A))
			{	// ���ֈړ�
				g_Player.spd = VALUE_MOVE;
				g_Player.dir = XM_PI / 2;
			}
			if (GetKeyboardPress(DIK_D))
			{	// �E�ֈړ�
				g_Player.spd = VALUE_MOVE;
				g_Player.dir = -XM_PI / 2;
			}
			if (GetKeyboardPress(DIK_W))
			{	// ��ֈړ�
				g_Player.spd = VALUE_MOVE;
				g_Player.dir = XM_PI;
			}
			if (GetKeyboardPress(DIK_S))
			{	// ���ֈړ�
				g_Player.spd = VALUE_MOVE;
				g_Player.dir = 0.0f;
			}

			// �Q�[���p�b�h�̏���
			if (IsButtonPressed(0, BUTTON_LEFT))
			{	// ���ֈړ�
				g_Player.spd = VALUE_MOVE;
				g_Player.dir = XM_PI / 2;
			}
			if (IsButtonPressed(0, BUTTON_RIGHT))
			{	// �E�ֈړ�
				g_Player.spd = VALUE_MOVE;
				g_Player.dir = -XM_PI / 2;
			}
			if (IsButtonPressed(0, BUTTON_UP))
			{	// ��ֈړ�
				g_Player.spd = VALUE_MOVE;
				g_Player.dir = XM_PI;
			}
			if (IsButtonPressed(0, BUTTON_DOWN))
			{	// ���ֈړ�
				g_Player.spd = VALUE_MOVE;
				g_Player.dir = 0.0f;
			}
		}


#ifdef _DEBUG
		if (GetKeyboardPress(DIK_R))
		{
			g_Player.pos.z = g_Player.pos.x = 0.0f;
			g_Player.rot.y = g_Player.dir = 0.0f;
			g_Player.spd = 0.0f;
		}
#endif


		// Key���͂���������ړ���������
		if (g_Player.spd > 0.0f)
		{
			g_Player.rot.y = g_Player.dir + cam->rot.y;

			// ���͂̂����������փv���C���[���������Ĉړ�������
			g_Player.pos.x -= sinf(g_Player.rot.y) * g_Player.spd;
			g_Player.pos.z -= cosf(g_Player.rot.y) * g_Player.spd;
		}
	}


	// ���C�L���X�g���đ����̍��������߂�
	XMFLOAT3 normal = { 0.0f, 1.0f, 0.0f };				// �Ԃ������|���S���̖@���x�N�g���i�����j
	XMFLOAT3 hitPosition;								// ��_
	hitPosition.y = g_Player.pos.y - PLAYER_OFFSET_Y;	// �O�ꂽ���p�ɏ��������Ă���
	bool ans = RayHitField(g_Player.pos, &hitPosition, &normal);
	g_Player.pos.y = hitPosition.y + PLAYER_OFFSET_Y;
	//g_Player.pos.y = PLAYER_OFFSET_Y;


	// �e���v���C���[�̈ʒu�ɍ��킹��
	XMFLOAT3 pos = g_Player.pos;
	pos.y -= (PLAYER_OFFSET_Y - 0.1f);
	SetPositionShadow(g_Player.shadowIdx, pos);

	// �e���ˏ���
	if (GetKeyboardTrigger(DIK_SPACE))
	{
		SetBullet(g_Player.pos, g_Player.rot);
	}

	// �Q�[���p�b�h�̏���
	if (IsButtonTriggered(0, BUTTON_A))
	{
		SetBullet(g_Player.pos, g_Player.rot);
	}


	g_Player.spd *= 0.5f;



	// �K�w�A�j���[�V����
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		g_Parts[i].tbl_adr = move_tbl_none;	// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i].tbl_size = sizeof(move_tbl_none) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

		
		if (g_Player.spd > 0.0f)
		{
			g_Parts[1].tbl_adr = move_tbl_arm;	// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
			g_Parts[1].tbl_size = sizeof(move_tbl_arm) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		
			g_Parts[2].tbl_adr = move_tbl_arm;	// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
			g_Parts[2].tbl_size = sizeof(move_tbl_arm) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

			g_Parts[3].tbl_adr = move_tbl_leg_left;	// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
			g_Parts[3].tbl_size = sizeof(move_tbl_leg_left) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

			g_Parts[4].tbl_adr = move_tbl_leg_right;	// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
			g_Parts[4].tbl_size = sizeof(move_tbl_leg_right) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

		}

		// �g���Ă���Ȃ珈������
		if ((g_Parts[i].use == TRUE) && (g_Parts[i].tbl_adr != NULL))
		{
			// �ړ�����
			int		index = (int)g_Parts[i].move_time;
			float	time = g_Parts[i].move_time - index;
			int		size = g_Parts[i].tbl_size;

			float dt = 1.0f / g_Parts[i].tbl_adr[index].frame;	// 1�t���[���Ői�߂鎞��
			g_Parts[i].move_time += dt;					// �A�j���[�V�����̍��v���Ԃɑ���

			if (index > (size - 2))	// �S�[�����I�[�o�[���Ă�����A�ŏ��֖߂�
			{
				g_Parts[i].move_time = 0.0f;
				index = 0;
			}

			// ���W�����߂�	X = StartX + (EndX - StartX) * ���̎���
			XMVECTOR p1 = XMLoadFloat3(&g_Parts[i].tbl_adr[index + 1].pos);	// ���̏ꏊ
			XMVECTOR p0 = XMLoadFloat3(&g_Parts[i].tbl_adr[index + 0].pos);	// ���݂̏ꏊ
			XMVECTOR vec = p1 - p0;
			XMStoreFloat3(&g_Parts[i].pos, p0 + vec * time);

			// ��]�����߂�	R = StartX + (EndX - StartX) * ���̎���
			XMVECTOR r1 = XMLoadFloat3(&g_Parts[i].tbl_adr[index + 1].rot);	// ���̊p�x
			XMVECTOR r0 = XMLoadFloat3(&g_Parts[i].tbl_adr[index + 0].rot);	// ���݂̊p�x
			XMVECTOR rot = r1 - r0;
			XMStoreFloat3(&g_Parts[i].rot, r0 + rot * time);

			// scale�����߂� S = StartX + (EndX - StartX) * ���̎���
			XMVECTOR s1 = XMLoadFloat3(&g_Parts[i].tbl_adr[index + 1].scl);	// ����Scale
			XMVECTOR s0 = XMLoadFloat3(&g_Parts[i].tbl_adr[index + 0].scl);	// ���݂�Scale
			XMVECTOR scl = s1 - s0;
			XMStoreFloat3(&g_Parts[i].scl, s0 + scl * time);

		}
	}

	// �����𐧌�
	if (g_Player.hit == TRUE)
	{
		g_fuchi = 1;
		fuchi_keep++;

		if (fuchi_keep == 20)
		{
			g_Player.hit = FALSE;
			fuchi_keep = 0;
			g_fuchi = 0;
		}
	}


	{	// �|�C���g���C�g
		LIGHT *light = GetLightData(1);
		XMFLOAT3 pos = g_Player.pos;
		pos.y += 20.0f;

		light->Position = pos;
		light->Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		light->Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		light->Type = LIGHT_TYPE_POINT;
		light->Enable = TRUE;
		SetLightData(1, light);
	}


	//////////////////////////////////////////////////////////////////////
	// �p������
	//////////////////////////////////////////////////////////////////////

	XMVECTOR vx, nvx, up;
	XMVECTOR quat;
	float len, angle;

	// �Q�̃x�N�g���̊O�ς�����ĔC�ӂ̉�]�������߂�
	g_Player.upVector = normal;
	up = { 0.0f, 1.0f, 0.0f, 0.0f };
	vx = XMVector3Cross(up, XMLoadFloat3(&g_Player.upVector));

	// ���߂���]������N�H�[�^�j�I�������o��
	nvx = XMVector3Length(vx);
	XMStoreFloat(&len, nvx);
	nvx = XMVector3Normalize(vx);
	angle = asinf(len);
	quat = XMQuaternionRotationNormal(nvx, angle);

	// �O��̃N�H�[�^�j�I�����獡��̃N�H�[�^�j�I���܂ł̉�]�����炩�ɂ���
	quat = XMQuaternionSlerp(XMLoadFloat4(&g_Player.quaternion), quat, 0.05f);

	// ����̃N�H�[�^�j�I���̌��ʂ�ۑ�����
	XMStoreFloat4(&g_Player.quaternion, quat);



#ifdef _DEBUG	// �f�o�b�O����\������
	PrintDebugProc("Player:�� �� �� ���@Space\n");
	PrintDebugProc("Player:X:%f Y:%f Z:%f\n", g_Player.pos.x, g_Player.pos.y, g_Player.pos.z);
	PrintDebugProc("player hp:%d\n", g_Player.hp);
	PrintDebugProc("fuchi:%d\n", fuchi_keep);
	PrintDebugProc("hit:%d\n", g_Player.hit);
#endif
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawPlayer(void)
{
	if (g_Player.use == TRUE)
	{	
		// �J�����O����
		SetCullingMode(CULL_MODE_NONE);

		XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_Player.scl.x, g_Player.scl.y, g_Player.scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_Player.rot.x, g_Player.rot.y + XM_PI, g_Player.rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �N�H�[�^�j�I���𔽉f
		XMMATRIX quatMatrix = XMMatrixRotationQuaternion(XMLoadFloat4(&g_Player.quaternion));
		mtxWorld = XMMatrixMultiply(mtxWorld, quatMatrix);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Player.pos.x, g_Player.pos.y, g_Player.pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Player.mtxWorld, mtxWorld);

		//�����̐ݒ�
		SetFuchi(g_fuchi);

		// ���f���`��
		DrawModel(&g_Player.model);



		// �p�[�c�̊K�w�A�j���[�V����
		for (int i = 0; i < PLAYER_PARTS_MAX; i++)
		{
			// ���[���h�}�g���b�N�X�̏�����
			mtxWorld = XMMatrixIdentity();

			// �X�P�[���𔽉f
			mtxScl = XMMatrixScaling(g_Parts[i].scl.x, g_Parts[i].scl.y, g_Parts[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// ��]�𔽉f
			mtxRot = XMMatrixRotationRollPitchYaw(g_Parts[i].rot.x, g_Parts[i].rot.y, g_Parts[i].rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			// �ړ��𔽉f
			mtxTranslate = XMMatrixTranslation(g_Parts[i].pos.x, g_Parts[i].pos.y, g_Parts[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			if (g_Parts[i].parent != NULL)	// �q����������e�ƌ�������
			{
				mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(&g_Parts[i].parent->mtxWorld));
				// ��
				// g_Player.mtxWorld���w���Ă���
			}

			XMStoreFloat4x4(&g_Parts[i].mtxWorld, mtxWorld);

			// �g���Ă���Ȃ珈������B�����܂ŏ������Ă��闝�R�͑��̃p�[�c�����̃p�[�c���Q�Ƃ��Ă���\�������邩��B
			if (g_Parts[i].use == FALSE) continue;

			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&mtxWorld);


			// ���f���`��
			DrawModel(&g_Parts[i].model);

		}

		//�����̐ݒ�
		SetFuchi(0);

		// �J�����O�ݒ��߂�
		SetCullingMode(CULL_MODE_BACK);
	}
}


//=============================================================================
// �v���C���[�����擾
//=============================================================================
PLAYER *GetPlayer(void)
{
	return &g_Player;
}


//=============================================================================
// �v���C���[�ƃ}�b�v�̓����蔻��
//=============================================================================
void CheckField(void)
{
	if (g_Player.pos.x < MAP_LEFT
		|| g_Player.pos.x > MAP_RIGHT
		|| g_Player.pos.z < MAP_DOWN
		|| g_Player.pos.z > MAP_TOP)
	{
		g_Player.pos = g_Player.pos_old;
		PlaySound(SOUND_LABEL_SE_object);
	}
}