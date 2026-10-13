//=============================================================================
//
// ポーズマネージャー処理 [PauseManager.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _PAUSE_MANAGER_H_// このマクロ定義がされていなかったら
#define _PAUSE_MANAGER_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "GameObject.h"

// 前方宣言
class CPauseItemComponent;
class CScene;
class CGame;

//*****************************************************************************
// ポーズマネージャークラス
//*****************************************************************************
class CPauseManager
{
public:
    CPauseManager();
    ~CPauseManager();

    HRESULT Init(CGame* owner);
    void Update(void);
    void Draw(void);
    void SetEnable(bool enable);

    bool IsEnable(void) const { return m_Enable; }

private:

    struct ITEM
    {
        CGameObject* Object;

        CPauseItemComponent* Component;
    };

    void CreateItem(const char* path, float x, float y, float w, float h, std::function<void()> func);

private:

    bool m_Enable = false;
    int m_SelectIndex = 0;
    std::vector<ITEM> m_Items;
    CGame* m_Owner = nullptr;
};

#endif

