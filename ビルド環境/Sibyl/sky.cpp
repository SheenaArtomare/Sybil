//=============================================================================
//
// 背景モデル処理 [Sky.cpp]
// Author : GP11A132 16 朱暁テイ
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "model.h"
#include "sky.h"
#include "debugproc.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_SKY				"data/MODEL/sky.obj"			// 読み込むモデル名
#define	SKY_OFFSET_Y			(XM_PI/18000)							// 回転速度


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static SKY				g_Sky[MAX_SKY];						// 背景
static float			g_rot_y;

static BOOL				g_Load = FALSE;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitSky(void)
{
	for (int i = 0; i < MAX_SKY; i++)
	{
		LoadModel(MODEL_SKY, &g_Sky[i].model);
		g_Sky[i].load = TRUE;

		g_Sky[i].use = TRUE;

		g_Sky[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Sky[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Sky[i].scl = XMFLOAT3(100.0f, 100.0f, 100.0f);

		// モデルのディフューズを保存しておく。色変え対応の為。
		GetModelDiffuse(&g_Sky[0].model, &g_Sky[0].diffuse[0]);
	}
	
	g_rot_y = SKY_OFFSET_Y;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitSky(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < MAX_SKY; i++)
	{
		if (g_Sky[i].load)
		{
			UnloadModel(&g_Sky[i].model);
			g_Sky[i].load = FALSE;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateSky(void)
{
	for (int i = 0; i < MAX_SKY; i++)
	{
		g_Sky[i].rot.y += g_rot_y;

		if (g_Sky[i].rot.y > XM_PI)
		{
			g_Sky[i].rot.y = 0.0f;
		}
	}


#ifdef _DEBUG	// デバッグ情報を表示する
#endif



}

//=============================================================================
// 描画処理
//=============================================================================
void DrawSky(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// カリング無効
	SetCullingMode(CULL_MODE_NONE);

	for (int i = 0; i < MAX_SKY; i++)
	{
		if (g_Sky[i].use == FALSE) continue;

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_Sky[i].scl.x, g_Sky[i].scl.y, g_Sky[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_Sky[i].rot.x, g_Sky[i].rot.y, g_Sky[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Sky[i].pos.x, g_Sky[i].pos.y, g_Sky[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Sky[i].mtxWorld, mtxWorld);

		// モデル描画
		DrawModel(&g_Sky[i].model);
	
	}

	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);
}

//=============================================================================
// エネミーの取得
//=============================================================================
SKY *GetSky()
{
	return &g_Sky[0];
}
