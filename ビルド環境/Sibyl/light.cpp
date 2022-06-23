//=============================================================================
//
// ライト処理 [light.cpp]
// Author : GP11A132 16 朱暁テイ
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "particle.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************



//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static LIGHT	g_Light[LIGHT_MAX];

static FOG		g_Fog;

static INTERPOLATION_DATA_LIGHT	*tbl_adr;			// アニメデータのテーブル先頭アドレス
static int					tbl_size;			// 登録したテーブルのレコード総数
static float				move_time;			// 実行時間

static int					timer;
static bool					thunder;

static INTERPOLATION_DATA_LIGHT move_tbl0[] = {	// カラー, フレーム
	{ XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 100.0f },
	{ XMFLOAT4(1.0f, 0.7f, 0.5f, 1.0f), 30.0f },
	{ XMFLOAT4(0.0f, 0.0f, 0.3f, 1.0f), 100.0f },
	{ XMFLOAT4(1.0f, 0.5f, 0.7f, 1.0f), 30.0f },
	{ XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 1.0f },
};


//=============================================================================
// 初期化処理
//=============================================================================
void InitLight(void)
{

	//ライト初期化
	for (int i = 0; i < LIGHT_MAX; i++)
	{
		g_Light[i].Position  = XMFLOAT3( 0.0f, 0.0f, 0.0f );
		g_Light[i].Direction = XMFLOAT3( 0.0f, -1.0f, 0.0f );
		g_Light[i].Diffuse   = XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );
		g_Light[i].Ambient   = XMFLOAT4( 0.0f, 0.0f, 0.0f, 0.0f );
		g_Light[i].Attenuation = 100.0f;	// 減衰距離
		g_Light[i].Type = LIGHT_TYPE_NONE;	// ライトのタイプ
		g_Light[i].Enable = false;			// ON / OFF
		SetLight(i, &g_Light[i]);
	}

	move_time = 0.0f;			// 線形補間用のタイマーをクリア
	tbl_adr = move_tbl0;		// 再生するアニメデータの先頭アドレスをセット
	tbl_size = sizeof(move_tbl0) / sizeof(INTERPOLATION_DATA_LIGHT);	// 再生するアニメデータのレコード数をセット


	// 並行光源の設定（世界を照らす光）
	g_Light[0].Position = XMFLOAT3(0.0f, 200.0f, 0.0f);			// 光の向き
	g_Light[0].Direction = XMFLOAT3( 0.0f, -1.0f, 0.0f );		// 光の向き
	g_Light[0].Diffuse   = XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );	// 光の色
	g_Light[0].Type = LIGHT_TYPE_DIRECTIONAL;					// 並行光源
	g_Light[0].Enable = true;									// このライトをON
	SetLight(0, &g_Light[0]);									// これで設定している


	// フォグの初期化（霧の効果）
	g_Fog.FogStart = 50.0f;										// 視点からこの距離離れるとフォグがかかり始める
	g_Fog.FogEnd   = 1000.0f;									// ここまで離れるとフォグの色で見えなくなる
	g_Fog.FogColor = XMFLOAT4( 0.0f, 0.0f, 0.0f, 1.0f );		// フォグの色
	SetFog(&g_Fog);
	SetFogEnable(true);		// 他の場所もチェックする shadow



}


//=============================================================================
// 更新処理
//=============================================================================
void UpdateLight(void)
{
	
	if (tbl_adr != NULL)	// 線形補間を実行する？
	{						// 線形補間の処理
		// 移動処理
		int		index = (int)move_time;
		float	time = move_time - index;
		int		size = tbl_size;

		float dt = 0.1f / tbl_adr[index].frame;	// 1フレームで進める時間
		move_time += dt;						// アニメーションの合計時間に足す

		if (index > (size - 2))	// ゴールをオーバーしていたら、最初へ戻す
		{
			move_time = 0.0f;
			index = 0;
		}

		// 座標を求める	X = StartX + (EndX - StartX) * 今の時間
		XMVECTOR p1 = XMLoadFloat4(&tbl_adr[index + 1].color);	// 次の場所
		XMVECTOR p0 = XMLoadFloat4(&tbl_adr[index + 0].color);	// 現在の場所
		XMVECTOR vec = p1 - p0;
		XMStoreFloat4(&g_Light[0].Diffuse, p0 + vec * time);
	}


	SetLight(0, &g_Light[0]);

	
	


}


//=============================================================================
// ライトの設定
// Typeによってセットするメンバー変数が変わってくる
//=============================================================================
void SetLightData(int index, LIGHT *light)
{
	SetLight(index, light);
}


LIGHT *GetLightData(int index)
{
	return(&g_Light[index]);
}


//=============================================================================
// フォグの設定
//=============================================================================
void SetFogData(FOG *fog)
{
	SetFog(fog);
}



