//=============================================================================
//
// リザルト処理 [Result.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _RESULT_H_// このマクロ定義がされていなかったら
#define _RESULT_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "Scene.h"

//*****************************************************************************
// 前方宣言
//*****************************************************************************
class CBlockManager;
class CSky;


//*****************************************************************************
// リザルトクラス
//*****************************************************************************
class CResult : public CScene
{
public:
	CResult();
	~CResult();

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
	static void SetClearRank(int idx) { m_clearRankIndex = idx; }

	static int GetClearRank(void) { return m_clearRankIndex; }

	void OnDeviceReset(void) override;

private:
	static int			m_clearRankIndex;	// クリア時のランクインデックス

	std::unique_ptr<CSky>	m_pSky;
};

#endif
