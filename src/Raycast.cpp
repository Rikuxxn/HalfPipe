//=============================================================================
//
// レイキャスト処理 [Raycast.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "Raycast.h"
#include "Renderer.h"
#include "Manager.h"
#include "Camera.h"


//=============================================================================
// マウス位置からワールドレイを取得する処理
//=============================================================================
bool CRaycast::GetMouseRay(D3DXVECTOR3& outOrigin, D3DXVECTOR3& outDir)
{
    // マウス座標取得
    POINT pt;
    GetCursorPos(&pt);

    // クライアント座標に変換
    HWND hwnd = GetActiveWindow();
    ScreenToClient(hwnd, &pt);

    // デバイス・ビューポート
    CRenderer* renderer = CManager::Instance()->GetRenderer();
    LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

    D3DVIEWPORT9 vp;
    pDevice->GetViewport(&vp);

    //char buf[256];
    //sprintf_s(buf,
    //    "Viewport = %ld x %ld\n",
    //    vp.Width,
    //    vp.Height);

    //OutputDebugStringA(buf);

    D3DXMATRIX world;
    D3DXMatrixIdentity(&world);

    // カメラの取得
    auto pCamera = CManager::Instance()->GetCamera();

    const D3DXMATRIX& view = pCamera->GetViewMatrix();
    const D3DXMATRIX& proj = pCamera->GetProjMatrix();

    // Unprojectで near/far の2点をワールド空間に変換
    D3DXVECTOR3 nearPt((float)pt.x, (float)pt.y, 0.0f);
    D3DXVECTOR3 farPt((float)pt.x, (float)pt.y, 1.0f);

    D3DXVECTOR3 worldNear, worldFar;
    D3DXVec3Unproject(&worldNear, &nearPt, &vp, &proj, &view, &world);
    D3DXVec3Unproject(&worldFar, &farPt, &vp, &proj, &view, &world);

    // レイの出発点と方向
    outOrigin = worldNear;
    outDir = worldFar - worldNear;
    D3DXVec3Normalize(&outDir, &outDir);

    return true;
}
//=============================================================================
// レイとAABBの当たり判定
//=============================================================================
bool CRaycast::IntersectAABB(const D3DXVECTOR3& rayOrigin,
    const D3DXVECTOR3& rayDir,
    const D3DXVECTOR3& boxMin,
    const D3DXVECTOR3& boxMax,
    float& outDist)
{
    const float EPSILON = 1e-6f;
    float tMin = 0.0f;
    float tMax = FLT_MAX;

    for (int nCnt = 0; nCnt < AXIS; nCnt++)
    {
        float rayOrig = ((float*)&rayOrigin)[nCnt];
        float rayDirI = ((float*)&rayDir)[nCnt];
        float bMin = ((float*)&boxMin)[nCnt];
        float bMax = ((float*)&boxMax)[nCnt];

        if (fabsf(rayDirI) < EPSILON)
        {
            if (rayOrig < bMin || rayOrig > bMax)
            {
                return false;
            }
        }
        else
        {
            float invDir = 1.0f / rayDirI;
            float t1 = (bMin - rayOrig) * invDir;
            float t2 = (bMax - rayOrig) * invDir;

            if (t1 > t2)
            {
                std::swap(t1, t2);
            }

            tMin = std::max(tMin, t1);
            tMax = std::min(tMax, t2);

            if (tMin > tMax)
            {
                return false;
            }
        }
    }

    outDist = tMin;
    return true;
}
//=============================================================================
// レイとOBBの当たり判定
//=============================================================================
bool CRaycast::IntersectOBB(
    const D3DXVECTOR3& rayOrigin,
    const D3DXVECTOR3& rayDir,
    const D3DXMATRIX& worldMatrix,
    const D3DXVECTOR3& halfSize,
    float& outDistance
)
{
    // ワールド→ローカルへの逆行列を計算
    D3DXMATRIX invWorld;
    if (D3DXMatrixInverse(&invWorld, NULL, &worldMatrix) == NULL)
    {
        return false;
    }

    // レイをローカル空間に変換
    D3DXVECTOR3 localOrigin, localDir;
    D3DXVec3TransformCoord(&localOrigin, &rayOrigin, &invWorld);
    D3DXVec3TransformNormal(&localDir, &rayDir, &invWorld);
    D3DXVec3Normalize(&localDir, &localDir);

    // ローカル空間のAABBに対するレイ当たり判定
    // AABBのmin/maxを計算
    D3DXVECTOR3 boxMin = -halfSize;
    D3DXVECTOR3 boxMax = +halfSize;

    float tmin = -FLT_MAX;
    float tmax = FLT_MAX;

    // 各軸ごとに交差計算
    for (int nCnt = 0; nCnt < AXIS; nCnt++)
    {
        float origin = ((float*)&localOrigin)[nCnt];
        float dir = ((float*)&localDir)[nCnt];
        float min = ((float*)&boxMin)[nCnt];
        float max = ((float*)&boxMax)[nCnt];

        if (fabsf(dir) < 1e-6f)
        {
            if (origin < min || origin > max)
            {
                return false;
            }
        }
        else
        {
            float t1 = (min - origin) / dir;
            float t2 = (max - origin) / dir;

            if (t1 > t2)
            {
                std::swap(t1, t2);
            }
            tmin = std::max(tmin, t1);
            tmax = std::min(tmax, t2);

            if (tmin > tmax)
            {
                return false;
            }
        }
    }

    // 最小交差距離を返す

    if (tmin < 0.0f)
    {
        return false; // レイの始点より後ろの交差は無効
    }

    outDistance = tmin;
    return true;
}
