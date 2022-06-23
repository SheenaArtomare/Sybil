//=============================================================================
//
// �^�C�g����ʏ��� [title.cpp]
// Author : GP11A132 16 ��Ńe�C
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "fade.h"
#include "sound.h"
#include "sprite.h"
#include "title.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// �w�i�T�C�Y
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(9)				// �e�N�X�`���̐�

#define TEXTURE_WIDTH_LOGO			(480)			// ���S�T�C�Y
#define TEXTURE_HEIGHT_LOGO			(220)			// 

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/title.png",
	"data/TEXTURE/cloud.png",
	"data/TEXTURE/star.png",
	"data/TEXTURE/cpylit.png",
	"data/TEXTURE/title_logo.png",
	"data/TEXTURE/startgame_1.png",
	"data/TEXTURE/startgame_2.png",
	"data/TEXTURE/exitgame_1.png",
	"data/TEXTURE/exitgame_2.png",

};


static BOOL						g_Use;						// TRUE:�g���Ă���  FALSE:���g�p
static float					g_w, g_h;					// ���ƍ���
static XMFLOAT3					g_Pos;						// �|���S���̍��W
static int						g_TexNo;					// �e�N�X�`���ԍ�
static int						g_Time;						// ���ԊǗ��p

float	cloud_offset;
float	star_alpha;
float	logo_alpha;
int		start_texno;
int		exit_texno;

static BOOL						g_Load = FALSE;


