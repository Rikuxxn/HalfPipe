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

	// 投擲数の設定
	void SetThrowNum(int num) { m_ThrowCount = num; }

	// 投擲
	void Throw(void);

private:
	static constexpr int MAX_DEF_THROW_NUM = 50;// 投げれる最大数

	D3DXVECTOR3 m_BallThrowOffset;				// ボールのオフセット
	int			m_ThrowCount;					// 投擲数
	float       m_BallThrowSpeed;				// ボールの速度
};

#endif