//=============================================================================
//
// デバッグマネージャー処理 [DebugManager.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "DebugManager.h"
#include "imguimaneger.h"
#include "Manager.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "TimeSystem.h"
#include "Raycast.h"
#include "Renderer.h"
#include "Camera.h"
//#include "ShadowMap.h"


namespace GuiParam
{
    constexpr float SPACING_Y = 10.0f;	// Yの余白
    constexpr float POS_X = 20.0f;	// Xの位置
    constexpr float POS_Y = 20.0f;	// Yの位置
    constexpr float SIZE_X = 420.0f;	// Xのサイズ
    constexpr float SIZE_Y = 500.0f;	// Yのサイズ
}

//=============================================================================
// 描画処理
//=============================================================================
void CDebugManager::Draw(void)
{
#ifdef _DEBUG

    // 名前空間GuiParamの使用
    using namespace GuiParam;

    // レンダラー取得
    auto renderer = CManager::Instance()->GetRenderer();

    ImGui::Begin("Debug Info");

    //---------------------------------
    // エンジン情報
    //---------------------------------

    ImGui::Text("FPS : %d", CManager::Instance()->GetTimeSystem()->GetFPS());

    ImGui::Dummy(ImVec2(0.0f, SPACING_Y)); // 空白を空ける

    ImGui::Text("DeltaTime : %.3f", CManager::Instance()->GetTimeSystem()->GetDeltaTime());

    ImGui::Separator();

    ImGui::Dummy(ImVec2(0.0f, SPACING_Y)); // 空白を空ける

    // ラベル
    ImGui::Text("Bloom Power"); ImGui::SameLine(120); // ラベルの位置ちょっと調整

    // ブルーム強さの取得
    float bloomPower = renderer->GetBloomPower();

    ImGui::SliderFloat("##BloomPowerSlider", &bloomPower, 0.0f, 1.0f, "%.1f");

    // ブルーム強さの設定
    renderer->SetBloomPower(bloomPower);

    ImGui::Separator();

    // シーンの取得
    auto scene = CManager::Instance()->GetSceneManager()->GetScene();

    //---------------------------------
    // プレイヤー
    //---------------------------------

    auto player = scene->FindGameObject("Player");

    if (player)
    {
        player->DrawDebugGUI();
    }

    //---------------------------------
    // カメラ
    //---------------------------------

    CCamera* pCamera = CManager::Instance()->GetCamera();

	if (pCamera)
	{
		pCamera->DrawDebugGUI();
	}

    //---------------------------------
    // ボール
    //---------------------------------

    auto ball = scene->FindGameObject("Ball");

    if (ball)
    {
        ball->DrawDebugGUI();
    }

    //// シャドウマップの取得
    //auto shadow = CManager::Instance()->GetRenderer()->GetShadowMap();

    //ImGui::Image(
    //    (ImTextureID)shadow->GetShadowTexture(),
    //    ImVec2(256, 256)
    //);

    ImGui::End();

#endif

}
