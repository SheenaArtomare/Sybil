//=============================================================================
//
// ファイル	: パーティクル処理 [particle.cpp]
// 作成者	: GP11A132 16 朱暁テイ
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "camera.h"
#include "model.h"
#include "player.h"
#include "enemy.h"
#include "item.h"
#include "particle.h"
#include "customFunc.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX					(2)			// テクスチャの数

#define	LENGTH_FIRE					(20)		// 炎の移動距離

#define	MAX_DIVIDED					(2)			// 雷の分岐数最大数
#define	THUNDER_BASEPOS_Y			(120.0f)	// 雷が分岐できる高さ
#define	PARTICLES_PER_DIVIDE		(5)			// 雷一屈折に何個のパーティクルを埋める
#define	THUNDER_PARTICLE_OFFSET_Y	(-1.5f)		// 雷の落下スピード

#define	MAX_HEALTH					(500)		// ケアル用のパーティクル

#define	MAX_EXPLOSION				(30)		// 爆破用のパーティクル

#define	MAX_ITEM_PARTICLE_1			(200)		// アイテムのパーティクル(サークル)
#define	MAX_ITEM_PARTICLE_2			(10)		// アイテムのパーティクル(柱形)

#define PARTICLE_BASE_SIZE			(10.0f)		// パーティクルのベースサイズ


//*****************************************************************************
// 構造体定義
//*****************************************************************************


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT MakeVertexParticle(void);
void SetThunder(XMFLOAT3 pos, XMFLOAT3 move, float pop);
void SetFire(XMFLOAT3 pos, XMFLOAT3 move, float pop);
void SetHealth(XMFLOAT3 pos, XMFLOAT3 move, float radius, float pop);
void SetExplosion(XMFLOAT3 pos, float pop);
void SetItem(XMFLOAT3 pos, float radius, float pop);


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static BOOL isLoad = FALSE;		// ロード済みフラグ

static ID3D11Buffer					*vertexBuffer = NULL;				// 頂点バッファ

static ID3D11ShaderResourceView		*textures[TEXTURE_MAX] = { NULL };	// テクスチャ情報
static char *textureNames[TEXTURE_MAX] =
{
	"data/TEXTURE/effect000.jpg",
	"data/TEXTURE/effect000.jpg",
};

static PARTICLE			particles[MAX_PARTICLE];		// パーティクル構造体
static PARTICLE_TYPE	types[PARTICLE_TYPE_MAX];		// パーティクルタイプ構造体

