//=============================================================================
//
// ゲーム処理 [Game.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _GAME_H_// このマクロ定義がされていなかったら
#define _GAME_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "Scene.h"
//#include "BlockManager.h"
//#include "PauseManager.h"
//#include "RankingManager.h"

//*****************************************************************************
// 前方宣言
//*****************************************************************************
class CPauseManager;
class CSky;

//*****************************************************************************
// ゲームクラス
//*****************************************************************************
class CGame : public CScene
{
public:
	CGame();
	~CGame();

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	virtual void UpdateLight(void);
	void Draw(void) override;
	void OnDeviceReset(void) override;
	//void ReleaseThumbnail(void) override;
	//void ResetThumbnail(void) override;

	//static CTime* GetTime(void) { return m_pTime; }
	//static CBlockManager* GetBlockManager(void) { return m_pBlockManager; }
	//static CPauseManager* GetPauseManager(void) { return m_pPauseManager; }
	void SetEnablePause(bool bPause);

	CPauseManager* GetPauseManager(void);

private:
	std::unique_ptr<CPauseManager> m_PauseManager;
	std::unique_ptr<CSky> m_pSky;
	//static CTime*					 m_pTime;				// タイムへのポインタ
};

#endif
