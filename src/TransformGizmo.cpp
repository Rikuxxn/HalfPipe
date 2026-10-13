//=============================================================================
//
// ギズモ処理 [TransformGizmo.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "TransformGizmo.h"
#include "Manager.h"
#include "Renderer.h"
#include "TransformComponent.h"
#include "Scene.h"
#include "SceneManager.h"
#include "RenderComponent.h"
#include "XModelRenderComponent.h"
#include "StageObject.h"
#include "MathUtility.h"
#include "Camera.h"
#include "Raycast.h"
#include "imguimaneger.h"
#include "Input.h"
#include "InputManager.h"
#include "XModelResource.h"


// 名前空間の使用
using namespace MathUtil;


//=============================================================================
// コンストラクタ
//=============================================================================
CTransformGizmo::CTransformGizmo() :
    m_PrevSelectedObject    (nullptr),
    m_Mode                  (GizmoMode::Move),
    m_SelectedAxis          (Axis::None),
    m_DragStartPosition     (INIT_VEC3),
    m_DragStartRotation     (INIT_VEC3),
    m_DragStartScale        (INIT_VEC3),
    m_DragStartAxisParam    (0.0f),
    m_DragStartAngle        (0.0f),
    m_IsDragging            (false)
{

}

//=============================================================================
// デストラクタ
//=============================================================================
CTransformGizmo::~CTransformGizmo()
{

}

//=============================================================================
// 移動ギズモ生成処理
//=============================================================================
CGameObject* CTransformGizmo::CreateAxis(const char* filePath)
{
    // シーンの取得
    auto scene = CManager::Instance()->GetSceneManager()->GetScene();

    auto obj = scene->CreateObject<CGameObject>();

    if (!obj)
    {
        return nullptr;
    }

    auto transform  = obj->AddComponent<CTransformComponent>();
    auto model      = obj->AddComponent<CXModelRenderComponent>();

    if (!transform || !model)
    {
        return nullptr;
    }

    model->SetLayer(RenderLayer::Gizmo);

    // モデル読み込み
    if (FAILED(model->Load(filePath)))
    {
        return nullptr;
    }

    return obj;
}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CTransformGizmo::Init(void)
{
    //==================================================
    // 移動ギズモ生成
    //==================================================

    m_MoveGizmo.X       = CreateAxis("data/MODELS/Gizmo/MoveGizmo_X.x");
    m_MoveGizmo.Y       = CreateAxis("data/MODELS/Gizmo/MoveGizmo_Y.x");
    m_MoveGizmo.Z       = CreateAxis("data/MODELS/Gizmo/MoveGizmo_Z.x");
    m_MoveGizmo.Center  = CreateAxis("data/MODELS/Gizmo/Gizmo_Center.x");

    if (!m_MoveGizmo.X ||
        !m_MoveGizmo.Y ||
        !m_MoveGizmo.Z ||
        !m_MoveGizmo.Center)
    {
        return E_FAIL;
    }

    //==================================================
    // 回転ギズモ生成
    //==================================================

    m_RotateGizmo.X      = CreateAxis("data/MODELS/Gizmo/RotGizmo_X.x");
    m_RotateGizmo.Y      = CreateAxis("data/MODELS/Gizmo/RotGizmo_Y.x");
    m_RotateGizmo.Z      = CreateAxis("data/MODELS/Gizmo/RotGizmo_Z.x");
    m_RotateGizmo.Center = CreateAxis("data/MODELS/Gizmo/Gizmo_Center.x");

    if (!m_RotateGizmo.X ||
        !m_RotateGizmo.Y ||
        !m_RotateGizmo.Z ||
        !m_RotateGizmo.Center)
    {
        return E_FAIL;
    }

    //==================================================
    // スケールギズモ生成
    //==================================================

    m_ScaleGizmo.X      = CreateAxis("data/MODELS/Gizmo/ScaleGizmo_X.x");
    m_ScaleGizmo.Y      = CreateAxis("data/MODELS/Gizmo/ScaleGizmo_Y.x");
    m_ScaleGizmo.Z      = CreateAxis("data/MODELS/Gizmo/ScaleGizmo_Z.x");
    m_ScaleGizmo.Center = CreateAxis("data/MODELS/Gizmo/Gizmo_Center.x");

    if (!m_ScaleGizmo.X ||
        !m_ScaleGizmo.Y ||
        !m_ScaleGizmo.Z ||
        !m_ScaleGizmo.Center)
    {
        return E_FAIL;
    }

    //==================================================
    // 初期モード
    //==================================================

    SetGizmoVisible(m_MoveGizmo, true);
    SetGizmoVisible(m_RotateGizmo, false);
    SetGizmoVisible(m_ScaleGizmo, false);

	return S_OK;
}

