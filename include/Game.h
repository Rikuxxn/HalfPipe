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

	D3DXVECTOR3 GetSpawnPosition(uint8_t playerId)
	{
		return
		{
			-400.0f,
			580.0f,
			-1330.0f
		};
	}

	D3DXVECTOR3 GetSpawnRotation(uint8_t playerId)
	{
		return
		{
			0.0f,
			180.0f,
			0.0f
		};
	}

	//// プレイヤー生成
	//void SpawnPlayer(uint8_t playerId);

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
	void OnDeviceReset(void) override;
	void SetEnablePause(bool bPause);

	CPauseManager* GetPauseManager(void);

private:
	std::unique_ptr<CPauseManager> m_PauseManager;
	std::unique_ptr<CSky> m_pSky;

	// 「点」 UI
	CGameObject* m_pPointUI;
};

#endif
