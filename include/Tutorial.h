//=============================================================================
//
// チュートリアル処理 [Tutorial.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _TUTORIAL_H_// このマクロ定義がされていなかったら
#define _TUTORIAL_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "Scene.h"
#include "State.h"

//*****************************************************************************
// 前方宣言
//*****************************************************************************
class CPauseManager;
class CSky;
class CPlayer;

//*****************************************************************************
// チュートリアルクラス
//*****************************************************************************
class CTutorial : public CScene
{
public:
	CTutorial();
	~CTutorial();

	D3DXVECTOR3 GetSpawnPosition(void)
	{
		return
		{
			-400.0f,
			580.0f,
			-1330.0f
		};
	}

	D3DXVECTOR3 GetSpawnRotation(void)
	{
		return
		{
			0.0f,
			180.0f,
			0.0f
		};
	}

	// プレイヤー生成
	void SpawnPlayer(uint8_t playerId);

	// 指定IDのプレイヤーを検索
	CPlayer* FindPlayerById(uint8_t playerId);

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;

private:
	std::unique_ptr<CSky> m_pSky;

	// 「点」 UI
	CGameObject* m_pPointUI;

	// チュートリアルUI
	CGameObject* m_pTutorialUI;

	int m_nReadyCount;		// 準備カウント
	float m_fCount;			// 画面遷移カウント
	bool m_IsGameStart;		// ゲーム開始フラグ

	// ステートを管理するクラスのインスタンス
	StateMachine<CTutorial> m_stateMachine;
};

#endif
