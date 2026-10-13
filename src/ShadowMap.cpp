//=============================================================================
//
// シャドウマップ処理 [ShadowMap.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "ShadowMap.h"
#include "RenderTarget.h"
#include "Manager.h"
#include "Renderer.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Player.h"
#include "TransformComponent.h"
#include "GameTime.h"
#include "EnvironmentUtility.h"

// 名前空間の使用
using namespace EnvironmentUtil;


//=============================================================================
// コンストラクタ
//=============================================================================
CShadowMap::CShadowMap() : 
    m_View          {},         // ライトビュー
    m_Projection    {},         // ライトプロジェクション
    m_LightDir      (INIT_VEC3) // ライト方向

{

}

//=============================================================================
// デストラクタ
//=============================================================================
CShadowMap::~CShadowMap()
{

}

//=============================================================================
// 初期化
//=============================================================================
HRESULT CShadowMap::Init(void)
{
    // レンダーターゲットの生成
    m_RenderTarget = std::make_unique<CRenderTarget>();

    // レンダーターゲット生成
    m_RenderTarget->Create(
        2048,
        2048,
        D3DFMT_A8R8G8B8,
        D3DFMT_D24S8);

	return S_OK;
}

//=============================================================================
// 開始
//=============================================================================
void CShadowMap::Begin(void)
{
    // 描画開始
    m_RenderTarget->Begin();

    D3DVIEWPORT9 vp{};

    vp.X = 0;
    vp.Y = 0;
    vp.Width = 2048;
    vp.Height = 2048;
    vp.MinZ = 0.0f;
    vp.MaxZ = 1.0f;

    // デバイスの取得
    auto device = CManager::Instance()->GetRenderer()->GetDevice();

    device->SetViewport(&vp);

    //---------------------------------
    // ゲーム時間
    //---------------------------------

    float time = CManager::Instance()->GetGameTime()->GetTimeOfDay();

    //-------------------------
    // ライトView
    //-------------------------

    // 太陽方向
    D3DXVECTOR3 sunDir = GetSunDirection(time);

    // 月方向
    D3DXVECTOR3 moonDir = GetMoonDirection(time);

    float sunHeight = sunDir.y;
    float moonHeight = moonDir.y;

    if (sunHeight > 0.05f)
    {
        // 太陽(光が来る方向)
        m_LightDir = -sunDir;
    }
    else if (moonHeight > 0.05f)
    {
        // 月(光が来る方向)
        m_LightDir = -moonDir;
    }
    else
    {
        /*m_LightDir = D3DXVECTOR3(0, -1, 0);*/
    }

    //---------------------------------
    // ライト位置
    //---------------------------------

    float distance = 800.0f;

    // プレイヤーの取得
    auto player = CManager::Instance()->GetSceneManager()->GetScene()->FindGameObject("Player");

    D3DXVECTOR3 center(0, 0, 0);

    if (player)
    {
        auto transform = player->GetComponent<CTransformComponent>();

        if (transform)
        {
            center = transform->Position;
        }
    }

    D3DXVECTOR3 lightPos = center - m_LightDir * distance;

    //---------------------------------
    // 注視点
    //---------------------------------

    D3DXVECTOR3 target = center;

    D3DXVECTOR3 up(0, 1, 0);

    if (fabs(m_LightDir.y) > 0.95f)
    {
        up = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
    }

    D3DXMatrixLookAtLH(
        &m_View,
        &lightPos,
        &target,
        &up);

    //-------------------------
    // Projection
    //-------------------------

    D3DXMatrixOrthoLH(
        &m_Projection,
        1200.0f,
        1200.0f,
        100.0f,
        3000.0f);
}

//=============================================================================
// 終了
//=============================================================================
void CShadowMap::End(void)
{
    m_RenderTarget->End();
}

//=============================================================================
// テクスチャの取得
//=============================================================================
IDirect3DTexture9* CShadowMap::GetShadowTexture(void) const
{
    return m_RenderTarget->GetTexture();
}

//=============================================================================
// デバイスロスト時
//=============================================================================
void CShadowMap::OnLostDevice(void)
{
    m_RenderTarget->Release();
}

//=============================================================================
// デバイスリセット時
//=============================================================================
void CShadowMap::OnResetDevice(void)
{
    Init();
}