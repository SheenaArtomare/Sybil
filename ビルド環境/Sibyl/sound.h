//=============================================================================
//
// サウンド処理 [sound.h]
// Author : GP11A132 16 朱暁テイ
//
//=============================================================================
#pragma once

#include <windows.h>
#include "xaudio2.h"						// サウンド処理で必要

//*****************************************************************************
// サウンドファイル
//*****************************************************************************
enum 
{
	SOUND_LABEL_BGM_title,		// タイトルBGM
	SOUND_LABEL_BGM_tutorial,	// チュートリアルBGM
	SOUND_LABEL_BGM_map,		// マップBGM
	SOUND_LABEL_BGM_bossbattle,	// ボス戦BGM
	SOUND_LABEL_BGM_ending,		// エンディング曲
	SOUND_LABEL_SE_system_01,	// システム効果音01
	SOUND_LABEL_SE_system_02,	// システム効果音02
	SOUND_LABEL_SE_system_03,	// システム効果音03
	SOUND_LABEL_SE_skill_a,		// プレイヤースキルA
	SOUND_LABEL_SE_skill_b,	// プレイヤースキルB
	SOUND_LABEL_SE_enemyskill_a,// エネミースキルA
	SOUND_LABEL_SE_enemyskill_b,// エネミースキルB
	SOUND_LABEL_SE_enemyskill_c1,// エネミースキルC1
	SOUND_LABEL_SE_enemyskill_c2,// エネミースキルC2
	SOUND_LABEL_SE_enemyskill_c3,// エネミースキルC3
	SOUND_LABEL_SE_object,		// オブジェクト効果音
	SOUND_LABEL_SE_hit,			// ヒット効果音
	SOUND_LABEL_SE_gameclear,	// GAMECLEAR効果音
	SOUND_LABEL_SE_gameover1,	// GAMEOVER効果音1
	SOUND_LABEL_SE_gameover2,	// GAMEOVER効果音2
	SOUND_LABEL_MAX,
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
BOOL InitSound(HWND hWnd);
void UninitSound(void);
void PlaySound(int label);
void StopSound(int label);
void StopSound(void);

