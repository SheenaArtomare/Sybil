//=============================================================================
//
// ���@�w���� [circle.cpp]
// Author : GP11A132 16 ��Ńe�C
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "shadow.h"
#include "model.h"
#include "player.h"
#include "circle.h"
#include "sound.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_MAX			(1)				// �e�N�X�`���̐�

#define	CIRCLE_WIDTH		(50.0f)			// ���_�T�C�Y
#define	CIRCLE_HEIGHT		(50.0f)			// ���_�T�C�Y
#define	CIRCLE_SHADOW_SIZE	(1.2f)			// �V���h�E�̃T�C�Y
#define	CIRCLE_MOVE_SPEED	(2.0f)			// �ړ���


//*****************************************************************************
// �\���̒�`
//*****************************************************************************


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT MakeVertexCircle(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer					*g_VertexBuffer = NULL;	// ���_�o�b�t�@
static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static CIRCLE						g_Circle[MAX_CIRCLE];	// ���@�w���[�N
static int							g_TexNo;				// �e�N�X�`���ԍ�

static char *g_TextureName[TEXTURE_MAX] =
{
	"data/TEXTURE/circle.png",
};

static BOOL							g_Load = FALSE;


//=============================================================================
// ����������
//=============================================================================
HRESULT InitCircle(void)
{
	MakeVertexCircle();

	// �e�N�X�`������
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TextureName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}

	g_TexNo = 0;

	// ���@�w���[�N�̏�����
	for (int nCntCircle = 0; nCntCircle < MAX_CIRCLE; nCntCircle++)
	{
		ZeroMemory(&g_Circle[nCntCircle].material, sizeof(g_Circle[nCntCircle].material));
		g_Circle[nCntCircle].material.Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };

		g_Circle[nCntCircle].pos = { 0.0f, 5.0f, 0.0f };
		g_Circle[nCntCircle].rot = { 0.0f, 0.0f, 0.0f };
		g_Circle[nCntCircle].scl = { 1.0f, 1.0f, 1.0f };
		g_Circle[nCntCircle].spd = 0.0f;
		g_Circle[nCntCircle].fWidth = CIRCLE_WIDTH;
		g_Circle[nCntCircle].fHeight = CIRCLE_HEIGHT;
		g_Circle[nCntCircle].use = FALSE;

	}

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitCircle(void)
{
	if (g_Load == FALSE) return;

	for (int nCntTex = 0; nCntTex < TEXTURE_MAX; nCntTex++)
	{
		if (g_Texture[nCntTex] != NULL)
		{// �e�N�X�`���̉��
			g_Texture[nCntTex]->Release();
			g_Texture[nCntTex] = NULL;
		}
	}

	if (g_VertexBuffer != NULL)
	{// ���_�o�b�t�@�̉��
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateCircle(void)
{
	for (int i = 0; i < MAX_CIRCLE; i++)
	{
		// �v���C���[�̏����擾
		PLAYER *player = GetPlayer();
		CAMERA *cam = GetCamera();

		float rot = player->rot.y;
		float dir = player->dir;
		
		g_Circle[i].pos = player->pos;
		g_Circle[i].spd = 0.0f;

		if (g_Circle[i].use)
		{
			if (g_Circle[i].spd > 0)
			{
				// ���@�w�̈ړ�����
				rot = dir + cam->rot.y;

				g_Circle[i].pos.x -= sinf(rot) * g_Circle[i].spd;
				g_Circle[i].pos.z -= cosf(rot) * g_Circle[i].spd;
			}
		}

		if (GetKeyboardRepeat(DIK_5))
		{
			g_Circle[i].use = TRUE;
			player->isATK = TRUE;

			// ���@�w�𓮂���
			if (GetKeyboardPress(DIK_A))
			{	// ���ֈړ�
				g_Circle[i].spd = CIRCLE_MOVE_SPEED;
				dir = XM_PI / 2;
			}
			if (GetKeyboardPress(DIK_D))
			{	// �E�ֈړ�
				g_Circle[i].spd = CIRCLE_MOVE_SPEED;
				dir = -XM_PI / 2;
			}
			if (GetKeyboardPress(DIK_W))
			{	// ��ֈړ�
				g_Circle[i].spd = CIRCLE_MOVE_SPEED;
				dir = XM_PI;
			}
			if (GetKeyboardPress(DIK_S))
			{	// ���ֈړ�
				g_Circle[i].spd = CIRCLE_MOVE_SPEED;
				dir = 0.0f;
			}


			// Key���͂���������ړ���������
			if (g_Circle[i].spd > 0.0f)
			{
				g_Circle[i].rot.y = dir + cam->rot.y;

				// ���͂̂����������փv���C���[���������Ĉړ�������
				g_Circle[i].pos.x -= sinf(g_Circle[i].rot.y) * g_Circle[i].spd;
				g_Circle[i].pos.z -= cosf(g_Circle[i].rot.y) * g_Circle[i].spd;
			}

			g_Circle[i].spd *= 0.5f;




			// �e�����
			//XMFLOAT3 pos = g_Circle[i].pos;
			//pos.y = 0.1f;
			//g_Circle[i].shadowIdx = CreateShadow(pos, CIRCLE_SHADOW_SIZE, CIRCLE_SHADOW_SIZE);

			
			// �e�����@�w�̈ʒu�ɍ��킹��
			//XMFLOAT3 pos = g_Circle[i].pos;
			//pos.y = 0.1f;
			//SetPositionShadow(g_Circle[i].shadowIdx, pos);

		}

		if (GetKeyboardRelease(DIK_5))
		{
			g_Circle[i].use = FALSE;
			player->isATK = FALSE;

			//ReleaseShadow(g_Circle[i].shadowIdx);
		}

	}

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawCircle(void)
{
	// ���e�X�g��L����
	SetAlphaTestEnable(TRUE);

	// ���C�e�B���O�𖳌�
	SetLightEnable(FALSE);

	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	for (int i = 0; i < MAX_CIRCLE; i++)
	{
		if (g_Circle[i].use)
		{
			// ���[���h�}�g���b�N�X�̏�����
			mtxWorld = XMMatrixIdentity();

			// �X�P�[���𔽉f
			mtxScl = XMMatrixScaling(g_Circle[i].scl.x, g_Circle[i].scl.y, g_Circle[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// ��]�𔽉f
			mtxRot = XMMatrixRotationRollPitchYaw(g_Circle[i].rot.x, g_Circle[i].rot.y + XM_PI, g_Circle[i].rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			// �ړ��𔽉f
			mtxTranslate = XMMatrixTranslation(g_Circle[i].pos.x, g_Circle[i].pos.y, g_Circle[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&mtxWorld);

			XMStoreFloat4x4(&g_Circle[i].mtxWorld, mtxWorld);


			// �}�e���A���ݒ�
			SetMaterial(g_Circle[i].material);

			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

			// �|���S���̕`��
			GetDeviceContext()->Draw(4, 0);
		}
	}

	// ���e�X�g�𖳌���
	SetAlphaTestEnable(FALSE);

	// ���C�e�B���O��L����
	SetLightEnable(TRUE);

}

//=============================================================================
// ���_���̍쐬
//=============================================================================
HRESULT MakeVertexCircle(void)
{
	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	// ���_�o�b�t�@�ɒl���Z�b�g����
	D3D11_MAPPED_SUBRESOURCE msr;
	GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

	float fWidth = CIRCLE_WIDTH;
	float fHeight = CIRCLE_HEIGHT;

	// ���_���W�̐ݒ�
	vertex[0].Position = { -fWidth / 2.0f, 0.0f,  fHeight / 2.0f };
	vertex[1].Position = {  fWidth / 2.0f, 0.0f,  fHeight / 2.0f };
	vertex[2].Position = { -fWidth / 2.0f, 0.0f, -fHeight / 2.0f };
	vertex[3].Position = {  fWidth / 2.0f, 0.0f, -fHeight / 2.0f };

	// �g�U���̐ݒ�
	vertex[0].Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	vertex[1].Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	vertex[2].Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	vertex[3].Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };

	// �e�N�X�`�����W�̐ݒ�
	vertex[0].TexCoord = { 0.0f, 0.0f };
	vertex[1].TexCoord = { 1.0f, 0.0f };
	vertex[2].TexCoord = { 0.0f, 1.0f };
	vertex[3].TexCoord = { 1.0f, 1.0f };

	GetDeviceContext()->Unmap(g_VertexBuffer, 0);

	return S_OK;
}



//=============================================================================
// �e�̎擾
//=============================================================================
CIRCLE *GetCircle(void)
{
	return &(g_Circle[0]);
}

