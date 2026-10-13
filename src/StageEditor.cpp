//=============================================================================
//
// ステージエディター処理 [StageEditor.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "StageEditor.h"
#include "Manager.h"
#include "Renderer.h"
#include "StageFactory.h"
#include "StageManager.h"
#include "SceneManager.h"
#include "Scene.h"
#include "LightManager.h"
#include "imguimaneger.h"
#include "FileDialogUtils.h"
#include "TransformComponent.h"
#include "RigidBodyComponent.h"
#include "XModelRenderComponent.h"
#include "Input.h"
#include "InputManager.h"
#include "imgui_internal.h"
#include "Raycast.h"
#include "XModelResource.h"
#include "DebugRenderer.h"
#include "ModelInstancingRenderer.h"
#include "GhostObjectComponent.h"
#include "MathUtility.h"
#include "TransformGizmo.h"


// 名前空間の使用
using namespace MathUtil;

namespace ThumbnailCamera
{
    const D3DXVECTOR3 EYE   { -120.0f, 100.0f, -120.0f };
    const D3DXVECTOR3 AT    { 0.0f, 0.0f, 0.0f };
    const D3DXVECTOR3 UP    { 0.0f, 1.0f, 0.0f };

    constexpr float FOV = 10.0f;
    constexpr float MIN_RENDER_DISTANCE = 1.0f;
    constexpr float MAX_RENDER_DISTANCE = 1000.0f;
}

//=============================================================================
// コンストラクタ
//=============================================================================
CStageEditor::CStageEditor() :
    m_pThumbnailRT              (nullptr),          // レンダーターゲット
    m_pThumbnailZ               (nullptr),          // サーフェス
    m_TransformGizmo            (nullptr),          // 移動ギズモ
    m_SelectedObject            (nullptr),          // 選択したブロック
    m_PrevSelectedObject        (nullptr),          // 前回の選択中のブロック 
    m_thumbWidth                (THUMB_WIDTH),      // サムネイルの幅
    m_thumbHeight               (THUMB_HEIGHT),     // サムネイルの高さ
    m_isDragging                (false),            // ドラッグ中か
    m_autoUpdateColliderSize    (true),             // スケールにコライダーを追従させるか
    m_prevSize                  (INIT_VEC3),        // 前回のサイズ
    m_prevDegRot                (INIT_VEC3)         // 前回の向き
{


}

