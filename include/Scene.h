//=============================================================================
//
// シーン処理 [Scene.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _SCENE_H_// このマクロ定義がされていなかったら
#define _SCENE_H_// 2重インクルード防止のマクロ定義

// 前方宣言
class CGameObject;

//*****************************************************************************
// シーンクラス
//*****************************************************************************
class CScene
{
private:

    std::vector<std::unique_ptr<CGameObject>> m_Objects;

    // 遅延登録
    std::vector<std::unique_ptr<CGameObject>> m_AddQueue;

public:
    CScene();
    virtual ~CScene();

    enum class Mode
    {
        Title,
        Connect,
        Game,
        Result,
        Max
    };

    // オブジェクトの生成
    CGameObject* CreateObject(void);

    // オブジェクトの生成
    template<class T>
    T* CreateObject(void)
    {
        auto object = std::make_unique<T>();

        //---------------------------------
        // Scene設定
        //---------------------------------

        object->SetScene(this);

        T* ptr = object.get();

        // ここではまだ本体に入れない
        m_AddQueue.push_back(std::move(object));

        return ptr;
    }

    // オブジェクトを探す処理(タグ)
    CGameObject* FindGameObject(const std::string& tag);

    // 複数のオブジェクトを探す処理(タグ)
    std::vector<CGameObject*> FindGameObjects(const std::string& tag);

    // オブジェクトを探す処理(型)
    template<typename T>
    T* FindGameObject(void)
    {
        for (auto& obj : m_Objects)
        {
            if (auto p = dynamic_cast<T*>(obj.get()))
            {
                return p;
            }
        }

        return nullptr;
    }

    // 複数のオブジェクトを探す処理(型)
    template<typename T>
    std::vector<T*> FindGameObjects(void)
    {
        std::vector<T*> result;

        for (auto& obj : m_Objects)
        {
            if (auto p = dynamic_cast<T*>(obj.get()))
            {
                result.push_back(p);
            }
        }

        return result;
    }

    // オブジェクトを探す処理(型とタグ)
    template<typename T>
    T* FindGameObject(const std::string& tag)
    {
        for (auto& obj : m_Objects)
        {
            if (obj && obj->GetTag() == tag)
            {
                return dynamic_cast<T*>(obj.get());
            }
        }

        return nullptr;
    }

    // 複数オブジェクトを探す処理(型とタグ)
    template<typename T>
    std::vector<T*> FindGameObjects(const std::string& tag)
    {
        std::vector<T*> result;

        for (auto& obj : m_Objects)
        {
            if (obj && obj->GetGroupName() == tag)
            {
                if (auto p = dynamic_cast<T*>(obj.get()))
                {
                    result.push_back(p);
                }
            }
        }

        return result;
    }

    // ゲームオブジェクトの取得関数
    const std::vector<std::unique_ptr<CGameObject>>& GetObjects(void) const;

    virtual HRESULT Init    (void) { return S_OK; }
    virtual void    Uninit (void);
    virtual void    Update (void);
    virtual void    Draw   (void);
    void DrawShadow(void);

    static std::unique_ptr<CScene>Create(Mode mode);

    //=========================================================================
    // ライト生成
    //=========================================================================

    CGameObject* CreateDirectionalLight(
        const D3DXVECTOR3& direction,
        const D3DXCOLOR& color);

    CGameObject* CreatePointLight(
        const D3DXVECTOR3& position,
        const D3DXCOLOR& color,
        float range,
        float attenuation1);

    CGameObject* CreateSpotLight(
        const D3DXVECTOR3& position,
        const D3DXVECTOR3& direction,
        const D3DXCOLOR& color,
        float range,
        float theta,
        float phi);

    // デバイスリセット(ライト再設定)
    virtual void OnDeviceReset(void) {}

};

#endif