//=============================================================================
// 更新処理
//=============================================================================
void CTransformGizmo::Update(CGameObject* selectedObject)
{
    //=========================================================
    // 選択状態が変化した場合だけ表示状態を変更
    //=========================================================
    if (selectedObject != m_PrevSelectedObject)
    {
        // 一旦すべて非表示
        SetGizmoVisible(m_MoveGizmo, false);
        SetGizmoVisible(m_RotateGizmo, false);
        SetGizmoVisible(m_ScaleGizmo, false);

        // 選択オブジェクトがある場合
        if (selectedObject)
        {
            switch (m_Mode)
            {
            case GizmoMode::Move:
                SetGizmoVisible(m_MoveGizmo, true);
                break;

            case GizmoMode::Rotate:
                SetGizmoVisible(m_RotateGizmo, true);
                break;

            case GizmoMode::Scale:
                SetGizmoVisible(m_ScaleGizmo, true);
                break;
            }
        }
        else
        {
            EndDrag();
        }

        m_PrevSelectedObject = selectedObject;
    }

    // 選択オブジェクトが無い
    if (!selectedObject)
    {
        return;
    }

    // ギズモの位置・大きさ
    UpdateGizmoTransform(selectedObject);

    //=========================================================================
    // マウスレイ
    //=========================================================================

    D3DXVECTOR3 rayOrigin;
    D3DXVECTOR3 rayDir;

    CRaycast::GetMouseRay(rayOrigin, rayDir);

    auto mouse = CManager::Instance()->GetInput()->GetMouse();

    //=========================================================================
    // ドラッグ開始
    //=========================================================================

    if (!m_IsDragging)
    {
        if (mouse->GetTrigger(0))
        {
            Axis axis = PickAxis(rayOrigin, rayDir);

            if (axis != Axis::None)
            {
                BeginDrag(selectedObject, axis, rayOrigin, rayDir);
            }
        }

        return;
    }

    //=========================================================================
    // ドラッグ終了
    //=========================================================================

    if (!mouse->GetPress(0))
    {
        EndDrag();
        return;
    }

    //---------------------------------------------------------
    // モード別処理
    //---------------------------------------------------------

    switch (m_Mode)
    {
    case GizmoMode::Move:
        UpdateMove(selectedObject, rayOrigin, rayDir);
        break;

    case GizmoMode::Rotate:
        UpdateRotate(selectedObject, rayOrigin, rayDir);
        break;

    case GizmoMode::Scale:
        UpdateScale(selectedObject, rayOrigin, rayDir);
        break;
    }
}

//=============================================================================
// ギズモモード設定
//=============================================================================
void CTransformGizmo::SetMode(GizmoMode mode)
{
    if (m_Mode == mode)
    {
        return;
    }

    // モード変更時にドラッグを解除
    CancelDrag();

    m_Mode = mode;

    // 全部非表示
    SetGizmoVisible(m_MoveGizmo, false);
    SetGizmoVisible(m_RotateGizmo, false);
    SetGizmoVisible(m_ScaleGizmo, false);

    // 現在のモードだけ表示
    switch (m_Mode)
    {
    case GizmoMode::Move:
        SetGizmoVisible(m_MoveGizmo, true);
        break;

    case GizmoMode::Rotate:
        SetGizmoVisible(m_RotateGizmo, true);
        break;

    case GizmoMode::Scale:
        SetGizmoVisible(m_ScaleGizmo, true);
        break;
    }
}

//=============================================================================
// ギズモの表示・非表示設定
//=============================================================================
void CTransformGizmo::SetGizmoVisible(GizmoObjects& gizmo, bool visible)
{
    auto SetVisible = [visible](CGameObject* object)
    {
        if (!object)
        {
            return;
        }

        // Xモデルレンダーコンポーネントの取得
        auto render = object->GetComponent<CXModelRenderComponent>();

        if (!render)
        {
            return;
        }

        render->SetVisible(visible);
    };

    SetVisible(gizmo.X);
    SetVisible(gizmo.Y);
    SetVisible(gizmo.Z);
    SetVisible(gizmo.Center);
}

