//=============================================================================
//
// �A�C�e������ [item.cpp]
// Author : GP11A132 16 ��Ńe�C
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "shadow.h"
#include "model.h"
#include "item.h"
#include "customfunc.h"
#include "debugproc.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_ITEM				"data/MODEL/item.obj"			// �ǂݍ��ރ��f����
#define	ITEM_ROT_Y				(XM_PI/200)						// ��]
#define	ITEM_OFFSET_Y_RADIAN	(XM_2PI/200)					// �����𐧌�p���W�A��
#define	ITEM_OFFSET_Y_RADIUS	(0.1f)							// �����̕�
#define	ITEM_SHADOW_SIZE		(0.5f)							// �����̕�

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ITEM				g_Item[MAX_ITEM];						// �w�i
static float			g_rot_y;
static float			g_offset_y;
static float			g_radian;
static float			g_radius;

static BOOL				g_Load = FALSE;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitItem(void)
{
	for (int i = 0; i < MAX_ITEM; i++)
	{
		LoadModel(MODEL_ITEM, &g_Item[i].model);
		g_Item[i].load = TRUE;

		g_Item[i].use = TRUE;
		g_Item[i].effect = FALSE;

		g_Item[i].pos = XMFLOAT3((float)(RandomRange(-600,600)), 28.0f, (float)(RandomRange(-600, 600)));
		g_Item[i].rot = XMFLOAT3(0.0f, 0.0f, 1.0f);
		g_Item[i].scl = XMFLOAT3(0.6f, 0.6f, 0.6f);

		g_Item[i].size = ITEM_SIZE;

		// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		GetModelDiffuse(&g_Item[0].model, &g_Item[0].diffuse[0]);
	
		XMFLOAT3 pos = g_Item[i].pos;
		pos.y -= 0.1f;
		g_Item[i].shadowIdx = CreateShadow(pos, ITEM_SHADOW_SIZE, ITEM_SHADOW_SIZE);
	}
	
	g_rot_y = ITEM_ROT_Y;
	g_radian = 0.0f;
	g_radius = ITEM_OFFSET_Y_RADIUS;
	g_offset_y = 0.0f;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitItem(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < MAX_ITEM; i++)
	{
		if (g_Item[i].load)
		{
			UnloadModel(&g_Item[i].model);
			g_Item[i].load = FALSE;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateItem(void)
{
	for (int i = 0; i < MAX_ITEM; i++)
	{
		g_Item[i].rot.y += g_rot_y;
		g_Item[i].pos.y += g_offset_y;
	
		// �e���A�C�e���̈ʒu�ɍ��킹��
		XMFLOAT3 pos = g_Item[i].pos;
		pos.y = 0.1f;
		SetPositionShadow(g_Item[i].shadowIdx, pos);
	}
	
	g_radian += ITEM_OFFSET_Y_RADIAN;
	g_offset_y = sinf(g_radian) * g_radius;

	if (g_radian > XM_2PI)
	{
		g_radian = 0.0f;
	}



#ifdef _DEBUG	// �f�o�b�O����\������
#endif



}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawItem(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// �J�����O����
	SetCullingMode(CULL_MODE_NONE);

	for (int i = 0; i < MAX_ITEM; i++)
	{
		if (g_Item[i].use == FALSE) continue;

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_Item[i].scl.x, g_Item[i].scl.y, g_Item[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_Item[i].rot.x, g_Item[i].rot.y, g_Item[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Item[i].pos.x, g_Item[i].pos.y, g_Item[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Item[i].mtxWorld, mtxWorld);

		// ���f���`��
		DrawModel(&g_Item[i].model);
	
	}

	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_BACK);
}

//=============================================================================
// �G�l�~�[�̎擾
//=============================================================================
ITEM *GetItem()
{
	return &g_Item[0];
}
