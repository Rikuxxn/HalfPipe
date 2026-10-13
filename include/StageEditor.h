//=============================================================================
//
// ステージエディター処理 [StageEditor.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _STAGE_EDITOR_H_// このマクロ定義がされていなかったら
#define _STAGE_EDITOR_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "StageObject.h"

// 前方宣言
class CTransformGizmo;


//*****************************************************************************
// ステージエディタークラス
//*****************************************************************************
class CStageEditor
{
private:

    // 読み込み用構造体
    struct StageObjectInfo
    {
        ComPtr<IDirect3DTexture9>       Thumbnail;  // サムネイル
        CStageObject::StageObjectType   Type;       // 種類
        std::string                     Category;   // カテゴリー
        std::string                     Name;       // 名前
        std::string                     FilePath;   // ファイルパス
    };

public:
    CStageEditor();
    ~CStageEditor();

    HRESULT Init    (void);
    void    Uninit  (void);
    void    Update  (void);
    void CreateGizmo(void);

    //*****************************************************************************
    // ImGuiサムネイル描画用関数
    //*****************************************************************************
    HRESULT InitThumbnailRenderTarget   (LPDIRECT3DDEVICE9 device);
    void    ReleaseThumbnailRenderTarget(void);
    void    GenerateThumbnails          (void);

    const StageObjectInfo* FindObjectInfo(CStageObject::StageObjectType type) const;

    CGameObject* GetSelectedObject(void) const { return m_SelectedObject; }

private:

    void DrawModelList(void);
    void DrawInspector(CStageObject* selectedObject);
    void DrawSaveLoad(void);

    void PickObject(void);

    // モデルリストの読み込み
    void LoadModelList(const char* filename);

    //*****************************************************************************
    // ImGuiサムネイル描画用関数
    //*****************************************************************************
    ComPtr<IDirect3DTexture9>   RenderThumbnail     (CStageObject* obj);
    IDirect3DTexture9*          GetThumbnailTexture (size_t index);

private:

    static constexpr float THUMB_WIDTH  = 100.0f;   // サムネイルの高さ
    static constexpr float THUMB_HEIGHT = 100.0f;   // サムネイルの高さ

    std::vector<StageObjectInfo> m_ObjectInfos;     // オブジェクト

    //*****************************************************************************
    // サムネイル用リソース
    //*****************************************************************************
    LPDIRECT3DTEXTURE9  m_pThumbnailRT;             // レンダーターゲット
    LPDIRECT3DSURFACE9  m_pThumbnailZ;              // サーフェス
    float               m_thumbWidth;               // サムネイルの幅
    float               m_thumbHeight;              // サムネイルの高さ
    bool                m_isDragging;               // ドラッグ中か
    bool                m_autoUpdateColliderSize;   // スケールにコライダーを追従させるか

    CTransformGizmo* m_TransformGizmo;              // 移動ギズモ
    CStageObject*    m_SelectedObject;              // 選択中のブロック
    CStageObject*    m_PrevSelectedObject;          // 前回の選択中のブロック
    D3DXVECTOR3      m_prevSize;                    // 前回のサイズ
    D3DXVECTOR3      m_prevDegRot;                  // 前回の向き
};

#endif