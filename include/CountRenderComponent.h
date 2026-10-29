//=============================================================================
//
// カウント描画コンポーネント処理 [CountRenderComponent.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _COUNT_RENDER_COMPONENT_H_// このマクロ定義がされていなかったら
#define _COUNT_RENDER_COMPONENT_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "RenderComponent.h"

// 前方宣言
class CTextureResource;

// カウントの種類
enum CountType
{
    Add,    // 加算
    Dec,    // 減算
};

//*****************************************************************************
// カウント描画コンポーネントクラス
//*****************************************************************************
class CCountRenderComponent : public CRenderComponent
{
public:
    CCountRenderComponent();
    ~CCountRenderComponent() override = default;

    // 初期化
    HRESULT Init(void) override;

    // 更新
    void Update(void) override;

    // 描画
    void Draw(void) override;

    // カウント設定
    void SetCount(int count);

    // カウント加算
    void AddCount(int count);

    // 現在表示しているカウント
    int GetCount(void) const { return m_Count; }

    // 目標カウント
    int GetTargetCount(void) const { return m_TargetCount; }

    // 1文字のサイズ設定
    void SetDigitSize(float width, float height);

    // 加算速度の設定（1秒あたりのポイント数）
    void SetCountSpeed(float speed);

    // 加算中かどうか 
    bool IsCounting(void) const { return m_Count != m_TargetCount; }

    // カウントの種類の設定
    void SetCountType(CountType type) { m_CountType = type; }

    // 影を描画するか
    bool IsShadowCaster(void) override
    {
        return false;
    }

private:

    // カウントの種類
    CountType m_CountType;

    // 現在表示しているカウント
    int m_Count;

    // 目標カウント 
    int m_TargetCount;

    // 1秒あたりの加算ポイント数 
    float m_CountSpeed;

    // 加算量の小数部分を保持 
    float m_CountRemainder;

    // 1文字のサイズ
    float m_DigitWidth;
    float m_DigitHeight;

    // 頂点バッファ
    Microsoft::WRL::ComPtr<IDirect3DVertexBuffer9> m_pVtxBuff;
};

#endif