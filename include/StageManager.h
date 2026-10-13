//=============================================================================
//
// ステージマネージャー処理 [StageManager.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _STAGE_MANAGER_H_// このマクロ定義がされていなかったら
#define _STAGE_MANAGER_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "StageObject.h"

// 前方宣言
class CStageEditor;

//*****************************************************************************
// ステージマネージャークラス
//*****************************************************************************
class CStageManager
{
public:
    CStageManager();
    ~CStageManager();

    HRESULT Init(void);
    void Reset(void);
    void Uninit(void);
    void Update(void);

    // 配置
    CStageObject* CreateObject(CStageObject::StageObjectType type, const D3DXVECTOR3& pos, bool enablePhysics);

    // 保存・読み込み
    void SaveToJson(const char* filename);
    void LoadFromJson(const char* filename);

    // ステージエディタのサムネイルの破棄と再設定
    void ReleaseThumbnail(void);
    void ResetThumbnail(void);

    CStageEditor* GetStageEditor(void) { return m_pStageEditor.get(); }

private:
    std::unique_ptr<CStageEditor> m_pStageEditor;
};

#endif