//=============================================================================
// デストラクタ
//=============================================================================
CStageEditor::~CStageEditor()
{
	// サムネイルの破棄
	ReleaseThumbnailRenderTarget();
}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CStageEditor::Init(void)
{
    m_SelectedObject = nullptr;
    m_PrevSelectedObject = nullptr;

    // デバイスの取得
    auto device = CManager::Instance()->GetRenderer()->GetDevice();

    // サムレイル用レンダーターゲットの初期化
    InitThumbnailRenderTarget(device);

    // モデルリストの読み込み
    LoadModelList("data/model_list.json");

    // サムネイルの生成
    GenerateThumbnails();

    return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void CStageEditor::Uninit(void)
{
    m_SelectedObject = nullptr;
    m_PrevSelectedObject = nullptr;
    m_TransformGizmo = nullptr;
}

//=============================================================================
// サムネイルのレンダーターゲットの初期化
//=============================================================================
HRESULT CStageEditor::InitThumbnailRenderTarget(LPDIRECT3DDEVICE9 device)
{
    if (!device)
    {
        return E_FAIL;
    }

    // 念のため既存リソースを破棄
    ReleaseThumbnailRenderTarget();

    // サムネイル用レンダーターゲットテクスチャの作成
    if (FAILED(device->CreateTexture(
        (UINT)m_thumbWidth, (UINT)m_thumbHeight,
        1, D3DUSAGE_RENDERTARGET,
        D3DFMT_A8R8G8B8,
        D3DPOOL_DEFAULT,
        &m_pThumbnailRT, nullptr)))
    {
        return E_FAIL;
    }

    // サムネイル用深度ステンシルサーフェスの作成
    if (FAILED(device->CreateDepthStencilSurface(
        (UINT)m_thumbWidth, (UINT)m_thumbHeight,
        D3DFMT_D24S8,
        D3DMULTISAMPLE_NONE, 0, TRUE,
        &m_pThumbnailZ, nullptr)))
    {
        return E_FAIL;
    }

    return S_OK;
}

//=============================================================================
// サムネイルのレンダーターゲットの設定
//=============================================================================
ComPtr<IDirect3DTexture9> CStageEditor::RenderThumbnail(CStageObject* obj)
{
    if (!obj || !m_pThumbnailRT || !m_pThumbnailZ)
    {
        return nullptr;
    }

    // デバイスの取得
    auto pDevice = CManager::Instance()->GetRenderer()->GetDevice();

    if (!pDevice)
    {
        return nullptr;
    }

    // 名前空間ThumbnailCameraの使用
    using namespace ThumbnailCamera;

    // サムネイル描画用の新規テクスチャ作成
    IDirect3DTexture9* pTex = nullptr;
    if (FAILED(pDevice->CreateTexture(
        (UINT)m_thumbWidth, (UINT)m_thumbHeight, 1,
        D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8,
        D3DPOOL_DEFAULT, &pTex, nullptr)))
    {
        return nullptr;
    }

    // 現在のレンダーターゲットと深度バッファを保存
    LPDIRECT3DSURFACE9 pOldRT = nullptr;
    LPDIRECT3DSURFACE9 pOldZ = nullptr;
    LPDIRECT3DSURFACE9 pNewRT = nullptr;

    pDevice->GetRenderTarget(0, &pOldRT);
    pDevice->GetDepthStencilSurface(&pOldZ);
    pTex->GetSurfaceLevel(0, &pNewRT);

    // サムネイル用のレンダーターゲットに切り替え
    pDevice->SetRenderTarget(0, pNewRT);
    pDevice->SetDepthStencilSurface(m_pThumbnailZ);

    // クリア
    pDevice->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(50, 50, 50), 1.0f, 0);
    pDevice->BeginScene();

    // 固定カメラ
    D3DXMATRIX matView, matProj;
    D3DXMatrixLookAtLH(&matView, &EYE, &AT, &UP);

    // プロジェクションマトリックスの作成
    D3DXMatrixPerspectiveFovLH(&matProj,
        D3DXToRadian(FOV),							// 視野角
        m_thumbWidth / m_thumbHeight,				// アスペクト比
        MIN_RENDER_DISTANCE,						// 近クリップ面
        MAX_RENDER_DISTANCE);						// 遠クリップ面

    pDevice->SetTransform(D3DTS_VIEW, &matView);
    pDevice->SetTransform(D3DTS_PROJECTION, &matProj);

    D3DLIGHT9 light{};
    ZeroMemory(&light, sizeof(light));

    light.Type = D3DLIGHT_DIRECTIONAL;
    light.Diffuse = D3DXCOLOR(0.8f, 0.8f, 0.8f, 1.0f);
    light.Specular = D3DXCOLOR(0, 0, 0, 1.0f);

    const D3DXVECTOR3 dirs[] =
    {
        { 0,-1, 0},
        { 0, 1, 0},
        { 1, 0, 0},
        {-1, 0, 0},
        { 0, 0, 1},
        { 0, 0,-1},
    };

    for (int i = 0; i < 6; i++)
    {
        light.Direction = dirs[i];

        pDevice->SetLight(i, &light);
        pDevice->LightEnable(i, TRUE);
    }

    // モデルインスタンシングの取得
    auto inst = CManager::Instance()->GetModelInstancingRenderer();

    inst->Begin();

    // ビュー・プロジェクションの設定
    inst->SetViewProjection(matView, matProj);

    // サムネイル用オブジェクト描画
    obj->Draw();

    inst->EndMain();

    for (int i = 0; i < 6; i++)
    {
        pDevice->LightEnable(i, FALSE);
    }

    // ライトを復元
    CLightManager::Instance()->RestoreLights();

    // 描画終了
    pDevice->EndScene();

    // 元のレンダーターゲットと深度バッファに戻す
    pDevice->SetRenderTarget(0, pOldRT);
    pDevice->SetDepthStencilSurface(pOldZ);

    // 破棄
    if (pOldRT) pOldRT->Release();
    if (pOldZ)  pOldZ->Release();
    if (pNewRT) pNewRT->Release();

    return pTex;
}
//=============================================================================
// サムネイル用のモデル生成処理
//=============================================================================
void CStageEditor::GenerateThumbnails(void)
{
    for (auto& info : m_ObjectInfos)
    {
        // 古いサムネイルを解放
        if (info.Thumbnail)
        {
            info.Thumbnail->Release();
            info.Thumbnail = nullptr;
        }

        //---------------------------------
        // 一時オブジェクト生成
        //---------------------------------
        
        auto obj = CManager::Instance()->GetStageManager()->CreateObject(info.Type, INIT_VEC3, false);

        if (!obj)
        {
            continue;
        }

        // トランスフォームコンポーネントの取得
        auto transform = obj->GetComponent<CTransformComponent>();

        transform->Scale = { 0.1f, 0.1f, 0.1f };

        //---------------------------------
        // サムネイル作成
        //---------------------------------

        info.Thumbnail = RenderThumbnail(obj);

        //---------------------------------
        // 削除
        //---------------------------------

        obj->Destroy();
    }
}
//=============================================================================
// サムネイルテクスチャの取得
//=============================================================================
IDirect3DTexture9* CStageEditor::GetThumbnailTexture(size_t index)
{
    assert(index < m_ObjectInfos.size());
    return m_ObjectInfos[index].Thumbnail.Get();
}
//=============================================================================
// サムネイルの破棄
//=============================================================================
void CStageEditor::ReleaseThumbnailRenderTarget(void)
{
    // レンダーターゲットの破棄
    if (m_pThumbnailRT)
    {
        m_pThumbnailRT->Release();
        m_pThumbnailRT = nullptr;
    }

    if (m_pThumbnailZ)
    {
        m_pThumbnailZ->Release();
        m_pThumbnailZ = nullptr;
    }

    // サムネイルキャッシュも解放しておく
    for (auto& info : m_ObjectInfos)
    {
        if (info.Thumbnail)
        {
            info.Thumbnail->Release();
            info.Thumbnail = nullptr;
        }
    }
}

