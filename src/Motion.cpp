//#include "Motion.h"
//
//std::unique_ptr<CMotion> CMotion::Load(const char* filepath)
//{
//    auto motion = std::make_unique<CMotion>();
//
//    FILE* fp = fopen(filepath, "r");
//    if (!fp) return nullptr;
//
//    char buf[256];
//
//    while (fscanf(fp, "%s", buf) != EOF)
//    {
//        if (strcmp(buf, "SCRIPT") == 0)
//        {
//            motion->LoadScript(fp);
//        }
//    }
//
//    fclose(fp);
//    return motion;
//}
//
//void CMotion::LoadScript(FILE* fp)
//{
//    char buf[256];
//
//    while (fscanf(fp, "%s", buf) != EOF)
//    {
//        if (strcmp(buf, "END_SCRIPT") == 0)
//            return;
//
//        if (strcmp(buf, "NUM_MODEL") == 0)
//        {
//            fscanf(fp, "%s", buf); // =
//            fscanf(fp, "%d", &m_nNumModel);
//        }
//        else if (strcmp(buf, "MOTIONSET") == 0)
//        {
//            LoadMotionSet(fp);
//        }
//        else if (strcmp(buf, "CHARACTERSET") == 0)
//        {
//            SkipCharacterSet(fp); // ←重要（後で説明）
//        }
//    }
//}
//
//void CMotion::SkipCharacterSet(FILE* fp)
//{
//    char buf[256];
//
//    while (fscanf(fp, "%s", buf) != EOF)
//    {
//        if (strcmp(buf, "END_CHARACTERSET") == 0)
//            return;
//    }
//}
//
//void CMotion::LoadMotionSet(FILE* fp)
//{
//    char buf[256];
//
//    int motionIndex = (int)m_aMotionInfo.size();
//    m_aMotionInfo.push_back(MOTION_INFO());
//
//    auto& motion = m_aMotionInfo[motionIndex];
//
//    while (fscanf(fp, "%s", buf) != EOF)
//    {
//        if (strcmp(buf, "END_MOTIONSET") == 0)
//            return;
//
//        if (strcmp(buf, "LOOP") == 0)
//        {
//            fscanf(fp, "%s", buf);
//            int loop;
//            fscanf(fp, "%d", &loop);
//            motion.bLoop = (loop != 0);
//        }
//        else if (strcmp(buf, "NUM_KEY") == 0)
//        {
//            fscanf(fp, "%s", buf);
//            fscanf(fp, "%d", &motion.nNumKey);
//
//            motion.aKeyInfo.resize(motion.nNumKey);
//
//            for (int i = 0; i < motion.nNumKey; i++)
//            {
//                LoadKeySet(fp, motion.aKeyInfo[i]);
//            }
//        }
//    }
//}
//
////=============================================================================
//// キーセット読み込み
////=============================================================================
//void CMotion::LoadKeySet(FILE* fp, KEY_FRAME& keyFrame)
//{
//    char buf[256];
//
//    while (fscanf(fp, "%s", buf) != EOF)
//    {
//        //---------------------------------
//        // KEYSET終了
//        //---------------------------------
//
//        if (strcmp(buf, "END_KEYSET") == 0)
//        {
//            return;
//        }
//
//        //---------------------------------
//        // フレーム数
//        //---------------------------------
//
//        if (strcmp(buf, "FRAME") == 0)
//        {
//            fscanf(fp, "%s", buf); // =
//            fscanf(fp, "%d", &keyFrame.nFrame);
//        }
//
//        //---------------------------------
//        // KEY
//        //---------------------------------
//
//        else if (strcmp(buf, "KEY") == 0)
//        {
//            KEY_FRAME::PART_KEY partKey{};
//
//            while (fscanf(fp, "%s", buf) != EOF)
//            {
//                //-------------------------
//                // KEY終了
//                //-------------------------
//
//                if (strcmp(buf, "END_KEY") == 0)
//                {
//                    break;
//                }
//
//                //-------------------------
//                // Position
//                //-------------------------
//
//                if (strcmp(buf, "POS") == 0)
//                {
//                    fscanf(fp, "%s", buf); // =
//
//                    fscanf(fp, "%f %f %f",
//                        &partKey.pos.x,
//                        &partKey.pos.y,
//                        &partKey.pos.z);
//                }
//
//                //-------------------------
//                // Rotation
//                //-------------------------
//
//                else if (strcmp(buf, "ROT") == 0)
//                {
//                    fscanf(fp, "%s", buf); // =
//
//                    fscanf(fp, "%f %f %f",
//                        &partKey.rot.x,
//                        &partKey.rot.y,
//                        &partKey.rot.z);
//                }
//            }
//
//            //---------------------------------
//            // パーツキー追加
//            //---------------------------------
//
//            keyFrame.aKey.push_back(partKey);
//        }
//    }
//}