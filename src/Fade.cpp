//=============================================================================
//
// フェード処理 [Fade.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "Fade.h"
#include "Manager.h"
#include "Renderer.h"
#include "SceneManager.h"

//=============================================================================
// コンストラクタ
//=============================================================================
CFade::CFade() :
	m_State		(FADE_NONE),		// 状態
	m_NextMode	(CScene::Mode::Max),// 次のモード
	m_Alpha		(1.0f)				// アルファ値
{

}
//=============================================================================
// デストラクタ
//=============================================================================
CFade::~CFade()
{
	// なし
}
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CFade::Init(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::Instance()->GetRenderer()->GetDevice();

	if (!pDevice)
	{
		return E_FAIL;
	}

	// 頂点バッファの生成
	HRESULT hr = pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4/*CObject::VERTEX*/,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		m_pVtxBuff.GetAddressOf(),
		NULL);

	if (FAILED(hr))
	{
		return hr;
	}


	VERTEX_2D* pVtx = nullptr;// 頂点情報へのポインタ

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標の設定
	pVtx[0].pos = { 0.0f,       0.0f,        0.0f };
	pVtx[1].pos = { FHD_WIDTH,  0.0f,        0.0f };
	pVtx[2].pos = { 0.0f,       FHD_HEIGHT,  0.0f };
	pVtx[3].pos = { FHD_WIDTH,  FHD_HEIGHT,  0.0f };

	// rhwの設定
	for (int i = 0; i < 4; i++)
	{
		pVtx[i].rhw = 1.0f;
	}

	// 頂点カラーの設定
	for (int i = 0; i < 4; i++)
	{
		pVtx[i].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, m_Alpha);
	}

	// テクスチャ座標の設定
	pVtx[0].tex = { 0.0f, 0.0f };
	pVtx[1].tex = { 1.0f, 0.0f };
	pVtx[2].tex = { 0.0f, 1.0f };
	pVtx[3].tex = { 1.0f, 1.0f };

	// 頂点バッファをアンロックする
	m_pVtxBuff->Unlock();

	// 最初はフェードイン
	m_State = FADE_IN;

	return S_OK;
}
//=============================================================================
// 更新処理
//=============================================================================
void CFade::Update(void)
{
	if (m_State == FADE_NONE)
	{
		return;
	}

	//---------------------------------
	// フェード更新
	//---------------------------------

	if (m_State == FADE_IN)
	{
		m_Alpha -= FADE_SPEED;

		if (m_Alpha <= 0.0f)
		{
			m_Alpha = 0.0f;
			m_State = FADE_NONE;
		}
	}
	else if (m_State == FADE_OUT)
	{
		m_Alpha += FADE_SPEED;

		if (m_Alpha >= 1.0f)
		{
			m_Alpha = 1.0f;

			// シーン切替
			CManager::Instance()->GetSceneManager()->SetMode(m_NextMode);

			// 次はフェードイン
			m_State = FADE_IN;
		}
	}

	//---------------------------------
	// 頂点カラー更新
	//---------------------------------

	VERTEX_2D* pVtx = nullptr;

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	D3DXCOLOR color = D3DXCOLOR(0.0f, 0.0f, 0.0f, m_Alpha);

	for (int i = 0; i < 4; i++)
	{
		pVtx[i].rhw = 1.0f;
		pVtx[i].col = color;
	}

	// 頂点バッファをアンロックする
	m_pVtxBuff->Unlock();
}
//=============================================================================
// 描画処理
//=============================================================================
void CFade::Draw(void)
{
	if (m_State == FADE_NONE)
	{
		return;
	}

	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::Instance()->GetRenderer()->GetDevice();

	if (!pDevice)
	{
		return;
	}

	// テクスチャの設定
	pDevice->SetTexture(0, nullptr);

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuff.Get(), 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	// ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}
//=============================================================================
// フェードアウト開始
//=============================================================================
void CFade::StartFadeOut(CScene::Mode next)
{
	m_State = FADE_OUT;
	m_NextMode = next;
}