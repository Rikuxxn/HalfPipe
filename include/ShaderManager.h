//=============================================================================
//
// シェーダーマネージャー処理 [ShaderManager.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _SHADER_MANAGER_H_// このマクロ定義がされていなかったら
#define _SHADER_MANAGER_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "ShaderResource.h"

// 前方宣言
//class CShaderResource;


//*****************************************************************************
// シェーダーマネージャークラス
//*****************************************************************************
class CShaderManager
{
public:
    CShaderManager();
    ~CShaderManager();

    //std::shared_ptr<CShaderResource> Load(const std::string& path);

    // デバイスリセット時
    void OnLostDevice(void);
    void OnResetDevice(void);


    //=============================================================================
    // 読み込み処理
    //=============================================================================
    template<typename T = CShaderResource>
    std::shared_ptr<T> Load(const std::string& path)
    {
        auto it = m_Shaders.find(path);

        if (it != m_Shaders.end())
        {
            return std::dynamic_pointer_cast<T>(it->second);
        }

        auto shader = std::make_shared<T>();

        if (FAILED(shader->Load(path)))
        {
            return nullptr;
        }

        m_Shaders[path] = shader;

        return shader;
    }
private:

    std::unordered_map<
        std::string,
        std::shared_ptr<CShaderResource>>
        m_Shaders;
};

#endif