//=============================================================================
//
// ゲームオブジェクト処理 [GameObject.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _GAME_OBJECT_H_// このマクロ定義がされていなかったら
#define _GAME_OBJECT_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "Tag.h"
#include "Component.h"

// 前方宣言
class CScene;

//*****************************************************************************
// ゲームオブジェクトクラス
//*****************************************************************************
class CGameObject
{
private:

    std::vector<std::unique_ptr<CComponent>> m_Components;

    bool        m_bActive;
    bool        m_bDestroy;
    bool        m_bUninitialized;

    std::string m_Tag;
    std::string m_Name;

    //---------------------------------
    // 所属シーン
    //---------------------------------

    CScene* m_pScene;

public:

    CGameObject()
    {
        m_bActive = true;
        m_bDestroy = false;
        m_bUninitialized = false;

        m_Tag = "GameObject";
        m_Name = "Object";

        m_pScene = nullptr;
    }

    virtual ~CGameObject() = default;

public:

    virtual HRESULT Init    (void) { return S_OK; }
    virtual void    Uninit  (void);
    virtual void    Update  (void);
    virtual void    Draw    (void);
    void DrawShadow(void);

    void SetScene(CScene* scene);
    CScene* GetScene(void) const;

    //--------------------------------------
    // Imguiのデバッグ描画
    //--------------------------------------

    virtual void DrawDebugGUI(void) {}

public:

    // 特定のコンポーネントの追加
    template<typename T, typename... Args>
    T* AddComponent(Args&&... args)
    {
        auto comp = std::make_unique<T>(std::forward<Args>(args)...);

        comp->SetOwner(this);

        T* ptr = comp.get();

        m_Components.emplace_back(std::move(comp));

        // 初期化
        ptr->Init();

        return ptr;
    }

    // 特定のコンポーネントの取得
    template<typename T>
    T* GetComponent(void)
    {
        for (auto& comp : m_Components)
        {
            if (auto ptr = dynamic_cast<T*>(comp.get()))
            {
                return ptr;
            }
        }

        return nullptr;
    }

    // 全てのコンポーネントの取得
    const std::vector<std::unique_ptr<CComponent>>& GetComponents(void) const;

    // 衝突判定
    virtual void OnCollisionEnter(CGameObject* /*other*/) {}
    virtual void OnCollisionStay(CGameObject* /*other*/) {}
    virtual void OnCollisionExit(CGameObject* /*other*/) {}

    // 接触判定
    virtual void OnTriggerEnter(CGameObject* /*other*/) {}
    virtual void OnTriggerStay(CGameObject* /*other*/) {}
    virtual void OnTriggerExit(CGameObject* /*other*/) {}

public:

    void Destroy(void) { m_bDestroy = true; }
    bool IsDestroy(void) const { return m_bDestroy; }

public:

    void SetActive(bool active) { m_bActive = active; }
    bool IsActive(void) const { return m_bActive; }

public:
    // タグを付けて、タグ検索
    void SetTag(const std::string& tag) { m_Tag = tag; }
    void SetGroupName(const std::string& name) { m_Name = name; }
    bool CompareTag(const std::string& tag) const { return m_Tag == tag; }
    const std::string& GetTag(void) const { return m_Tag; }
    const std::string& GetGroupName(void) const { return m_Name; }

};

#endif