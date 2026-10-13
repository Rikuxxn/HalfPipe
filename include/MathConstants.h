//=============================================================================
//
// 計算用定数 [MathConstants.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _MATH_CONSTANTS_H_// このマクロ定義がされていなかったら
#define _MATH_CONSTANTS_H_// 2重インクルード防止のマクロ定義


//*****************************************************************************
// 名前空間 MathConst
//*****************************************************************************
namespace MathConst
{
	// int型の定数
	constexpr int I_ANGLE_HALF	= 180;		// int型の角度の半分値
	constexpr int I_ANGLE_MAX	= 360;		// int型の角度の最大値
	constexpr int I_DOUBLE		= 2;		// int型の二倍値
	constexpr int I_RAND_NUM	= 10000;	// 0.0～1.0の乱数用

	// float型の定数
	constexpr float F_ANGLE_HALF = 180.0f;	// float型の角度の半分値
	constexpr float F_ANGLE_MAX	= 360.0f;	// float型の角度の最大値
	constexpr float F_DOUBLE	= 2.0f;		// float型の二倍値
	constexpr float HALF		= 0.5f;		// 半分値
	constexpr float F_RAND_NUM	= 10000.0f;	// 0.0～1.0の乱数用
	constexpr float PI_HALF		= 1.57f;	// 円周率の半分

	// 頂点
	constexpr int VERTEX = 4;	// 頂点数
};

#endif