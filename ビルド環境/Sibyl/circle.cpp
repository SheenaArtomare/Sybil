//=============================================================================
//
// 魔法陣処理 [circle.cpp]
// Author : GP11A132 16 朱暁テイ
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
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX			(1)				// テクスチャの数

#define	CIRCLE_WIDTH		(50.0f)			// 頂点サイズ
#define	CIRCLE_HEIGHT		(50.0f)			// 頂点サイズ
#define	CIRCLE_SHADOW_SIZE	(1.2f)			// シャドウのサイズ
#define	CIRCLE_MOVE_SPEED	(2.0f)			// 移動量


//*****************************************************************************
// 構造体定義
//*****************************************************************************


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT MakeVertexCircle(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer					*g_VertexBuffer = NULL;	// 頂点バッファ
static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static CIRCLE						g_Circle[MAX_CIRCLE];	// 魔法陣ワーク
static int							g_TexNo;				// テクスチャ番号

static char *g_TextureName[TEXTURE_MAX] =
{
	"data/TEXTURE/circle.png",
};

static BOOL							g_Load = FALSE;


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitCircle(void)
{
	MakeVertexCircle();

	// テクスチャ生成
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

	// 魔法陣ワークの初期化
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
// 終了処理
//=============================================================================
void UninitCircle(void)
{
	if (g_Load == FALSE) return;

	for (int nCntTex = 0; nCntTex < TEXTURE_MAX; nCntTex++)
	{
		if (g_Texture[nCntTex] != NULL)
		{// テクスチャの解放
			g_Texture[nCntTex]->Release();
			g_Texture[nCntTex] = NULL;
		}
	}

	if (g_VertexBuffer != NULL)
	{// 頂点バッファの解放
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateCircle(void)
{
	for (int i = 0; i < MAX_CIRCLE; i++)
	{
		// プレイヤーの情報を取得
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
				// 魔法陣の移動処理
				rot = dir + cam->rot.y;

				g_Circle[i].pos.x -= sinf(rot) * g_Circle[i].spd;
				g_Circle[i].pos.z -= cosf(rot) * g_Circle[i].spd;
			}
		}

		if (GetKeyboardRepeat(DIK_5))
		{
			g_Circle[i].use = TRUE;
			player->isATK = TRUE;

			// 魔法陣を動かす
			if (GetKeyboardPress(DIK_A))
			{	// 左へ移動
				g_Circle[i].spd = CIRCLE_MOVE_SPEED;
				dir = XM_PI / 2;
			}
			if (GetKeyboardPress(DIK_D))
			{	// 右へ移動
				g_Circle[i].spd = CIRCLE_MOVE_SPEED;
				dir = -XM_PI / 2;
			}
			if (GetKeyboardPress(DIK_W))
			{	// 上へ移動
				g_Circle[i].spd = CIRCLE_MOVE_SPEED;
				dir = XM_PI;
			}
			if (GetKeyboardPress(DIK_S))
			{	// 下へ移動
				g_Circle[i].spd = CIRCLE_MOVE_SPEED;
				dir = 0.0f;
			}


			// Key入力があったら移動処理する
			if (g_Circle[i].spd > 0.0f)
			{
				g_Circle[i].rot.y = dir + cam->rot.y;

				// 入力のあった方向へプレイヤーを向かせて移動させる
				g_Circle[i].pos.x -= sinf(g_Circle[i].rot.y) * g_Circle[i].spd;
				g_Circle[i].pos.z -= cosf(g_Circle[i].rot.y) * g_Circle[i].spd;
			}

			g_Circle[i].spd *= 0.5f;




			// 影を作る
			//XMFLOAT3 pos = g_Circle[i].pos;
			//pos.y = 0.1f;
			//g_Circle[i].shadowIdx = CreateShadow(pos, CIRCLE_SHADOW_SIZE, CIRCLE_SHADOW_SIZE);

			
			// 影も魔法陣の位置に合わせる
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
// 描画処理
//=============================================================================
void DrawCircle(void)
{
	// αテストを有効に
	SetAlphaTestEnable(TRUE);

	// ライティングを無効
	SetLightEnable(FALSE);

	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	for (int i = 0; i < MAX_CIRCLE; i++)
	{
		if (g_Circle[i].use)
		{
			// ワールドマトリックスの初期化
			mtxWorld = XMMatrixIdentity();

			// スケールを反映
			mtxScl = XMMatrixScaling(g_Circle[i].scl.x, g_Circle[i].scl.y, g_Circle[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// 回転を反映
			mtxRot = XMMatrixRotationRollPitchYaw(g_Circle[i].rot.x, g_Circle[i].rot.y + XM_PI, g_Circle[i].rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			// 移動を反映
			mtxTranslate = XMMatrixTranslation(g_Circle[i].pos.x, g_Circle[i].pos.y, g_Circle[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ワールドマトリックスの設定
			SetWorldMatrix(&mtxWorld);

			XMStoreFloat4x4(&g_Circle[i].mtxWorld, mtxWorld);


			// マテリアル設定
			SetMaterial(g_Circle[i].material);

			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

			// ポリゴンの描画
			GetDeviceContext()->Draw(4, 0);
		}
	}

	// αテストを無効に
	SetAlphaTestEnable(FALSE);

	// ライティングを有効に
	SetLightEnable(TRUE);

}

//=============================================================================
// 頂点情報の作成
//=============================================================================
HRESULT MakeVertexCircle(void)
{
	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	// 頂点バッファに値をセットする
	D3D11_MAPPED_SUBRESOURCE msr;
	GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

	float fWidth = CIRCLE_WIDTH;
	float fHeight = CIRCLE_HEIGHT;

	// 頂点座標の設定
	vertex[0].Position = { -fWidth / 2.0f, 0.0f,  fHeight / 2.0f };
	vertex[1].Position = {  fWidth / 2.0f, 0.0f,  fHeight / 2.0f };
	vertex[2].Position = { -fWidth / 2.0f, 0.0f, -fHeight / 2.0f };
	vertex[3].Position = {  fWidth / 2.0f, 0.0f, -fHeight / 2.0f };

	// 拡散光の設定
	vertex[0].Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	vertex[1].Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	vertex[2].Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	vertex[3].Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };

	// テクスチャ座標の設定
	vertex[0].TexCoord = { 0.0f, 0.0f };
	vertex[1].TexCoord = { 1.0f, 0.0f };
	vertex[2].TexCoord = { 0.0f, 1.0f };
	vertex[3].TexCoord = { 1.0f, 1.0f };

	GetDeviceContext()->Unmap(g_VertexBuffer, 0);

	return S_OK;
}



//=============================================================================
// 弾の取得
//=============================================================================
CIRCLE *GetCircle(void)
{
	return &(g_Circle[0]);
}

