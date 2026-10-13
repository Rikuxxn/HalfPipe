//=============================================================================
//
// テクスチャマネージャー処理 [TextureManager.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _TEXTURE_MANAGER_H_// このマクロ定義がされていなかったら
#define _TEXTURE_MANAGER_H_// 2重インクルード防止のマクロ定義

// 前方宣言
class CTextureResource;

//*****************************************************************************
// テクスチャマネージャークラス
//*****************************************************************************
class CTextureManager
{
private:
    std::unordered_map<std::string, std::shared_ptr<CTextureResource>> m_Textures;

public:
    std::shared_ptr<CTextureResource> Load(const std::string& path);
};

#endif