//=============================================================================
// ����������
//=============================================================================
HRESULT InitTitle(void)
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
	g_Pos   = XMFLOAT3(g_w/2, g_h/2, 0.0f);
	g_TexNo = 0;
	g_Time = 200;

	cloud_offset = 0.0f;
	star_alpha = 0.0f;
	logo_alpha = 0.0f;
	start_texno = 6;
	exit_texno = 7;

	//alpha = 1.0f;
	//flag_alpha = TRUE;

	// BGM�Đ�
	PlaySound(SOUND_LABEL_BGM_title);

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitTitle(void)
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
void UpdateTitle(void)
{
	if (g_Time < 0)
	{
		if (GetKeyboardTrigger(DIK_S) || GetKeyboardTrigger(DIK_W))
		{// S/W��������A�I����؂�ւ���
			if (start_texno == 6)
			{
				start_texno--;
				exit_texno++;
			}
			else
			{
				start_texno = 6;
				exit_texno = 7;
			}
			PlaySound(SOUND_LABEL_SE_system_01);
		}


		if (GetKeyboardTrigger(DIK_RETURN))
		{// Enter��������A�X�e�[�W��؂�ւ���
			if (start_texno == 6)
			{
				PlaySound(SOUND_LABEL_SE_system_02);
				SetFade(FADE_OUT, MODE_TUTORIAL);
			}
			else
			{
				PlaySound(SOUND_LABEL_SE_system_02);
				Sleep(800);
				exit(-1);
			}
		}


		// �Q�[���p�b�h�œ��͏���
		if (IsButtonTriggered(0, BUTTON_UP) || IsButtonTriggered(0, BUTTON_DOWN))
		{// S/W��������A�I����؂�ւ���
			if (start_texno == 6)
			{
				start_texno--;
				exit_texno++;
			}
			else
			{
				start_texno = 6;
				exit_texno = 7;
			}
			PlaySound(SOUND_LABEL_SE_system_01);
		}


		else if (IsButtonTriggered(0, BUTTON_START))
		{
			if (start_texno == 6)
			{
				PlaySound(SOUND_LABEL_SE_system_02);
				SetFade(FADE_OUT, MODE_TUTORIAL);
			}
			else
			{
				PlaySound(SOUND_LABEL_SE_system_02);
				Sleep(800);
				exit(-1);
			}
		}
		else if (IsButtonTriggered(0, BUTTON_B))
		{
			if (start_texno == 6)
			{
				PlaySound(SOUND_LABEL_SE_system_02);
				SetFade(FADE_OUT, MODE_TUTORIAL);
			}
			else
			{
				PlaySound(SOUND_LABEL_SE_system_02);
				Sleep(800);
				exit(-1);
			}
		}
	}
	
	g_Time--;

	if (g_Time < 150 && logo_alpha < 1.0f)
	{
		logo_alpha += 0.01f;
	}
	
	//if (flag_alpha == TRUE)
	//{
	//	alpha -= 0.02f;
	//	if (alpha <= 0.0f)
	//	{
	//		alpha = 0.0f;
	//		flag_alpha = FALSE;
	//	}
	//}
	//else
	//{
	//	alpha += 0.02f;
	//	if (alpha >= 1.0f)
	//	{
	//		alpha = 1.0f;
	//		flag_alpha = TRUE;
	//	}
	//}






#ifdef _DEBUG	// �f�o�b�O����\������
	//char *str = GetDebugStr();
	//sprintf(&str[strlen(str)], " PX:%.2f PY:%.2f", g_Pos.x, g_Pos.y);
	
#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawTitle(void)
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

	// �^�C�g���̔w�i��`��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSprite(g_VertexBuffer, g_Pos.x, g_Pos.y, g_w, g_h, 0.0f, 0.0f, 1.0f, 1.0f);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	// �^�C�g���̉_��`��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSprite(g_VertexBuffer, g_Pos.x, g_Pos.y, g_w, g_h, cloud_offset, 0.0f, 1.0f, 1.0f);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

		// �_�̃e�N�X�`����U���W��i�X����
		cloud_offset -= 0.0001f;
	}

	// �^�C�g���̐����`��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(g_VertexBuffer, g_Pos.x, g_Pos.y, g_w, g_h, 0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, star_alpha));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

		// �^�C�g�����S��alpha�l��i�X����
		if (star_alpha < 1.0f)
		{
			star_alpha += 0.001f;
		}
	}

	// �^�C�g���̃R�s�[���C�^�[��`��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[3]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSprite(g_VertexBuffer, g_Pos.x, g_Pos.y, g_w, g_h, 0.0f, 0.0f, 1.0f, 1.0f);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	// �^�C�g���̃��S��`��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[4]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	//	SetSprite(g_VertexBuffer, g_Pos.x, g_Pos.y, TEXTURE_WIDTH_LOGO, TEXTURE_HEIGHT_LOGO, 0.0f, 0.0f, 1.0f, 1.0f);
		SetSpriteColor(g_VertexBuffer, g_Pos.x, g_Pos.y, g_w, g_h, 0.0f, 0.0f, 1.0f, 1.0f,
						XMFLOAT4(1.0f, 1.0f, 1.0f, logo_alpha));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	// �^�C�g���̃��j���[��`��
	if (g_Time < 0)
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[start_texno]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(g_VertexBuffer, g_Pos.x, g_Pos.y, g_w, g_h, 0.0f, 0.0f, 1.0f, 1.0f,
						XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[exit_texno]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(g_VertexBuffer, g_Pos.x, g_Pos.y, g_w, g_h, 0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

//	// �����Z�̃e�X�g
//	SetBlendState(BLEND_MODE_ADD);		// ���Z����
////	SetBlendState(BLEND_MODE_SUBTRACT);	// ���Z����
//	for(int i=0; i<30; i++)
//	{
//		// �e�N�X�`���ݒ�
//		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);
//
//		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
//		float dx = 100.0f;
//		float dy = 100.0f;
//		float sx = (float)(rand() % 100);
//		float sy = (float)(rand() % 100);
//
//
//		SetSpriteColor(g_VertexBuffer, dx+sx, dy+sy, 50, 50, 0.0f, 0.0f, 1.0f, 1.0f,
//			XMFLOAT4(0.3f, 0.3f, 1.0f, 0.5f));
//
//		// �|���S���`��
//		GetDeviceContext()->Draw(4, 0);
//	}
//	SetBlendState(BLEND_MODE_ALPHABLEND);	// ���������������ɖ߂�

}





