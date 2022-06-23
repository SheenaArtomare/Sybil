//=============================================================================
//
// モデル処理 [player.cpp]
// Author : GP11A132 16 朱暁テイ
//
//=============================================================================
#include "main.h"
#include "input.h"
#include "camera.h"
#include "debugproc.h"
#include "model.h"
#include "player.h"
#include "shadow.h"
#include "light.h"
#include "bullet.h"
#include "meshfield.h"
#include "sound.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_PLAYER			"data/MODEL/P_body.obj"			// 読み込むモデル名
#define	MODEL_PLAYER_HEAD		"data/MODEL/P_head.obj"			// 読み込むモデル名
#define	MODEL_PLAYER_ARM_LEFT	"data/MODEL/P_arm_left.obj"		// 読み込むモデル名
#define	MODEL_PLAYER_ARM_RIGHT	"data/MODEL/P_arm_right.obj"		// 読み込むモデル名
#define	MODEL_PLAYER_LEG_LEFT	"data/MODEL/P_leg_left.obj"		// 読み込むモデル名
#define	MODEL_PLAYER_LEG_RIGHT	"data/MODEL/P_leg_right.obj"		// 読み込むモデル名


#define	VALUE_MOVE			(2.0f)							// 移動量
#define	VALUE_ROTATE		(XM_PI * 0.02f)					// 回転量

#define PLAYER_SHADOW_SIZE	(0.5f)							// 影の大きさ
#define PLAYER_OFFSET_Y		(14.0f)							// プレイヤーの足元をあわせる

#define PLAYER_PARTS_MAX	(5)								// プレイヤーのパーツの数


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static PLAYER		g_Player;						// プレイヤー

static PLAYER		g_Parts[PLAYER_PARTS_MAX];		// プレイヤーのパーツ用

static BOOL			g_Load = FALSE;

static int			g_fuchi;						// 縁取りフラグ
static int			fuchi_keep;						// 縁取りをしばらく維持

