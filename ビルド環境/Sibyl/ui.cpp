//=============================================================================
//
// UI���� [ui.cpp]
// Author : GP11A132 16 ��Ńe�C
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "model.h"
#include "enemy.h"
#include "player.h"
#include "ui.h"
#include "sprite.h"
#include "sound.h"
#include "fade.h"
#include "tutorial.h"
#include "debugproc.h"
#include "input.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(350)	// �L�����T�C�Y
#define TEXTURE_HEIGHT				(150)	// 
#define TEXTURE_MAX					(12)	// �e�N�X�`���̐�

#define TEXTURE_WIDTH_PLAYER		(270)	// �L�����T�C�Y
#define TEXTURE_HEIGHT_PLAYER		(136)	// 

#define TEXTURE_WIDTH_PLAYER_2		(170)	// �L�����T�C�Y
#define TEXTURE_HEIGHT_PLAYER_2		(19)	// 

#define TEXTURE_WIDTH_ENEMY			(750)	// �L�����T�C�Y
#define TEXTURE_HEIGHT_ENEMY		(160)	// 

#define TEXTURE_WIDTH_ENEMY_2		(584)	// �L�����T�C�Y
#define TEXTURE_HEIGHT_ENEMY_2		(25)	// 

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/ui000.png",
	"data/TEXTURE/ui001.png",
	"data/TEXTURE/ui002.png",
	"data/TEXTURE/ui003.png",
	"data/TEXTURE/ui004.png",
	"data/TEXTURE/player_hp0.png",
	"data/TEXTURE/player_hp1.png",
	"data/TEXTURE/player_hp2.png",
	"data/TEXTURE/enemy_hp0.png",
	"data/TEXTURE/enemy_hp1.png",
	"data/TEXTURE/enemy_hp2.png",
	"data/TEXTURE/enemy_hp3.png",
};


static BOOL						g_Use;						// TRUE:�g���Ă���  FALSE:���g�p
static float					g_w, g_h;					// ���ƍ���
static XMFLOAT3					g_Pos;						// �|���S���̍��W
static int						g_TexNo;					// �e�N�X�`���ԍ�

static int						g_time;						// ���ԊǗ��p
static int						g_sound;					// ���ʉ���炷�p
static int						g_step;						// ���ԊǗ��p

static BOOL						g_Load = FALSE;

																	// �v���C���[��HP�\��
static BOOL						g_Use_player;						// TRUE:�g���Ă���  FALSE:���g�p
static float					g_w_player, g_h_player;				// ���ƍ���max
static float					g_offset_player;					// ���ƍ���
static XMFLOAT3					g_Pos_player;						// �|���S���̍��W
static int						g_TexNo_player;						// �e�N�X�`���ԍ�

																	// �G�l�~�[��HP�\��
static BOOL						g_Use_enemy;						// TRUE:�g���Ă���  FALSE:���g�p
static float					g_w_enemy, g_h_enemy;				// ���ƍ���max
static float					g_offset_enemy;						// ���ƍ���
static XMFLOAT3					g_Pos_enemy;						// �|���S���̍��W
static int						g_TexNo_enemy;						// �e�N�X�`���ԍ�


