//=============================================================================
//
// キャラクターアセット処理 [CharacterAsset.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "CharacterAsset.h"


//=============================================================================
// 読み込み処理(総括)
//=============================================================================
std::shared_ptr<CCharacterAsset> CCharacterAsset::Load(const char* filepath)
{
    auto asset = std::make_shared<CCharacterAsset>();

    FILE* fp = fopen(filepath, "r");

    if (!fp)
    {
        return nullptr;
    }

    char buf[256];

    //---------------------------------
    // SCRIPT探索
    //---------------------------------

    while (fscanf(fp, "%s", buf) != EOF)
    {
        if (strcmp(buf, "SCRIPT") != 0)
        {
            continue;
        }

        //---------------------------------
        // SCRIPT内
        //---------------------------------

        while (fscanf(fp, "%s", buf) != EOF)
        {
            if (strcmp(buf, "END_SCRIPT") == 0)
            {
                break;
            }

            //---------------------------------
            // MODEL_FILENAME
            //---------------------------------

            if (strcmp(buf, "MODEL_FILENAME") == 0)
            {
                PART part;

                fscanf(fp, "%s", buf); // =

                fscanf(fp, "%s", buf);

                part.path = buf;

                asset->Parts.push_back(part);

                continue;
            }

            //---------------------------------
            // CHARACTERSET
            //---------------------------------

            if (strcmp(buf, "CHARACTERSET") == 0)
            {
                asset->LoadCharacterSet(fp);

                continue;
            }

            //---------------------------------
            // MOTIONSET
            //---------------------------------

            if (strcmp(buf, "MOTIONSET") == 0)
            {
                asset->LoadMotionSet(fp);

                continue;
            }
        }
    }

    fclose(fp);

    return asset;
}

//=============================================================================
// キャラクター情報読み込み
//=============================================================================
void CCharacterAsset::LoadCharacterSet(FILE* fp)
{
    char buf[256];

    while (ReadToken(fp, buf))
    {
        //---------------------------------
        // 終了
        //---------------------------------

        if (strcmp(buf, "END_CHARACTERSET") == 0)
        {
            return;
        }

        //---------------------------------
        // PARTSSET
        //---------------------------------

        if (strcmp(buf, "PARTSSET") != 0)
        {
            continue;
        }

        int index = -1;

        while (ReadToken(fp, buf))
        {
            if (strcmp(buf, "END_PARTSSET") == 0)
            {
                break;
            }

            //---------------------------------
            // INDEX
            //---------------------------------

            if (strcmp(buf, "INDEX") == 0)
            {
                ReadToken(fp, buf); // =

                ReadToken(fp, buf);

                index = atoi(buf);

                continue;
            }

            if (index < 0 || index >= Parts.size())
            {
                continue;
            }

            //---------------------------------
            // PARENT
            //---------------------------------

            if (strcmp(buf, "PARENT") == 0)
            {
                ReadToken(fp, buf); // =

                ReadToken(fp, buf);

                Parts[index].parentIndex = atoi(buf);

                continue;
            }

            //---------------------------------
            // POS
            //---------------------------------

            if (strcmp(buf, "POS") == 0)
            {
                ReadToken(fp, buf); // =

                ReadToken(fp, buf);
                Parts[index].offsetPos.x = (float)atof(buf);

                ReadToken(fp, buf);
                Parts[index].offsetPos.y = (float)atof(buf);

                ReadToken(fp, buf);
                Parts[index].offsetPos.z = (float)atof(buf);

                continue;
            }

            //---------------------------------
            // ROT
            //---------------------------------

            if (strcmp(buf, "ROT") == 0)
            {
                ReadToken(fp, buf); // =

                ReadToken(fp, buf);
                Parts[index].offsetRot.x = (float)atof(buf);

                ReadToken(fp, buf);
                Parts[index].offsetRot.y = (float)atof(buf);

                ReadToken(fp, buf);
                Parts[index].offsetRot.z = (float)atof(buf);

                continue;
            }
        }
    }
}

