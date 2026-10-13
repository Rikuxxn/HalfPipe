//=============================================================================
//
// ライトコンポーネント処理 [LightComponent.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _LIGHT_COMPONENT_H_// このマクロ定義がされていなかったら
#define _LIGHT_COMPONENT_H_// 2重インクルード防止のマクロ定義


//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "Component.h"



//*****************************************************************************
// ライトコンポーネントクラス
//*****************************************************************************
class CLightComponent : public CComponent
{
private:

    D3DLIGHT9 m_Light;
    bool      m_Enable;
    int       m_LightIndex;

public:

    CLightComponent();
    ~CLightComponent();

    HRESULT Init    (void) override;
    void    Update  (void) override;

public:

    D3DLIGHT9&  GetLight    (void)       { return m_Light; }
    bool        IsEnable    (void) const { return m_Enable; }
    void        OnDestroy   (void);

    void SetType        (D3DLIGHTTYPE type) { m_Light.Type = type; }
    void SetEnable      (bool enable)       { m_Enable = enable; }
    void SetDiffuse     (const D3DXCOLOR& color);
    void SetAmbient     (const D3DXCOLOR& color);
    void SetSpecular    (const D3DXCOLOR& color);
    void SetDirection   (const D3DXVECTOR3& dir);
    void SetRange       (float range);
    void SetAttenuation (float a0, float a1, float a2);

    //---------------------------------
    // スポットライト
    //---------------------------------

    void SetTheta   (float theta);
    void SetPhi     (float phi);
    void SetFalloff (float falloff);
};

#endif
