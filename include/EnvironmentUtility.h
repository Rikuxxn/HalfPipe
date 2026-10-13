//=============================================================================
//
// 環境ユーティリティ処理 [EnvironmentUtility.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _ENVIRONMENT_UTILITY_H_// このマクロ定義がされていなかったら
#define _ENVIRONMENT_UTILITY_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// 名前空間 EnvironmentUtil
//*****************************************************************************
namespace EnvironmentUtil
{
    // 太陽の方向を計算して返す
    D3DXVECTOR3 GetSunDirection(float time);

    // 月の方向を計算して返す
    D3DXVECTOR3 GetMoonDirection(float time);
}

#endif