// プレイヤーの階層アニメーションデータ

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


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitPlayer(void)
{
	LoadModel(MODEL_PLAYER, &g_Player.model);
	g_Player.load = TRUE;

	g_Player.pos = { 0.0f, PLAYER_OFFSET_Y, 0.0f };
	g_Player.pos_old = g_Player.pos;
	g_Player.rot = { 0.0f, 0.0f, 0.0f };
	g_Player.scl = { 0.5f, 0.5f, 0.5f };

	g_Player.spd = 0.0f;			// 移動スピードクリア
	g_Player.size = PLAYER_SIZE;	// 当たり判定の大きさ

	g_Player.use = TRUE;
	g_Player.hit = FALSE;
	g_Player.isATK = FALSE;
	g_Player.hp = g_Player.hp_max = 100;
	g_Player.atk = 10;

	// ここでプレイヤー用の影を作成している
	XMFLOAT3 pos = g_Player.pos;
	pos.y -= (PLAYER_OFFSET_Y - 0.1f);
	g_Player.shadowIdx = CreateShadow(pos, PLAYER_SHADOW_SIZE, PLAYER_SHADOW_SIZE);
	//          ↑
	//        このメンバー変数が生成した影のIndex番号

	// 階層アニメーション用の初期化処理
	g_Player.parent = NULL;			// 本体（親）なのでNULLを入れる

	// パーツの初期化
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		g_Parts[i].use = FALSE;

		// 位置・回転・スケールの初期設定
		g_Parts[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Parts[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Parts[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

		// 親子関係
		g_Parts[i].parent = &g_Player;		// ← ここに親のアドレスを入れる
	//	g_Parts[腕].parent= &g_Player;		// 腕だったら親は本体（プレイヤー）
	//	g_Parts[手].parent= &g_Paerts[腕];	// 指が腕の子供だった場合の例

		// 階層アニメーション用のメンバー変数の初期化
		g_Parts[i].tbl_adr = NULL;		// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i].move_time = 0.0f;	// 実行時間をクリア
		g_Parts[i].tbl_size = 0;		// 再生するアニメデータのレコード数をセット

		// パーツの読み込みはまだしていない
		g_Parts[i].load = 0;
	}

	g_Parts[0].use = TRUE;
	g_Parts[0].parent   = &g_Player;		// 親をセット
	g_Parts[0].load = 1;
	LoadModel(MODEL_PLAYER_HEAD, &g_Parts[0].model);

	g_Parts[1].use = TRUE;
	g_Parts[1].parent   = &g_Player;		// 親をセット
	g_Parts[1].load = 1;
	LoadModel(MODEL_PLAYER_ARM_LEFT, &g_Parts[1].model);

	g_Parts[2].use = TRUE;
	g_Parts[2].parent = &g_Player;		// 親をセット
	g_Parts[2].load = 1;
	LoadModel(MODEL_PLAYER_ARM_RIGHT, &g_Parts[2].model);

	g_Parts[3].use = TRUE;
	g_Parts[3].parent = &g_Player;		// 親をセット
	g_Parts[3].load = 1;
	LoadModel(MODEL_PLAYER_LEG_LEFT, &g_Parts[3].model);

	g_Parts[4].use = TRUE;
	g_Parts[4].parent = &g_Player;		// 親をセット
	g_Parts[4].load = 1;
	LoadModel(MODEL_PLAYER_LEG_RIGHT, &g_Parts[4].model);

	g_fuchi = 0;
	fuchi_keep = 0;
	
	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitPlayer(void)
{
	if (g_Load == FALSE) return;

	// モデルの解放処理
	if (g_Player.load)
	{
		UnloadModel(&g_Player.model);
		g_Player.load = FALSE;
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdatePlayer(void)
{
	g_Player.spd = 0.0f;
	g_Player.pos_old = g_Player.pos;

	CAMERA *cam = GetCamera();

	if (g_Player.use == TRUE)
	{
		// 攻撃してないなら
		if (g_Player.isATK == FALSE)
		{// 移動させちゃう
			if (GetKeyboardPress(DIK_A))
			{	// 左へ移動
				g_Player.spd = VALUE_MOVE;
				g_Player.dir = XM_PI / 2;
			}
			if (GetKeyboardPress(DIK_D))
			{	// 右へ移動
				g_Player.spd = VALUE_MOVE;
				g_Player.dir = -XM_PI / 2;
			}
			if (GetKeyboardPress(DIK_W))
			{	// 上へ移動
				g_Player.spd = VALUE_MOVE;
				g_Player.dir = XM_PI;
			}
			if (GetKeyboardPress(DIK_S))
			{	// 下へ移動
				g_Player.spd = VALUE_MOVE;
				g_Player.dir = 0.0f;
			}

			// ゲームパッドの処理
			if (IsButtonPressed(0, BUTTON_LEFT))
			{	// 左へ移動
				g_Player.spd = VALUE_MOVE;
				g_Player.dir = XM_PI / 2;
			}
			if (IsButtonPressed(0, BUTTON_RIGHT))
			{	// 右へ移動
				g_Player.spd = VALUE_MOVE;
				g_Player.dir = -XM_PI / 2;
			}
			if (IsButtonPressed(0, BUTTON_UP))
			{	// 上へ移動
				g_Player.spd = VALUE_MOVE;
				g_Player.dir = XM_PI;
			}
			if (IsButtonPressed(0, BUTTON_DOWN))
			{	// 下へ移動
				g_Player.spd = VALUE_MOVE;
				g_Player.dir = 0.0f;
			}
		}


#ifdef _DEBUG
		if (GetKeyboardPress(DIK_R))
		{
			g_Player.pos.z = g_Player.pos.x = 0.0f;
			g_Player.rot.y = g_Player.dir = 0.0f;
			g_Player.spd = 0.0f;
		}
#endif


		// Key入力があったら移動処理する
		if (g_Player.spd > 0.0f)
		{
			g_Player.rot.y = g_Player.dir + cam->rot.y;

			// 入力のあった方向へプレイヤーを向かせて移動させる
			g_Player.pos.x -= sinf(g_Player.rot.y) * g_Player.spd;
			g_Player.pos.z -= cosf(g_Player.rot.y) * g_Player.spd;
		}
	}


	// レイキャストして足元の高さを求める
	XMFLOAT3 normal = { 0.0f, 1.0f, 0.0f };				// ぶつかったポリゴンの法線ベクトル（向き）
	XMFLOAT3 hitPosition;								// 交点
	hitPosition.y = g_Player.pos.y - PLAYER_OFFSET_Y;	// 外れた時用に初期化しておく
	bool ans = RayHitField(g_Player.pos, &hitPosition, &normal);
	g_Player.pos.y = hitPosition.y + PLAYER_OFFSET_Y;
	//g_Player.pos.y = PLAYER_OFFSET_Y;


	// 影もプレイヤーの位置に合わせる
	XMFLOAT3 pos = g_Player.pos;
	pos.y -= (PLAYER_OFFSET_Y - 0.1f);
	SetPositionShadow(g_Player.shadowIdx, pos);

	// 弾発射処理
	if (GetKeyboardTrigger(DIK_SPACE))
	{
		SetBullet(g_Player.pos, g_Player.rot);
	}

	// ゲームパッドの処理
	if (IsButtonTriggered(0, BUTTON_A))
	{
		SetBullet(g_Player.pos, g_Player.rot);
	}


	g_Player.spd *= 0.5f;



	// 階層アニメーション
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		g_Parts[i].tbl_adr = move_tbl_none;	// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i].tbl_size = sizeof(move_tbl_none) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット

		
		if (g_Player.spd > 0.0f)
		{
			g_Parts[1].tbl_adr = move_tbl_arm;	// 再生するアニメデータの先頭アドレスをセット
			g_Parts[1].tbl_size = sizeof(move_tbl_arm) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		
			g_Parts[2].tbl_adr = move_tbl_arm;	// 再生するアニメデータの先頭アドレスをセット
			g_Parts[2].tbl_size = sizeof(move_tbl_arm) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット

			g_Parts[3].tbl_adr = move_tbl_leg_left;	// 再生するアニメデータの先頭アドレスをセット
			g_Parts[3].tbl_size = sizeof(move_tbl_leg_left) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット

			g_Parts[4].tbl_adr = move_tbl_leg_right;	// 再生するアニメデータの先頭アドレスをセット
			g_Parts[4].tbl_size = sizeof(move_tbl_leg_right) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット

		}

		// 使われているなら処理する
		if ((g_Parts[i].use == TRUE) && (g_Parts[i].tbl_adr != NULL))
		{
			// 移動処理
			int		index = (int)g_Parts[i].move_time;
			float	time = g_Parts[i].move_time - index;
			int		size = g_Parts[i].tbl_size;

			float dt = 1.0f / g_Parts[i].tbl_adr[index].frame;	// 1フレームで進める時間
			g_Parts[i].move_time += dt;					// アニメーションの合計時間に足す

			if (index > (size - 2))	// ゴールをオーバーしていたら、最初へ戻す
			{
				g_Parts[i].move_time = 0.0f;
				index = 0;
			}

			// 座標を求める	X = StartX + (EndX - StartX) * 今の時間
			XMVECTOR p1 = XMLoadFloat3(&g_Parts[i].tbl_adr[index + 1].pos);	// 次の場所
			XMVECTOR p0 = XMLoadFloat3(&g_Parts[i].tbl_adr[index + 0].pos);	// 現在の場所
			XMVECTOR vec = p1 - p0;
			XMStoreFloat3(&g_Parts[i].pos, p0 + vec * time);

			// 回転を求める	R = StartX + (EndX - StartX) * 今の時間
			XMVECTOR r1 = XMLoadFloat3(&g_Parts[i].tbl_adr[index + 1].rot);	// 次の角度
			XMVECTOR r0 = XMLoadFloat3(&g_Parts[i].tbl_adr[index + 0].rot);	// 現在の角度
			XMVECTOR rot = r1 - r0;
			XMStoreFloat3(&g_Parts[i].rot, r0 + rot * time);

			// scaleを求める S = StartX + (EndX - StartX) * 今の時間
			XMVECTOR s1 = XMLoadFloat3(&g_Parts[i].tbl_adr[index + 1].scl);	// 次のScale
			XMVECTOR s0 = XMLoadFloat3(&g_Parts[i].tbl_adr[index + 0].scl);	// 現在のScale
			XMVECTOR scl = s1 - s0;
			XMStoreFloat3(&g_Parts[i].scl, s0 + scl * time);

		}
	}

	// 縁取りを制御
	if (g_Player.hit == TRUE)
	{
		g_fuchi = 1;
		fuchi_keep++;

		if (fuchi_keep == 20)
		{
			g_Player.hit = FALSE;
			fuchi_keep = 0;
			g_fuchi = 0;
		}
	}


	{	// ポイントライト
		LIGHT *light = GetLightData(1);
		XMFLOAT3 pos = g_Player.pos;
		pos.y += 20.0f;

		light->Position = pos;
		light->Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		light->Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		light->Type = LIGHT_TYPE_POINT;
		light->Enable = TRUE;
		SetLightData(1, light);
	}


	//////////////////////////////////////////////////////////////////////
	// 姿勢制御
	//////////////////////////////////////////////////////////////////////

	XMVECTOR vx, nvx, up;
	XMVECTOR quat;
	float len, angle;

	// ２つのベクトルの外積を取って任意の回転軸を求める
	g_Player.upVector = normal;
	up = { 0.0f, 1.0f, 0.0f, 0.0f };
	vx = XMVector3Cross(up, XMLoadFloat3(&g_Player.upVector));

	// 求めた回転軸からクォータニオンを作り出す
	nvx = XMVector3Length(vx);
	XMStoreFloat(&len, nvx);
	nvx = XMVector3Normalize(vx);
	angle = asinf(len);
	quat = XMQuaternionRotationNormal(nvx, angle);

	// 前回のクォータニオンから今回のクォータニオンまでの回転を滑らかにする
	quat = XMQuaternionSlerp(XMLoadFloat4(&g_Player.quaternion), quat, 0.05f);

	// 今回のクォータニオンの結果を保存する
	XMStoreFloat4(&g_Player.quaternion, quat);



#ifdef _DEBUG	// デバッグ情報を表示する
	PrintDebugProc("Player:↑ → ↓ ←　Space\n");
	PrintDebugProc("Player:X:%f Y:%f Z:%f\n", g_Player.pos.x, g_Player.pos.y, g_Player.pos.z);
	PrintDebugProc("player hp:%d\n", g_Player.hp);
	PrintDebugProc("fuchi:%d\n", fuchi_keep);
	PrintDebugProc("hit:%d\n", g_Player.hit);
#endif
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawPlayer(void)
{
	if (g_Player.use == TRUE)
	{	
		// カリング無効
		SetCullingMode(CULL_MODE_NONE);

		XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_Player.scl.x, g_Player.scl.y, g_Player.scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_Player.rot.x, g_Player.rot.y + XM_PI, g_Player.rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// クォータニオンを反映
		XMMATRIX quatMatrix = XMMatrixRotationQuaternion(XMLoadFloat4(&g_Player.quaternion));
		mtxWorld = XMMatrixMultiply(mtxWorld, quatMatrix);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Player.pos.x, g_Player.pos.y, g_Player.pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Player.mtxWorld, mtxWorld);

		//縁取りの設定
		SetFuchi(g_fuchi);

		// モデル描画
		DrawModel(&g_Player.model);



		// パーツの階層アニメーション
		for (int i = 0; i < PLAYER_PARTS_MAX; i++)
		{
			// ワールドマトリックスの初期化
			mtxWorld = XMMatrixIdentity();

			// スケールを反映
			mtxScl = XMMatrixScaling(g_Parts[i].scl.x, g_Parts[i].scl.y, g_Parts[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// 回転を反映
			mtxRot = XMMatrixRotationRollPitchYaw(g_Parts[i].rot.x, g_Parts[i].rot.y, g_Parts[i].rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			// 移動を反映
			mtxTranslate = XMMatrixTranslation(g_Parts[i].pos.x, g_Parts[i].pos.y, g_Parts[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			if (g_Parts[i].parent != NULL)	// 子供だったら親と結合する
			{
				mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(&g_Parts[i].parent->mtxWorld));
				// ↑
				// g_Player.mtxWorldを指している
			}

			XMStoreFloat4x4(&g_Parts[i].mtxWorld, mtxWorld);

			// 使われているなら処理する。ここまで処理している理由は他のパーツがこのパーツを参照している可能性があるから。
			if (g_Parts[i].use == FALSE) continue;

			// ワールドマトリックスの設定
			SetWorldMatrix(&mtxWorld);


			// モデル描画
			DrawModel(&g_Parts[i].model);

		}

		//縁取りの設定
		SetFuchi(0);

		// カリング設定を戻す
		SetCullingMode(CULL_MODE_BACK);
	}
}


//=============================================================================
// プレイヤー情報を取得
//=============================================================================
PLAYER *GetPlayer(void)
{
	return &g_Player;
}


//=============================================================================
// プレイヤーとマップの当たり判定
//=============================================================================
void CheckField(void)
{
	if (g_Player.pos.x < MAP_LEFT
		|| g_Player.pos.x > MAP_RIGHT
		|| g_Player.pos.z < MAP_DOWN
		|| g_Player.pos.z > MAP_TOP)
	{
		g_Player.pos = g_Player.pos_old;
		PlaySound(SOUND_LABEL_SE_object);
	}
}