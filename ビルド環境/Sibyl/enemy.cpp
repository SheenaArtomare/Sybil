//=============================================================================
//
// エネミーモデル処理 [enemy.cpp]
// Author : GP11A132 16 朱暁テイ
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "model.h"
#include "enemy.h"
#include "shadow.h"
#include "light.h"
#include "player.h"
#include "debugproc.h"
#include "sound.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_ENEMY				"data/MODEL/E_body.obj"			// 読み込むモデル名
#define	MODEL_ENEMY_HEAD		"data/MODEL/E_head.obj"			// 読み込むモデル名
#define	MODEL_ENEMY_ARM_LEFT	"data/MODEL/E_arm_left.obj"		// 読み込むモデル名
#define	MODEL_ENEMY_ARM_RIGHT	"data/MODEL/E_arm_right.obj"	// 読み込むモデル名
#define	MODEL_ENEMY_LEG_LEFT	"data/MODEL/E_leg_left.obj"		// 読み込むモデル名
#define	MODEL_ENEMY_LEG_RIGHT	"data/MODEL/E_leg_right.obj"	// 読み込むモデル名
#define	MODEL_ENEMY_TAIL		"data/MODEL/E_tail.obj"			// 読み込むモデル名
#define	MODEL_ENEMY_EAR_LEFT	"data/MODEL/E_ear_left.obj"		// 読み込むモデル名
#define	MODEL_ENEMY_EAR_RIGHT	"data/MODEL/E_ear_right.obj"	// 読み込むモデル名

#define	VALUE_MOVE			(1.7f)						// 移動量
#define	VALUE_ROTATE		(XM_PI * 0.02f)				// 回転量

#define ENEMY_SHADOW_SIZE	(0.7f)						// 影の大きさ
#define ENEMY_OFFSET_Y		(19.0f)						// エネミーの足元をあわせる

#define ENEMY_PARTS_MAX		(8)							// エネミーのパーツの数


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ENEMY			g_Enemy[MAX_ENEMY];				// エネミー

static ENEMY			g_Parts[ENEMY_PARTS_MAX];		// エネミーのパーツ用

static BOOL				g_Load = FALSE;

static int				g_cd;							// 攻撃のクールダウン
static int				g_sound;						// SEを鳴らす
static int				g_fuchi;						// 縁取りフラグ
static int				fuchi_keep;						// 縁取りをしばらく維持


static INTERPOLATION_DATA move_tbl[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(   0.0f, ENEMY_OFFSET_Y,  20.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(ENEMY_SHADOW_SIZE, ENEMY_SHADOW_SIZE, ENEMY_SHADOW_SIZE), 60*2 },
	{ XMFLOAT3(-200.0f, ENEMY_OFFSET_Y,  20.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(ENEMY_SHADOW_SIZE, ENEMY_SHADOW_SIZE, ENEMY_SHADOW_SIZE), 60*1 },
	{ XMFLOAT3(-200.0f, ENEMY_OFFSET_Y, 200.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(ENEMY_SHADOW_SIZE, ENEMY_SHADOW_SIZE, ENEMY_SHADOW_SIZE), 60*0.5f },
	{ XMFLOAT3(   0.0f, ENEMY_OFFSET_Y,  20.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(ENEMY_SHADOW_SIZE, ENEMY_SHADOW_SIZE, ENEMY_SHADOW_SIZE), 60*2 },

};

static INTERPOLATION_DATA move_tbl_none[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),      XMFLOAT3(1.0f, 1.0f, 1.0f), 0 },

};

static INTERPOLATION_DATA move_tbl_arm[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),             XMFLOAT3(1.0f, 1.0f, 1.0f), 10 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, XM_PI / 4, 0.0f),        XMFLOAT3(1.0f, 1.0f, 1.0f), 10 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),             XMFLOAT3(1.0f, 1.0f, 1.0f), 10 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, (-1)*(XM_PI / 4), 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 10 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),             XMFLOAT3(1.0f, 1.0f, 1.0f), 10 },

};