//=============================================================================
// ギズモ共通処理
//=============================================================================
void CTransformGizmo::UpdateGizmoTransform(CGameObject* selectedObject)
{
    auto targetTransform = selectedObject->GetComponent<CTransformComponent>();

    if (!targetTransform)
    {
        CancelDrag();
        return;
    }

    // 現在のギズモ取得
    GizmoObjects* gizmo = GetCurrentGizmo();

    if (!gizmo)
    {
        return;
    }

    if (!gizmo->X ||
        !gizmo->Y ||
        !gizmo->Z ||
        !gizmo->Center)
    {
        return;
    }

    D3DXVECTOR3 pos = targetTransform->Position;

    // カメラの取得
    auto camera = CManager::Instance()->GetCamera();

    if (!camera)
    {
        return;
    }

    //=========================================================================
    // 選択オブジェクトがある場合
    //=========================================================================

    // 選択オブジェクトの位置を保存
    pos = targetTransform->Position;

    //=========================================================================
    // ギズモの位置
    //=========================================================================

    auto xTransform = gizmo->X->GetComponent<CTransformComponent>();
    auto yTransform = gizmo->Y->GetComponent<CTransformComponent>();
    auto zTransform = gizmo->Z->GetComponent<CTransformComponent>();
    auto cTransform = gizmo->Center->GetComponent<CTransformComponent>();

    if (xTransform)
    {
        xTransform->Position = pos;
    }

    if (yTransform)
    {
        yTransform->Position = pos;
    }

    if (zTransform)
    {
        zTransform->Position = pos;
    }

    if (cTransform)
    {
        cTransform->Position = pos;
    }

    //=========================================================================
    // ギズモの回転
    //=========================================================================
    if (m_Mode == GizmoMode::Rotate)
    {
        if (xTransform)
        {
            xTransform->Rotation = targetTransform->Rotation;
        }

        if (yTransform)
        {
            yTransform->Rotation = targetTransform->Rotation;
        }

        if (zTransform)
        {
            zTransform->Rotation = targetTransform->Rotation;
        }
    }

    //=========================================================================
    // カメラとの距離
    //=========================================================================

    D3DXVECTOR3 cameraPos = camera->GetPosV();

    D3DXVECTOR3 diff = pos - cameraPos;

    float distance = D3DXVec3Length(&diff);

    //=========================================================================
    // 距離に応じてギズモを拡大
    //=========================================================================

    float scale = distance / 200.0f;

    // 小さくなりすぎないようにする
    scale = Max(scale, 0.1f);

    D3DXVECTOR3 gizmoScale(scale, scale, scale);

    //=========================================================================
    // ギズモのスケール
    //=========================================================================

    if (xTransform)
    {
        xTransform->Scale = gizmoScale;
    }

    if (yTransform)
    {
        yTransform->Scale = gizmoScale;
    }

    if (zTransform)
    {
        zTransform->Scale = gizmoScale;
    }

    if (cTransform)
    {
        cTransform->Scale = gizmoScale;
    }
}

//=============================================================================
// 位置更新
//=============================================================================
void CTransformGizmo::UpdateMove(
    CGameObject* selectedObject,
    const D3DXVECTOR3& rayOrigin,
    const D3DXVECTOR3& rayDir)
{
    auto transform = selectedObject->GetComponent<CTransformComponent>();

    if (!transform)
    {
        return;
    }

    D3DXVECTOR3 axisDir = GetAxisDirection(m_SelectedAxis);

    float currentAxisParam =
        CalculateAxisParameter(
            rayOrigin,
            rayDir,
            m_DragStartPosition,
            axisDir);

    float movement = currentAxisParam - m_DragStartAxisParam;

    // 軸方向に移動
    transform->Position = m_DragStartPosition + axisDir * movement;
}

