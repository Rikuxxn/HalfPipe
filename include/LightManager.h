//=============================================================================
//
// ライトマネージャー処理 [LightManager.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _LIGHT_MANAGER_H_// このマクロ定義がされていなかったら
#define _LIGHT_MANAGER_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// ライトマネージャークラス
//*****************************************************************************
class CLightManager
{
public:
    static constexpr int MAX_LIGHTS = 8;

private:
    struct Slot
    {
        bool used = false;
        D3DLIGHT9 light{};
    };

    Slot m_Slots[MAX_LIGHTS];

public:
    static CLightManager* Instance(void);

    int  RegisterLight(const D3DLIGHT9& light);
    void FreeLight(int index);
    void RestoreLights(void);

    void UpdateLight(int index, const D3DLIGHT9& light);
    void EnableLight(int index, bool enable);
};

#endif
