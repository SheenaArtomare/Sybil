//=============================================================================
//
// リザルト画面処理 [result.cpp]
// Author : GP11A132 16 朱暁テイ
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
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// 背景サイズ
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(5)				// テクスチャの数

#define TEXTURE_WIDTH_KEY			(150)			// ケーサイズ
#define TEXTURE_HEIGHT_KEY			(60)			// 

#define TEXTURE_WIDTH_WORD			(960)			// ケーサイズ
#define TEXTURE_HEIGHT_WORD			(1500)			// 

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/resultBG.png",
	"data/TEXTURE/resultWORD.png",
	"data/TEXTURE/window_5.png",
	"data/TEXTURE/gameclear.png",
	"data/TEXTURE/gameover.png",
};


static BOOL						g_Use;						// TRUE:使っている  FALSE:未使用
static float					g_w, g_h;					// 幅と高さ
static XMFLOAT3					g_Pos;						// ポリゴンの座標
static int						g_TexNo;					// テクスチャ番号

float							pos_start, pos_end;			// 文字がスクロールし始めた時の位置, スクロール終わった後の位置
float							word_offset;				//

static int						g_Time;						// タイトルモードへの自動遷移を管理
int								g_wait;						// リザルドモードへの自動遷移を管理

static int						g_Result;					// リザルドをチャック 1:clear 2:over
int								result_texno;				// テクスチャ番号       clear/over
float							result_alpha;				// リザルドロゴの不透明度
int								result_sound;				// リザルドの効果音を鳴らすタイミングを管理

static BOOL						g_Load = FALSE;	

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitResult(void)
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


	// 変数の初期化
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
		// BGM再生
		PlaySound(SOUND_LABEL_BGM_ending);
	}
	
	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
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
// 更新処理
//=============================================================================
void UpdateResult(void)
{
	int mode = GetMode();

	if (mode == MODE_RESULT)
	{	//文字をスクロールさせる処理
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
		{// Enter押したら、ステージを切り替える
			SetFade(FADE_OUT, MODE_TITLE);
		}
		// ゲームパッドで入力処理
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

#ifdef _DEBUG	// デバッグ情報を表示する
	
#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawResult(void)
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

	if (mode == MODE_RESULT)
	{
		// リザルトの背景を描画
		{
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteLeftTop(g_VertexBuffer, 0.0f, 0.0f, g_w, g_h, 0.0f, 0.0f, 1.0f, 1.0f);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}

		// リザルトの文字を描画
		{
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSprite(g_VertexBuffer, g_Pos.x, pos_start, TEXTURE_WIDTH_WORD, TEXTURE_HEIGHT_WORD, 0.0f, 0.0f, 1.0f, 1.0f);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}

		// リザルトのキーを描画
		{
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSprite(g_VertexBuffer, g_Pos.x + 390.0f, g_Pos.y + 230.0f, TEXTURE_WIDTH_KEY, TEXTURE_HEIGHT_KEY, 0.0f, 0.0f, 1.0f, 1.0f);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
	}


	if (mode == MODE_GAME && g_Result > 0)
	{
		if (g_Result == 2)
		{
			result_texno = 4;
		}
	
		// リザルトの背景を描画
		{
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[result_texno]);

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColor(g_VertexBuffer, g_Pos.x, g_Pos.y, g_w, g_h, 0.0f, 0.0f, 1.0f, 1.0f,
				XMFLOAT4(1.0f, 1.0f, 1.0f, result_alpha));

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
	}

}


