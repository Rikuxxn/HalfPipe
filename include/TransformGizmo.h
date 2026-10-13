//=============================================================================
//
// ギズモ処理 [TransformGizmo.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _TRANSFORM_GIZMO_H_// このマクロ定義がされていなかったら
#define _TRANSFORM_GIZMO_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "GameObject.h"

//*****************************************************************************
// ギズモクラス
//*****************************************************************************
class CTransformGizmo : public CGameObject
{
public:

    // ギズモの種類
    enum class GizmoMode
    {
        Move,
        Rotate,
        Scale,
    };

    // 操作軸
    enum class Axis
    {
        None,
        X,
        Y,
        Z,
        Center,
    };

    // ギズモモデル
    struct GizmoObjects
    {
        CGameObject* X = nullptr;
        CGameObject* Y = nullptr;
        CGameObject* Z = nullptr;
        CGameObject* Center = nullptr;

        bool Visible = false;
    };

public:
    CTransformGizmo();
    ~CTransformGizmo();

    CGameObject* CreateAxis(const char* filePath);

    HRESULT Init(void);
    void Update(CGameObject* selectedObject);

    // ギズモモード
    void SetMode(GizmoMode mode);
    GizmoMode GetMode(void) const { return m_Mode; }

    // ギズモの表示・非表示設定
    void SetGizmoVisible(GizmoObjects& gizmo, bool visible);

    // ドラッグ中か
    bool IsDragging(void) const { return m_IsDragging; }

    // ギズモがレイにヒットしたか
    bool IsHit(const D3DXVECTOR3& rayOrigin, const D3DXVECTOR3& rayDir);

private:

    D3DXVECTOR3 GetAxisDirection(Axis axis);

    Axis PickAxis   (const D3DXVECTOR3& rayOrigin, const D3DXVECTOR3& rayDir);
   
    // 軸との交差判定
    void CheckAxis(
        CGameObject* axisObject,
        Axis axis,
        const D3DXVECTOR3& rayOrigin,
        const D3DXVECTOR3& rayDir,
        float& closestDistance,
        Axis& hitAxis);

    // マウスレイと軸の最近接点を求める
    float CalculateAxisParameter(
        const D3DXVECTOR3& rayOrigin,
        const D3DXVECTOR3& rayDir,
        const D3DXVECTOR3& axisOrigin,
        const D3DXVECTOR3& axisDir);

    // 回転角度を求める
    float CalculateRotationAngle(
        const D3DXVECTOR3& rayOrigin,
        const D3DXVECTOR3& rayDir,
        const D3DXVECTOR3& center,
        const D3DXVECTOR3& axisDir);

    // ギズモ共通処理
    void UpdateGizmoTransform(CGameObject* selectedObject);

    // ドラッグ開始
    void BeginDrag(
        CGameObject* selectedObject,
        Axis axis,
        const D3DXVECTOR3& rayOrigin,
        const D3DXVECTOR3& rayDir);

    // ドラッグ終了
    void EndDrag(void);

    // ドラッグキャンセル
    void CancelDrag(void);

    // 移動
    void UpdateMove(
        CGameObject* selectedObject,
        const D3DXVECTOR3& rayOrigin,
        const D3DXVECTOR3& rayDir);

    // 回転
    void UpdateRotate(
        CGameObject* selectedObject,
        const D3DXVECTOR3& rayOrigin,
        const D3DXVECTOR3& rayDir);

    // スケール
    void UpdateScale(
        CGameObject* selectedObject,
        const D3DXVECTOR3& rayOrigin,
        const D3DXVECTOR3& rayDir);

    GizmoObjects* GetCurrentGizmo(void);

private:
    GizmoObjects m_MoveGizmo;
    GizmoObjects m_RotateGizmo;
    GizmoObjects m_ScaleGizmo;
    CGameObject* m_PrevSelectedObject;  // 前回の選択オブジェクト
    GizmoMode    m_Mode;                // ギズモモード
    Axis         m_SelectedAxis;        // 選択中の軸
    D3DXVECTOR3  m_DragStartPosition;   // ドラッグ開始時の位置
    D3DXVECTOR3  m_DragStartRotation;   // ドラッグ開始時の向き
    D3DXVECTOR3  m_DragStartScale;      // ドラッグ開始時のスケール
    float        m_DragStartAxisParam;  // ドラッグ開始時の軸上パラメータ
    float        m_DragStartAngle;      // 回転開始時の角度
    bool         m_IsDragging;          // ドラッグ中か
};

#endif