//=============================================================================
// モーション読み込み
//=============================================================================
void CCharacterAsset::LoadMotionSet(FILE* fp)
{
    char buf[256];

    Motions.push_back(MOTION_INFO());

    auto& motion = Motions.back();

    while (ReadToken(fp, buf))
    {
        //---------------------------------
        // 終了
        //---------------------------------

        if (strcmp(buf, "END_MOTIONSET") == 0)
        {
            return;
        }

        //---------------------------------
        // LOOP
        //---------------------------------

        if (strcmp(buf, "LOOP") == 0)
        {
            ReadToken(fp, buf); // =

            ReadToken(fp, buf);

            motion.bLoop = (atoi(buf) != 0);

            continue;
        }

        //---------------------------------
        // NUM_KEY
        //---------------------------------

        if (strcmp(buf, "NUM_KEY") == 0)
        {
            ReadToken(fp, buf); // =

            ReadToken(fp, buf);

            int numKey = atoi(buf);

            //---------------------------------
            // KEYSET
            //---------------------------------

            for (int i = 0; i < numKey; i++)
            {
                while (ReadToken(fp, buf))
                {
                    if (strcmp(buf, "KEYSET") == 0)
                    {
                        break;
                    }
                }

                LoadKeySet(fp, motion);
            }
        }
    }
}

//=============================================================================
// キーセット読み込み
//=============================================================================
void CCharacterAsset::LoadKeySet( FILE* fp, MOTION_INFO& motion)
{
    char buf[256];

    KEY_FRAME keyFrame;

    //---------------------------------
    // パーツ数確保
    //---------------------------------

    keyFrame.aKey.resize(Parts.size());

    int partIndex = 0;

    while (ReadToken(fp, buf))
    {
        //---------------------------------
        // 終了
        //---------------------------------

        if (strcmp(buf, "END_KEYSET") == 0)
        {
            motion.aKeyInfo.push_back(keyFrame);

            return;
        }

        //---------------------------------
        // FRAME
        //---------------------------------

        if (strcmp(buf, "FRAME") == 0)
        {
            ReadToken(fp, buf); // =

            ReadToken(fp, buf);

            keyFrame.nFrame = atoi(buf);

            continue;
        }

        //---------------------------------
        // KEY
        //---------------------------------

        if (strcmp(buf, "KEY") == 0)
        {
            while (ReadToken(fp, buf))
            {
                if (strcmp(buf, "END_KEY") == 0)
                {
                    partIndex++;

                    break;
                }

                // 範囲外防止
                if (partIndex >= keyFrame.aKey.size())
                {
                    continue;
                }

                //---------------------------------
                // POS
                //---------------------------------

                if (strcmp(buf, "POS") == 0)
                {
                    // コメントスキップ
                    ReadToken(fp, buf);

                    ReadToken(fp, buf);
                    keyFrame.aKey[partIndex].pos.x = (float)atof(buf);

                    ReadToken(fp, buf);
                    keyFrame.aKey[partIndex].pos.y = (float)atof(buf);

                    ReadToken(fp, buf);
                    keyFrame.aKey[partIndex].pos.z = (float)atof(buf);

                    continue;
                }

                //---------------------------------
                // ROT
                //---------------------------------

                if (strcmp(buf, "ROT") == 0)
                {
                    ReadToken(fp, buf); // =

                    ReadToken(fp, buf);
                    keyFrame.aKey[partIndex].rot.x = (float)atof(buf);

                    ReadToken(fp, buf);
                    keyFrame.aKey[partIndex].rot.y = (float)atof(buf);

                    ReadToken(fp, buf);
                    keyFrame.aKey[partIndex].rot.z = (float)atof(buf);

                    continue;
                }
            }
        }
    }
}

//=============================================================================
// コメントスキップ関数
//=============================================================================
bool CCharacterAsset::ReadToken(FILE* fp, char* buf)
{
    while (fscanf(fp, "%s", buf) != EOF)
    {
        //---------------------------------
        // コメント
        //---------------------------------

        if (buf[0] == '#')
        {
            fgets(buf, 256, fp);

            continue;
        }

        return true;
    }

    return false;
}