static INTERPOLATION_DATA move_tbl_leg_left[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),             XMFLOAT3(1.0f, 1.0f, 1.0f), 10 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(XM_PI / 4, 0.0f, 0.0f),        XMFLOAT3(1.0f, 1.0f, 1.0f), 10 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),             XMFLOAT3(1.0f, 1.0f, 1.0f), 10 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3((-1)*(XM_PI / 4), 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 10 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),             XMFLOAT3(1.0f, 1.0f, 1.0f), 10 },

};

static INTERPOLATION_DATA move_tbl_leg_right[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),             XMFLOAT3(1.0f, 1.0f, 1.0f), 10 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3((-1)*(XM_PI / 4), 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 10 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),             XMFLOAT3(1.0f, 1.0f, 1.0f), 10 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(XM_PI / 4, 0.0f, 0.0f),        XMFLOAT3(1.0f, 1.0f, 1.0f), 10 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),             XMFLOAT3(1.0f, 1.0f, 1.0f), 10 },

};

static INTERPOLATION_DATA move_tbl_tail[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),             XMFLOAT3(1.0f, 1.0f, 1.0f), 10 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, XM_PI / 8, XM_PI / 8),        XMFLOAT3(1.0f, 1.0f, 1.0f), 10 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),             XMFLOAT3(1.0f, 1.0f, 1.0f), 10 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, (-1)*(XM_PI / 8), (-1)*(XM_PI / 8)), XMFLOAT3(1.0f, 1.0f, 1.0f), 10 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),             XMFLOAT3(1.0f, 1.0f, 1.0f), 10 },

};

