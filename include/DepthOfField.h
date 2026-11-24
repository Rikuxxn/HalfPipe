//=============================================================================
//
// 被写界深度処理 [DepthOfField.h]
// Author : SOICHIRO SASAKI
//
//=============================================================================
#ifndef _DOF_SHADER_H_// このマクロ定義がされていなかったら
#define _DOF_SHADER_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "ShaderResource.h"


//*****************************************************************************
// 被写界深度シェーダークラス
//*****************************************************************************
class CDoFShader : public CShaderResource
{
public:

    virtual void Apply(CGameObject* owner) override;

private:
	float m_fGaussStart;
	float m_fGaussEnd;
	LPDIRECT3DTEXTURE9 m_mainTexture;
	LPDIRECT3DTEXTURE9 m_gaussianTexture;
	LPDIRECT3DTEXTURE9 m_depthTexture;
};

#endif