//=============================================================================
// 更新処理
//=============================================================================
void CStageEditor::Update(void)
{
    //---------------------------------
    // ImGui設定
    //---------------------------------

    // GUIスタイルの取得
    ImGuiStyle& style = ImGui::GetStyle();

    style.GrabRounding      = 10.0f;		// スライダーのつまみを丸く
    style.ScrollbarRounding = 10.0f;		// スクロールバーの角
    style.ChildRounding     = 10.0f;		// 子ウィンドウの角
    style.WindowRounding    = 10.0f;		// ウィンドウ全体の角

    // 場所
    CImGuiManager::Instance().SetPosImgui(ImVec2(1280.0f, 20.0f));

    // サイズ
    CImGuiManager::Instance().SetSizeImgui(ImVec2(420.0f, 500.0f));

    // 最初のGUI
    CImGuiManager::Instance().StartImgui("BlockInfo", CImGuiManager::IMGUITYPE_DEFOULT);

    // シーンの取得
    auto scene = CManager::Instance()->GetSceneManager()->GetScene();

    // ステージオブジェクトの取得
    auto objects = scene->FindGameObjects<CStageObject>("StageObject");

    // ブロック選択処理
    PickObject();

    // ブロックがない場合
    if (objects.empty())
    {
        m_SelectedObject = nullptr;
        ImGui::Text("No blocks placed yet.");
    }
    else
    {
        // 初期状態ではブロック0を選択
        if (!m_SelectedObject)
        {
            m_SelectedObject = objects[0];
        }

        ImGui::Text("Object Num : %d", (int)objects.size());
    
        ImGui::Dummy(ImVec2(0.0f, 10.0f)); // 空白を空ける

        int selectedIndex = 0;

        if (m_SelectedObject)
        {
            for (size_t i = 0; i < objects.size(); i++)
            {
                if (objects[i] == m_SelectedObject)
                {
                    selectedIndex = (int)i;
                    break;
                }
            }
        }

        // インデックス選択
        if (ImGui::SliderInt("Object Index", &selectedIndex, 0, (int)objects.size() - 1))
        {
            m_SelectedObject = objects[selectedIndex];
        }

        if (m_SelectedObject)
        {
            DrawInspector(m_SelectedObject);
        }

        ImGui::Dummy(ImVec2(0.0f, 10.0f)); // 空白を空ける

        if (m_TransformGizmo)
        {
            ImGui::Text("Gizmo Mode");

            // 移動
            if (ImGui::RadioButton("Move", m_TransformGizmo->GetMode() == CTransformGizmo::GizmoMode::Move))
            {
                m_TransformGizmo->SetMode(CTransformGizmo::GizmoMode::Move);
            }

            ImGui::SameLine();

            // 回転
            if (ImGui::RadioButton("Rotate", m_TransformGizmo->GetMode() == CTransformGizmo::GizmoMode::Rotate))
            {
                m_TransformGizmo->SetMode(CTransformGizmo::GizmoMode::Rotate);
            }

            ImGui::SameLine();

            // 拡縮
            if (ImGui::RadioButton("Scale", m_TransformGizmo->GetMode() == CTransformGizmo::GizmoMode::Scale))
            {
                m_TransformGizmo->SetMode(CTransformGizmo::GizmoMode::Scale);
            }

            ImGui::Dummy(ImVec2(0.0f, 10.0f));

            //==================================================
            // ギズモ更新
            //==================================================

            m_TransformGizmo->Update(m_SelectedObject);
        }

        //if (GetUpdateCollider() == false)
        //{
        //    // コライダーの調整処理
        //    UpdateCollider(m_selectedBlock);
        //}
        //else
        //{
        //    ImGui::Dummy(ImVec2(0.0f, 10.0f)); // 空白を空ける
        //    ImGui::Text("Auto Update Collider Size is Active");
        //}
    }

    ImGui::Dummy(ImVec2(0.0f, 10.0f)); // 空白を空ける

    // モデルリスト
    DrawModelList();

    ImGui::Dummy(ImVec2(0.0f, 10.0f)); // 空白を空ける

    // Save / Load ボタン
    DrawSaveLoad();

    ImGui::End();
}