static INTERPOLATION_DATA move_tbl_ear_left[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),             XMFLOAT3(1.0f, 1.0f, 1.0f), 10 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3((-1)*(XM_PI / 32),0.0f, XM_PI / 32), XMFLOAT3(1.0f, 1.0f, 1.0f), 10 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),             XMFLOAT3(1.0f, 1.0f, 1.0f), 10 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f,(-1)*(XM_PI / 32)),        XMFLOAT3(1.0f, 1.0f, 1.0f), 10 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),             XMFLOAT3(1.0f, 1.0f, 1.0f), 10 },

};

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitEnemy(void)
{
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		LoadModel(MODEL_ENEMY, &g_Enemy[i].model);
		g_Enemy[i].load = TRUE;

		g_Enemy[i].pos = XMFLOAT3(120, ENEMY_OFFSET_Y, -500.0f);
		g_Enemy[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Enemy[i].scl = XMFLOAT3(0.7f, 0.7f, 0.7f);

		g_Enemy[i].spd = 0.0f;			// 移動スピードクリア
		g_Enemy[i].size = ENEMY_SIZE;	// 当たり判定の大きさ

		// モデルのディフューズを保存しておく。色変え対応の為。
		GetModelDiffuse(&g_Enemy[0].model, &g_Enemy[0].diffuse[0]);

		XMFLOAT3 pos = g_Enemy[i].pos;
		pos.y -= (ENEMY_OFFSET_Y - 0.1f);
		g_Enemy[i].shadowIdx = CreateShadow(pos, ENEMY_SHADOW_SIZE, ENEMY_SHADOW_SIZE);

		g_Enemy[i].move_time = 0.0f;	// 線形補間用のタイマーをクリア
		g_Enemy[i].tbl_adr = NULL;		// 再生するアニメデータの先頭アドレスをセット
		g_Enemy[i].tbl_size = 0;		// 再生するアニメデータのレコード数をセット

		g_Enemy[i].use = TRUE;			// TRUE:生きてる
		g_Enemy[i].hit = FALSE;
		g_Enemy[i].hp = g_Enemy[i].hp_max = 100;
		g_Enemy[i].atk = 2;
		g_Enemy[i].skill = 1;

		// 階層アニメーション用の初期化処理
		g_Enemy[i].parent = NULL;			// 本体（親）なのでNULLを入れる
	}


	// 0番だけ線形補間で動かしてみる
	g_Enemy[0].move_time = 0.0f;		// 線形補間用のタイマーをクリア
	g_Enemy[0].tbl_adr = move_tbl;		// 再生するアニメデータの先頭アドレスをセット
	g_Enemy[0].tbl_size = sizeof(move_tbl) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット



	// パーツの初期化
	for (int i = 0; i < ENEMY_PARTS_MAX; i++)
	{
		g_Parts[i].use = FALSE;

		// 位置・回転・スケールの初期設定
		g_Parts[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Parts[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Parts[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

		// 親子関係
		g_Parts[i].parent = &g_Enemy[0];		// ← ここに親のアドレスを入れる
		//	g_Parts[腕].parent= &g_Enemy[0];		// 腕だったら親は本体（プレイヤー）
		//	g_Parts[手].parent= &g_Paerts[腕];	// 指が腕の子供だった場合の例

		// 階層アニメーション用のメンバー変数の初期化
		g_Parts[i].tbl_adr = NULL;		// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i].move_time = 0.0f;	// 実行時間をクリア
		g_Parts[i].tbl_size = 0;		// 再生するアニメデータのレコード数をセット

		// パーツの読み込みはまだしていない
		g_Parts[i].load = 0;
	}

	g_Parts[0].use = TRUE;
	g_Parts[0].parent = &g_Enemy[0];		// 親をセット
	g_Parts[0].load = 1;
	LoadModel(MODEL_ENEMY_HEAD, &g_Parts[0].model);

	g_Parts[1].use = TRUE;
	g_Parts[1].parent = &g_Enemy[0];		// 親をセット
	g_Parts[1].load = 1;
	LoadModel(MODEL_ENEMY_ARM_LEFT, &g_Parts[1].model);

	g_Parts[2].use = TRUE;
	g_Parts[2].parent = &g_Enemy[0];		// 親をセット
	g_Parts[2].load = 1;
	LoadModel(MODEL_ENEMY_ARM_RIGHT, &g_Parts[2].model);

	g_Parts[3].use = TRUE;
	g_Parts[3].parent = &g_Enemy[0];		// 親をセット
	g_Parts[3].load = 1;
	LoadModel(MODEL_ENEMY_LEG_LEFT, &g_Parts[3].model);

	g_Parts[4].use = TRUE;
	g_Parts[4].parent = &g_Enemy[0];		// 親をセット
	g_Parts[4].load = 1;
	LoadModel(MODEL_ENEMY_LEG_RIGHT, &g_Parts[4].model);

	g_Parts[5].use = TRUE;
	g_Parts[5].parent = &g_Enemy[0];		// 親をセット
	g_Parts[5].load = 1;
	LoadModel(MODEL_ENEMY_TAIL, &g_Parts[5].model);

	g_Parts[6].use = TRUE;
	g_Parts[6].parent = &g_Enemy[0];		// 親をセット
	g_Parts[6].load = 1;
	LoadModel(MODEL_ENEMY_EAR_LEFT, &g_Parts[6].model);

	g_Parts[7].use = TRUE;
	g_Parts[7].parent = &g_Enemy[0];		// 親をセット
	g_Parts[7].load = 1;
	LoadModel(MODEL_ENEMY_EAR_RIGHT, &g_Parts[7].model);

	g_cd = 0;
	g_sound = 1;
	g_fuchi = 0;
	fuchi_keep = 0;
	
	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitEnemy(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].load)
		{
			UnloadModel(&g_Enemy[i].model);
			g_Enemy[i].load = FALSE;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateEnemy(void)
{
	//return;

	int mode = GetMode();

	if (mode == MODE_TUTORIAL)
	{
		g_Enemy[0].pos = XMFLOAT3(-150, ENEMY_OFFSET_Y, 10);
	
		// 影もエネミーの位置に合わせる
		XMFLOAT3 pos = g_Enemy[0].pos;
		pos.y -= (ENEMY_OFFSET_Y - 0.1f);
		SetPositionShadow(g_Enemy[0].shadowIdx, pos);
	}

	if (mode == MODE_GAME)
	{
		// エネミーを動かく場合は、影も合わせて動かす事を忘れないようにね！
		for (int i = 0; i < MAX_ENEMY; i++)
		{
			PLAYER *player = GetPlayer();		// プレイヤーの情報を取得
			g_Enemy[i].spd = 0.0f;

			if (g_Enemy[i].use == TRUE && player[0].use == TRUE)			// このエネミーが使われている？
			{									// Yes

				// ホーミング処理
				XMFLOAT3 pos_homing = player->pos;
				XMFLOAT3 temp = XMFLOAT3(g_Enemy[i].pos.x - pos_homing.x, 0.0f, g_Enemy[i].pos.z - pos_homing.z);
				float lenSq = (temp.x * temp.x) + (temp.z * temp.z);

				if (g_Enemy[i].skill > 0 && g_cd == 0)
				{
					g_Enemy[i].spd = VALUE_MOVE;

					pos_homing.x -= g_Enemy[i].pos.x;
					pos_homing.z -= g_Enemy[i].pos.z;

					float angle = atan2f(pos_homing.z, pos_homing.x) + XM_PI;		// その差分を使って角度を求めている
					g_Enemy[i].rot.y = g_Enemy[i].dir - angle + XM_PI / 2;

					// 入力のあった方向へプレイヤーを向かせて移動させる
					g_Enemy[i].pos.x -= cosf(angle) * g_Enemy[i].spd;
					g_Enemy[i].pos.z -= sinf(angle) * g_Enemy[i].spd;

					//XMVECTOR pos1 = XMLoadFloat3(&player->pos);
					//XMVECTOR pos2 = XMLoadFloat3(&g_Enemy[i].pos);

					//pos2 += (pos1 - pos2) * 0.01f;

					//XMStoreFloat3(&g_Enemy[i].pos, pos2);

				}

				g_Enemy[i].skill++;
				g_sound++;

				// プレイヤーに近付いたら攻撃させる
				if (lenSq <= 30000.0f && g_Enemy[i].use == TRUE && g_cd == 0)
				{
					g_Enemy[i].skill = 0;
					g_sound = 0;
					g_cd++;

					if (g_sound == 0)
					{
						PlaySound(SOUND_LABEL_SE_enemyskill_b);
					}
				}

				if (g_cd > 0)
				{
					g_cd++;

					if (g_cd > 100)
					{
						g_cd = 0;
					}
				}


				//if (g_Enemy[i].tbl_adr != NULL)	// 線形補間を実行する？
				//{								// 線形補間の処理
				//	// 移動処理
				//	int		index = (int)g_Enemy[i].move_time;
				//	float	time = g_Enemy[i].move_time - index;
				//	int		size = g_Enemy[i].tbl_size;

				//	float dt = 1.0f / g_Enemy[i].tbl_adr[index].frame;	// 1フレームで進める時間
				//	g_Enemy[i].move_time += dt;							// アニメーションの合計時間に足す

				//	if (index > (size - 2))	// ゴールをオーバーしていたら、最初へ戻す
				//	{
				//		g_Enemy[i].move_time = 0.0f;
				//		index = 0;
				//	}

				//	// 座標を求める	X = StartX + (EndX - StartX) * 今の時間
				//	XMVECTOR p1 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 1].pos);	// 次の場所
				//	XMVECTOR p0 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 0].pos);	// 現在の場所
				//	XMVECTOR vec = p1 - p0;
				//	XMStoreFloat3(&g_Enemy[i].pos, p0 + vec * time);

				//	// 回転を求める	R = StartX + (EndX - StartX) * 今の時間
				//	XMVECTOR r1 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 1].rot);	// 次の角度
				//	XMVECTOR r0 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 0].rot);	// 現在の角度
				//	XMVECTOR rot = r1 - r0;
				//	XMStoreFloat3(&g_Enemy[i].rot, r0 + rot * time);

				//	// scaleを求める S = StartX + (EndX - StartX) * 今の時間
				//	XMVECTOR s1 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 1].scl);	// 次のScale
				//	XMVECTOR s0 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 0].scl);	// 現在のScale
				//	XMVECTOR scl = s1 - s0;
				//	XMStoreFloat3(&g_Enemy[i].scl, s0 + scl * time);

				//}

				// 影もエネミーの位置に合わせる
				XMFLOAT3 pos = g_Enemy[i].pos;
				pos.y -= (ENEMY_OFFSET_Y - 0.1f);
				SetPositionShadow(g_Enemy[i].shadowIdx, pos);


				// 階層アニメーション
				for (int j = 0; j < ENEMY_PARTS_MAX; j++)
				{
					g_Parts[j].tbl_adr = move_tbl_none;	// 再生するアニメデータの先頭アドレスをセット
					g_Parts[j].tbl_size = sizeof(move_tbl_none) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット


					if (g_Enemy[i].spd > 0.0f)
					{
						g_Parts[1].tbl_adr = move_tbl_arm;	// 再生するアニメデータの先頭アドレスをセット
						g_Parts[1].tbl_size = sizeof(move_tbl_arm) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット

						g_Parts[2].tbl_adr = move_tbl_arm;	// 再生するアニメデータの先頭アドレスをセット
						g_Parts[2].tbl_size = sizeof(move_tbl_arm) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット

						g_Parts[3].tbl_adr = move_tbl_leg_left;	// 再生するアニメデータの先頭アドレスをセット
						g_Parts[3].tbl_size = sizeof(move_tbl_leg_left) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット

						g_Parts[4].tbl_adr = move_tbl_leg_right;	// 再生するアニメデータの先頭アドレスをセット
						g_Parts[4].tbl_size = sizeof(move_tbl_leg_right) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット

						g_Parts[5].tbl_adr = move_tbl_tail;	// 再生するアニメデータの先頭アドレスをセット
						g_Parts[5].tbl_size = sizeof(move_tbl_tail) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット

						g_Parts[6].tbl_adr = move_tbl_ear_left;	// 再生するアニメデータの先頭アドレスをセット
						g_Parts[6].tbl_size = sizeof(move_tbl_ear_left) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット

						g_Parts[7].tbl_adr = move_tbl_ear_left;	// 再生するアニメデータの先頭アドレスをセット
						g_Parts[7].tbl_size = sizeof(move_tbl_ear_left) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット

					}

					// 使われているなら処理する
					if ((g_Parts[j].use == TRUE) && (g_Parts[j].tbl_adr != NULL))
					{
						// 移動処理
						int		index = (int)g_Parts[j].move_time;
						float	time = g_Parts[j].move_time - index;
						int		size = g_Parts[j].tbl_size;

						float dt = 1.0f / g_Parts[j].tbl_adr[index].frame;	// 1フレームで進める時間
						g_Parts[j].move_time += dt;					// アニメーションの合計時間に足す

						if (index > (size - 2))	// ゴールをオーバーしていたら、最初へ戻す
						{
							g_Parts[j].move_time = 0.0f;
							index = 0;
						}

						// 座標を求める	X = StartX + (EndX - StartX) * 今の時間
						XMVECTOR p1 = XMLoadFloat3(&g_Parts[j].tbl_adr[index + 1].pos);	// 次の場所
						XMVECTOR p0 = XMLoadFloat3(&g_Parts[j].tbl_adr[index + 0].pos);	// 現在の場所
						XMVECTOR vec = p1 - p0;
						XMStoreFloat3(&g_Parts[j].pos, p0 + vec * time);

						// 回転を求める	R = StartX + (EndX - StartX) * 今の時間
						XMVECTOR r1 = XMLoadFloat3(&g_Parts[j].tbl_adr[index + 1].rot);	// 次の角度
						XMVECTOR r0 = XMLoadFloat3(&g_Parts[j].tbl_adr[index + 0].rot);	// 現在の角度
						XMVECTOR rot = r1 - r0;
						XMStoreFloat3(&g_Parts[j].rot, r0 + rot * time);

						// scaleを求める S = StartX + (EndX - StartX) * 今の時間
						XMVECTOR s1 = XMLoadFloat3(&g_Parts[j].tbl_adr[index + 1].scl);	// 次のScale
						XMVECTOR s0 = XMLoadFloat3(&g_Parts[j].tbl_adr[index + 0].scl);	// 現在のScale
						XMVECTOR scl = s1 - s0;
						XMStoreFloat3(&g_Parts[j].scl, s0 + scl * time);

					}
				}
			}

			// 縁取りを制御
			if (g_Enemy[i].hit == TRUE)
			{
				g_fuchi = 1;
				fuchi_keep++;

				if (fuchi_keep == 20)
				{
					g_Enemy[i].hit = FALSE;
					fuchi_keep = 0;
					g_fuchi = 0;
				}
			}


#ifdef _DEBUG	// デバッグ情報を表示する
			PrintDebugProc("enmey hp:%d\n", g_Enemy[i].hp);
#endif



		}
	}

	
	// ポイントライトのテスト
	{
		LIGHT *light = GetLightData(2);
		XMFLOAT3 pos = g_Enemy[0].pos;
		pos.y += 20.0f;

		light->Position = pos;
		light->Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		light->Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		light->Type = LIGHT_TYPE_POINT;
		light->Enable = TRUE;
		SetLightData(2, light);
	}


}

//=============================================================================
// 描画処理
//=============================================================================
void DrawEnemy(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// カリング無効
	SetCullingMode(CULL_MODE_NONE);

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].use == FALSE) continue;

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_Enemy[i].scl.x, g_Enemy[i].scl.y, g_Enemy[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_Enemy[i].rot.x, g_Enemy[i].rot.y + XM_PI, g_Enemy[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Enemy[i].pos.x, g_Enemy[i].pos.y, g_Enemy[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Enemy[i].mtxWorld, mtxWorld);

		//縁取りの設定
		SetFuchi(g_fuchi);

		// モデル描画
		DrawModel(&g_Enemy[i].model);
	


		// パーツの階層アニメーション
		for (int j = 0; j < ENEMY_PARTS_MAX; j++)
		{
			// ワールドマトリックスの初期化
			mtxWorld = XMMatrixIdentity();

			// スケールを反映
			mtxScl = XMMatrixScaling(g_Parts[j].scl.x, g_Parts[j].scl.y, g_Parts[j].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// 回転を反映
			mtxRot = XMMatrixRotationRollPitchYaw(g_Parts[j].rot.x, g_Parts[j].rot.y, g_Parts[j].rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			// 移動を反映
			mtxTranslate = XMMatrixTranslation(g_Parts[j].pos.x, g_Parts[j].pos.y, g_Parts[j].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			if (g_Parts[j].parent != NULL)	// 子供だったら親と結合する
			{
				mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(&g_Parts[j].parent->mtxWorld));
				// ↑
				// g_Enemy.mtxWorldを指している
			}

			XMStoreFloat4x4(&g_Parts[j].mtxWorld, mtxWorld);

			// 使われているなら処理する。ここまで処理している理由は他のパーツがこのパーツを参照している可能性があるから。
			if (g_Parts[j].use == FALSE) continue;

			// ワールドマトリックスの設定
			SetWorldMatrix(&mtxWorld);


			// モデル描画
			DrawModel(&g_Parts[j].model);

		}
	}

	//縁取りの設定
	SetFuchi(0);

	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);
}

//=============================================================================
// エネミーの取得
//=============================================================================
ENEMY *GetEnemy()
{
	return &g_Enemy[0];
}
