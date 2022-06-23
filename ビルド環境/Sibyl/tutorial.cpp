//=============================================================================
//
// チュートリアル画面処理 [tutorial.cpp]
// Author : GP11A132 16 朱暁テイ
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "model.h"
#include "camera.h"
#include "input.h"
#include "sound.h"
#include "fade.h"
#include "Tutorial.h"

#include "player.h"
#include "enemy.h"
#include "meshfield.h"
#include "meshwall.h"
#include "sky.h"
#include "item.h"
#include "shadow.h"
#include "tree.h"
#include "bullet.h"
#include "score.h"
#include "ui.h"
#include "particle.h"
#include "collision.h"
#include "result.h"
#include "debugproc.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************



//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void CheckHitTutorial(void);



//*****************************************************************************
// グローバル変数
//*****************************************************************************
static int	g_ViewPortType_Tutorial = TYPE_FULL_SCREEN;

static BOOL	g_bPause = TRUE;	// ポーズON/OFF
static int	g_Result;			// リザルドをチャック 1:終了

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitTutorial(void)
{
	g_ViewPortType_Tutorial = TYPE_FULL_SCREEN;
	g_Result = 0;

	// フィールドの初期化
	InitMeshField(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), 100, 100, 13.0f, 13.0f);

	// 背景の初期化
	InitSky();

	// ライトを有効化	// 影の初期化処理
	InitShadow();

	// プレイヤーの初期化
	InitPlayer();

	// エネミーの初期化
	InitEnemy();

	// 壁の初期化
	InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_TOP), XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(MAP_LEFT, 0.0f, 0.0f), XMFLOAT3(0.0f, -XM_PI * 0.50f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(MAP_RIGHT, 0.0f, 0.0f), XMFLOAT3(0.0f, XM_PI * 0.50f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_DOWN), XMFLOAT3(0.0f,  XM_PI, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);

	// 壁(裏側用の半透明)
	InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_TOP), XMFLOAT3(0.0f,    XM_PI, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(MAP_LEFT, 0.0f, 0.0f), XMFLOAT3(0.0f,   XM_PI * 0.50f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(MAP_RIGHT, 0.0f, 0.0f), XMFLOAT3(0.0f, -XM_PI * 0.50f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_DOWN), XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);

	// 木を生やす
	InitTree();

	// 弾の初期化
	InitBullet();

	// UIの初期化
	InitUI();

	// リザルドの初期化
	InitResult();

	// BGM再生
	PlaySound(SOUND_LABEL_BGM_tutorial);

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitTutorial(void)
{
	// リザルドの終了処理
	UninitResult();

	// UIの終了処理
	UninitUI();

	// 弾の終了処理
	UninitBullet();

	// 木の終了処理
	UninitTree();

	// 壁の終了処理
	UninitMeshWall();

	// 地面の終了処理
	UninitMeshField();

	// 背景の終了処理
	UninitSky();

	// エネミーの終了処理
	UninitEnemy();

	// プレイヤーの終了処理
	UninitPlayer();

	// 影の終了処理
	UninitShadow();

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateTutorial(void)
{
#ifdef _DEBUG
	if (GetKeyboardTrigger(DIK_V))
	{
		g_ViewPortType_Tutorial = (g_ViewPortType_Tutorial + 1) % TYPE_NONE;
		SetViewPort(g_ViewPortType_Tutorial);
	}

	if (GetKeyboardTrigger(DIK_P))
	{
		g_bPause = g_bPause ? FALSE : TRUE;
	}


#endif

	if(g_bPause == FALSE)
		return;

	// 地面処理の更新
	UpdateMeshField();

	// 背景の更新
	UpdateSky();

	// プレイヤーの更新処理
	UpdatePlayer();

	// エネミーの更新処理
	UpdateEnemy();

	// 壁処理の更新
	UpdateMeshWall();

	// 木の更新処理
	UpdateTree();

	// 弾の更新処理
	UpdateBullet();

	// 影の更新処理
	UpdateShadow();

	// 当たり判定処理
	CheckHitTutorial();

	// MAPとの当たり判定
	CheckField();

	// UIの更新処理
	UpdateUI();

	// リザルドの更新処理
	UpdateResult();

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawTutorial0(void)
{
	// 3Dの物を描画する処理
	// 地面の描画処理
	DrawMeshField();

	// 背景の描画処理
	DrawSky();

	// 影の描画処理
	DrawShadow();

	// エネミーの描画処理
	DrawEnemy();

	// プレイヤーの描画処理
	DrawPlayer();

	// 弾の描画処理
	DrawBullet();

	// 壁の描画処理
	DrawMeshWall();

	// 木の描画処理
	DrawTree();


	// 2Dの物を描画する処理
	// Z比較なし
	SetDepthEnable(FALSE);

	// ライティングを無効
	SetLightEnable(FALSE);

	// UIの描画処理
	DrawUI();

	// リザルドの描画処理
	DrawResult();

	// ライティングを有効に
	SetLightEnable(TRUE);

	// Z比較あり
	SetDepthEnable(TRUE);
}


void DrawTutorial(void)
{
	XMFLOAT3 pos;


#ifdef _DEBUG
	// デバッグ表示
	PrintDebugProc("ViewPortType:%d\n", g_ViewPortType_Tutorial);

#endif

	// プレイヤー視点
	pos = GetPlayer()->pos;
	pos.y = 35.0f;			// カメラ酔いを防ぐためにクリアしている
	SetCameraAT(pos);
	SetCamera();

	switch(g_ViewPortType_Tutorial)
	{
	case TYPE_FULL_SCREEN:
		SetViewPort(TYPE_FULL_SCREEN);
		DrawTutorial0();
		break;

	case TYPE_LEFT_HALF_SCREEN:
	case TYPE_RIGHT_HALF_SCREEN:
		SetViewPort(TYPE_LEFT_HALF_SCREEN);
		DrawTutorial0();

		// エネミー視点
		pos = GetEnemy()->pos;
		pos.y = 0.0f;
		SetCameraAT(pos);
		SetCamera();
		SetViewPort(TYPE_RIGHT_HALF_SCREEN);
		DrawTutorial0();
		break;

	case TYPE_UP_HALF_SCREEN:
	case TYPE_DOWN_HALF_SCREEN:
		SetViewPort(TYPE_UP_HALF_SCREEN);
		DrawTutorial0();

		// エネミー視点
		pos = GetEnemy()->pos;
		pos.y = 0.0f;
		SetCameraAT(pos);
		SetCamera();
		SetViewPort(TYPE_DOWN_HALF_SCREEN);
		DrawTutorial0();
		break;

	}

}


//=============================================================================
// 当たり判定処理
//=============================================================================
void CheckHitTutorial(void)
{
	ENEMY *enemy = GetEnemy();		// エネミーのポインターを初期化
	PLAYER *player = GetPlayer();	// プレイヤーのポインターを初期化
	BULLET *bullet = GetBullet();	// 弾のポインターを初期化


	// プレイヤーの弾と敵
	for (int i = 0; i < MAX_BULLET; i++)
	{
		//弾の有効フラグをチェックする
		if (bullet[i].use == FALSE)
			continue;

		// 敵と当たってるか調べる
		for (int j = 0; j < MAX_ENEMY; j++)
		{
			//敵の有効フラグをチェックする
			if (enemy[j].use == FALSE)
				continue;

			//BCの当たり判定
			if (CollisionBC(bullet[i].pos, enemy[j].pos, bullet[i].fWidth, enemy[j].size))
			{
				// 当たったから未使用に戻す
				bullet[i].use = FALSE;
				ReleaseShadow(bullet[i].shadowIdx);

				enemy[j].use = FALSE;
					ReleaseShadow(enemy[j].shadowIdx);

					PlaySound(SOUND_LABEL_SE_hit);
			}
		}

	}




	// エネミーが全部死亡したら状態遷移
	int enemy_count = 0;
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (enemy[i].use == FALSE) continue;
		enemy_count++;
	}

	// エネミーが０匹？
	if (enemy_count == 0)
	{
		g_Result = 1;
	}

}

//=============================================================================
// リザルドを取得
//=============================================================================
int GetResultTutorial(void)
{
	return g_Result;
}
