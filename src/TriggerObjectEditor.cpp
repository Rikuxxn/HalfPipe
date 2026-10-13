//=============================================================================
//
// トリガーオブジェクトエディター処理 [TriggerObjectEditor.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "TriggerObjectEditor.h"
#include "TriggerObjectManager.h"
#include "Manager.h"
#include "imguimaneger.h"
#include "SceneManager.h"
#include "imgui_internal.h"
#include "Raycast.h"
#include "FileDialogUtils.h"
#include "Input.h"
#include "InputManager.h"
#include "TransformComponent.h"
#include "GhostObjectComponent.h"
#include "TransformGizmo.h"
#include "MathUtility.h"

// 名前空間の使用
using namespace MathUtil;


//=============================================================================
// コンストラクタ
//=============================================================================
CTriggerObjectEditor::CTriggerObjectEditor() :
    m_TransformGizmo    (nullptr),          // 移動ギズモ
    m_SelectedObject    (nullptr),          // 選択したブロック
    m_PrevSelectedObject(nullptr),          // 前回の選択中のブロック 
    m_prevSize          (INIT_VEC3),        // 前回のサイズ
    m_isDragging        (false)             // ドラッグ中か
{

}

//=============================================================================
// デストラクタ
//=============================================================================
CTriggerObjectEditor::~CTriggerObjectEditor()
{

}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CTriggerObjectEditor::Init(void)
{
    m_SelectedObject = nullptr;
    m_PrevSelectedObject = nullptr;

    // トリガーリストの読み込み
    LoadTriggerList("data/trigger_list.json");

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void CTriggerObjectEditor::Uninit(void)
{
    m_SelectedObject = nullptr;
    m_PrevSelectedObject = nullptr;
    m_TransformGizmo = nullptr;
}

//=============================================================================
// 更新処理
//=============================================================================
void CTriggerObjectEditor::Update(void)
{
    //---------------------------------
    // ImGui設定
    //---------------------------------

    // GUIスタイルの取得
    ImGuiStyle& style = ImGui::GetStyle();

    style.GrabRounding = 10.0f;		// スライダーのつまみを丸く
    style.ScrollbarRounding = 10.0f;		// スクロールバーの角
    style.ChildRounding = 10.0f;		// 子ウィンドウの角
    style.WindowRounding = 10.0f;		// ウィンドウ全体の角

    // 場所
    CImGuiManager::Instance().SetPosImgui(ImVec2(1280.0f, 530.0f));

    // サイズ
    CImGuiManager::Instance().SetSizeImgui(ImVec2(420.0f, 420.0f));

    // 最初のGUI
    CImGuiManager::Instance().StartImgui("TriggerInfo", CImGuiManager::IMGUITYPE_DEFOULT);

    // トリガーオブジェクトの取得
    const auto& objects = CManager::Instance()->GetTriggerObjectManager()->GetTriggerObjects();

    // トリガー選択処理
    PickTriggerObject();

    // トリガーがない場合
    if (objects.empty())
    {
        m_SelectedObject = nullptr;
        ImGui::Text("No trigger objects.");
    }
    else
    {
        // 初期状態ではブロック0を選択
        if (!m_SelectedObject)
        {
            m_SelectedObject = objects[0];
        }

        ImGui::Text("Trigger Num : %d", (int)objects.size());

        ImGui::Dummy(ImVec2(0.0f, 10.0f));

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

        if (ImGui::SliderInt("Trigger Index", &selectedIndex, 0, (int)objects.size() - 1))
        {
            m_SelectedObject = objects[selectedIndex];
        }

        if (m_SelectedObject)
        {
            DrawInspector(m_SelectedObject, objects);
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
    }

    ImGui::Dummy(ImVec2(0.0f, 10.0f));

    // トリガー一覧
    DrawTriggerList();

    ImGui::Dummy(ImVec2(0.0f, 10.0f));

    // Save / Load
    DrawSaveLoad();

    // imgui終了
    ImGui::End();
}

//=============================================================================
// 移動ギズモ生成
//=============================================================================
void CTriggerObjectEditor::CreateGizmo(void)
{
    if (m_TransformGizmo)
    {
        return;
    }

    //==================================================
    // 移動ギズモ
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
// インスペクタービュー
//=============================================================================
void CTriggerObjectEditor::DrawInspector(CTriggerObject* selectedObject, std::vector<CTriggerObject*> triggerObjects)
{
    if (!selectedObject)
    {
        return;
    }

    // トランスフォームコンポーネントの取得
    auto transform = selectedObject->GetComponent<CTransformComponent>();

    if (!transform)
    {
        return;
    }

    // ゴーストオブジェクトコンポーネントの取得
    auto ghost = selectedObject->GetComponent<CGhostObjectComponent>();

    // ラジアン→角度に一時変換（静的変数で保持し操作中のみ更新）
    D3DXVECTOR3 degRot = D3DXToDegree(transform->Rotation);

    if (selectedObject != m_PrevSelectedObject)
    {
        m_prevSize = transform->Scale;
        m_PrevSelectedObject = m_SelectedObject;
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

    // サイズ変更チェック
    bool isSizeChanged = (transform->Scale != m_prevSize);

    ImGui::Dummy(ImVec2(0.0f, 10.0f)); // 空白を空ける

    // 直接入力された0以下を補正
    transform->Scale.x = Max(transform->Scale.x, 0.1f);
    transform->Scale.y = Max(transform->Scale.y, 0.1f);
    transform->Scale.z = Max(transform->Scale.z, 0.1f);

    ////*********************************************************************
    //// トリガーの固有処理
    ////*********************************************************************
    //{
    //    selectedObject->DrawCustomUI();
    //}

    // 角度→ラジアンに戻す
    D3DXVECTOR3 rotRad = D3DXToRadian(degRot);

    // サイズ(拡大率)が変わったときだけ呼ぶ
    if (isSizeChanged)
    {
        if (ghost)
        {
            ghost->UpdateFromScale(transform->Scale);
        }

        m_prevSize = transform->Scale;
    }

    if (isRotChanged)
    {
        // 回転が変わった時だけセット
        transform->SetRotation(rotRad);
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
        if (m_SelectedObject)
        {
            int index = -1;

            for (size_t i = 0; i < triggerObjects.size(); i++)
            {
                if (triggerObjects[i] == m_SelectedObject)
                {
                    index = (int)i;
                    break;
                }
            }

            // 次に選択するオブジェクト
            CTriggerObject* nextSelected = nullptr;

            if (index + 1 < (int)triggerObjects.size())
            {
                nextSelected = triggerObjects[index + 1];
            }
            else if (index - 1 >= 0)
            {
                nextSelected = triggerObjects[index - 1];
            }

            // 管理リストから削除
            CManager::Instance()->GetTriggerObjectManager()->RemoveTrigger(m_SelectedObject);

            // 選択変更
            m_SelectedObject = nextSelected;
        }
    }

    ImGui::PopStyleColor(3);
}

//=============================================================================
// トリガーリストリスト
//=============================================================================
void CTriggerObjectEditor::DrawTriggerList(void)
{
    // トリガータイプ一覧
    if (ImGui::TreeNode("Trigger Types"))
    {
        ImGui::BeginChild("TriggerTypeList", ImVec2(0, 500), true); // スクロール領域

        for (size_t i = 0; i < m_ObjectInfos.size(); i++)
        {
            auto& info = m_ObjectInfos[i];


            ImGui::PushID((int)i);
            // 名前だけ表示
            ImGui::Selectable(info.Name.c_str());

            // ドラッグ開始
            if (ImGui::BeginDragDropSource())
            {
                m_isDragging = true;

                auto type = info.Type;

                ImGui::SetDragDropPayload("TRIGGER_TYPE", &type, sizeof(type));

                ImGui::Text("%s", info.Name.c_str());

                ImGui::EndDragDropSource();
            }

            ImGui::Dummy(ImVec2(0.0f, 10.0f)); // 画像間の余白

            ImGui::PopID();
        }

        ImGui::EndChild();
        ImGui::TreePop();
    }

    //---------------------------------
    // ドロップされたら生成
    //---------------------------------

    CInputMouse* pMouse = CManager::Instance()->GetInput()->GetMouse();

    if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) && m_isDragging)
    {
        ImGuiContext* ctx = ImGui::GetCurrentContext();

        if (ctx->DragDropPayload.IsDataType("TRIGGER_TYPE"))
        {
            m_isDragging = false;

            auto type = *(CTriggerObject::TriggerObjectType*)ctx->DragDropPayload.Data;

            D3DXVECTOR3 pos = pMouse->GetGroundHitPosition();
            pos.y = 30.0f;

            auto triggerManager = CManager::Instance()->GetTriggerObjectManager();
            auto obj = triggerManager->CreateTrigger(type, pos);

            m_SelectedObject = obj;
            m_PrevSelectedObject = nullptr;    // 初回更新させる
        }
    }
}

//=============================================================================
// Save / Load ボタン
//=============================================================================
void CTriggerObjectEditor::DrawSaveLoad(void)
{
    // トリガーオブジェクトマネージャーの取得
    auto trigger = CManager::Instance()->GetTriggerObjectManager();

    if (ImGui::Button("Save"))
    {
        // ダイアログを開いてファイルに保存
        std::string path = OpenWindowsSaveFileDialog();

        if (!path.empty())
        {
            // データの保存
            trigger->SaveToJson(path.c_str());
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
            trigger->LoadFromJson(path.c_str());
        }
    }
}

//=============================================================================
// トリガーオブジェクトの選択
//=============================================================================
void CTriggerObjectEditor::PickTriggerObject(void)
{
    // ImGui使用中は無効
    if (ImGui::GetIO().WantCaptureMouse)
    {
        return;
    }

    // 左クリック
    if (!CManager::Instance()->GetInput()->GetMouse()->GetTrigger(0))
    {
        return;
    }

    D3DXVECTOR3 origin;
    D3DXVECTOR3 dir;

    // マウスのレイ取得
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

    btVector3 from(origin.x, origin.y, origin.z);
    btVector3 to(
        origin.x + dir.x * 10000.0f,
        origin.y + dir.y * 10000.0f,
        origin.z + dir.z * 10000.0f);

    auto world = CManager::Instance()->GetPhysicsWorld();

    btCollisionWorld::ClosestRayResultCallback ray(from, to);

    world->rayTest(from, to, ray);

    if (!ray.hasHit())
    {
        m_SelectedObject = nullptr;
        return;
    }

    CGameObject* obj = static_cast<CGameObject*>(ray.m_collisionObject->getUserPointer());

    if (!obj)
    {
        m_SelectedObject = nullptr;
        return;
    }

    m_SelectedObject = dynamic_cast<CTriggerObject*>(obj);
}

//=============================================================================
// トリガーリスト読み込み
//=============================================================================
void CTriggerObjectEditor::LoadTriggerList(const char* filename)
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
        TriggerObjectInfo info;

        info.Type = (CTriggerObject::TriggerObjectType)data["type"];
        info.Name = data["name"];

        m_ObjectInfos.push_back(std::move(info));
    }
}

//=============================================================================
// トリガーオブジェクト情報取得
//=============================================================================
const CTriggerObjectEditor::TriggerObjectInfo* CTriggerObjectEditor::FindTriggerObjectInfo(CTriggerObject::TriggerObjectType type) const
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