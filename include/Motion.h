////=============================================================================
////
//// モーション処理 [Motion.h]
//// Author : RIKU TANEKAWA
////
////=============================================================================
//#ifndef _MOTION_H_// このマクロ定義がされていなかったら
//#define _MOTION_H_// 2重インクルード防止のマクロ定義
//
//
////*****************************************************************************
//// インクルードファイル
////*****************************************************************************
//
////*****************************************************************************
//// モーションクラス
////*****************************************************************************
//class CMotion
//{
//public:
//    static std::unique_ptr<CMotion> Load(const char* filepath);
//
//    struct KEY_FRAME
//    {
//        int nFrame; // このキーの長さ
//
//        struct PART_KEY
//        {
//            D3DXVECTOR3 pos;
//            D3DXVECTOR3 rot;
//        };
//
//        std::vector<PART_KEY> aKey;
//    };
//
//    struct MOTION_INFO
//    {
//        bool bLoop = false;
//
//        int nNumKey = 0;
//        int nNumParts = 0;
//        std::vector<KEY_FRAME> aKeyInfo;
//    };
//
//private:
//    void LoadScript(FILE* fp);
//    void LoadMotionSet(FILE* fp);
//    void SkipCharacterSet(FILE* fp);
//    void LoadKeySet(FILE* fp, KEY_FRAME& key);
//
//public:
//    std::vector<MOTION_INFO> m_aMotionInfo;
//    int m_nNumModel = 0;
//};
//
//#endif
