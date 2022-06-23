//=============================================================================
//
// ���C�g���� [light.cpp]
// Author : GP11A132 16 ��Ńe�C
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "particle.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************



//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static LIGHT	g_Light[LIGHT_MAX];

static FOG		g_Fog;

static INTERPOLATION_DATA_LIGHT	*tbl_adr;			// �A�j���f�[�^�̃e�[�u���擪�A�h���X
static int					tbl_size;			// �o�^�����e�[�u���̃��R�[�h����
static float				move_time;			// ���s����

static int					timer;
static bool					thunder;

static INTERPOLATION_DATA_LIGHT move_tbl0[] = {	// �J���[, �t���[��
	{ XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 100.0f },
	{ XMFLOAT4(1.0f, 0.7f, 0.5f, 1.0f), 30.0f },
	{ XMFLOAT4(0.0f, 0.0f, 0.3f, 1.0f), 100.0f },
	{ XMFLOAT4(1.0f, 0.5f, 0.7f, 1.0f), 30.0f },
	{ XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 1.0f },
};


//=============================================================================
// ����������
//=============================================================================
void InitLight(void)
{

	//���C�g������
	for (int i = 0; i < LIGHT_MAX; i++)
	{
		g_Light[i].Position  = XMFLOAT3( 0.0f, 0.0f, 0.0f );
		g_Light[i].Direction = XMFLOAT3( 0.0f, -1.0f, 0.0f );
		g_Light[i].Diffuse   = XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );
		g_Light[i].Ambient   = XMFLOAT4( 0.0f, 0.0f, 0.0f, 0.0f );
		g_Light[i].Attenuation = 100.0f;	// ��������
		g_Light[i].Type = LIGHT_TYPE_NONE;	// ���C�g�̃^�C�v
		g_Light[i].Enable = false;			// ON / OFF
		SetLight(i, &g_Light[i]);
	}

	move_time = 0.0f;			// ���`��ԗp�̃^�C�}�[���N���A
	tbl_adr = move_tbl0;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	tbl_size = sizeof(move_tbl0) / sizeof(INTERPOLATION_DATA_LIGHT);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g


	// ���s�����̐ݒ�i���E���Ƃ炷���j
	g_Light[0].Position = XMFLOAT3(0.0f, 200.0f, 0.0f);			// ���̌���
	g_Light[0].Direction = XMFLOAT3( 0.0f, -1.0f, 0.0f );		// ���̌���
	g_Light[0].Diffuse   = XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );	// ���̐F
	g_Light[0].Type = LIGHT_TYPE_DIRECTIONAL;					// ���s����
	g_Light[0].Enable = true;									// ���̃��C�g��ON
	SetLight(0, &g_Light[0]);									// ����Őݒ肵�Ă���


	// �t�H�O�̏������i���̌��ʁj
	g_Fog.FogStart = 50.0f;										// ���_���炱�̋��������ƃt�H�O��������n�߂�
	g_Fog.FogEnd   = 1000.0f;									// �����܂ŗ����ƃt�H�O�̐F�Ō����Ȃ��Ȃ�
	g_Fog.FogColor = XMFLOAT4( 0.0f, 0.0f, 0.0f, 1.0f );		// �t�H�O�̐F
	SetFog(&g_Fog);
	SetFogEnable(true);		// ���̏ꏊ���`�F�b�N���� shadow



}


//=============================================================================
// �X�V����
//=============================================================================
void UpdateLight(void)
{
	
	if (tbl_adr != NULL)	// ���`��Ԃ����s����H
	{						// ���`��Ԃ̏���
		// �ړ�����
		int		index = (int)move_time;
		float	time = move_time - index;
		int		size = tbl_size;

		float dt = 0.1f / tbl_adr[index].frame;	// 1�t���[���Ői�߂鎞��
		move_time += dt;						// �A�j���[�V�����̍��v���Ԃɑ���

		if (index > (size - 2))	// �S�[�����I�[�o�[���Ă�����A�ŏ��֖߂�
		{
			move_time = 0.0f;
			index = 0;
		}

		// ���W�����߂�	X = StartX + (EndX - StartX) * ���̎���
		XMVECTOR p1 = XMLoadFloat4(&tbl_adr[index + 1].color);	// ���̏ꏊ
		XMVECTOR p0 = XMLoadFloat4(&tbl_adr[index + 0].color);	// ���݂̏ꏊ
		XMVECTOR vec = p1 - p0;
		XMStoreFloat4(&g_Light[0].Diffuse, p0 + vec * time);
	}


	SetLight(0, &g_Light[0]);

	
	


}


//=============================================================================
// ���C�g�̐ݒ�
// Type�ɂ���ăZ�b�g���郁���o�[�ϐ����ς���Ă���
//=============================================================================
void SetLightData(int index, LIGHT *light)
{
	SetLight(index, light);
}


LIGHT *GetLightData(int index)
{
	return(&g_Light[index]);
}


//=============================================================================
// �t�H�O�̐ݒ�
//=============================================================================
void SetFogData(FOG *fog)
{
	SetFog(fog);
}



