//=============================================================================
//
// �t�@�C��	: �p�[�e�B�N������ [particle.cpp]
// �쐬��	: GP11A132 16 ��Ńe�C
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "camera.h"
#include "model.h"
#include "player.h"
#include "enemy.h"
#include "item.h"
#include "particle.h"
#include "customFunc.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_MAX					(2)			// �e�N�X�`���̐�

#define	LENGTH_FIRE					(20)		// ���̈ړ�����

#define	MAX_DIVIDED					(2)			// ���̕��򐔍ő吔
#define	THUNDER_BASEPOS_Y			(120.0f)	// ��������ł��鍂��
#define	PARTICLES_PER_DIVIDE		(5)			// ������܂ɉ��̃p�[�e�B�N���𖄂߂�
#define	THUNDER_PARTICLE_OFFSET_Y	(-1.5f)		// ���̗����X�s�[�h

#define	MAX_HEALTH					(500)		// �P�A���p�̃p�[�e�B�N��

#define	MAX_EXPLOSION				(30)		// ���j�p�̃p�[�e�B�N��

#define	MAX_ITEM_PARTICLE_1			(200)		// �A�C�e���̃p�[�e�B�N��(�T�[�N��)
#define	MAX_ITEM_PARTICLE_2			(10)		// �A�C�e���̃p�[�e�B�N��(���`)

#define PARTICLE_BASE_SIZE			(10.0f)		// �p�[�e�B�N���̃x�[�X�T�C�Y


//*****************************************************************************
// �\���̒�`
//*****************************************************************************


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT MakeVertexParticle(void);
void SetThunder(XMFLOAT3 pos, XMFLOAT3 move, float pop);
void SetFire(XMFLOAT3 pos, XMFLOAT3 move, float pop);
void SetHealth(XMFLOAT3 pos, XMFLOAT3 move, float radius, float pop);
void SetExplosion(XMFLOAT3 pos, float pop);
void SetItem(XMFLOAT3 pos, float radius, float pop);


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static BOOL isLoad = FALSE;		// ���[�h�ς݃t���O

static ID3D11Buffer					*vertexBuffer = NULL;				// ���_�o�b�t�@