//=============================================================================
// 回転更新
//=============================================================================
void CTransformGizmo::UpdateRotate(
    CGameObject* selectedObject,
    const D3DXVECTOR3& rayOrigin,
    const D3DXVECTOR3& rayDir)
{
    if (!selectedObject)
    {
        return;
    }

    auto transform = selectedObject->GetComponent<CTransformComponent>();

    if (!transform)
    {
        return;
    }

    // ローカル軸
    D3DXVECTOR3 localAxis = GetAxisDirection(m_SelectedAxis);

    //=========================================================
   // 現在のオブジェクトの回転
   //=========================================================
    D3DXMATRIX matRot;

    D3DXMatrixRotationYawPitchRoll(
        &matRot,
        transform->Rotation.y,
        transform->Rotation.x,
        transform->Rotation.z);

    //=========================================================
    // ローカル軸 → ワールド軸
    //=========================================================
    D3DXVECTOR3 axisDir;

    D3DXVec3TransformNormal(&axisDir, &localAxis, &matRot);

    D3DXVec3Normalize(&axisDir, &axisDir);

    // 現在のマウス位置から回転角度を取得
    float currentAngle = CalculateRotationAngle(
            rayOrigin,
            rayDir,
            m_DragStartPosition,
            axisDir);

    float movement = currentAngle - m_DragStartAngle;

    // 回転速度
    const float rotateSensitivity = 0.8f;

    movement *= rotateSensitivity;

    // 軸方向に回転
    D3DXVECTOR3 rotation = m_DragStartRotation + localAxis * movement;

    transform->SetRotation(rotation);
}

//=============================================================================
// スケール更新
//=============================================================================
void CTransformGizmo::UpdateScale(
    CGameObject* selectedObject,
    const D3DXVECTOR3& rayOrigin,
    const D3DXVECTOR3& rayDir)
{
    if (!selectedObject)
    {
        return;
    }

    auto transform = selectedObject->GetComponent<CTransformComponent>();

    if (!transform)
    {
        return;
    }

    //---------------------------------------------------------
    // 軸方向
    //---------------------------------------------------------

    D3DXVECTOR3 axisDir = GetAxisDirection(m_SelectedAxis);

    //---------------------------------------------------------
    // 現在の軸パラメータ
    //---------------------------------------------------------

    float currentAxisParam =
        CalculateAxisParameter(
            rayOrigin,
            rayDir,
            m_DragStartPosition,
            axisDir);

    //---------------------------------------------------------
    // ドラッグ開始位置からの移動量
    //---------------------------------------------------------

    float movement = currentAxisParam - m_DragStartAxisParam;

    //---------------------------------------------------------
    // スケール変化量
    //---------------------------------------------------------

    const float scaleSensitivity = 0.02f;

    float scaleDelta = movement * scaleSensitivity;

    //---------------------------------------------------------
    // 開始時スケールから計算
    //---------------------------------------------------------

    D3DXVECTOR3 scale = m_DragStartScale;

    switch (m_SelectedAxis)
    {
    case Axis::X:
        scale.x += scaleDelta;
        break;

    case Axis::Y:
        scale.y += scaleDelta;
        break;

    case Axis::Z:
        scale.z += scaleDelta;
        break;

    case Axis::Center:
    {
        scale.x += scaleDelta;
        scale.y += scaleDelta;
        scale.z += scaleDelta;
        break;
    }

    default:
        return;
    }

    //---------------------------------------------------------
    // 最小値
    //---------------------------------------------------------

    const float minScale = 0.1f;

    scale.x = Max(scale.x, minScale);
    scale.y = Max(scale.y, minScale);
    scale.z = Max(scale.z, minScale);

    //---------------------------------------------------------
    // 適用
    //---------------------------------------------------------

    transform->Scale = scale;
}

//=============================================================================
// 現在のギズモ取得
//=============================================================================
CTransformGizmo::GizmoObjects* CTransformGizmo::GetCurrentGizmo(void)
{
    switch (m_Mode)
    {
    case GizmoMode::Move:
        return &m_MoveGizmo;

    case GizmoMode::Rotate:
        return &m_RotateGizmo;

    case GizmoMode::Scale:
        return &m_ScaleGizmo;
    }

    return nullptr;
}

