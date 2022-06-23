//=============================================================================
//
// ���U���g��ʏ��� [result.cpp]
// Author : GP11A132 16 ��Ńe�C
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "result.h"
#include "input.h"
#include "fade.h"
#include "sound.h"
#include "sprite.h"
#include "score.h"
#include "game.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// �w�i�T�C�Y
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(5)				// �e�N�X�`���̐�

#define TEXTURE_WIDTH_KEY			(150)			// �P�[�T�C�Y
#define TEXTURE_HEIGHT_KEY			(60)			// 

#define TEXTURE_WIDTH_WORD			(960)			// �P�[�T�C�Y
#define TEXTURE_HEIGHT_WORD			(1500)			// 

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/resultBG.png",
	"data/TEXTURE/resultWORD.png",
	"data/TEXTURE/window_5.png",
	"data/TEXTURE/gameclear.png",
	"data/TEXTURE/gameover.png",
};


static BOOL						g_Use;						// TRUE:�g���Ă���  FALSE:���g�p
static float					g_w, g_h;					// ���ƍ���
static XMFLOAT3					g_Pos;						// �|���S���̍��W
static int						g_TexNo;					// �e�N�X�`���ԍ�

float							pos_start, pos_end;			// �������X�N���[�����n�߂����̈ʒu, �X�N���[���I�������̈ʒu
float							word_offset;				//

static int						g_Time;						// �^�C�g�����[�h�ւ̎����J�ڂ��Ǘ�
int								g_wait;						// ���U���h���[�h�ւ̎����J�ڂ��Ǘ�

static int						g_Result;					// ���U���h���`���b�N 1:clear 2:over
int								result_texno;				// �e�N�X�`���ԍ�       clear/over
float							result_alpha;				// ���U���h���S�̕s�����x
int								result_sound;				// ���U���h�̌��ʉ���炷�^�C�~���O���Ǘ�

static BOOL						g_Load = FALSE;	

//=============================================================================
// ����������
//=============================================================================
HRESULT InitResult(void)
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


	// �ϐ��̏�����
	g_Use   = TRUE;
	g_w     = TEXTURE_WIDTH;
	g_h     = TEXTURE_HEIGHT;
	g_Pos   = { g_w / 2, g_h / 2, 0.0f };
	g_TexNo = 0;

	pos_start = (float)SCREEN_HEIGHT + (TEXTURE_HEIGHT_WORD * 0.5);
	pos_end = (float)((TEXTURE_HEIGHT_WORD * 0.5) - (SCREEN_HEIGHT * 0.5)) * (-1);
	word_offset = 1.5f;

	g_Time = 0;
	g_wait = 0;

	g_Result = 0;
	result_texno = 3;
	result_alpha = 0.0f;
	result_sound = 0;

	int mode = GetMode();

	if (mode == MODE_RESULT)
	{
		// BGM�Đ�
		PlaySound(SOUND_LABEL_BGM_ending);
	}
	
	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitResult(void)
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
void UpdateResult(void)
{
	int mode = GetMode();

	if (mode == MODE_RESULT)
	{	//�������X�N���[�������鏈��
		pos_start -= word_offset;

		if (pos_start < pos_end)
		{
			pos_start = pos_end;
		}

		if (pos_start == pos_end)
		{
			g_Time++;

			if (g_Time > 250)
			{
				SetFade(FADE_OUT, MODE_TITLE);
			}
		}
		
		if (GetKeyboardTrigger(DIK_RETURN))
		{// Enter��������A�X�e�[�W��؂�ւ���
			SetFade(FADE_OUT, MODE_TITLE);
		}
		// �Q�[���p�b�h�œ��͏���
		else if (IsButtonTriggered(0, BUTTON_START))
		{
			SetFade(FADE_OUT, MODE_TITLE);
		}
		else if (IsButtonTriggered(0, BUTTON_B))
		{
			SetFade(FADE_OUT, MODE_TITLE);
		}

	}


	else if (mode == MODE_GAME)
	{
		g_Result = GetResult();
		
		if (g_Result > 0)
		{
			g_wait++;
			result_alpha += 0.008f;
			result_sound++;

			if (result_alpha > 1.0f)
			{
				result_alpha = 1.0f;
			}

			if (result_sound == 10)
			{
				if (g_Result == 1)
				{
					PlaySound(SOUND_LABEL_SE_gameclear);
				}
				else
				{
					PlaySound(SOUND_LABEL_SE_gameover1);
					PlaySound(SOUND_LABEL_SE_gameover2);
				}
			}

			if (g_wait > 300)
			{
				SetFade(FADE_OUT, MODE_RESULT);
			}

		}
	}

#ifdef _DEBUG	// �f�o�b�O����\������
	
#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawResult(void)
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

	if (mode == MODE_RESULT)
	{
		// ���U���g�̔w�i��`��
		{
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteLeftTop(g_VertexBuffer, 0.0f, 0.0f, g_w, g_h, 0.0f, 0.0f, 1.0f, 1.0f);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}

		// ���U���g�̕�����`��
		{
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSprite(g_VertexBuffer, g_Pos.x, pos_start, TEXTURE_WIDTH_WORD, TEXTURE_HEIGHT_WORD, 0.0f, 0.0f, 1.0f, 1.0f);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}

		// ���U���g�̃L�[��`��
		{
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSprite(g_VertexBuffer, g_Pos.x + 390.0f, g_Pos.y + 230.0f, TEXTURE_WIDTH_KEY, TEXTURE_HEIGHT_KEY, 0.0f, 0.0f, 1.0f, 1.0f);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
	}


	if (mode == MODE_GAME && g_Result > 0)
	{
		if (g_Result == 2)
		{
			result_texno = 4;
		}
	
		// ���U���g�̔w�i��`��
		{
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[result_texno]);

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColor(g_VertexBuffer, g_Pos.x, g_Pos.y, g_w, g_h, 0.0f, 0.0f, 1.0f, 1.0f,
				XMFLOAT4(1.0f, 1.0f, 1.0f, result_alpha));

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
	}

}


