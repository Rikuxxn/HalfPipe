//=============================================================================
//
// シェーダーリソース処理 [ShaderResource.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _SHADER_RESOURCE_H_// このマクロ定義がされていなかったら
#define _SHADER_RESOURCE_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************

// 前方宣言
class CGameObject;

//*****************************************************************************
// シェーダーリソースクラス
//*****************************************************************************
class CShaderResource
{
public:
    CShaderResource();
    ~CShaderResource();

    HRESULT Load(const std::string& path);

    virtual void Apply(CGameObject* /*owner*/) {}

    ID3DXEffect* GetEffect(void) const { return m_pEffect.Get(); }

    // デバイスリセット時
    void OnLostDevice(void);
    void OnResetDevice(void);

private:

    ComPtr<ID3DXEffect> m_pEffect;
};

#endif