//=============================================================================
// ドラッグ開始
//=============================================================================
void CTransformGizmo::BeginDrag(
    CGameObject* selectedObject,
    Axis axis,
    const D3DXVECTOR3& rayOrigin,
    const D3DXVECTOR3& rayDir)
{
    auto transform = selectedObject->GetComponent<CTransformComponent>();

    if (!transform)
    {
        return;
    }

    m_SelectedAxis = axis;
    m_IsDragging = true;

    //---------------------------------------------------------
    // 開始時Transformを保存
    //---------------------------------------------------------

    m_DragStartPosition = transform->Position;
    m_DragStartRotation = transform->Rotation;
    m_DragStartScale    = transform->Scale;

    //---------------------------------------------------------
    // 移動用パラメータ
    //---------------------------------------------------------

    // ローカル軸
    D3DXVECTOR3 localAxis = GetAxisDirection(axis);

    //=========================================================
    // 現在の回転行列
    //=========================================================
    D3DXMATRIX matRot;

    D3DXMatrixRotationYawPitchRoll(
        &matRot,
        transform->Rotation.y,
        transform->Rotation.x,
        transform->Rotation.z);

    //=========================================================
    // ローカル軸 → ワールド軸
    //=========================================================
    D3DXVECTOR3 axisDir;

    D3DXVec3TransformNormal(&axisDir, &localAxis, &matRot);

    D3DXVec3Normalize(&axisDir, &axisDir);

    m_DragStartAxisParam = CalculateAxisParameter(
            rayOrigin,
            rayDir,
            m_DragStartPosition,
            axisDir);

    //---------------------------------------------------------
    // 回転用はここで初期角度を計算する
    //---------------------------------------------------------
    if (m_Mode == GizmoMode::Rotate)
    {
        m_DragStartAngle = CalculateRotationAngle(
                rayOrigin,
                rayDir,
                m_DragStartPosition,
                axisDir);
    }
}

//=============================================================================
// ドラッグ終了
//=============================================================================
void CTransformGizmo::EndDrag(void)
{
    m_IsDragging = false;
    m_SelectedAxis = Axis::None;
}

//=============================================================================
// ドラッグキャンセル時(非選択になった時)
//=============================================================================
void CTransformGizmo::CancelDrag(void)
{
    m_IsDragging    = false;
    m_SelectedAxis  = Axis::None;

    m_DragStartPosition = INIT_VEC3;
    m_DragStartRotation = INIT_VEC3;
    m_DragStartScale    = INIT_VEC3;

    m_DragStartAxisParam    = 0.0f;
    m_DragStartAngle        = 0.0f;
}

//=============================================================================
// 軸方向取得
//=============================================================================
D3DXVECTOR3 CTransformGizmo::GetAxisDirection(Axis axis)
{
    switch (axis)
    {
    case Axis::X:
        return D3DXVECTOR3(1.0f, 0.0f, 0.0f);

    case Axis::Y:
        return D3DXVECTOR3(0.0f, 1.0f, 0.0f);

    case Axis::Z:
        return D3DXVECTOR3(0.0f, 0.0f, 1.0f);

    //case Axis::Center:
    //    return D3DXVECTOR3(1.0f, 1.0f, 1.0f);

    default:
        return INIT_VEC3;
    }
}

//=============================================================================
// ギズモのヒット判定
//=============================================================================
bool CTransformGizmo::IsHit(const D3DXVECTOR3& rayOrigin, const D3DXVECTOR3& rayDir)
{
    return PickAxis(rayOrigin, rayDir) != Axis::None;
}

//=============================================================================
// 選択処理
//=============================================================================
CTransformGizmo::Axis CTransformGizmo::PickAxis(const D3DXVECTOR3& rayOrigin, const D3DXVECTOR3& rayDir)
{
    Axis hitAxis = Axis::None;
    float closestDistance = FLT_MAX;

    // 現在のギズモ取得
    GizmoObjects* gizmo = GetCurrentGizmo();

    if (!gizmo)
    {
        return Axis::None;
    }

    CheckAxis(
        gizmo->X,
        Axis::X,
        rayOrigin,
        rayDir,
        closestDistance,
        hitAxis);

    CheckAxis(
        gizmo->Y,
        Axis::Y,
        rayOrigin,
        rayDir,
        closestDistance,
        hitAxis);

    CheckAxis(
        gizmo->Z,
        Axis::Z,
        rayOrigin,
        rayDir,
        closestDistance,
        hitAxis);

    CheckAxis(
        gizmo->Center,
        Axis::Center,
        rayOrigin,
        rayDir,
        closestDistance,
        hitAxis);

    return hitAxis;
}