//=============================================================================
// 移動ギズモ生成
//=============================================================================
void CStageEditor::CreateGizmo(void)
{
    if (m_TransformGizmo)
    {
        return;
    }

    //==================================================
    // ギズモ
    //==================================================

    // シーンの取得
    auto scene = CManager::Instance()->GetSceneManager()->GetScene();

    m_TransformGizmo = scene->CreateObject<CTransformGizmo>();

    if (!m_TransformGizmo)
    {
        return;
    }

    m_TransformGizmo->Init();
}

//=============================================================================
// モデルリスト
//=============================================================================
void CStageEditor::DrawModelList(void)
{
    // ブロックタイプ一覧
    if (ImGui::TreeNode("Block Types"))
    {
        ImGui::BeginChild("BlockTypeList", ImVec2(0, 500), true); // スクロール領域

        for (size_t i = 0; i < m_ObjectInfos.size(); i++)
        {
            auto& info = m_ObjectInfos[i];

            if (!info.Thumbnail)
            {
                continue;
            }

            ImGui::PushID((int)i);
            ImGui::Image(reinterpret_cast<ImTextureID>(info.Thumbnail.Get()), ImVec2(m_thumbWidth, m_thumbHeight));
            //ImGui::Text("%s", info.Name.c_str());

            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
            {
                m_isDragging = true;// ドラッグ中

                auto type = info.Type;

                ImGui::SetDragDropPayload("BLOCK_TYPE", &type, sizeof(type));
                ImGui::Text("%s", info.Name.c_str());
                ImGui::Image(reinterpret_cast<ImTextureID>(info.Thumbnail.Get()), ImVec2(m_thumbWidth, m_thumbHeight));
                //ImGui::Text("%s", info.Name.c_str());
                ImGui::EndDragDropSource();
            }

            // マウスの取得
            CInputMouse* pMouse = CManager::Instance()->GetInput()->GetMouse();

            if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) && m_isDragging)
            {
                // 現在のImGuiの内部状態（コンテキスト）へのポインターを取得
                ImGuiContext* ctx = ImGui::GetCurrentContext();

                if (ctx->DragDropPayload.IsDataType("BLOCK_TYPE"))
                {
                    // ドラッグ中を解除
                    m_isDragging = false;

                    CStageObject::StageObjectType type = *(CStageObject::StageObjectType*)ctx->DragDropPayload.Data;

                    D3DXVECTOR3 pos = pMouse->GetGroundHitPosition();
                    pos.y = 30.0f;

                    // ステージマネージャーの取得
                    auto stageManager = CManager::Instance()->GetStageManager();

                    // ステージオブジェクト生成
                    auto obj = stageManager->CreateObject(type, pos, true);

                    // 初期化
                    if (obj)
                    {
                        obj->Init();
                    }

                    m_SelectedObject = obj;
                    m_PrevSelectedObject = nullptr;    // 初回更新させる
                }
            }

            ImGui::Dummy(ImVec2(0.0f, 10.0f)); // 画像間の余白

            ImGui::PopID();
        }

        ImGui::EndChild();
        ImGui::TreePop();
    }
}

