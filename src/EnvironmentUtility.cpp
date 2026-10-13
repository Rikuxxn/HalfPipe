//
// 環境ユーティリティ処理 [EnvironmentUtility.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// 名前空間 EnvironmentUtil
//*****************************************************************************
namespace EnvironmentUtil
{
    //=========================================================================
    // 太陽の方向を計算して返す
    //=========================================================================
    D3DXVECTOR3 GetSunDirection(float time)
    {
        float angle = time * D3DX_PI * 2.0f - D3DX_PI * 0.5f;

        D3DXVECTOR3 dir;

        dir.x = cosf(angle);
        dir.y = sinf(angle);
        dir.z = -0.2f;

        // 正規化
        D3DXVec3Normalize(&dir, &dir);

        return dir;
    }

    //=========================================================================
    // 月の方向を計算して返す
    //=========================================================================
    D3DXVECTOR3 GetMoonDirection(float time)
    {
        float angle = time * D3DX_PI * 2.0f + D3DX_PI * 0.5f;

        D3DXVECTOR3 dir;

        dir.x = cosf(angle);
        dir.y = sinf(angle);
        dir.z = 0.2f;

        // 正規化
        D3DXVec3Normalize(&dir, &dir);

        return dir;
    }
}