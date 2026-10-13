//=============================================================================
//
// ポーズ項目コンポーネント処理 [PauseItemComponent.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _PAUSE_ITEM_COMPONENT_H_// このマクロ定義がされていなかったら
#define _PAUSE_ITEM_COMPONENT_H_// 2重インクルード防止のマクロ定義


//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "Component.h"

//*****************************************************************************
// ポーズ項目コンポーネントクラス
//*****************************************************************************
class CPauseItemComponent : public CComponent
{
public:
    CPauseItemComponent() = default;
    ~CPauseItemComponent() = default;

    bool Selected = false;

    std::function<void()> OnExecute;

public:

    virtual void Update(void) override;
};

#endif