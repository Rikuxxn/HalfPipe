//=============================================================================
//
// ライトマネージャー処理 [LightManager.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "LightManager.h"
#include "Manager.h"
#include "Renderer.h"


//=============================================================================
// インスタンス生成処理
//=============================================================================
CLightManager* CLightManager::Instance(void)
{
    static CLightManager instance;
    return &instance;
}

//=============================================================================
// ライトの登録
//=============================================================================
int CLightManager::RegisterLight(const D3DLIGHT9& light)
{
    auto device = CManager::Instance()->GetRenderer()->GetDevice();

    for (int i = 0; i < MAX_LIGHTS; i++)
    {
        if (!m_Slots[i].used)
        {
            m_Slots[i].used = true;
            m_Slots[i].light = light;

            device->SetLight(i, &light);
            device->LightEnable(i, TRUE);

            return i;
        }
    }

    return -1; // 空きなし
}

//=============================================================================
// 更新処理
//=============================================================================
void CLightManager::UpdateLight(int index, const D3DLIGHT9& light)
{
    if (index < 0 || index >= MAX_LIGHTS) return;
    if (!m_Slots[index].used) return;

    m_Slots[index].light = light;

    auto device = CManager::Instance()->GetRenderer()->GetDevice();
    device->SetLight(index, &light);
}

//=============================================================================
// ライトの解放
//=============================================================================
void CLightManager::FreeLight(int index)
{
    if (index < 0 || index >= MAX_LIGHTS) return;

    auto device = CManager::Instance()->GetRenderer()->GetDevice();

    device->LightEnable(index, FALSE);

    m_Slots[index].used = false;
}

//=============================================================================
// ライトの使用設定
//=============================================================================
void CLightManager::EnableLight(int index, bool enable)
{
    if (index < 0 || index >= MAX_LIGHTS) return;

    auto device = CManager::Instance()->GetRenderer()->GetDevice();

    device->LightEnable(index, enable);
}

//=============================================================================
// ライトの再設定
//=============================================================================
void CLightManager::RestoreLights(void)
{
    auto device = CManager::Instance()->GetRenderer()->GetDevice();

    for (int i = 0; i < MAX_LIGHTS; i++)
    {
        if (!m_Slots[i].used)
        {
            continue;
        }

        device->SetLight(i, &m_Slots[i].light);
        device->LightEnable(i, TRUE);
    }
}