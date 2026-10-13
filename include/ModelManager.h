//=============================================================================
//
// モデルマネージャー処理 [ModelManager.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _MODEL_MANAGER_H_// このマクロ定義がされていなかったら
#define _MODEL_MANAGER_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************


// 前方宣言
class CXModelResource;


//*****************************************************************************
// モデルマネージャークラス
//*****************************************************************************
class CModelManager
{
public:

    static CModelManager& GetInstance(void);

    std::shared_ptr<CXModelResource> Load(const std::string& filepath);

    void Release(void);

private:

    std::unordered_map<
        std::string,
        std::shared_ptr<CXModelResource>
    > m_Models;
};

#endif