//=============================================================================
// インスペクタービュー
//=============================================================================
void CStageEditor::DrawInspector(CStageObject* selectedObject)
{
    if (selectedObject)
    {
        //// 選択中のブロックの色を変える
        //m_SelectedObject->SetSelected(true);

        // トランスフォームコンポーネントの取得
        auto transform = selectedObject->GetComponent<CTransformComponent>();

        if (!transform)
        {
            return;
        }

        // リジッドボディコンポーネントの取得
        auto rigid = selectedObject->GetComponent<CRigidBodyComponent>();

        if (!rigid)
        {
            ImGui::Text("No Rigidbody");
            return;
        }

        btScalar mass = rigid->GetMass();	// 選択中のブロックの質量の取得

        // ラジアン→角度に一時変換（静的変数で保持し操作中のみ更新）
        D3DXVECTOR3 degRot = D3DXToDegree(transform->Rotation);

        // 前回のサイズを保持
        D3DXVECTOR3 prevSize = transform->Scale;

        if (selectedObject != m_PrevSelectedObject)
        {
            m_prevSize = transform->Scale;
            m_prevDegRot = D3DXToDegree(transform->Rotation);
            m_PrevSelectedObject = m_SelectedObject;
        }

        bool isDynamic = rigid->IsDynamic();

        if (ImGui::Checkbox("is Dynamic", &isDynamic))
        {
            // true = Dynamic
            // false = Kinematic
            rigid->SetDynamic(isDynamic);
        }

        //*********************************************************************
        // POS の調整
        //*********************************************************************

        ImGui::Dummy(ImVec2(0.0f, 10.0f)); // 空白を空ける

        // ラベル
        ImGui::Text("POS"); ImGui::SameLine(60); // ラベルの位置ちょっと調整

        // X
        ImGui::Text("X:"); ImGui::SameLine();
        ImGui::SetNextItemWidth(80);
        ImGui::DragFloat("##Block_pos_x", &transform->Position.x, 1.0f, -9000.0f, 9000.0f, "%.1f");

        // Y
        ImGui::SameLine();
        ImGui::Text("Y:"); ImGui::SameLine();
        ImGui::SetNextItemWidth(80);
        ImGui::DragFloat("##Block_pos_y", &transform->Position.y, 1.0f, -9000.0f, 9000.0f, "%.1f");

        // Z
        ImGui::SameLine();
        ImGui::Text("Z:"); ImGui::SameLine();
        ImGui::SetNextItemWidth(80);
        ImGui::DragFloat("##Block_pos_z", &transform->Position.z, 1.0f, -9000.0f, 9000.0f, "%.1f");

        //*********************************************************************
        // ROT の調整
        //*********************************************************************

        ImGui::Dummy(ImVec2(0.0f, 10.0f));

        ImGui::Text("ROT"); ImGui::SameLine(60);

        ImGui::Text("X:"); ImGui::SameLine();
        ImGui::SetNextItemWidth(80);
        bool changedX = ImGui::DragFloat("##Block_rot_x", &degRot.x, 0.1f, -180.0f, 180.0f, "%.2f");

        ImGui::SameLine();
        ImGui::Text("Y:"); ImGui::SameLine();
        ImGui::SetNextItemWidth(80);
        bool changedY = ImGui::DragFloat("##Block_rot_y", &degRot.y, 0.1f, -180.0f, 180.0f, "%.2f");

        ImGui::SameLine();
        ImGui::Text("Z:"); ImGui::SameLine();
        ImGui::SetNextItemWidth(80);
        bool changedZ = ImGui::DragFloat("##Block_rot_z", &degRot.z, 0.1f, -180.0f, 180.0f, "%.2f");

        bool isRotChanged = changedX || changedY || changedZ;

        //*********************************************************************
        // SIZE の調整
        //*********************************************************************

        ImGui::Dummy(ImVec2(0.0f, 10.0f)); // 空白を空ける

        // ラベル
        ImGui::Text("SIZE"); ImGui::SameLine(60); // ラベルの位置ちょっと調整

        // X
        ImGui::Text("X:"); ImGui::SameLine();
        ImGui::SetNextItemWidth(80);
        ImGui::DragFloat("##Block_size_x", &transform->Scale.x, 0.1f, 0.1f, 100.0f, "%.1f");

        // Y
        ImGui::SameLine();
        ImGui::Text("Y:"); ImGui::SameLine();
        ImGui::SetNextItemWidth(80);
        ImGui::DragFloat("##Block_size_y", &transform->Scale.y, 0.1f, 0.1f, 100.0f, "%.1f");

        // Z
        ImGui::SameLine();
        ImGui::Text("Z:"); ImGui::SameLine();
        ImGui::SetNextItemWidth(80);
        ImGui::DragFloat("##Block_size_z", &transform->Scale.z, 0.1f, 0.1f, 100.0f, "%.1f");

        // チェックボックス：拡大率に応じて自動更新するか
        ImGui::Checkbox("Auto Update Collider Size", &m_autoUpdateColliderSize);

        // サイズ変更チェック
        bool isSizeChanged = (transform->Scale != m_prevSize);

        ImGui::Dummy(ImVec2(0.0f, 10.0f)); // 空白を空ける

        // 直接入力された0以下を補正
        transform->Scale.x = Max(transform->Scale.x, 0.1f);
        transform->Scale.y = Max(transform->Scale.y, 0.1f);
        transform->Scale.z = Max(transform->Scale.z, 0.1f);


        // ブロックの特殊処理
        selectedObject->DrawCustomUI();


        //*********************************************************************
        // 質量 の調整
        //*********************************************************************

        ImGui::Dummy(ImVec2(0.0f, 10.0f)); // 空白を空ける

        // ラベル
        ImGui::Text("MASS"); ImGui::SameLine(60); // ラベルの位置ちょっと調整

        // スライダー（範囲: 0.0f ～ 80.0f）

        float Mass = (float)mass;

        ImGui::SliderFloat("##MassSlider", &Mass, 0.0f, 80.0f, "%.2f");

        // 角度→ラジアンに戻す
        D3DXVECTOR3 rotRad = D3DXToRadian(degRot);

        // サイズ(拡大率)が変わったときだけ呼ぶ
        if (m_autoUpdateColliderSize == true && isSizeChanged)
        {
            if (rigid)
            {
                rigid->UpdateFromScale(transform->Scale);
            }

            m_prevSize = transform->Scale;
        }

        if (isRotChanged)
        {
            // 静的ブロックなら即物理Transformも更新して同期
            if (!isDynamic)
            {
                // 回転が変わった時だけセット
                transform->SetRotation(rotRad);
            }
        }
        else
        {
            degRot = D3DXToDegree(transform->Rotation);
        }

        //*********************************************************************
        // ブロックの削除
        //*********************************************************************

        ImGui::Dummy(ImVec2(0.0f, 10.0f));

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.1f, 0.1f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.6f, 0.0f, 0.0f, 1.0f));

        if (ImGui::Button("Delete"))
        {
            if (m_autoUpdateColliderSize && m_SelectedObject)
            {
                // シーンの取得
                auto scene = CManager::Instance()->GetSceneManager()->GetScene();

                // ステージオブジェクトの取得
                auto objects = scene->FindGameObjects<CStageObject>("StageObject");

                int index = -1;

                for (size_t i = 0; i < objects.size(); i++)
                {
                    if (objects[i] == m_SelectedObject)
                    {
                        index = (int)i;
                        break;
                    }
                }

                m_SelectedObject->Destroy();

                if (index + 1 < (int)objects.size())
                {
                    m_SelectedObject = objects[index + 1];
                }
                else if (index - 1 >= 0)
                {
                    m_SelectedObject = objects[index - 1];
                }
                else
                {
                    m_SelectedObject = nullptr;
                }
            }
            else
            {// m_autoUpdateColliderSizeがfalseの時は何もしない

            }
        }

        ImGui::PopStyleColor(3);
    }
}

