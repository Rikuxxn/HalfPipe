//=============================================================================
//
// 投擲コンポーネント処理 [ThrowComponent.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _THROW_COMPONENT_H_// このマクロ定義がされていなかったら
#define _THROW_COMPONENT_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "Component.h"

//*****************************************************************************
// 投擲コンポーネントクラス
//*****************************************************************************
class CThrowComponent : public CComponent
{
public:
	CThrowComponent();
	~CThrowComponent() = default;

	HRESULT Init	(void) override;
	void	Update	(void) override;

	// 投擲
	void Throw(void);

private:
	D3DXVECTOR3 m_BallThrowOffset;  // ボールのオフセット
	float       m_BallThrowSpeed;   // ボールの速度
};

#endif