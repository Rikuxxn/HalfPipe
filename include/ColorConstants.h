//=============================================================================
//
// カラー定数 [ColorConstants.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _COLOR_CONSTANTS_H_// このマクロ定義がされていなかったら
#define _COLOR_CONSTANTS_H_// 2重インクルード防止のマクロ定義

//***************************************************************************** 
// 色関連の定数 
//***************************************************************************** 
namespace ColorConst
{
	//*************************************************************************
	// D3DCOLOR
	//*************************************************************************
	namespace D3D
	{
		//=====================================================================
		// 基本色 
		//=====================================================================
		constexpr D3DCOLOR WHITE	= D3DCOLOR_ARGB(255, 255, 255, 255);
		constexpr D3DCOLOR BLACK	= D3DCOLOR_ARGB(255, 0, 0, 0);
		constexpr D3DCOLOR RED		= D3DCOLOR_ARGB(255, 255, 0, 0);
		constexpr D3DCOLOR GREEN	= D3DCOLOR_ARGB(255, 0, 255, 0);
		constexpr D3DCOLOR BLUE		= D3DCOLOR_ARGB(255, 0, 0, 255);
		constexpr D3DCOLOR YELLOW	= D3DCOLOR_ARGB(255, 255, 255, 0);
		constexpr D3DCOLOR CYAN		= D3DCOLOR_ARGB(255, 0, 255, 255);
		constexpr D3DCOLOR MAGENTA	= D3DCOLOR_ARGB(255, 255, 0, 255);

		//=====================================================================
		// グレー 
		//=====================================================================
		constexpr D3DCOLOR GRAY			= D3DCOLOR_ARGB(255, 128, 128, 128);
		constexpr D3DCOLOR LIGHT_GRAY	= D3DCOLOR_ARGB(255, 192, 192, 192);
		constexpr D3DCOLOR DARK_GRAY	= D3DCOLOR_ARGB(255, 64, 64, 64);

		//=====================================================================
		// 透明 
		//=====================================================================
		constexpr D3DCOLOR INVISIBLE		= D3DCOLOR_ARGB(0, 0, 0, 0);
		constexpr D3DCOLOR HALF_ALPHA_WHITE = D3DCOLOR_ARGB(128, 255, 255, 255);

		//=====================================================================
		// デバッグ用 
		//=====================================================================
		constexpr D3DCOLOR DEBUG_COLLISION	= D3DCOLOR_ARGB(128, 255, 0, 0);
		constexpr D3DCOLOR DEBUG_TRIGGER	= D3DCOLOR_ARGB(128, 0, 255, 0);
		constexpr D3DCOLOR DEBUG_RAY		= D3DCOLOR_ARGB(255, 255, 255, 0);
	}

	//*************************************************************************
	// D3DXCOLOR
	//*************************************************************************
	namespace DX
	{
		//===================================================================== 
		// 基本色 
		//=====================================================================
		inline const D3DXCOLOR WHITE	= D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		inline const D3DXCOLOR BLACK	= D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
		inline const D3DXCOLOR RED		= D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
		inline const D3DXCOLOR GREEN	= D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f);
		inline const D3DXCOLOR BLUE		= D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
		inline const D3DXCOLOR YELLOW	= D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f);
		inline const D3DXCOLOR CYAN		= D3DXCOLOR(0.0f, 1.0f, 1.0f, 1.0f);
		inline const D3DXCOLOR MAGENTA	= D3DXCOLOR(1.0f, 0.0f, 1.0f, 1.0f);

		//=====================================================================
		// グレー 
		//=====================================================================
		inline const D3DXCOLOR GRAY			= D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
		inline const D3DXCOLOR LIGHT_GRAY	= D3DXCOLOR(0.75f, 0.75f, 0.75f, 1.0f);
		inline const D3DXCOLOR DARK_GRAY	= D3DXCOLOR(0.25f, 0.25f, 0.25f, 1.0f);

		//=====================================================================
		// 透明
		//=====================================================================
		inline const D3DXCOLOR INVISIBLE		= D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);
		inline const D3DXCOLOR HALF_ALPHA_WHITE	= D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f);

		//=====================================================================
		// デバッグ用 
		//=====================================================================
		inline const D3DXCOLOR DEBUG_COLLISION	= D3DXCOLOR(1.0f, 0.0f, 0.0f, 0.5f);
		inline const D3DXCOLOR DEBUG_TRIGGER	= D3DXCOLOR(0.0f, 1.0f, 0.0f, 0.5f);
		inline const D3DXCOLOR DEBUG_RAY		= D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f);
	}
}

#endif
