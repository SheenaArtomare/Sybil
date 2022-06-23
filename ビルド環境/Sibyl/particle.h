//=============================================================================
//
// ファイル	: パーティクル処理 [particle.h]
// 作成者	: GP11A132 16 朱暁テイ
//
//=============================================================================
#pragma once


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MAX_PARTICLE				(1500)		// パーティクル最大数
#define	PARTICLE_COLLISION_SIZE_X	(5.0f)		// 当たり判定の大きさ
#define	PARTICLE_COLLISION_SIZE_Y	(5.0f)		// 当たり判定の大きさ


//*****************************************************************************
// 構造体定義
//*****************************************************************************

// パーティクルの種類
enum
{
	PARTICLE_TYPE_EXPLOSION,			// ヒット・爆破
	PARTICLE_TYPE_SNOW,					// 環境・雪
	PARTICLE_TYPE_THUNDER,				// 攻撃・雷
	PARTICLE_TYPE_FIRE,					// 攻撃・炎1
	PARTICLE_TYPE_FLAME,				// 攻撃・炎2
	PARTICLE_TYPE_HEALTH,				// ケアル1
	PARTICLE_TYPE_CARE,					// ケアル2
	PARTICLE_TYPE_ITEM,					// アイテム
	PARTICLE_TYPE_MAX,
};

struct PARTICLE_TYPE
{
	int texIdx;		// テクスチャ番号

	// [0]が最小値、[1]が最大値
	int		cnt[2];		// 生成数

	int		life[2];	// 寿命

	float	sclX[2];	// 拡縮X
	float	sclY[2];	// 拡縮Y
	float	sclZ[2];	// 拡縮Z

	float	spdX[2];	// 速度X
	float	spdY[2];	// 速度Y
	float	spdZ[2];	// 速度Z
	
	float	pop[2];		// 表示タイミング

	int		digits;		// 小数点以下
	XMFLOAT4	col;	// 色
};


// パーティクル構造体
struct PARTICLE
{
	XMFLOAT3		pos;			// 位置
	XMFLOAT3		rot;			// 回転
	XMFLOAT3		scl;			// 拡縮
	XMFLOAT3		spd;			// 移動量

	MATERIAL		mat;			// マテリアル

	XMFLOAT2		size;			// サイズ

	int				life;			// 寿命

	int				type;			// 種類

	float			pop;			// 表示タイミング

	BOOL			inUse;			// 使用中フラグ
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitParticle(void);
void UninitParticle(void);
void UpdateParticle(void);
void DrawParticle(void);

void SetParticle(int type, XMFLOAT3 pos);
PARTICLE *GetParticle();
