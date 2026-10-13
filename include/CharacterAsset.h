//=============================================================================
//
// キャラクターアセット処理 [CharacterAsset.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _CHARACTER_ASSET_H_// このマクロ定義がされていなかったら
#define _CHARACTER_ASSET_H_// 2重インクルード防止のマクロ定義


//*****************************************************************************
// インクルードファイル
//*****************************************************************************

//*****************************************************************************
// キャラクターアセット
//*****************************************************************************
class CCharacterAsset
{
public:

    //---------------------------------
    // パーツ情報
    //---------------------------------

    struct PART
    {
        std::string path;

        int parentIndex = -1;

        D3DXVECTOR3 offsetPos = { 0,0,0 };

        D3DXVECTOR3 offsetRot = { 0,0,0 };
    };

    //---------------------------------
    // モーション
    //---------------------------------

    struct KEY_FRAME
    {
        int nFrame;

        struct PART_KEY
        {
            D3DXVECTOR3 pos;
            D3DXVECTOR3 rot;
        };

        std::vector<PART_KEY> aKey;
    };

    struct MOTION_INFO
    {
        bool bLoop = false;

        std::vector<KEY_FRAME> aKeyInfo;
    };

public:

    //---------------------------------
    // データ
    //---------------------------------

    std::vector<PART> Parts;

    std::vector<MOTION_INFO> Motions;

public:

    static std::shared_ptr<CCharacterAsset>
        Load(const char* filepath);

private:

    void LoadCharacterSet(FILE* fp);

    void LoadMotionSet(FILE* fp);

    void LoadKeySet(FILE* fp, MOTION_INFO& motion);

    // コメントスキップ関数
    bool ReadToken(FILE* fp, char* buf);
};

#endif