//=============================================================================
// ����������
//=============================================================================
HRESULT InitUI(void)
{
	ID3D11Device *pDevice = GetDevice();

	//�e�N�X�`������
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TexturName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}


	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);


	// ui�̏�����
	g_Use   = FALSE;
	g_w     = TEXTURE_WIDTH;
	g_h     = TEXTURE_HEIGHT;
	g_Pos   = { 480.0f, 90.0f, 0.0f };
	g_TexNo = 1;

	g_time = 0;
	g_step = 0;
	g_sound = 0;
	
	
	// �v���C���[��HP�\��������
	g_Use_player = TRUE;
	g_w_player = TEXTURE_WIDTH_PLAYER;
	g_h_player = TEXTURE_HEIGHT_PLAYER;
	g_offset_player = 0.0f;
	g_Pos_player = { 150.0f, 460.0f, 0.0f };
	g_TexNo_player = 5;


	// �G�l�~�[��HP�\��������
	g_Use_enemy = TRUE;
	g_w_enemy = TEXTURE_WIDTH_ENEMY;
	g_h_enemy = TEXTURE_HEIGHT_ENEMY;
	g_offset_enemy = 0.0f;
	g_Pos_enemy = { 480.0f, 80.0f, 0.0f };
	g_TexNo_enemy = 8;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitUI(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (g_Texture[i])
		{
			g_Texture[i]->Release();
			g_Texture[i] = NULL;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateUI(void)
{
	int mode = GetMode();
	int result = GetResultTutorial();

	if (mode == MODE_TUTORIAL)
	{
		g_time++;

		// ui��؂�ւ���
		if (g_time == 80 && g_step == 0)
		{
			g_Use = TRUE;
			PlaySound(SOUND_LABEL_SE_system_03);
		}

		if (GetKeyboardTrigger(DIK_W) || GetKeyboardTrigger(DIK_S) || GetKeyboardTrigger(DIK_A) || GetKeyboardTrigger(DIK_D) 
			|| IsButtonPressed(0, BUTTON_LEFT) || IsButtonPressed(0, BUTTON_RIGHT) || IsButtonPressed(0, BUTTON_UP) || IsButtonPressed(0, BUTTON_DOWN))
		{
			if (g_Use == TRUE && g_step == 0)
			{
				g_TexNo++;
				g_step++;
				PlaySound(SOUND_LABEL_SE_system_03);
			}
		}

		if (GetKeyboardTrigger(DIK_Q) || GetKeyboardTrigger(DIK_E) || IsButtonPressed(0, BUTTON_L) || IsButtonPressed(0, BUTTON_R))
		{
			if (g_step == 1)
			{
				g_TexNo++;
				g_step++;
				PlaySound(SOUND_LABEL_SE_system_03);
			}
		}

		if (result == 1)
		{
			if (g_step == 2)
			{
				g_TexNo++;
				g_step++;
				PlaySound(SOUND_LABEL_SE_system_03);
				g_time = 0;
			}
		}

		if (g_TexNo == 4 && g_time == 200)
		{
			SetFade(FADE_OUT, MODE_GAME);
		}
	}


	PLAYER *player = GetPlayer();

	// ���O��HP�ɍ��킹�ĕ`��
	g_offset_player = TEXTURE_WIDTH_PLAYER_2 * ((float)player->hp / (float)player->hp_max);


	ENEMY *enemy = GetEnemy();

	// ���O��HP�ɍ��킹�ĕ`��
	g_offset_enemy = TEXTURE_WIDTH_ENEMY_2 * ((float)enemy->hp / (float)enemy->hp_max);


#ifdef _DEBUG	// �f�o�b�O����\������
	//char *str = GetDebugStr();
	//sprintf(&str[strlen(str)], " PX:%.2f PY:%.2f", g_Pos.x, g_Pos.y);
	PrintDebugProc("time:%d\n", g_time);

#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawUI(void)
{
	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// �}�g���N�X�ݒ�
	SetWorldViewProjection2D();

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// �}�e���A���ݒ�
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);


	int mode = GetMode();

	if (mode == MODE_TUTORIAL)
	{
		if (g_Use == TRUE)
		{	
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColor(g_VertexBuffer, g_Pos.x, g_Pos.y, g_w, g_h, 0.0f, 0.0f, 1.0f, 1.0f,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
	}

	// �v���C���[��HP��`��
	if (g_Use_player == TRUE)
	{	
		for (int i = 0; i < 3; i++)
		{
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo_player + i]);

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColor(g_VertexBuffer, g_Pos_player.x, g_Pos_player.y, g_w_player, g_h_player, 0.0f, 0.0f, 1.0f, 1.0f,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

			if (i == 1)
			{
				SetSpriteLeftTop(g_VertexBuffer, g_Pos_player.x - 55.0f , g_Pos_player.y + 25.0f, g_offset_player, TEXTURE_HEIGHT_PLAYER_2, 0.0f, 0.0f, 1.0f, 1.0f);
			}
			
			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}


		// �G�l�~�[��HP��`��
		if (g_Use_enemy == TRUE && mode == MODE_GAME)
		{
			for (int i = 0; i < 4; i++)
			{
				// �e�N�X�`���ݒ�
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo_enemy + i]);

				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				SetSpriteColor(g_VertexBuffer, g_Pos_enemy.x, g_Pos_enemy.y, g_w_enemy, g_h_enemy, 0.0f, 0.0f, 1.0f, 1.0f,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

				if (i == 2)
				{
					SetSpriteLeftTop(g_VertexBuffer, g_Pos_enemy.x -293.0f, g_Pos_enemy.y + 7.0f, g_offset_enemy, TEXTURE_HEIGHT_ENEMY_2, 0.0f, 0.0f, 1.0f, 1.0f);
				}

				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}
		}
	}
}