static int				dividedCnt;						// 雷の分岐数
static float			item_radian1;
static float			item_radian2;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitParticle(void)
{
	// 頂点情報の作成
	MakeVertexParticle();

	// テクスチャ生成
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		textures[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(), textureNames[i], NULL, NULL, &textures[i], NULL);
	}

	// 構造体初期化
	for(int i = 0; i < MAX_PARTICLE; i++)
	{
		particles[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		particles[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		particles[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		particles[i].spd = XMFLOAT3(1.0f, 1.0f, 1.0f);

		ZeroMemory(&particles[i].mat, sizeof(particles[i].mat));
		particles[i].mat.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		particles[i].size = XMFLOAT2(PARTICLE_COLLISION_SIZE_X, PARTICLE_COLLISION_SIZE_Y);

		particles[i].life = 0;
		particles[i].inUse = FALSE;
	}

	item_radian1 = 0.0f;
	item_radian2 = 0.0f;

	// 雪の初期化
	{
		types[PARTICLE_TYPE_SNOW].texIdx = 1;	// テクスチャ設定

		types[PARTICLE_TYPE_SNOW].life[0] = 250;	// 寿命設定
		types[PARTICLE_TYPE_SNOW].life[1] = 300;

		types[PARTICLE_TYPE_SNOW].cnt[0] = 1;
		types[PARTICLE_TYPE_SNOW].cnt[1] = 1;

		types[PARTICLE_TYPE_SNOW].sclX[0] = 0.4f;
		types[PARTICLE_TYPE_SNOW].sclX[1] = 0.4f;

		types[PARTICLE_TYPE_SNOW].sclY[0] = 0.4f;
		types[PARTICLE_TYPE_SNOW].sclY[1] = 0.4f;

		types[PARTICLE_TYPE_SNOW].sclZ[0] = 0.4f;
		types[PARTICLE_TYPE_SNOW].sclZ[1] = 0.4f;

		types[PARTICLE_TYPE_SNOW].spdX[0] = -0.4f;
		types[PARTICLE_TYPE_SNOW].spdX[1] = +0.4f;

		types[PARTICLE_TYPE_SNOW].spdY[0] = -1.5f;
		types[PARTICLE_TYPE_SNOW].spdY[1] = -0.5f;

		types[PARTICLE_TYPE_SNOW].spdZ[0] = -0.4f;
		types[PARTICLE_TYPE_SNOW].spdZ[1] = +0.4f;

		types[PARTICLE_TYPE_SNOW].pop[0] = 0.0f;
		types[PARTICLE_TYPE_SNOW].pop[1] = 0.0f;

		types[PARTICLE_TYPE_SNOW].col = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	}


	// 炎1の初期化
	{
		types[PARTICLE_TYPE_FIRE].texIdx = 1;	// テクスチャ設定

		types[PARTICLE_TYPE_FIRE].life[0] = 50;	// 寿命設定
		types[PARTICLE_TYPE_FIRE].life[1] = 70;

		types[PARTICLE_TYPE_FIRE].cnt[0] = 20;
		types[PARTICLE_TYPE_FIRE].cnt[1] = 30;

		types[PARTICLE_TYPE_FIRE].sclX[0] = 0.1f;
		types[PARTICLE_TYPE_FIRE].sclX[1] = 0.1f;

		types[PARTICLE_TYPE_FIRE].sclY[0] = 0.1f;
		types[PARTICLE_TYPE_FIRE].sclY[1] = 0.1f;

		types[PARTICLE_TYPE_FIRE].sclZ[0] = 0.1f;
		types[PARTICLE_TYPE_FIRE].sclZ[1] = 0.1f;

		types[PARTICLE_TYPE_FIRE].spdX[0] = -0.2f;
		types[PARTICLE_TYPE_FIRE].spdX[1] = +0.2f;

		types[PARTICLE_TYPE_FIRE].spdY[0] = 0.0f;
		types[PARTICLE_TYPE_FIRE].spdY[1] = +0.2f;

		types[PARTICLE_TYPE_FIRE].spdZ[0] = -0.2f;
		types[PARTICLE_TYPE_FIRE].spdZ[1] = +0.2f;

		types[PARTICLE_TYPE_FIRE].pop[0] = 0.0f;
		types[PARTICLE_TYPE_FIRE].pop[1] = 0.0f;

		types[PARTICLE_TYPE_FIRE].col = XMFLOAT4(1.0f, 0.7f, 0.7f, 1.0f);
	}

	// 炎2の初期化
	{
		types[PARTICLE_TYPE_FLAME].texIdx = 1;	// テクスチャ設定

		types[PARTICLE_TYPE_FLAME].life[0] = 70;	// 寿命設定
		types[PARTICLE_TYPE_FLAME].life[1] = 70;

		types[PARTICLE_TYPE_FLAME].cnt[0] = 1;
		types[PARTICLE_TYPE_FLAME].cnt[1] = 1;

		types[PARTICLE_TYPE_FLAME].sclX[0] = 0.5f;
		types[PARTICLE_TYPE_FLAME].sclX[1] = 0.5f;

		types[PARTICLE_TYPE_FLAME].sclY[0] = 0.5f;
		types[PARTICLE_TYPE_FLAME].sclY[1] = 0.5f;

		types[PARTICLE_TYPE_FLAME].sclZ[0] = 0.5f;
		types[PARTICLE_TYPE_FLAME].sclZ[1] = 0.5f;

		types[PARTICLE_TYPE_FLAME].spdX[0] = 0.0f;
		types[PARTICLE_TYPE_FLAME].spdX[1] = 0.0f;

		types[PARTICLE_TYPE_FLAME].spdY[0] = 0.0f;
		types[PARTICLE_TYPE_FLAME].spdY[1] = 0.0f;

		types[PARTICLE_TYPE_FLAME].spdZ[0] = 0.0f;
		types[PARTICLE_TYPE_FLAME].spdZ[1] = 0.0f;

		types[PARTICLE_TYPE_FLAME].pop[0] = 0.0f;
		types[PARTICLE_TYPE_FLAME].pop[1] = 0.0f;

		types[PARTICLE_TYPE_FLAME].col = XMFLOAT4(1.0f, 0.6f, 0.5f, 1.0f);
	}

	// 雷の初期化
	{
		types[PARTICLE_TYPE_THUNDER].texIdx = 1;	// テクスチャ設定

		types[PARTICLE_TYPE_THUNDER].life[0] = 40;	// 寿命設定
		types[PARTICLE_TYPE_THUNDER].life[1] = 40;

		types[PARTICLE_TYPE_THUNDER].cnt[0] = 1;
		types[PARTICLE_TYPE_THUNDER].cnt[1] = 1;

		types[PARTICLE_TYPE_THUNDER].sclX[0] = 0.3f;
		types[PARTICLE_TYPE_THUNDER].sclX[1] = 0.35f;

		types[PARTICLE_TYPE_THUNDER].sclY[0] = 0.3f;
		types[PARTICLE_TYPE_THUNDER].sclY[1] = 0.35f;

		types[PARTICLE_TYPE_THUNDER].sclZ[0] = 0.3f;
		types[PARTICLE_TYPE_THUNDER].sclZ[1] = 0.35f;

		types[PARTICLE_TYPE_THUNDER].spdX[0] = 0.0f;
		types[PARTICLE_TYPE_THUNDER].spdX[1] = 0.0f;

		types[PARTICLE_TYPE_THUNDER].spdY[0] = 0.0f;
		types[PARTICLE_TYPE_THUNDER].spdY[1] = 0.0f;

		types[PARTICLE_TYPE_THUNDER].spdZ[0] = 0.0f;
		types[PARTICLE_TYPE_THUNDER].spdZ[1] = 0.0f;

		types[PARTICLE_TYPE_THUNDER].pop[0] = 0.0f;
		types[PARTICLE_TYPE_THUNDER].pop[1] = 0.0f;

		types[PARTICLE_TYPE_THUNDER].col = XMFLOAT4(1.0f, 0.7f, 1.0f, 1.0f);
	}


	// ケアル1の初期化
	{
		types[PARTICLE_TYPE_HEALTH].texIdx = 1;	// テクスチャ設定

		types[PARTICLE_TYPE_HEALTH].life[0] = 160;	// 寿命設定
		types[PARTICLE_TYPE_HEALTH].life[1] = 160;

		types[PARTICLE_TYPE_HEALTH].cnt[0] = 1;
		types[PARTICLE_TYPE_HEALTH].cnt[1] = 1;

		types[PARTICLE_TYPE_HEALTH].sclX[0] = 0.1f;
		types[PARTICLE_TYPE_HEALTH].sclX[1] = 0.1f;

		types[PARTICLE_TYPE_HEALTH].sclY[0] = 0.1f;
		types[PARTICLE_TYPE_HEALTH].sclY[1] = 0.1f;

		types[PARTICLE_TYPE_HEALTH].sclZ[0] = 0.1f;
		types[PARTICLE_TYPE_HEALTH].sclZ[1] = 0.1f;

		types[PARTICLE_TYPE_HEALTH].spdX[0] = -0.0f;
		types[PARTICLE_TYPE_HEALTH].spdX[1] = +0.0f;

		types[PARTICLE_TYPE_HEALTH].spdY[0] = -0.0f;
		types[PARTICLE_TYPE_HEALTH].spdY[1] = -0.0f;

		types[PARTICLE_TYPE_HEALTH].spdZ[0] = -0.0f;
		types[PARTICLE_TYPE_HEALTH].spdZ[1] = +0.0f;

		types[PARTICLE_TYPE_HEALTH].pop[0] = 0.0f;
		types[PARTICLE_TYPE_HEALTH].pop[1] = 0.0f;

		types[PARTICLE_TYPE_HEALTH].col = XMFLOAT4(0.5f, 1.0f, 0.7f, 1.0f);
	}


	// ケアル2の初期化
	{
		types[PARTICLE_TYPE_CARE].texIdx = 1;	// テクスチャ設定

		types[PARTICLE_TYPE_CARE].life[0] = 160;	// 寿命設定
		types[PARTICLE_TYPE_CARE].life[1] = 160;

		types[PARTICLE_TYPE_CARE].cnt[0] = 1;
		types[PARTICLE_TYPE_CARE].cnt[1] = 1;

		types[PARTICLE_TYPE_CARE].sclX[0] = 0.1f;
		types[PARTICLE_TYPE_CARE].sclX[1] = 0.1f;

		types[PARTICLE_TYPE_CARE].sclY[0] = 0.1f;
		types[PARTICLE_TYPE_CARE].sclY[1] = 0.1f;

		types[PARTICLE_TYPE_CARE].sclZ[0] = 0.1f;
		types[PARTICLE_TYPE_CARE].sclZ[1] = 0.1f;

		types[PARTICLE_TYPE_CARE].spdX[0] = -0.1f;
		types[PARTICLE_TYPE_CARE].spdX[1] = +0.1f;

		types[PARTICLE_TYPE_CARE].spdY[0] = -0.2f;
		types[PARTICLE_TYPE_CARE].spdY[1] = -0.1f;

		types[PARTICLE_TYPE_CARE].spdZ[0] = -0.1f;
		types[PARTICLE_TYPE_CARE].spdZ[1] = +0.1f;

		types[PARTICLE_TYPE_CARE].pop[0] = 0.0f;
		types[PARTICLE_TYPE_CARE].pop[1] = 0.0f;

		types[PARTICLE_TYPE_CARE].col = XMFLOAT4(0.7f, 1.0f, 0.5f, 1.0f);
	}


	// 爆発の初期化
	{
		types[PARTICLE_TYPE_EXPLOSION].texIdx = 1;

		types[PARTICLE_TYPE_EXPLOSION].life[0] = 30;
		types[PARTICLE_TYPE_EXPLOSION].life[1] = 40;

		types[PARTICLE_TYPE_EXPLOSION].cnt[0] = 10;
		types[PARTICLE_TYPE_EXPLOSION].cnt[1] = 20;

		types[PARTICLE_TYPE_EXPLOSION].sclX[0] = 0.8f;
		types[PARTICLE_TYPE_EXPLOSION].sclX[1] = 0.8f;

		types[PARTICLE_TYPE_EXPLOSION].sclY[0] = 0.8f;
		types[PARTICLE_TYPE_EXPLOSION].sclY[1] = 0.8f;

		types[PARTICLE_TYPE_EXPLOSION].sclZ[0] = 0.8f;
		types[PARTICLE_TYPE_EXPLOSION].sclZ[1] = 0.8f;

		types[PARTICLE_TYPE_EXPLOSION].spdX[0] = 0.0f;
		types[PARTICLE_TYPE_EXPLOSION].spdX[1] = 0.0f;

		types[PARTICLE_TYPE_EXPLOSION].spdY[0] = 0.0f;
		types[PARTICLE_TYPE_EXPLOSION].spdY[1] = 0.0f;

		types[PARTICLE_TYPE_EXPLOSION].spdZ[0] = 0.0f;
		types[PARTICLE_TYPE_EXPLOSION].spdZ[1] = 0.0f;

		types[PARTICLE_TYPE_EXPLOSION].pop[0] = 0.0f;
		types[PARTICLE_TYPE_EXPLOSION].pop[1] = 0.0f;

		types[PARTICLE_TYPE_EXPLOSION].col = XMFLOAT4(1.0f, 0.7f, 0.0f, 1.0f);
	}

	// アイテムの初期化
	{
		types[PARTICLE_TYPE_ITEM].texIdx = 1;	// テクスチャ設定

		types[PARTICLE_TYPE_ITEM].life[0] = 1;	// 寿命設定
		types[PARTICLE_TYPE_ITEM].life[1] = 1;

		types[PARTICLE_TYPE_ITEM].cnt[0] = 1;
		types[PARTICLE_TYPE_ITEM].cnt[1] = 1;

		types[PARTICLE_TYPE_ITEM].sclX[0] = 0.1f;
		types[PARTICLE_TYPE_ITEM].sclX[1] = 0.1f;

		types[PARTICLE_TYPE_ITEM].sclY[0] = 0.1f;
		types[PARTICLE_TYPE_ITEM].sclY[1] = 0.1f;

		types[PARTICLE_TYPE_ITEM].sclZ[0] = 0.1f;
		types[PARTICLE_TYPE_ITEM].sclZ[1] = 0.1f;

		types[PARTICLE_TYPE_ITEM].spdX[0] = 0.0f;
		types[PARTICLE_TYPE_ITEM].spdX[1] = 0.0f;

		types[PARTICLE_TYPE_ITEM].spdY[0] = 0.0f;
		types[PARTICLE_TYPE_ITEM].spdY[1] = 0.0f;

		types[PARTICLE_TYPE_ITEM].spdZ[0] = 0.0f;
		types[PARTICLE_TYPE_ITEM].spdZ[1] = 0.0f;

		types[PARTICLE_TYPE_ITEM].pop[0] = 0.0f;
		types[PARTICLE_TYPE_ITEM].pop[1] = 0.0f;

		types[PARTICLE_TYPE_ITEM].col = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	}

	isLoad = TRUE;
	return S_OK;
}


//=============================================================================
// 終了処理
//=============================================================================
void UninitParticle(void)
{
	if (isLoad == FALSE) return;

	//テクスチャの解放
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (textures[i] != NULL)
		{
			textures[i]->Release();
			textures[i] = NULL;
		}
	}

	// 頂点バッファの解放
	if (vertexBuffer != NULL)
	{
		vertexBuffer->Release();
		vertexBuffer = NULL;
	}

	isLoad = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateParticle(void)
{
	for (int i = 0; i < MAX_PARTICLE; i++)
	{
		if (particles[i].inUse == FALSE) continue;

		// 移動処理
		particles[i].pos.x += particles[i].spd.x;
		particles[i].pos.y += particles[i].spd.y;
		particles[i].pos.z += particles[i].spd.z;

		// 寿命の処理
		particles[i].life--;
		if (particles[i].life <= 10)
		{	// 寿命が３０フレーム切ったら段々透明になっていく
			particles[i].mat.Diffuse.w -= 0.1f;
			if (particles[i].mat.Diffuse.w < 0.0f)
			{
				particles[i].mat.Diffuse.w = 0.0f;
			}
		}
		// 寿命切ったら消す
		if (particles[i].life <= 0)
		{
			particles[i].inUse = FALSE;
		}
		// 地面の下に行かせないように
		if (particles[i].pos.y < 0)
		{
			particles[i].pos.y = 0.0f;
			particles[i].life = 10;
			particles[i].spd = XMFLOAT3(0.0f, 0.0f, 0.0f);
		}
		

		// 表示タイミングの処理
		if (particles[i].pop > 0.0f)
		{
			particles[i].pop--;
		}
	}


	// 必要な情報を取得
	PLAYER *player = GetPlayer();
	ENEMY *enemy = GetEnemy();

	if (enemy[0].hp > enemy[0].hp_max / 2)
	// 環境雪の処理
	{
		XMFLOAT3 pos;
		pos.x = (float)(RandomRange(-500, 500));
		pos.y = 200.0f;
		pos.z = (float)(RandomRange(-500, 500));

		SetParticle(PARTICLE_TYPE_SNOW, pos);
	}


	// 炎の発生処理
	if (enemy[0].skill == 0)
	{
		XMFLOAT3 pos = enemy[0].pos;
		XMFLOAT3 move = XMFLOAT3(- sinf(enemy[0].rot.y) * 3.0f, 0.0f, - cosf(enemy[0].rot.y) * 3.0f);

		pos.x = enemy[0].pos.x - sinf(enemy[0].rot.y) * 30.0f;
		pos.z = enemy[0].pos.z - cosf(enemy[0].rot.y) * 30.0f;
		
		types[PARTICLE_TYPE_FIRE].pop[0] = 0.0f;
		types[PARTICLE_TYPE_FIRE].pop[1] = 0.0f;

		types[PARTICLE_TYPE_FLAME].pop[0] = 0.0f;
		types[PARTICLE_TYPE_FLAME].pop[1] = 0.0f;

		SetFire(pos, move, 0.0f);
	}
	

	// 雷の発生処理
	if (enemy[0].hp < 50)
	{
		int k = rand() % 40;

		if (k == 0) 
		{
			dividedCnt = 0;

			// パーティクルをセット
			XMFLOAT3 pos = XMFLOAT3(0.0f, THUNDER_BASEPOS_Y, 0.0f);
			XMFLOAT3 move = XMFLOAT3(0.0f, THUNDER_PARTICLE_OFFSET_Y, 0.0f);

			//pos.x = player->pos.x - sinf(player->rot.y) * 60.0f;
			//pos.z = player->pos.z - cosf(player->rot.y) * 60.0f;

			pos.x = (float)(RandomRange(-500, 500));
			pos.z = (float)(RandomRange(-500, 500));

			SetThunder(pos, move, 0.0f);
		}

		//// 複数の雷を出す場合
		//// 2個目の雷の処理
		// pos = XMFLOAT3(200.0f, 150.0f, 0.0f);
		// move = XMFLOAT3(0.0f, THUNDER_PARTICLE_OFFSET_Y, 0.0f);

		//types[PARTICLE_TYPE_THUNDER].pop[0] = 0.0f;
		//types[PARTICLE_TYPE_THUNDER].pop[1] = 0.0f;

		//SetThunder(pos, move, 0.0f);
	}


	//爆破
	if (player->hit == TRUE)
	{
		XMFLOAT3 pos = player->pos;

		types[PARTICLE_TYPE_EXPLOSION].pop[0] = 0.0f;
		types[PARTICLE_TYPE_EXPLOSION].pop[1] = 0.0f;
	
		SetExplosion(pos, 0.0f);

	}

	//アイテム
	{
		ITEM *item = GetItem();

		for (int i = 0; i < MAX_ITEM; i++)
		{
			if (item[i].use == TRUE)
			{
				XMFLOAT3 pos = item[i].pos;
				pos.y = 0.0f;
				XMFLOAT3 move = XMFLOAT3(0.0f, 0.0f, 0.0f);
				float radius = 12.0f;

				types[PARTICLE_TYPE_ITEM].pop[0] = 0.0f;
				types[PARTICLE_TYPE_ITEM].pop[1] = 0.0f;

				SetItem(pos, radius, 0.0f);
			}
		

			//ケアル
			if (item[i].effect == TRUE)
			{
				PLAYER *player = GetPlayer();

				XMFLOAT3 pos = player->pos;
				XMFLOAT3 move = XMFLOAT3(0.0f, 0.0f, 0.0f);
				float radius = 2.0f;

				pos.y = player->pos.y + 40.0f;

				types[PARTICLE_TYPE_HEALTH].pop[0] = 0.0f;
				types[PARTICLE_TYPE_HEALTH].pop[1] = 0.0f;

				types[PARTICLE_TYPE_CARE].pop[0] = 0.0f;
				types[PARTICLE_TYPE_CARE].pop[1] = 0.0f;

				SetHealth(pos, move, radius, 0.0f);
				
				item[i].effect = FALSE;
			}
		}
	}



}


// 炎の移動処理
void SetFire(XMFLOAT3 pos, XMFLOAT3 move,float pop)
{
	// パラメータを初期化
	types[PARTICLE_TYPE_FIRE].sclX[0] = 0.1f;
	types[PARTICLE_TYPE_FIRE].sclX[1] = 0.1f;

	types[PARTICLE_TYPE_FIRE].sclY[0] = 0.1f;
	types[PARTICLE_TYPE_FIRE].sclY[1] = 0.1f;

	types[PARTICLE_TYPE_FIRE].sclZ[0] = 0.1f;
	types[PARTICLE_TYPE_FIRE].sclZ[1] = 0.1f;

	types[PARTICLE_TYPE_FLAME].sclX[0] = 0.5f;
	types[PARTICLE_TYPE_FLAME].sclX[1] = 0.5f;

	types[PARTICLE_TYPE_FLAME].sclY[0] = 0.5f;
	types[PARTICLE_TYPE_FLAME].sclY[1] = 0.5f;

	types[PARTICLE_TYPE_FLAME].sclZ[0] = 0.5f;
	types[PARTICLE_TYPE_FLAME].sclZ[1] = 0.5f;

	types[PARTICLE_TYPE_FIRE].spdX[0] = -0.2f;
	types[PARTICLE_TYPE_FIRE].spdX[1] = +0.2f;

	types[PARTICLE_TYPE_FIRE].spdY[0] = 0.0f;
	types[PARTICLE_TYPE_FIRE].spdY[1] = +0.2f;

	types[PARTICLE_TYPE_FIRE].spdZ[0] = -0.2f;
	types[PARTICLE_TYPE_FIRE].spdZ[1] = +0.2f;

	// 相対位置を合わせる
	types[PARTICLE_TYPE_FIRE].spdX[0] += move.x;
	types[PARTICLE_TYPE_FIRE].spdX[1] += move.x;

	types[PARTICLE_TYPE_FIRE].spdZ[0] += move.z;
	types[PARTICLE_TYPE_FIRE].spdZ[1] += move.z;

	types[PARTICLE_TYPE_FLAME].spdX[0] = move.x;
	types[PARTICLE_TYPE_FLAME].spdX[1] = move.x;

	types[PARTICLE_TYPE_FLAME].spdZ[0] = move.z;
	types[PARTICLE_TYPE_FLAME].spdZ[1] = move.z;

	for (int i = 0; i < LENGTH_FIRE; i++)
	{
		SetParticle(PARTICLE_TYPE_FIRE, pos);
		SetParticle(PARTICLE_TYPE_FLAME, pos);

		// タイミングをずらして表示させる
		types[PARTICLE_TYPE_FIRE].pop[0] = pop + 5.0f * i;
		types[PARTICLE_TYPE_FIRE].pop[1] = pop + 5.0f * i;
	
		types[PARTICLE_TYPE_FLAME].pop[0] = pop + 5.0f * i;
		types[PARTICLE_TYPE_FLAME].pop[1] = pop + 5.0f * i;

		pos.x += move.x;
		pos.y += move.y;
		pos.z += move.z;

		types[PARTICLE_TYPE_FIRE].sclX[0] += 0.01f;
		types[PARTICLE_TYPE_FIRE].sclX[1] += 0.01f;

		types[PARTICLE_TYPE_FIRE].sclY[0] += 0.01f;
		types[PARTICLE_TYPE_FIRE].sclY[1] += 0.01f;

		types[PARTICLE_TYPE_FIRE].sclZ[0] += 0.01f;
		types[PARTICLE_TYPE_FIRE].sclZ[1] += 0.01f;

		types[PARTICLE_TYPE_FLAME].sclX[0] += 0.07f;
		types[PARTICLE_TYPE_FLAME].sclX[1] += 0.07f;

		types[PARTICLE_TYPE_FLAME].sclY[0] += 0.07f;
		types[PARTICLE_TYPE_FLAME].sclY[1] += 0.07f;

		types[PARTICLE_TYPE_FLAME].sclZ[0] += 0.07f;
		types[PARTICLE_TYPE_FLAME].sclZ[1] += 0.07f;
	}

};


// 雷の組立処理
void SetThunder(XMFLOAT3 pos, XMFLOAT3 move, float pop)
{
	for (int k = 1; k < 150; k++)
	{
		if (pos.y < 0.0f) break;	// 地面に着いたら終わり

		if (k % PARTICLES_PER_DIVIDE == 0)
		{// PARTICLES_PER_DIVIDE個セットしたら

			// 移動量をセット
			move.x = RandomFloat(2, 1.5f, -1.5f);
			move.z = RandomFloat(2, 1.5f, -1.5f);

			if (RandomRange(0, 100) < 30 && dividedCnt < MAX_DIVIDED && pos.y > THUNDER_BASEPOS_Y * 0.5f)
			{// 30％の確率で分岐させる
				dividedCnt++;

				// 分岐された雷の動きを元の反対の数値にする
				XMFLOAT3 invMove = { move.x * -1, move.y, move.z * -1 };
				
				// 大きさをセット
				float scl = RandomFloat(2, 0.35f, 0.3f);
				
				types[PARTICLE_TYPE_THUNDER].sclX[0] = scl;
				types[PARTICLE_TYPE_THUNDER].sclX[1] = scl;

				types[PARTICLE_TYPE_THUNDER].sclZ[0] = scl;
				types[PARTICLE_TYPE_THUNDER].sclZ[1] = scl;

				types[PARTICLE_TYPE_THUNDER].sclZ[0] = scl;
				types[PARTICLE_TYPE_THUNDER].sclZ[1] = scl;

				SetThunder(pos, invMove, types[PARTICLE_TYPE_THUNDER].pop[0]);
			}

		}
		
		SetParticle(PARTICLE_TYPE_THUNDER, pos);

		pos.x += move.x;
		pos.y += move.y;
		pos.z += move.z;

		// タイミングをずらして表示させる
		types[PARTICLE_TYPE_THUNDER].pop[0] = pop + 0.1f * k;
		types[PARTICLE_TYPE_THUNDER].pop[1] = pop + 0.1f * k;
	}
};



// ケアルの組立処理
void SetHealth(XMFLOAT3 pos, XMFLOAT3 move, float radius, float pop)
{
	XMFLOAT3 temp = pos;
	float radian = 0.0f;
	float add = XM_PI / 40;
	
	for (int i = 0; i < MAX_HEALTH; i++)
	{
		if (radian > XM_2PI)
		{
			radius += 5.0f;
			temp.y -= 10.0f;
			radian = 0.0f;
		}
		
		SetParticle(PARTICLE_TYPE_HEALTH, temp);
		SetParticle(PARTICLE_TYPE_CARE, temp);

		temp.x = pos.x + sinf(radian) * radius;
		temp.z = pos.z + cosf(radian) * radius;

		radian += add;

		// タイミングをずらして表示させる
		types[PARTICLE_TYPE_HEALTH].pop[0] = pop + 0.2f * i;
		types[PARTICLE_TYPE_HEALTH].pop[1] = pop + 0.2f * i;

		types[PARTICLE_TYPE_CARE].pop[0] = pop + 0.2f * i;
		types[PARTICLE_TYPE_CARE].pop[1] = pop + 0.2f * i;
	}

};


// 爆破の組立処理
void SetExplosion(XMFLOAT3 pos, float pop)
{
	XMFLOAT3 temp = pos;
	XMFLOAT3 move = XMFLOAT3(0.0f, 0.0f, 0.0f);
	float scl = 0.0f;

	for (int i = 0; i < MAX_EXPLOSION; i++)
	{
	
		int col = rand() & 3;
		switch (col)
		{
		case 0:
			types[PARTICLE_TYPE_EXPLOSION].col = XMFLOAT4(0.7f, 0.4f, 0.0f, 1.0f);
			break;

		case 1:
			types[PARTICLE_TYPE_EXPLOSION].col = XMFLOAT4(0.4f, 0.2f, 0.0f, 1.0f);
			break;
		
		case 2:
			types[PARTICLE_TYPE_EXPLOSION].col = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
			break;
		}

		SetParticle(PARTICLE_TYPE_EXPLOSION, temp);

		move.x = RandomFloat(2, 7.0f, -7.0f);
		move.y = RandomFloat(2, 7.0f, -7.0f);
		move.z = RandomFloat(2, 7.0f, -7.0f);

		temp.x = pos.x + move.x;
		temp.y = pos.y + move.y;
		temp.z = pos.z + move.z;

		// タイミングをずらして表示させる
		types[PARTICLE_TYPE_EXPLOSION].pop[0] = pop + 0.5f * i;
		types[PARTICLE_TYPE_EXPLOSION].pop[1] = pop + 0.5f * i;
	}
}


// アイテムの組立処理
void SetItem(XMFLOAT3 pos, float radius, float pop)
{
	float radian = 0.0f;
	XMFLOAT3 temp = pos;

	// 各パーティクル間の距離を指定
	float add = XM_2PI / MAX_ITEM_PARTICLE_1;

	// 地面のサークルを生成
	for (int i = 0; i < MAX_ITEM_PARTICLE_1; i++)
	{
		// 360°を越えたらbreakする
		if (radian > XM_2PI)break;

		types[PARTICLE_TYPE_ITEM].sclY[0] = 0.2f;
		types[PARTICLE_TYPE_ITEM].sclY[1] = 0.2f;

		SetParticle(PARTICLE_TYPE_ITEM, temp);

		temp.x = pos.x + sinf(radian) * radius;
		temp.z = pos.z + cosf(radian) * radius;

		radian += add;

		// タイミングをずらして表示させる
		//types[PARTICLE_TYPE_ITEM].pop[0] = pop + 0.3f * i;
		//types[PARTICLE_TYPE_ITEM].pop[1] = pop + 0.3f * i;

	}
	
	// 角度を0.0fに戻す
	radian = item_radian1;
	
	// ｙ座標を0.0fに戻す
	temp.y = 0.0f;

	// 各パーティクル間の距離を初期化
	add = XM_2PI / MAX_ITEM_PARTICLE_2;
	
	// パーティクル不透明度のデータを取っておく
	float old_alpha = types[PARTICLE_TYPE_ITEM].col.w;
	
	// 不透明度をsinカーブで表示　0.0f+0.3f～1.0f
	float alpha = sinf(item_radian2) + 0.3f;
	
	if (alpha > 1.0f)
	{
		alpha = 1.0f;
	}
	
	// 柱形パーティクルを生成する
	for (int i = 0; i < MAX_ITEM_PARTICLE_2; i++)
	{
		types[PARTICLE_TYPE_ITEM].sclY[0] = 5.0f;
		types[PARTICLE_TYPE_ITEM].sclY[1] = 5.0f;

		temp.x = pos.x + sinf(radian) * radius;
		temp.z = pos.z + cosf(radian) * radius;

		radian += add;
		types[PARTICLE_TYPE_ITEM].col.w = alpha;

		SetParticle(PARTICLE_TYPE_ITEM, temp);

	}
	
	// パーティクルの不透明度を戻す
	types[PARTICLE_TYPE_ITEM].col.w = old_alpha;
	
	// 柱形パーティクル毎回の回転量を足す
	item_radian1 += 0.001f;
	
	// 柱形パーティクル毎回の不透明度を足す
	item_radian2 += 0.01f;

	if (item_radian1 > XM_2PI)
	{
		item_radian1 = 0.0f;
	}

	if (item_radian2 > XM_2PI)
	{
		item_radian2 = 0.0f;
	}

};


//=============================================================================
// 描画処理
//=============================================================================
void DrawParticle(void)
{
	CAMERA *cam = GetCamera();
	XMMATRIX mtxScl, mtxTranslate, mtxWorld, mtxView;

	// ライティングを無効に
	SetLightEnable(false);

	// 加算合成に設定
	SetBlendState(BLEND_MODE_ADD);

	// Z比較無し
	SetDepthEnable(false);

	// フォグ無効
	SetFogEnable(false);

	int state = rand() % 4;

	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	for(int i = 0; i < MAX_PARTICLE; i++)
	{
		if (particles[i].inUse == FALSE || particles[i].pop > 0.0f) continue;

		if (state == 1 && particles[i].type == PARTICLE_TYPE_THUNDER)
		{
			// 減算合成に設定
			SetBlendState(BLEND_MODE_SUBTRACT);
		}
		
		else
		{
			// 加算合成に設定
			SetBlendState(BLEND_MODE_ADD);
		}

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// ビューマトリックスを取得
		mtxView = XMLoadFloat4x4(&cam->mtxView);

		// ビルボード処理
		mtxWorld.r[0].m128_f32[0] = mtxView.r[0].m128_f32[0];
		mtxWorld.r[0].m128_f32[1] = mtxView.r[1].m128_f32[0];
		mtxWorld.r[0].m128_f32[2] = mtxView.r[2].m128_f32[0];

		mtxWorld.r[1].m128_f32[0] = mtxView.r[0].m128_f32[1];
		mtxWorld.r[1].m128_f32[1] = mtxView.r[1].m128_f32[1];
		mtxWorld.r[1].m128_f32[2] = mtxView.r[2].m128_f32[1];

		mtxWorld.r[2].m128_f32[0] = mtxView.r[0].m128_f32[2];
		mtxWorld.r[2].m128_f32[1] = mtxView.r[1].m128_f32[2];
		mtxWorld.r[2].m128_f32[2] = mtxView.r[2].m128_f32[2];

		// スケールを反映
		mtxScl = XMMatrixScaling(particles[i].scl.x, particles[i].scl.y, particles[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(particles[i].pos.x, particles[i].pos.y, particles[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		// テクスチャ設定
		int texIndex = types[particles[i].type].texIdx;
		GetDeviceContext()->PSSetShaderResources(0, 1, &textures[texIndex]);

		// マテリアルを設定
		SetMaterial(particles[i].mat);

		// ポリゴンの描画
		GetDeviceContext()->Draw(4, 0);
	}
	

	// ライティングを有効に
	SetLightEnable(true);

	// 通常ブレンドに戻す
	SetBlendState(BLEND_MODE_ALPHABLEND);

	// Z比較有効
	SetDepthEnable(TRUE);

}


//=============================================================================
// 頂点情報の作成
//=============================================================================
HRESULT MakeVertexParticle(void)
{
	float a = PARTICLE_BASE_SIZE;

	VERTEX_3D vertexArray[4] =
	{
		{ XMFLOAT3(-5.0f, +5.0f, 0.0f),  XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(+5.0f, +5.0f, 0.0f),  XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(-5.0f, -5.0f, 0.0f),  XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(+5.0f, -5.0f, 0.0f),  XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
	};

	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.pSysMem = vertexArray;

	GetDevice()->CreateBuffer(&bd, &sd, &vertexBuffer);

	return S_OK;
}


//=============================================================================
// パーティクルの発生処理
//=============================================================================
void SetParticle(int type, XMFLOAT3 pos)
{
	// 生成するエフェクトを選択
	PARTICLE_TYPE *curType = &types[type];

	// 生成数を決定
	int cnt = RandomRange(curType->cnt[0], curType->cnt[1]);

	for (int i = 0; i < cnt; i++)
	{
		for (int j = 0; j < MAX_PARTICLE; j++)
		{
			// パーティクル生成
			if (particles[j].inUse == FALSE)
			{
				particles[j].type = type;

				particles[j].pos = pos;

				particles[j].life = RandomRange(curType->life[0], curType->life[1]);

				particles[j].scl.x = (float)(RandomRange((int)(curType->sclX[0] * 10), (int)(curType->sclX[1] * 10))) * 0.1f;
				particles[j].scl.y = (float)(RandomRange((int)(curType->sclY[0] * 10), (int)(curType->sclY[1] * 10))) * 0.1f;
				particles[j].scl.z = (float)(RandomRange((int)(curType->sclZ[0] * 10), (int)(curType->sclZ[1] * 10))) * 0.1f;

				particles[j].spd.x = (float)(RandomRange((int)(curType->spdX[0] * 10), (int)(curType->spdX[1] * 10))) * 0.1f;
				particles[j].spd.y = (float)(RandomRange((int)(curType->spdY[0] * 10), (int)(curType->spdY[1] * 10))) * 0.1f;
				particles[j].spd.z = (float)(RandomRange((int)(curType->spdZ[0] * 10), (int)(curType->spdZ[1] * 10))) * 0.1f;

				particles[j].pop = (float)(RandomRange((int)(curType->pop[0] * 10), (int)(curType->pop[1] * 10))) * 0.1f;

				particles[j].mat.Diffuse = curType->col;


				particles[j].inUse = TRUE;
				break;
			}
		}
	}
}


//=============================================================================
// パーティクル情報の取得
//=============================================================================
PARTICLE *GetParticle()
{
	return &particles[0];
}
