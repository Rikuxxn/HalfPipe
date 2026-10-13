//=============================================================================
//
// シーンマネージャー処理 [SceneManager.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _SCENE_MANAGER_H_// このマクロ定義がされていなかったら
#define _SCENE_MANAGER_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "Scene.h"


//*****************************************************************************
// シーンマネージャークラス
//*****************************************************************************
class CSceneManager
{
private:

    std::unique_ptr<CScene> m_CurrentScene;
    CScene::Mode            m_Mode;

public:
    CSceneManager();
    ~CSceneManager() {};

    HRESULT Init(CScene::Mode mode);
    void    Update (void);
    void    Draw   (void);
    void    DrawShadow(void);

    void            SetMode (CScene::Mode mode);
    CScene*         GetScene(void) const { return m_CurrentScene.get(); }
    CScene::Mode    GetMode (void) const { return m_Mode; }
};

#endif