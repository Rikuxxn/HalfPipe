//=============================================================================
//
// トリガーオブジェクトマネージャー処理 [TriggerObjectManager.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _TRIGGER_OBJECT_MANAGER_H_// このマクロ定義がされていなかったら
#define _TRIGGER_OBJECT_MANAGER_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "TriggerObject.h"

// 前方宣言
class CTriggerObjectEditor;

//*****************************************************************************
// トリガーオブジェクトマネージャークラス
//*****************************************************************************
class CTriggerObjectManager
{
public:
    CTriggerObjectManager();
    ~CTriggerObjectManager();

    HRESULT Init    (void);
    void    Reset   (void);
    void    Uninit  (void);
    void    Update  (void);

    // 配置
    CTriggerObject* CreateTrigger(CTriggerObject::TriggerObjectType type, const D3DXVECTOR3& pos);

    void RemoveTrigger(CTriggerObject* object);

    // 保存・読み込み
    void SaveToJson(const char* filename);
    void LoadFromJson(const char* filename);

    const std::vector<CTriggerObject*>& GetTriggerObjects(void) const
    {
        return m_TriggerObjects;
    }

    CTriggerObjectEditor* GetTriggerObjectEditor(void) { return m_pTriggerObjectEditor.get(); }

private:
private:
    std::vector<CTriggerObject*>            m_TriggerObjects;
    std::unique_ptr<CTriggerObjectEditor>   m_pTriggerObjectEditor;

};

#endif