//=============================================================================
// Save / Load ボタン
//=============================================================================
void CStageEditor::DrawSaveLoad(void)
{
    // ステージマネージャーの取得
    auto stage = CManager::Instance()->GetStageManager();

    if (ImGui::Button("Save"))
    {
        // ダイアログを開いてファイルに保存
        std::string path = OpenWindowsSaveFileDialog();

        if (!path.empty())
        {
            // データの保存
            stage->SaveToJson(path.c_str());
        }
    }

    ImGui::SameLine(0);

    if (ImGui::Button("Load"))
    {
        // ダイアログを開いてファイルを開く
        std::string path = OpenWindowsOpenFileDialog();

        if (!path.empty())
        {
            // データの読み込み
            stage->LoadFromJson(path.c_str());
        }
    }
}

//=============================================================================
// ブロック選択処理
//=============================================================================
void CStageEditor::PickObject(void)
{
    // ImGuiがマウスを使ってるなら選択処理をキャンセル
    if (ImGui::GetIO().WantCaptureMouse)
    {
        return;
    }

    // 左クリックのみ
    if (!CManager::Instance()->GetInput()->GetMouse()->GetTrigger(0))
    {
        return;
    }

    D3DXVECTOR3 origin;
    D3DXVECTOR3 dir;

    // マウスレイ取得
    CRaycast::GetMouseRay(origin, dir);

    //==================================================
    // ギズモをクリックしているか確認
    //==================================================

    if (m_TransformGizmo)
    {
        if (m_TransformGizmo->IsHit(origin, dir))
        {
            // ギズモをクリックした場合は、
            // StageObjectの選択処理を行わない
            return;
        }
    }

    btVector3 from(
        origin.x,
        origin.y,
        origin.z
    );

    btVector3 to(
        origin.x + dir.x * 10000.0f,
        origin.y + dir.y * 10000.0f,
        origin.z + dir.z * 10000.0f
    );

    // 物理ワールドの取得
    auto world = CManager::Instance()->GetPhysicsWorld();

    btCollisionWorld::ClosestRayResultCallback ray(from, to);

    world->rayTest(from, to, ray);

    if (!ray.hasHit())
    {
        m_SelectedObject = nullptr;
        return;
    }

    auto collisionObject = ray.m_collisionObject;

    if (!collisionObject)
    {
        m_SelectedObject = nullptr;
        return;
    }

    auto obj = static_cast<CGameObject*>(collisionObject->getUserPointer());

    if (!obj)
    {
        m_SelectedObject = nullptr;
        return;
    }

    auto stage = dynamic_cast<CStageObject*>(obj);

    if (stage)
    {
        m_SelectedObject = stage;

        OutputDebugStringA(
            (stage->GetTag() + "\n").c_str()
        );
    }
    else
    {
        m_SelectedObject = nullptr;
    }
}

//=============================================================================
// モデルリスト読み込み
//=============================================================================
void CStageEditor::LoadModelList(const char* filename)
{
    std::ifstream file(filename);

    // 開けなかったら飛ばす
    if (!file.is_open())
    {
        return;
    }

    json j;
    file >> j;

    for (auto& data : j)
    {
        StageObjectInfo info;

        info.Type = (CStageObject::StageObjectType)data["type"];
        info.Category = data["category"];
        info.Name = data["name"];
        if (data.contains("filepath"))
        {
            info.FilePath = data["filepath"];
        }

        m_ObjectInfos.push_back(std::move(info));
    }
}

//=============================================================================
// オブジェクト情報取得
//=============================================================================
const CStageEditor::StageObjectInfo* CStageEditor::FindObjectInfo(CStageObject::StageObjectType type) const
{
    for (const auto& info : m_ObjectInfos)
    {
        if (info.Type == type)
        {
            return &info;
        }
    }

    return nullptr;
}