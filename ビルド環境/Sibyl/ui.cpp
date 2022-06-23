//=============================================================================
//
// UI処理 [ui.cpp]
// Author : GP11A132 16 朱暁テイ
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
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(350)	// キャラサイズ
#define TEXTURE_HEIGHT				(150)	// 
#define TEXTURE_MAX					(12)	// テクスチャの数

#define TEXTURE_WIDTH_PLAYER		(270)	// キャラサイズ
#define TEXTURE_HEIGHT_PLAYER		(136)	// 

#define TEXTURE_WIDTH_PLAYER_2		(170)	// キャラサイズ
#define TEXTURE_HEIGHT_PLAYER_2		(19)	// 

#define TEXTURE_WIDTH_ENEMY			(750)	// キャラサイズ
#define TEXTURE_HEIGHT_ENEMY		(160)	// 

#define TEXTURE_WIDTH_ENEMY_2		(584)	// キャラサイズ
#define TEXTURE_HEIGHT_ENEMY_2		(25)	// 

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

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


static BOOL						g_Use;						// TRUE:使っている  FALSE:未使用
static float					g_w, g_h;					// 幅と高さ
static XMFLOAT3					g_Pos;						// ポリゴンの座標
static int						g_TexNo;					// テクスチャ番号

static int						g_time;						// 時間管理用
static int						g_sound;					// 効果音を鳴らす用
static int						g_step;						// 時間管理用

static BOOL						g_Load = FALSE;

																	// プレイヤーのHP表示
static BOOL						g_Use_player;						// TRUE:使っている  FALSE:未使用
static float					g_w_player, g_h_player;				// 幅と高さmax
static float					g_offset_player;					// 幅と高さ
static XMFLOAT3					g_Pos_player;						// ポリゴンの座標
static int						g_TexNo_player;						// テクスチャ番号

																	// エネミーのHP表示
static BOOL						g_Use_enemy;						// TRUE:使っている  FALSE:未使用
static float					g_w_enemy, g_h_enemy;				// 幅と高さmax
static float					g_offset_enemy;						// 幅と高さ
static XMFLOAT3					g_Pos_enemy;						// ポリゴンの座標
static int						g_TexNo_enemy;						// テクスチャ番号


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitUI(void)
{
	ID3D11Device *pDevice = GetDevice();

	//テクスチャ生成
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


	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);


	// uiの初期化
	g_Use   = FALSE;
	g_w     = TEXTURE_WIDTH;
	g_h     = TEXTURE_HEIGHT;
	g_Pos   = { 480.0f, 90.0f, 0.0f };
	g_TexNo = 1;

	g_time = 0;
	g_step = 0;
	g_sound = 0;
	
	
	// プレイヤーのHP表示初期化
	g_Use_player = TRUE;
	g_w_player = TEXTURE_WIDTH_PLAYER;
	g_h_player = TEXTURE_HEIGHT_PLAYER;
	g_offset_player = 0.0f;
	g_Pos_player = { 150.0f, 460.0f, 0.0f };
	g_TexNo_player = 5;


	// エネミーのHP表示初期化
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
// 終了処理
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
// 更新処理
//=============================================================================
void UpdateUI(void)
{
	int mode = GetMode();
	int result = GetResultTutorial();

	if (mode == MODE_TUTORIAL)
	{
		g_time++;

		// uiを切り替える
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

	// 直前のHPに合わせて描画
	g_offset_player = TEXTURE_WIDTH_PLAYER_2 * ((float)player->hp / (float)player->hp_max);


	ENEMY *enemy = GetEnemy();

	// 直前のHPに合わせて描画
	g_offset_enemy = TEXTURE_WIDTH_ENEMY_2 * ((float)enemy->hp / (float)enemy->hp_max);


#ifdef _DEBUG	// デバッグ情報を表示する
	//char *str = GetDebugStr();
	//sprintf(&str[strlen(str)], " PX:%.2f PY:%.2f", g_Pos.x, g_Pos.y);
	PrintDebugProc("time:%d\n", g_time);

#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawUI(void)
{
	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// マトリクス設定
	SetWorldViewProjection2D();

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);


	int mode = GetMode();

	if (mode == MODE_TUTORIAL)
	{
		if (g_Use == TRUE)
		{	
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColor(g_VertexBuffer, g_Pos.x, g_Pos.y, g_w, g_h, 0.0f, 0.0f, 1.0f, 1.0f,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
	}

	// プレイヤーのHPを描画
	if (g_Use_player == TRUE)
	{	
		for (int i = 0; i < 3; i++)
		{
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo_player + i]);

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColor(g_VertexBuffer, g_Pos_player.x, g_Pos_player.y, g_w_player, g_h_player, 0.0f, 0.0f, 1.0f, 1.0f,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

			if (i == 1)
			{
				SetSpriteLeftTop(g_VertexBuffer, g_Pos_player.x - 55.0f , g_Pos_player.y + 25.0f, g_offset_player, TEXTURE_HEIGHT_PLAYER_2, 0.0f, 0.0f, 1.0f, 1.0f);
			}
			
			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}


		// エネミーのHPを描画
		if (g_Use_enemy == TRUE && mode == MODE_GAME)
		{
			for (int i = 0; i < 4; i++)
			{
				// テクスチャ設定
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo_enemy + i]);

				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteColor(g_VertexBuffer, g_Pos_enemy.x, g_Pos_enemy.y, g_w_enemy, g_h_enemy, 0.0f, 0.0f, 1.0f, 1.0f,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

				if (i == 2)
				{
					SetSpriteLeftTop(g_VertexBuffer, g_Pos_enemy.x -293.0f, g_Pos_enemy.y + 7.0f, g_offset_enemy, TEXTURE_HEIGHT_ENEMY_2, 0.0f, 0.0f, 1.0f, 1.0f);
				}

				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}
		}
	}
}

