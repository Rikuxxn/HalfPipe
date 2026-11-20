//=============================================================================
//
// コンポーネント基底処理 [Component.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _COMPONENT_H_// このマクロ定義がされていなかったら
#define _COMPONENT_H_// 2重インクルード防止のマクロ定義

// 前方宣言
class CGameObject;

//*****************************************************************************
// コンポーネント基底クラス
//*****************************************************************************
class CComponent
{
protected:
    CGameObject* m_pOwner;
    bool m_bActive;
    bool m_bUpdateEnabled;

public:
    CComponent()
    {
        m_pOwner = nullptr;
        m_bActive = true;
        m_bUpdateEnabled = true;
    }

    virtual ~CComponent() = default;

public:

    virtual HRESULT Init(void) { return S_OK; }
    virtual void    Uninit (void) {}
    virtual void    Update (void) {}
    virtual void    Draw   (void) {}

    void SetOwner           (CGameObject* pOwner)   { m_pOwner = pOwner; }
    void SetActive          (bool active)           { m_bActive = active; }
    void SetUpdateEnabled   (bool enable)           { m_bUpdateEnabled = enable; }

    CGameObject* GetOwner(void) const { return m_pOwner; }

    bool IsActive(void) const { return m_bActive; }
    bool IsUpdateEnabled(void) const { return m_bUpdateEnabled; }

    //--------------------------------------
    // Imguiのデバッグ描画
    //--------------------------------------
#ifdef _DEBUG
    virtual void DrawDebugGUI(void) {}
#endif

};

#endif