static ID3D11ShaderResourceView		*textures[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����
static char *textureNames[TEXTURE_MAX] =
{
	"data/TEXTURE/effect000.jpg",
	"data/TEXTURE/effect000.jpg",
};

static PARTICLE			particles[MAX_PARTICLE];		// �p�[�e�B�N���\����
static PARTICLE_TYPE	types[PARTICLE_TYPE_MAX];		// �p�[�e�B�N���^�C�v�\����

static int				dividedCnt;						// ���̕���
static float			item_radian1;
static float			item_radian2;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitParticle(void)
{
	// ���_���̍쐬
	MakeVertexParticle();

	// �e�N�X�`������
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		textures[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(), textureNames[i], NULL, NULL, &textures[i], NULL);
	}

	// �\���̏�����
	for(int i = 0; i < MAX_PARTICLE; i++)
	{
		particles[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		particles[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		particles[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		particles[i].spd = XMFLOAT3(1.0f, 1.0f, 1.0f);

		ZeroMemory(&particles[i].mat, sizeof(particles[i].mat));
		particles[i].mat.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		particles[i].size = XMFLOAT2(PARTICLE_COLLISION_SIZE_X, PARTICLE_COLLISION_SIZE_Y);

		particles[i].life = 0;
		particles[i].inUse = FALSE;
	}

	item_radian1 = 0.0f;
	item_radian2 = 0.0f;

	// ��̏�����
	{
		types[PARTICLE_TYPE_SNOW].texIdx = 1;	// �e�N�X�`���ݒ�

		types[PARTICLE_TYPE_SNOW].life[0] = 250;	// �����ݒ�
		types[PARTICLE_TYPE_SNOW].life[1] = 300;

		types[PARTICLE_TYPE_SNOW].cnt[0] = 1;
		types[PARTICLE_TYPE_SNOW].cnt[1] = 1;

		types[PARTICLE_TYPE_SNOW].sclX[0] = 0.4f;
		types[PARTICLE_TYPE_SNOW].sclX[1] = 0.4f;

		types[PARTICLE_TYPE_SNOW].sclY[0] = 0.4f;
		types[PARTICLE_TYPE_SNOW].sclY[1] = 0.4f;

		types[PARTICLE_TYPE_SNOW].sclZ[0] = 0.4f;
		types[PARTICLE_TYPE_SNOW].sclZ[1] = 0.4f;

		types[PARTICLE_TYPE_SNOW].spdX[0] = -0.4f;
		types[PARTICLE_TYPE_SNOW].spdX[1] = +0.4f;

		types[PARTICLE_TYPE_SNOW].spdY[0] = -1.5f;
		types[PARTICLE_TYPE_SNOW].spdY[1] = -0.5f;

		types[PARTICLE_TYPE_SNOW].spdZ[0] = -0.4f;
		types[PARTICLE_TYPE_SNOW].spdZ[1] = +0.4f;

		types[PARTICLE_TYPE_SNOW].pop[0] = 0.0f;
		types[PARTICLE_TYPE_SNOW].pop[1] = 0.0f;

		types[PARTICLE_TYPE_SNOW].col = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	}


	// ��1�̏�����
	{
		types[PARTICLE_TYPE_FIRE].texIdx = 1;	// �e�N�X�`���ݒ�

		types[PARTICLE_TYPE_FIRE].life[0] = 50;	// �����ݒ�
		types[PARTICLE_TYPE_FIRE].life[1] = 70;

		types[PARTICLE_TYPE_FIRE].cnt[0] = 20;
		types[PARTICLE_TYPE_FIRE].cnt[1] = 30;

		types[PARTICLE_TYPE_FIRE].sclX[0] = 0.1f;
		types[PARTICLE_TYPE_FIRE].sclX[1] = 0.1f;

		types[PARTICLE_TYPE_FIRE].sclY[0] = 0.1f;
		types[PARTICLE_TYPE_FIRE].sclY[1] = 0.1f;

		types[PARTICLE_TYPE_FIRE].sclZ[0] = 0.1f;
		types[PARTICLE_TYPE_FIRE].sclZ[1] = 0.1f;

		types[PARTICLE_TYPE_FIRE].spdX[0] = -0.2f;
		types[PARTICLE_TYPE_FIRE].spdX[1] = +0.2f;

		types[PARTICLE_TYPE_FIRE].spdY[0] = 0.0f;
		types[PARTICLE_TYPE_FIRE].spdY[1] = +0.2f;

		types[PARTICLE_TYPE_FIRE].spdZ[0] = -0.2f;
		types[PARTICLE_TYPE_FIRE].spdZ[1] = +0.2f;

		types[PARTICLE_TYPE_FIRE].pop[0] = 0.0f;
		types[PARTICLE_TYPE_FIRE].pop[1] = 0.0f;

		types[PARTICLE_TYPE_FIRE].col = XMFLOAT4(1.0f, 0.7f, 0.7f, 1.0f);
	}

	// ��2�̏�����
	{
		types[PARTICLE_TYPE_FLAME].texIdx = 1;	// �e�N�X�`���ݒ�

		types[PARTICLE_TYPE_FLAME].life[0] = 70;	// �����ݒ�
		types[PARTICLE_TYPE_FLAME].life[1] = 70;

		types[PARTICLE_TYPE_FLAME].cnt[0] = 1;
		types[PARTICLE_TYPE_FLAME].cnt[1] = 1;

		types[PARTICLE_TYPE_FLAME].sclX[0] = 0.5f;
		types[PARTICLE_TYPE_FLAME].sclX[1] = 0.5f;

		types[PARTICLE_TYPE_FLAME].sclY[0] = 0.5f;
		types[PARTICLE_TYPE_FLAME].sclY[1] = 0.5f;

		types[PARTICLE_TYPE_FLAME].sclZ[0] = 0.5f;
		types[PARTICLE_TYPE_FLAME].sclZ[1] = 0.5f;

		types[PARTICLE_TYPE_FLAME].spdX[0] = 0.0f;
		types[PARTICLE_TYPE_FLAME].spdX[1] = 0.0f;

		types[PARTICLE_TYPE_FLAME].spdY[0] = 0.0f;
		types[PARTICLE_TYPE_FLAME].spdY[1] = 0.0f;

		types[PARTICLE_TYPE_FLAME].spdZ[0] = 0.0f;
		types[PARTICLE_TYPE_FLAME].spdZ[1] = 0.0f;

		types[PARTICLE_TYPE_FLAME].pop[0] = 0.0f;
		types[PARTICLE_TYPE_FLAME].pop[1] = 0.0f;

		types[PARTICLE_TYPE_FLAME].col = XMFLOAT4(1.0f, 0.6f, 0.5f, 1.0f);
	}

	// ���̏�����
	{
		types[PARTICLE_TYPE_THUNDER].texIdx = 1;	// �e�N�X�`���ݒ�

		types[PARTICLE_TYPE_THUNDER].life[0] = 40;	// �����ݒ�
		types[PARTICLE_TYPE_THUNDER].life[1] = 40;

		types[PARTICLE_TYPE_THUNDER].cnt[0] = 1;
		types[PARTICLE_TYPE_THUNDER].cnt[1] = 1;

		types[PARTICLE_TYPE_THUNDER].sclX[0] = 0.3f;
		types[PARTICLE_TYPE_THUNDER].sclX[1] = 0.35f;

		types[PARTICLE_TYPE_THUNDER].sclY[0] = 0.3f;
		types[PARTICLE_TYPE_THUNDER].sclY[1] = 0.35f;

		types[PARTICLE_TYPE_THUNDER].sclZ[0] = 0.3f;
		types[PARTICLE_TYPE_THUNDER].sclZ[1] = 0.35f;

		types[PARTICLE_TYPE_THUNDER].spdX[0] = 0.0f;
		types[PARTICLE_TYPE_THUNDER].spdX[1] = 0.0f;

		types[PARTICLE_TYPE_THUNDER].spdY[0] = 0.0f;
		types[PARTICLE_TYPE_THUNDER].spdY[1] = 0.0f;

		types[PARTICLE_TYPE_THUNDER].spdZ[0] = 0.0f;
		types[PARTICLE_TYPE_THUNDER].spdZ[1] = 0.0f;

		types[PARTICLE_TYPE_THUNDER].pop[0] = 0.0f;
		types[PARTICLE_TYPE_THUNDER].pop[1] = 0.0f;

		types[PARTICLE_TYPE_THUNDER].col = XMFLOAT4(1.0f, 0.7f, 1.0f, 1.0f);
	}


	// �P�A��1�̏�����
	{
		types[PARTICLE_TYPE_HEALTH].texIdx = 1;	// �e�N�X�`���ݒ�

		types[PARTICLE_TYPE_HEALTH].life[0] = 160;	// �����ݒ�
		types[PARTICLE_TYPE_HEALTH].life[1] = 160;

		types[PARTICLE_TYPE_HEALTH].cnt[0] = 1;
		types[PARTICLE_TYPE_HEALTH].cnt[1] = 1;

		types[PARTICLE_TYPE_HEALTH].sclX[0] = 0.1f;
		types[PARTICLE_TYPE_HEALTH].sclX[1] = 0.1f;

		types[PARTICLE_TYPE_HEALTH].sclY[0] = 0.1f;
		types[PARTICLE_TYPE_HEALTH].sclY[1] = 0.1f;

		types[PARTICLE_TYPE_HEALTH].sclZ[0] = 0.1f;
		types[PARTICLE_TYPE_HEALTH].sclZ[1] = 0.1f;

		types[PARTICLE_TYPE_HEALTH].spdX[0] = -0.0f;
		types[PARTICLE_TYPE_HEALTH].spdX[1] = +0.0f;

		types[PARTICLE_TYPE_HEALTH].spdY[0] = -0.0f;
		types[PARTICLE_TYPE_HEALTH].spdY[1] = -0.0f;

		types[PARTICLE_TYPE_HEALTH].spdZ[0] = -0.0f;
		types[PARTICLE_TYPE_HEALTH].spdZ[1] = +0.0f;

		types[PARTICLE_TYPE_HEALTH].pop[0] = 0.0f;
		types[PARTICLE_TYPE_HEALTH].pop[1] = 0.0f;

		types[PARTICLE_TYPE_HEALTH].col = XMFLOAT4(0.5f, 1.0f, 0.7f, 1.0f);
	}


	// �P�A��2�̏�����
	{
		types[PARTICLE_TYPE_CARE].texIdx = 1;	// �e�N�X�`���ݒ�

		types[PARTICLE_TYPE_CARE].life[0] = 160;	// �����ݒ�
		types[PARTICLE_TYPE_CARE].life[1] = 160;

		types[PARTICLE_TYPE_CARE].cnt[0] = 1;
		types[PARTICLE_TYPE_CARE].cnt[1] = 1;

		types[PARTICLE_TYPE_CARE].sclX[0] = 0.1f;
		types[PARTICLE_TYPE_CARE].sclX[1] = 0.1f;

		types[PARTICLE_TYPE_CARE].sclY[0] = 0.1f;
		types[PARTICLE_TYPE_CARE].sclY[1] = 0.1f;

		types[PARTICLE_TYPE_CARE].sclZ[0] = 0.1f;
		types[PARTICLE_TYPE_CARE].sclZ[1] = 0.1f;

		types[PARTICLE_TYPE_CARE].spdX[0] = -0.1f;
		types[PARTICLE_TYPE_CARE].spdX[1] = +0.1f;

		types[PARTICLE_TYPE_CARE].spdY[0] = -0.2f;
		types[PARTICLE_TYPE_CARE].spdY[1] = -0.1f;

		types[PARTICLE_TYPE_CARE].spdZ[0] = -0.1f;
		types[PARTICLE_TYPE_CARE].spdZ[1] = +0.1f;

		types[PARTICLE_TYPE_CARE].pop[0] = 0.0f;
		types[PARTICLE_TYPE_CARE].pop[1] = 0.0f;

		types[PARTICLE_TYPE_CARE].col = XMFLOAT4(0.7f, 1.0f, 0.5f, 1.0f);
	}


	// �����̏�����
	{
		types[PARTICLE_TYPE_EXPLOSION].texIdx = 1;

		types[PARTICLE_TYPE_EXPLOSION].life[0] = 30;
		types[PARTICLE_TYPE_EXPLOSION].life[1] = 40;

		types[PARTICLE_TYPE_EXPLOSION].cnt[0] = 10;
		types[PARTICLE_TYPE_EXPLOSION].cnt[1] = 20;

		types[PARTICLE_TYPE_EXPLOSION].sclX[0] = 0.8f;
		types[PARTICLE_TYPE_EXPLOSION].sclX[1] = 0.8f;

		types[PARTICLE_TYPE_EXPLOSION].sclY[0] = 0.8f;
		types[PARTICLE_TYPE_EXPLOSION].sclY[1] = 0.8f;

		types[PARTICLE_TYPE_EXPLOSION].sclZ[0] = 0.8f;
		types[PARTICLE_TYPE_EXPLOSION].sclZ[1] = 0.8f;

		types[PARTICLE_TYPE_EXPLOSION].spdX[0] = 0.0f;
		types[PARTICLE_TYPE_EXPLOSION].spdX[1] = 0.0f;

		types[PARTICLE_TYPE_EXPLOSION].spdY[0] = 0.0f;
		types[PARTICLE_TYPE_EXPLOSION].spdY[1] = 0.0f;

		types[PARTICLE_TYPE_EXPLOSION].spdZ[0] = 0.0f;
		types[PARTICLE_TYPE_EXPLOSION].spdZ[1] = 0.0f;

		types[PARTICLE_TYPE_EXPLOSION].pop[0] = 0.0f;
		types[PARTICLE_TYPE_EXPLOSION].pop[1] = 0.0f;

		types[PARTICLE_TYPE_EXPLOSION].col = XMFLOAT4(1.0f, 0.7f, 0.0f, 1.0f);
	}

	// �A�C�e���̏�����
	{
		types[PARTICLE_TYPE_ITEM].texIdx = 1;	// �e�N�X�`���ݒ�

		types[PARTICLE_TYPE_ITEM].life[0] = 1;	// �����ݒ�
		types[PARTICLE_TYPE_ITEM].life[1] = 1;

		types[PARTICLE_TYPE_ITEM].cnt[0] = 1;
		types[PARTICLE_TYPE_ITEM].cnt[1] = 1;

		types[PARTICLE_TYPE_ITEM].sclX[0] = 0.1f;
		types[PARTICLE_TYPE_ITEM].sclX[1] = 0.1f;

		types[PARTICLE_TYPE_ITEM].sclY[0] = 0.1f;
		types[PARTICLE_TYPE_ITEM].sclY[1] = 0.1f;

		types[PARTICLE_TYPE_ITEM].sclZ[0] = 0.1f;
		types[PARTICLE_TYPE_ITEM].sclZ[1] = 0.1f;

		types[PARTICLE_TYPE_ITEM].spdX[0] = 0.0f;
		types[PARTICLE_TYPE_ITEM].spdX[1] = 0.0f;

		types[PARTICLE_TYPE_ITEM].spdY[0] = 0.0f;
		types[PARTICLE_TYPE_ITEM].spdY[1] = 0.0f;

		types[PARTICLE_TYPE_ITEM].spdZ[0] = 0.0f;
		types[PARTICLE_TYPE_ITEM].spdZ[1] = 0.0f;

		types[PARTICLE_TYPE_ITEM].pop[0] = 0.0f;
		types[PARTICLE_TYPE_ITEM].pop[1] = 0.0f;

		types[PARTICLE_TYPE_ITEM].col = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	}

	isLoad = TRUE;
	return S_OK;
}


//=============================================================================
// �I������
//=============================================================================
void UninitParticle(void)
{
	if (isLoad == FALSE) return;

	//�e�N�X�`���̉��
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (textures[i] != NULL)
		{
			textures[i]->Release();
			textures[i] = NULL;
		}
	}

	// ���_�o�b�t�@�̉��
	if (vertexBuffer != NULL)
	{
		vertexBuffer->Release();
		vertexBuffer = NULL;
	}

	isLoad = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateParticle(void)
{
	for (int i = 0; i < MAX_PARTICLE; i++)
	{
		if (particles[i].inUse == FALSE) continue;

		// �ړ�����
		particles[i].pos.x += particles[i].spd.x;
		particles[i].pos.y += particles[i].spd.y;
		particles[i].pos.z += particles[i].spd.z;

		// �����̏���
		particles[i].life--;
		if (particles[i].life <= 10)
		{	// �������R�O�t���[���؂�����i�X�����ɂȂ��Ă���
			particles[i].mat.Diffuse.w -= 0.1f;
			if (particles[i].mat.Diffuse.w < 0.0f)
			{
				particles[i].mat.Diffuse.w = 0.0f;
			}
		}
		// �����؂��������
		if (particles[i].life <= 0)
		{
			particles[i].inUse = FALSE;
		}
		// �n�ʂ̉��ɍs�����Ȃ��悤��
		if (particles[i].pos.y < 0)
		{
			particles[i].pos.y = 0.0f;
			particles[i].life = 10;
			particles[i].spd = XMFLOAT3(0.0f, 0.0f, 0.0f);
		}
		

		// �\���^�C�~���O�̏���
		if (particles[i].pop > 0.0f)
		{
			particles[i].pop--;
		}
	}


	// �K�v�ȏ����擾
	PLAYER *player = GetPlayer();
	ENEMY *enemy = GetEnemy();

	if (enemy[0].hp > enemy[0].hp_max / 2)
	// ����̏���
	{
		XMFLOAT3 pos;
		pos.x = (float)(RandomRange(-500, 500));
		pos.y = 200.0f;
		pos.z = (float)(RandomRange(-500, 500));

		SetParticle(PARTICLE_TYPE_SNOW, pos);
	}


	// ���̔�������
	if (enemy[0].skill == 0)
	{
		XMFLOAT3 pos = enemy[0].pos;
		XMFLOAT3 move = XMFLOAT3(- sinf(enemy[0].rot.y) * 3.0f, 0.0f, - cosf(enemy[0].rot.y) * 3.0f);

		pos.x = enemy[0].pos.x - sinf(enemy[0].rot.y) * 30.0f;
		pos.z = enemy[0].pos.z - cosf(enemy[0].rot.y) * 30.0f;
		
		types[PARTICLE_TYPE_FIRE].pop[0] = 0.0f;
		types[PARTICLE_TYPE_FIRE].pop[1] = 0.0f;

		types[PARTICLE_TYPE_FLAME].pop[0] = 0.0f;
		types[PARTICLE_TYPE_FLAME].pop[1] = 0.0f;

		SetFire(pos, move, 0.0f);
	}
	

	// ���̔�������
	if (enemy[0].hp < 50)
	{
		int k = rand() % 40;

		if (k == 0) 
		{
			dividedCnt = 0;

			// �p�[�e�B�N�����Z�b�g
			XMFLOAT3 pos = XMFLOAT3(0.0f, THUNDER_BASEPOS_Y, 0.0f);
			XMFLOAT3 move = XMFLOAT3(0.0f, THUNDER_PARTICLE_OFFSET_Y, 0.0f);

			//pos.x = player->pos.x - sinf(player->rot.y) * 60.0f;
			//pos.z = player->pos.z - cosf(player->rot.y) * 60.0f;

			pos.x = (float)(RandomRange(-500, 500));
			pos.z = (float)(RandomRange(-500, 500));

			SetThunder(pos, move, 0.0f);
		}

		//// �����̗����o���ꍇ
		//// 2�ڂ̗��̏���
		// pos = XMFLOAT3(200.0f, 150.0f, 0.0f);
		// move = XMFLOAT3(0.0f, THUNDER_PARTICLE_OFFSET_Y, 0.0f);

		//types[PARTICLE_TYPE_THUNDER].pop[0] = 0.0f;
		//types[PARTICLE_TYPE_THUNDER].pop[1] = 0.0f;

		//SetThunder(pos, move, 0.0f);
	}


	//���j
	if (player->hit == TRUE)
	{
		XMFLOAT3 pos = player->pos;

		types[PARTICLE_TYPE_EXPLOSION].pop[0] = 0.0f;
		types[PARTICLE_TYPE_EXPLOSION].pop[1] = 0.0f;
	
		SetExplosion(pos, 0.0f);

	}

	//�A�C�e��
	{
		ITEM *item = GetItem();

		for (int i = 0; i < MAX_ITEM; i++)
		{
			if (item[i].use == TRUE)
			{
				XMFLOAT3 pos = item[i].pos;
				pos.y = 0.0f;
				XMFLOAT3 move = XMFLOAT3(0.0f, 0.0f, 0.0f);
				float radius = 12.0f;

				types[PARTICLE_TYPE_ITEM].pop[0] = 0.0f;
				types[PARTICLE_TYPE_ITEM].pop[1] = 0.0f;

				SetItem(pos, radius, 0.0f);
			}
		

			//�P�A��
			if (item[i].effect == TRUE)
			{
				PLAYER *player = GetPlayer();

				XMFLOAT3 pos = player->pos;
				XMFLOAT3 move = XMFLOAT3(0.0f, 0.0f, 0.0f);
				float radius = 2.0f;

				pos.y = player->pos.y + 40.0f;

				types[PARTICLE_TYPE_HEALTH].pop[0] = 0.0f;
				types[PARTICLE_TYPE_HEALTH].pop[1] = 0.0f;

				types[PARTICLE_TYPE_CARE].pop[0] = 0.0f;
				types[PARTICLE_TYPE_CARE].pop[1] = 0.0f;

				SetHealth(pos, move, radius, 0.0f);
				
				item[i].effect = FALSE;
			}
		}
	}



}


// ���̈ړ�����
void SetFire(XMFLOAT3 pos, XMFLOAT3 move,float pop)
{
	// �p�����[�^��������
	types[PARTICLE_TYPE_FIRE].sclX[0] = 0.1f;
	types[PARTICLE_TYPE_FIRE].sclX[1] = 0.1f;

	types[PARTICLE_TYPE_FIRE].sclY[0] = 0.1f;
	types[PARTICLE_TYPE_FIRE].sclY[1] = 0.1f;

	types[PARTICLE_TYPE_FIRE].sclZ[0] = 0.1f;
	types[PARTICLE_TYPE_FIRE].sclZ[1] = 0.1f;

	types[PARTICLE_TYPE_FLAME].sclX[0] = 0.5f;
	types[PARTICLE_TYPE_FLAME].sclX[1] = 0.5f;

	types[PARTICLE_TYPE_FLAME].sclY[0] = 0.5f;
	types[PARTICLE_TYPE_FLAME].sclY[1] = 0.5f;

	types[PARTICLE_TYPE_FLAME].sclZ[0] = 0.5f;
	types[PARTICLE_TYPE_FLAME].sclZ[1] = 0.5f;

	types[PARTICLE_TYPE_FIRE].spdX[0] = -0.2f;
	types[PARTICLE_TYPE_FIRE].spdX[1] = +0.2f;

	types[PARTICLE_TYPE_FIRE].spdY[0] = 0.0f;
	types[PARTICLE_TYPE_FIRE].spdY[1] = +0.2f;

	types[PARTICLE_TYPE_FIRE].spdZ[0] = -0.2f;
	types[PARTICLE_TYPE_FIRE].spdZ[1] = +0.2f;

	// ���Έʒu�����킹��
	types[PARTICLE_TYPE_FIRE].spdX[0] += move.x;
	types[PARTICLE_TYPE_FIRE].spdX[1] += move.x;

	types[PARTICLE_TYPE_FIRE].spdZ[0] += move.z;
	types[PARTICLE_TYPE_FIRE].spdZ[1] += move.z;

	types[PARTICLE_TYPE_FLAME].spdX[0] = move.x;
	types[PARTICLE_TYPE_FLAME].spdX[1] = move.x;

	types[PARTICLE_TYPE_FLAME].spdZ[0] = move.z;
	types[PARTICLE_TYPE_FLAME].spdZ[1] = move.z;

	for (int i = 0; i < LENGTH_FIRE; i++)
	{
		SetParticle(PARTICLE_TYPE_FIRE, pos);
		SetParticle(PARTICLE_TYPE_FLAME, pos);

		// �^�C�~���O�����炵�ĕ\��������
		types[PARTICLE_TYPE_FIRE].pop[0] = pop + 5.0f * i;
		types[PARTICLE_TYPE_FIRE].pop[1] = pop + 5.0f * i;
	
		types[PARTICLE_TYPE_FLAME].pop[0] = pop + 5.0f * i;
		types[PARTICLE_TYPE_FLAME].pop[1] = pop + 5.0f * i;

		pos.x += move.x;
		pos.y += move.y;
		pos.z += move.z;

		types[PARTICLE_TYPE_FIRE].sclX[0] += 0.01f;
		types[PARTICLE_TYPE_FIRE].sclX[1] += 0.01f;

		types[PARTICLE_TYPE_FIRE].sclY[0] += 0.01f;
		types[PARTICLE_TYPE_FIRE].sclY[1] += 0.01f;

		types[PARTICLE_TYPE_FIRE].sclZ[0] += 0.01f;
		types[PARTICLE_TYPE_FIRE].sclZ[1] += 0.01f;

		types[PARTICLE_TYPE_FLAME].sclX[0] += 0.07f;
		types[PARTICLE_TYPE_FLAME].sclX[1] += 0.07f;

		types[PARTICLE_TYPE_FLAME].sclY[0] += 0.07f;
		types[PARTICLE_TYPE_FLAME].sclY[1] += 0.07f;

		types[PARTICLE_TYPE_FLAME].sclZ[0] += 0.07f;
		types[PARTICLE_TYPE_FLAME].sclZ[1] += 0.07f;
	}

};


// ���̑g������
void SetThunder(XMFLOAT3 pos, XMFLOAT3 move, float pop)
{
	for (int k = 1; k < 150; k++)
	{
		if (pos.y < 0.0f) break;	// �n�ʂɒ�������I���

		if (k % PARTICLES_PER_DIVIDE == 0)
		{// PARTICLES_PER_DIVIDE�Z�b�g������

			// �ړ��ʂ��Z�b�g
			move.x = RandomFloat(2, 1.5f, -1.5f);
			move.z = RandomFloat(2, 1.5f, -1.5f);

			if (RandomRange(0, 100) < 30 && dividedCnt < MAX_DIVIDED && pos.y > THUNDER_BASEPOS_Y * 0.5f)
			{// 30���̊m���ŕ��򂳂���
				dividedCnt++;

				// ���򂳂ꂽ���̓��������̔��΂̐��l�ɂ���
				XMFLOAT3 invMove = { move.x * -1, move.y, move.z * -1 };
				
				// �傫�����Z�b�g
				float scl = RandomFloat(2, 0.35f, 0.3f);
				
				types[PARTICLE_TYPE_THUNDER].sclX[0] = scl;
				types[PARTICLE_TYPE_THUNDER].sclX[1] = scl;

				types[PARTICLE_TYPE_THUNDER].sclZ[0] = scl;
				types[PARTICLE_TYPE_THUNDER].sclZ[1] = scl;

				types[PARTICLE_TYPE_THUNDER].sclZ[0] = scl;
				types[PARTICLE_TYPE_THUNDER].sclZ[1] = scl;

				SetThunder(pos, invMove, types[PARTICLE_TYPE_THUNDER].pop[0]);
			}

		}
		
		SetParticle(PARTICLE_TYPE_THUNDER, pos);

		pos.x += move.x;
		pos.y += move.y;
		pos.z += move.z;

		// �^�C�~���O�����炵�ĕ\��������
		types[PARTICLE_TYPE_THUNDER].pop[0] = pop + 0.1f * k;
		types[PARTICLE_TYPE_THUNDER].pop[1] = pop + 0.1f * k;
	}
};



// �P�A���̑g������
void SetHealth(XMFLOAT3 pos, XMFLOAT3 move, float radius, float pop)
{
	XMFLOAT3 temp = pos;
	float radian = 0.0f;
	float add = XM_PI / 40;
	
	for (int i = 0; i < MAX_HEALTH; i++)
	{
		if (radian > XM_2PI)
		{
			radius += 5.0f;
			temp.y -= 10.0f;
			radian = 0.0f;
		}
		
		SetParticle(PARTICLE_TYPE_HEALTH, temp);
		SetParticle(PARTICLE_TYPE_CARE, temp);

		temp.x = pos.x + sinf(radian) * radius;
		temp.z = pos.z + cosf(radian) * radius;

		radian += add;

		// �^�C�~���O�����炵�ĕ\��������
		types[PARTICLE_TYPE_HEALTH].pop[0] = pop + 0.2f * i;
		types[PARTICLE_TYPE_HEALTH].pop[1] = pop + 0.2f * i;

		types[PARTICLE_TYPE_CARE].pop[0] = pop + 0.2f * i;
		types[PARTICLE_TYPE_CARE].pop[1] = pop + 0.2f * i;
	}

};


// ���j�̑g������
void SetExplosion(XMFLOAT3 pos, float pop)
{
	XMFLOAT3 temp = pos;
	XMFLOAT3 move = XMFLOAT3(0.0f, 0.0f, 0.0f);
	float scl = 0.0f;

	for (int i = 0; i < MAX_EXPLOSION; i++)
	{
	
		int col = rand() & 3;
		switch (col)
		{
		case 0:
			types[PARTICLE_TYPE_EXPLOSION].col = XMFLOAT4(0.7f, 0.4f, 0.0f, 1.0f);
			break;

		case 1:
			types[PARTICLE_TYPE_EXPLOSION].col = XMFLOAT4(0.4f, 0.2f, 0.0f, 1.0f);
			break;
		
		case 2:
			types[PARTICLE_TYPE_EXPLOSION].col = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
			break;
		}

		SetParticle(PARTICLE_TYPE_EXPLOSION, temp);

		move.x = RandomFloat(2, 7.0f, -7.0f);
		move.y = RandomFloat(2, 7.0f, -7.0f);
		move.z = RandomFloat(2, 7.0f, -7.0f);

		temp.x = pos.x + move.x;
		temp.y = pos.y + move.y;
		temp.z = pos.z + move.z;

		// �^�C�~���O�����炵�ĕ\��������
		types[PARTICLE_TYPE_EXPLOSION].pop[0] = pop + 0.5f * i;
		types[PARTICLE_TYPE_EXPLOSION].pop[1] = pop + 0.5f * i;
	}
}


// �A�C�e���̑g������
void SetItem(XMFLOAT3 pos, float radius, float pop)
{
	float radian = 0.0f;
	XMFLOAT3 temp = pos;

	// �e�p�[�e�B�N���Ԃ̋������w��
	float add = XM_2PI / MAX_ITEM_PARTICLE_1;

	// �n�ʂ̃T�[�N���𐶐�
	for (int i = 0; i < MAX_ITEM_PARTICLE_1; i++)
	{
		// 360�����z������break����
		if (radian > XM_2PI)break;

		types[PARTICLE_TYPE_ITEM].sclY[0] = 0.2f;
		types[PARTICLE_TYPE_ITEM].sclY[1] = 0.2f;

		SetParticle(PARTICLE_TYPE_ITEM, temp);

		temp.x = pos.x + sinf(radian) * radius;
		temp.z = pos.z + cosf(radian) * radius;

		radian += add;

		// �^�C�~���O�����炵�ĕ\��������
		//types[PARTICLE_TYPE_ITEM].pop[0] = pop + 0.3f * i;
		//types[PARTICLE_TYPE_ITEM].pop[1] = pop + 0.3f * i;

	}
	
	// �p�x��0.0f�ɖ߂�
	radian = item_radian1;
	
	// �����W��0.0f�ɖ߂�
	temp.y = 0.0f;

	// �e�p�[�e�B�N���Ԃ̋�����������
	add = XM_2PI / MAX_ITEM_PARTICLE_2;
	
	// �p�[�e�B�N���s�����x�̃f�[�^������Ă���
	float old_alpha = types[PARTICLE_TYPE_ITEM].col.w;
	
	// �s�����x��sin�J�[�u�ŕ\���@0.0f+0.3f�`1.0f
	float alpha = sinf(item_radian2) + 0.3f;
	
	if (alpha > 1.0f)
	{
		alpha = 1.0f;
	}
	
	// ���`�p�[�e�B�N���𐶐�����
	for (int i = 0; i < MAX_ITEM_PARTICLE_2; i++)
	{
		types[PARTICLE_TYPE_ITEM].sclY[0] = 5.0f;
		types[PARTICLE_TYPE_ITEM].sclY[1] = 5.0f;

		temp.x = pos.x + sinf(radian) * radius;
		temp.z = pos.z + cosf(radian) * radius;

		radian += add;
		types[PARTICLE_TYPE_ITEM].col.w = alpha;

		SetParticle(PARTICLE_TYPE_ITEM, temp);

	}
	
	// �p�[�e�B�N���̕s�����x��߂�
	types[PARTICLE_TYPE_ITEM].col.w = old_alpha;
	
	// ���`�p�[�e�B�N������̉�]�ʂ𑫂�
	item_radian1 += 0.001f;
	
	// ���`�p�[�e�B�N������̕s�����x�𑫂�
	item_radian2 += 0.01f;

	if (item_radian1 > XM_2PI)
	{
		item_radian1 = 0.0f;
	}

	if (item_radian2 > XM_2PI)
	{
		item_radian2 = 0.0f;
	}

};


//=============================================================================
// �`�揈��
//=============================================================================
void DrawParticle(void)
{
	CAMERA *cam = GetCamera();
	XMMATRIX mtxScl, mtxTranslate, mtxWorld, mtxView;

	// ���C�e�B���O�𖳌���
	SetLightEnable(false);

	// ���Z�����ɐݒ�
	SetBlendState(BLEND_MODE_ADD);

	// Z��r����
	SetDepthEnable(false);

	// �t�H�O����
	SetFogEnable(false);

	int state = rand() % 4;

	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	for(int i = 0; i < MAX_PARTICLE; i++)
	{
		if (particles[i].inUse == FALSE || particles[i].pop > 0.0f) continue;

		if (state == 1 && particles[i].type == PARTICLE_TYPE_THUNDER)
		{
			// ���Z�����ɐݒ�
			SetBlendState(BLEND_MODE_SUBTRACT);
		}
		
		else
		{
			// ���Z�����ɐݒ�
			SetBlendState(BLEND_MODE_ADD);
		}

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �r���[�}�g���b�N�X���擾
		mtxView = XMLoadFloat4x4(&cam->mtxView);

		// �r���{�[�h����
		mtxWorld.r[0].m128_f32[0] = mtxView.r[0].m128_f32[0];
		mtxWorld.r[0].m128_f32[1] = mtxView.r[1].m128_f32[0];
		mtxWorld.r[0].m128_f32[2] = mtxView.r[2].m128_f32[0];

		mtxWorld.r[1].m128_f32[0] = mtxView.r[0].m128_f32[1];
		mtxWorld.r[1].m128_f32[1] = mtxView.r[1].m128_f32[1];
		mtxWorld.r[1].m128_f32[2] = mtxView.r[2].m128_f32[1];

		mtxWorld.r[2].m128_f32[0] = mtxView.r[0].m128_f32[2];
		mtxWorld.r[2].m128_f32[1] = mtxView.r[1].m128_f32[2];
		mtxWorld.r[2].m128_f32[2] = mtxView.r[2].m128_f32[2];

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(particles[i].scl.x, particles[i].scl.y, particles[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(particles[i].pos.x, particles[i].pos.y, particles[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		// �e�N�X�`���ݒ�
		int texIndex = types[particles[i].type].texIdx;
		GetDeviceContext()->PSSetShaderResources(0, 1, &textures[texIndex]);

		// �}�e���A����ݒ�
		SetMaterial(particles[i].mat);

		// �|���S���̕`��
		GetDeviceContext()->Draw(4, 0);
	}
	

	// ���C�e�B���O��L����
	SetLightEnable(true);

	// �ʏ�u�����h�ɖ߂�
	SetBlendState(BLEND_MODE_ALPHABLEND);

	// Z��r�L��
	SetDepthEnable(TRUE);

}


//=============================================================================
// ���_���̍쐬
//=============================================================================
HRESULT MakeVertexParticle(void)
{
	float a = PARTICLE_BASE_SIZE;

	VERTEX_3D vertexArray[4] =
	{
		{ XMFLOAT3(-5.0f, +5.0f, 0.0f),  XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(+5.0f, +5.0f, 0.0f),  XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(-5.0f, -5.0f, 0.0f),  XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(+5.0f, -5.0f, 0.0f),  XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
	};

	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.pSysMem = vertexArray;

	GetDevice()->CreateBuffer(&bd, &sd, &vertexBuffer);

	return S_OK;
}


//=============================================================================
// �p�[�e�B�N���̔�������
//=============================================================================
void SetParticle(int type, XMFLOAT3 pos)
{
	// ��������G�t�F�N�g��I��
	PARTICLE_TYPE *curType = &types[type];

	// ������������
	int cnt = RandomRange(curType->cnt[0], curType->cnt[1]);

	for (int i = 0; i < cnt; i++)
	{
		for (int j = 0; j < MAX_PARTICLE; j++)
		{
			// �p�[�e�B�N������
			if (particles[j].inUse == FALSE)
			{
				particles[j].type = type;

				particles[j].pos = pos;

				particles[j].life = RandomRange(curType->life[0], curType->life[1]);

				particles[j].scl.x = (float)(RandomRange((int)(curType->sclX[0] * 10), (int)(curType->sclX[1] * 10))) * 0.1f;
				particles[j].scl.y = (float)(RandomRange((int)(curType->sclY[0] * 10), (int)(curType->sclY[1] * 10))) * 0.1f;
				particles[j].scl.z = (float)(RandomRange((int)(curType->sclZ[0] * 10), (int)(curType->sclZ[1] * 10))) * 0.1f;

				particles[j].spd.x = (float)(RandomRange((int)(curType->spdX[0] * 10), (int)(curType->spdX[1] * 10))) * 0.1f;
				particles[j].spd.y = (float)(RandomRange((int)(curType->spdY[0] * 10), (int)(curType->spdY[1] * 10))) * 0.1f;
				particles[j].spd.z = (float)(RandomRange((int)(curType->spdZ[0] * 10), (int)(curType->spdZ[1] * 10))) * 0.1f;

				particles[j].pop = (float)(RandomRange((int)(curType->pop[0] * 10), (int)(curType->pop[1] * 10))) * 0.1f;

				particles[j].mat.Diffuse = curType->col;


				particles[j].inUse = TRUE;
				break;
			}
		}
	}
}


//=============================================================================
// �p�[�e�B�N�����̎擾
//=============================================================================
PARTICLE *GetParticle()
{
	return &particles[0];
}
