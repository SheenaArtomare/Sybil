//=============================================================================
//
// アイテム処理 [item.cpp]
// Author : GP11A132 16 朱暁テイ
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "shadow.h"
#include "model.h"
#include "item.h"
#include "customfunc.h"
#include "debugproc.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_ITEM				"data/MODEL/item.obj"			// 読み込むモデル名
#define	ITEM_ROT_Y				(XM_PI/200)						// 回転
#define	ITEM_OFFSET_Y_RADIAN	(XM_2PI/200)					// 動きを制御用ラジアン
#define	ITEM_OFFSET_Y_RADIUS	(0.1f)							// 動きの幅
#define	ITEM_SHADOW_SIZE		(0.5f)							// 動きの幅

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ITEM				g_Item[MAX_ITEM];						// 背景
static float			g_rot_y;
static float			g_offset_y;
static float			g_radian;
static float			g_radius;

static BOOL				g_Load = FALSE;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitItem(void)
{
	for (int i = 0; i < MAX_ITEM; i++)
	{
		LoadModel(MODEL_ITEM, &g_Item[i].model);
		g_Item[i].load = TRUE;

		g_Item[i].use = TRUE;
		g_Item[i].effect = FALSE;

		g_Item[i].pos = XMFLOAT3((float)(RandomRange(-600,600)), 28.0f, (float)(RandomRange(-600, 600)));
		g_Item[i].rot = XMFLOAT3(0.0f, 0.0f, 1.0f);
		g_Item[i].scl = XMFLOAT3(0.6f, 0.6f, 0.6f);

		g_Item[i].size = ITEM_SIZE;

		// モデルのディフューズを保存しておく。色変え対応の為。
		GetModelDiffuse(&g_Item[0].model, &g_Item[0].diffuse[0]);
	
		XMFLOAT3 pos = g_Item[i].pos;
		pos.y -= 0.1f;
		g_Item[i].shadowIdx = CreateShadow(pos, ITEM_SHADOW_SIZE, ITEM_SHADOW_SIZE);
	}
	
	g_rot_y = ITEM_ROT_Y;
	g_radian = 0.0f;
	g_radius = ITEM_OFFSET_Y_RADIUS;
	g_offset_y = 0.0f;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitItem(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < MAX_ITEM; i++)
	{
		if (g_Item[i].load)
		{
			UnloadModel(&g_Item[i].model);
			g_Item[i].load = FALSE;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateItem(void)
{
	for (int i = 0; i < MAX_ITEM; i++)
	{
		g_Item[i].rot.y += g_rot_y;
		g_Item[i].pos.y += g_offset_y;
	
		// 影もアイテムの位置に合わせる
		XMFLOAT3 pos = g_Item[i].pos;
		pos.y = 0.1f;
		SetPositionShadow(g_Item[i].shadowIdx, pos);
	}
	
	g_radian += ITEM_OFFSET_Y_RADIAN;
	g_offset_y = sinf(g_radian) * g_radius;

	if (g_radian > XM_2PI)
	{
		g_radian = 0.0f;
	}



#ifdef _DEBUG	// デバッグ情報を表示する
#endif



}

//=============================================================================
// 描画処理
//=============================================================================
void DrawItem(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// カリング無効
	SetCullingMode(CULL_MODE_NONE);

	for (int i = 0; i < MAX_ITEM; i++)
	{
		if (g_Item[i].use == FALSE) continue;

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_Item[i].scl.x, g_Item[i].scl.y, g_Item[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_Item[i].rot.x, g_Item[i].rot.y, g_Item[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Item[i].pos.x, g_Item[i].pos.y, g_Item[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Item[i].mtxWorld, mtxWorld);

		// モデル描画
		DrawModel(&g_Item[i].model);
	
	}

	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);
}

//=============================================================================
// エネミーの取得
//=============================================================================
ITEM *GetItem()
{
	return &g_Item[0];
}