//=============================================================================
// 軸判定処理
//=============================================================================
void CTransformGizmo::CheckAxis(
    CGameObject* axisObject,
    Axis axis,
    const D3DXVECTOR3& rayOrigin,
    const D3DXVECTOR3& rayDir,
    float& closestDistance,
    Axis& hitAxis)
{
    if (!axisObject)
    {
        return;
    }

    auto transform = axisObject->GetComponent<CTransformComponent>();

    auto model = axisObject->GetComponent<CXModelRenderComponent>();

    if (!transform || !model)
    {
        return;
    }

    auto resource = model->GetModel();

    if (!resource)
    {
        return;
    }

    auto mesh = resource->GetMesh();

    if (!mesh)
    {
        return;
    }

    // モデルのワールド行列
    D3DXMATRIX world = transform->GetWorldMatrix();

    // Rayをモデルローカル空間へ変換
    D3DXMATRIX invWorld;

    if (D3DXMatrixInverse(&invWorld, nullptr, &world) == nullptr)
    {
        OutputDebugStringA(" D3DXMatrixInverse FAILED\n");
        return;
    }

    D3DXVECTOR3 localOrigin;
    D3DXVECTOR3 localDir;

    D3DXVec3TransformCoord(
        &localOrigin,
        &rayOrigin,
        &invWorld);

    D3DXVec3TransformNormal(
        &localDir,
        &rayDir,
        &invWorld);

    D3DXVec3Normalize(
        &localDir,
        &localDir);

    BOOL hit = FALSE;

    FLOAT distance = 0.0f;

    if (FAILED(D3DXIntersect(
        mesh,
        &localOrigin,
        &localDir,
        &hit,
        nullptr,
        nullptr,
        nullptr,
        &distance,
        nullptr,
        nullptr)))
    {
        return;
    }

    if (!hit)
    {
        return;
    }

    if (distance < closestDistance)
    {
        closestDistance = distance;
        hitAxis = axis;
    }
}

//=============================================================================
// マウスレイと軸の距離を求める
//=============================================================================
float CTransformGizmo::CalculateAxisParameter(
    const D3DXVECTOR3& rayOrigin,
    const D3DXVECTOR3& rayDir,
    const D3DXVECTOR3& axisOrigin,
    const D3DXVECTOR3& axisDir)
{
    D3DXVECTOR3 w = rayOrigin - axisOrigin;

    float a = D3DXVec3Dot(&rayDir, &rayDir);
    float b = D3DXVec3Dot(&rayDir, &axisDir);
    float c = D3DXVec3Dot(&axisDir, &axisDir);
    float d = D3DXVec3Dot(&rayDir, &w);
    float e = D3DXVec3Dot(&axisDir, &w);

    float denominator = a * c - b * b;

    // レイと軸がほぼ平行
    if (fabsf(denominator) < 0.0001f)
    {
        // とりあえず軸方向への射影を使う
        return e;
    }

    // 軸上のパラメータ
    float s = (a * e - b * d) / denominator;

    return s;
}

//=============================================================================
// 回転角度を求める
//=============================================================================
float CTransformGizmo::CalculateRotationAngle(
    const D3DXVECTOR3& rayOrigin,
    const D3DXVECTOR3& rayDir,
    const D3DXVECTOR3& center,
    const D3DXVECTOR3& axisDir)
{
    // 回転軸に垂直な平面との交点を求める

    float denominator = D3DXVec3Dot(&rayDir, &axisDir);

    // レイと回転平面がほぼ平行
    if (fabsf(denominator) < 0.0001f)
    {
        return m_DragStartAngle;
    }

    D3DXVECTOR3 centerToRay =
        center - rayOrigin;

    float t = D3DXVec3Dot(&centerToRay, &axisDir) / denominator;

    D3DXVECTOR3 hit = rayOrigin + rayDir * t;

    // 中心から交点へのベクトル
    D3DXVECTOR3 v = hit - center;

    // 回転軸方向の成分を除去
    v -= axisDir * D3DXVec3Dot(&v, &axisDir);

    float length = D3DXVec3Length(&v);

    if (length < 0.0001f)
    {
        return m_DragStartAngle;
    }

    // 正規化
    D3DXVec3Normalize(&v, &v);

    // 回転平面上の基準軸
    D3DXVECTOR3 reference;

    if (fabsf(axisDir.y) < 0.9f)
    {
        reference = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
    }
    else
    {
        reference = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
    }

    // reference を回転軸に垂直な方向へ投影
    reference -= axisDir * D3DXVec3Dot(&reference, &axisDir);

    D3DXVec3Normalize(&reference, &reference);

    // もう一つの基準軸
    D3DXVECTOR3 tangent;

    D3DXVec3Cross(&tangent, &axisDir, &reference);

    D3DXVec3Normalize(&tangent, &tangent);

    // atan2 で角度を求める
    float x = D3DXVec3Dot(&v, &reference);

    float y = D3DXVec3Dot(&v, &tangent);

    return atan2f(y, x);
}