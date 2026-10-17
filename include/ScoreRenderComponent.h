//=============================================================================
//
// スコア描画コンポーネント処理 [ScoreRenderComponent.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _SCORE_RENDER_COMPONENT_H_// このマクロ定義がされていなかったら
#define _SCORE_RENDER_COMPONENT_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "RenderComponent.h"

// 前方宣言
class CTextureResource;


//*****************************************************************************
// スコア描画コンポーネントクラス
//*****************************************************************************
class CScoreRenderComponent : public CRenderComponent
{
public:
    CScoreRenderComponent();
    ~CScoreRenderComponent() override = default;

    // 初期化
    HRESULT Init(void) override;

    // 更新
    void Update(void) override;

    // 描画
    void Draw(void) override;

    // スコア設定
    void SetScore(int score);

    // スコア加算
    void AddScore(int score);

    // 現在表示しているスコア
    int GetScore(void) const { return m_Score; } 
    
    // 目標スコア 
    int GetTargetScore(void) const { return m_TargetScore; }

    // 1文字のサイズ設定
    void SetDigitSize(float width, float height);

    // 加算速度の設定（1秒あたりのポイント数）
    void SetCountSpeed(float speed); 
    
    // 加算中かどうか 
    bool IsCounting(void) const { return m_Score != m_TargetScore; }

private:

    // 現在表示しているスコア
    int m_Score; 
    
    // 目標スコア 
    int m_TargetScore;
    
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