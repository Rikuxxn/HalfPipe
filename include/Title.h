//=============================================================================
//
// タイトル処理 [Title.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _TITLE_H_// このマクロ定義がされていなかったら
#define _TITLE_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "Scene.h"


// 前方宣言
class CSky;
class CWater;


//*****************************************************************************
// タイトルクラス
//*****************************************************************************
class CTitle : public CScene
{
public:
	CTitle();
	~CTitle();

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
	virtual void ResetLight(void);
	void OnDeviceReset(void) override;

private:

	std::unique_ptr<CSky>	m_pSky;
	std::unique_ptr<CWater> m_pWater;

};

#endif
