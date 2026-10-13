//=============================================================================
//
// サウンドマネージャー処理 [SoundManager.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "SoundManager.h"
#include "MathUtility.h"
#include "MathConstants.h"
#include <json.hpp>

// 名前空間の使用
using namespace MathUtil;
using namespace MathConst;


//*****************************************************************************
// 型定義
//*****************************************************************************
using json = nlohmann::json;

//=============================================================================
// コンストラクタ
//=============================================================================
CSoundManager::CSoundManager() :
	m_pXAudio2			(nullptr),
	m_pMasteringVoice	(nullptr),
	m_NextHandle		(0),
	m_BGMHandle			(INVALID_HANDLE),
	m_ListenerPosition	(INIT_VEC3),
	m_MasterVolume		(1.0f),
	m_BGMVolume			(1.0f),
	m_SEVolume			(1.0f)
{
    m_Listener = {};
}

//=============================================================================
// デストラクタ
//=============================================================================
CSoundManager::~CSoundManager()
{

}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CSoundManager::Init(HWND hWnd)
{
    HRESULT hr;

    CoInitializeEx(NULL, COINIT_MULTITHREADED);

    hr = XAudio2Create(&m_pXAudio2, 0);
    if (FAILED(hr))
    {
        MessageBox(hWnd, "XAudio2オブジェクトの作成に失敗！", "警告！", MB_ICONWARNING);
        CoUninitialize();
        return hr;
    }

    XAUDIO2_DEVICE_DETAILS deviceDetails;
    hr = m_pXAudio2->GetDeviceDetails(0, &deviceDetails);
    if (FAILED(hr))
    {
        MessageBox(hWnd, "デバイス情報の取得に失敗！", "警告！", MB_ICONWARNING);
        m_pXAudio2->Release();
        m_pXAudio2 = nullptr;
        CoUninitialize();
        return hr;
    }

    UINT32 channels = deviceDetails.OutputFormat.Format.nChannels;
    UINT32 sampleRate = deviceDetails.OutputFormat.Format.nSamplesPerSec;

    hr = m_pXAudio2->CreateMasteringVoice(&m_pMasteringVoice, channels, sampleRate);
    if (FAILED(hr))
    {
        MessageBox(hWnd, "マスターボイスの生成に失敗！", "警告！", MB_ICONWARNING);
        m_pXAudio2->Release();
        m_pXAudio2 = nullptr;
        CoUninitialize();
        return hr;
    }

    X3DAudioInitialize(deviceDetails.OutputFormat.dwChannelMask, X3DAUDIO_SPEED_OF_SOUND, m_X3DInstance);

    m_Listener.Position     = { 0.0f, 0.0f, 0.0f };
    m_Listener.OrientFront  = { 0.0f, 0.0f, 1.0f };
    m_Listener.OrientTop    = { 0.0f, 1.0f, 0.0f };
    m_Listener.Velocity     = { 0.0f, 0.0f, 0.0f };

    // サウンド情報の読み込み
    if (FAILED(LoadSoundInfo("data/SoundList.json")))
    {
        return E_FAIL;
    }

    for (int nCnt = 0; nCnt < SOUND_LABEL_MAX; ++nCnt)
    {
        hr = LoadWave((SOUND_LABEL)nCnt);
        if (FAILED(hr))
        {
            const SoundInfo& info = m_SoundInfo[nCnt];

            char msg[128];
            sprintf_s(msg, "サウンドの読み込みに失敗: %s", info.filename.c_str());
            MessageBox(hWnd, msg, "Error", MB_OK);
            Uninit();
            return hr;
        }
    }

    return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void CSoundManager::Uninit(void)
{
    // 全てのSoundInstanceを停止・破棄
    for (auto& inst : m_Instances)
    {
        DestroyInstance(inst);
    }

    m_Instances.clear();

    // 各サウンドのオーディオデータ解放
    for (int nCnt = 0; nCnt < SOUND_LABEL_MAX; ++nCnt)
    {
        if (m_SoundData[nCnt].pAudioData)
        {
            free(m_SoundData[nCnt].pAudioData);
            m_SoundData[nCnt].pAudioData = nullptr;
        }
    }

    // マスターボイスの破棄
    if (m_pMasteringVoice)
    {
        m_pMasteringVoice->DestroyVoice();
        m_pMasteringVoice = nullptr;
    }

    // XAudio2オブジェクトの開放
    if (m_pXAudio2)
    {
        m_pXAudio2->Release();
        m_pXAudio2 = nullptr;
    }

    // COMライブラリの終了処理
    CoUninitialize();
}

//=============================================================================
// サウンド更新
//=============================================================================
void CSoundManager::Update(void)
{
    //------------------------------------------------------------------------
    // インスタンスを更新
    //------------------------------------------------------------------------
    for (auto it = m_Instances.begin(); it != m_Instances.end();)
    {
        SoundInstance& instance = *it;

        //------------------------------------------------------------------------
        // 無効なインスタンス
        //------------------------------------------------------------------------
        if (!instance.active || instance.pSourceVoice == nullptr)
        {
            if (m_BGMHandle == instance.handle)
            {
                m_BGMHandle = INVALID_HANDLE;
            }

            if (instance.pSourceVoice)
            {
                DestroyInstance(instance);
            }

            it = m_Instances.erase(it);
            continue;
        }

        //------------------------------------------------------------------------
        // 3Dサウンド更新
        //------------------------------------------------------------------------
        if (instance.spatial == SoundSpatial::SOUND_3D)
        {
            Update3DSound(instance);
        }

        //------------------------------------------------------------------------
        // 再生終了チェック
        //------------------------------------------------------------------------
        XAUDIO2_VOICE_STATE state = {};

        instance.pSourceVoice->GetState(&state/*, XAUDIO2_VOICE_NOSAMPLESPLAYED*/);

        //------------------------------------------------------------------------
        // 非ループサウンドでバッファがなくなった
        //------------------------------------------------------------------------
        if (!instance.looping && state.BuffersQueued == 0)
        {
            if (m_BGMHandle == instance.handle)
            {
                m_BGMHandle = INVALID_HANDLE;
            }

            DestroyInstance(instance);

            it = m_Instances.erase(it);
            continue;
        }

        ++it;
    }
}

//=============================================================================
// 3Dサウンド更新
//=============================================================================
void CSoundManager::Update3DSound(SoundInstance& instance)
{
    if (instance.pSourceVoice == nullptr)
    {
        return;
    }

    //=========================================================================
    // エミッター位置更新
    //=========================================================================

    instance.emitter.Position.x = instance.position.x;
    instance.emitter.Position.y = instance.position.y;
    instance.emitter.Position.z = instance.position.z;

    //=========================================================================
    // 4chパンニング行列
    //=========================================================================

    FLOAT32 matrix[OUTPUT_CHANNELS] = {};

    CalculateCustomPanning(instance, matrix);

    //=========================================================================
    // パンニング適用
    //
    // 入力 : 1ch
    // 出力 : 4ch
    //=========================================================================

    instance.pSourceVoice->SetOutputMatrix(m_pMasteringVoice, INPUT_CHANNELS, OUTPUT_CHANNELS, matrix);

    //=========================================================================
    // 距離による音量
    //=========================================================================
   
    // リスナーから音源へのベクトル
    D3DXVECTOR3 toSound = instance.position - m_ListenerPosition;

    // 距離
    float distance = D3DXVec3Length(&toSound);

    //--------------------------------------------------------------------------
    // 距離による減衰
    //--------------------------------------------------------------------------

    float volumeScale = 1.0f;

    if (distance <= instance.minDistance)
    {
        // 最小距離以内なら最大音量
        volumeScale = 1.0f;
    }
    else if (distance >= instance.maxDistance)
    {
        // 最大距離より外なら無音
        volumeScale = 0.0f;
    }
    else
    {
        float range = instance.maxDistance - instance.minDistance;

        if (range > 0.0f)
        {
            float t = (distance - instance.minDistance) / range;

            // 0～1
            t = Max(0.0f, Min(1.0f, t));

            // 線形減衰
            volumeScale = 1.0f - t;
        }
    }

    //--------------------------------------------------------------------------
    // 音量更新
    //--------------------------------------------------------------------------

    UpdateInstanceVolume(instance, volumeScale);
}

//=============================================================================
// 音量更新
//=============================================================================
void CSoundManager::UpdateInstanceVolume(SoundInstance& instance, float attenuation)
{
    if (instance.pSourceVoice == nullptr)
    {
        return;
    }

    const SoundInfo& info = m_SoundInfo[instance.label];

    float volume = info.volume;

    // 音量
    if (info.category == SoundCategory::BGM)
    {
        volume *= m_BGMVolume;
    }
    else
    {
        volume *= m_SEVolume;
    }

    // マスター音量
    volume *= m_MasterVolume;

    // 3D距離による減衰
    volume *= attenuation;

    // 音量制限
    volume = Max(0.0f, Min(1.0f, volume));

    instance.pSourceVoice->SetVolume(volume);
}

//=============================================================================
// 4chカスタムパンニング計算
//
// matrix[0] : Front Left
// matrix[1] : Front Right
// matrix[2] : Rear Left
// matrix[3] : Rear Right
//=============================================================================
void CSoundManager::CalculateCustomPanning(SoundInstance& instance, FLOAT32* matrix)
{
    if (matrix == nullptr)
    {
        return;
    }

    //=========================================================================
    // リスナーの向き
    //=========================================================================

    D3DXVECTOR3 front = m_Listener.OrientFront;
    D3DXVECTOR3 up = m_Listener.OrientTop;

    D3DXVec3Normalize(&front, &front);
    D3DXVec3Normalize(&up, &up);

    // リスナーの右方向
    D3DXVECTOR3 right;

    D3DXVec3Cross(&right, &up, &front);

    // 正規化
    D3DXVec3Normalize(&right, &right);

    //=========================================================================
    // リスナー → 音源方向
    //=========================================================================

    D3DXVECTOR3 toSound = instance.position - m_ListenerPosition;

    // 距離
    float distance = D3DXVec3Length(&toSound);

    //=========================================================================
    // リスナーと音源が同じ位置の場合
    //=========================================================================

    if (distance > 0.0001f)
    {
        toSound /= distance;
    }
    else
    {
        // 正面扱い
        toSound = front;
    }

    //=========================================================================
    // 左右方向
    //
    // -1.0 = 完全左
    //  0.0 = 正面
    // +1.0 = 完全右
    //=========================================================================

    float panLR = D3DXVec3Dot(&right, &toSound);

    panLR = Max(-1.0f, Min(1.0f, panLR));

    //=========================================================================
    // 前後方向
    //
    // +1.0 = 正面
    //  0.0 = 真横
    // -1.0 = 真後ろ
    //=========================================================================

    float panFB = D3DXVec3Dot(&front, &toSound);

    panFB = Max(-1.0f, Min(1.0f, panFB));

    //=========================================================================
    // 左右の重み
    //=========================================================================

    float lrWeight = panLR * MathConst::HALF;

    //=========================================================================
    // 前後の重み
    //=========================================================================

    float fbWeight = panFB * MathConst::HALF + MathConst::HALF;

    //=========================================================================
    // 後方音量
    //
    // 真後ろでも完全無音にしない
    //=========================================================================

    const float minVolume = MIN_BACK_VOLUME;

    fbWeight = fbWeight * (1.0f - minVolume) + minVolume;

    float backWeight = 1.0f - fbWeight;

    //=========================================================================
    // 4chへ分配
    //=========================================================================

    float frontLeft     = (MathConst::HALF - lrWeight) * fbWeight;
    float frontRight    = (MathConst::HALF + lrWeight) * fbWeight;
    float rearLeft      = (MathConst::HALF - lrWeight) * backWeight;
    float rearRight     = (MathConst::HALF + lrWeight) * backWeight;

    //=========================================================================
    // 行列へ設定
    //=========================================================================

    matrix[0] = frontLeft;
    matrix[1] = frontRight;
    matrix[2] = rearLeft;
    matrix[3] = rearRight;
}

//=============================================================================
// リスナー位置・向き更新
//=============================================================================
void CSoundManager::UpdateListener(const D3DXVECTOR3& position, const D3DXVECTOR3& front)
{
    m_ListenerPosition = position;

    m_Listener.Position.x = position.x;
    m_Listener.Position.y = position.y;
    m_Listener.Position.z = position.z;

    m_Listener.OrientFront.x = front.x;
    m_Listener.OrientFront.y = front.y;
    m_Listener.OrientFront.z = front.z;

    // 現在鳴っている3Dサウンドを更新
    for (auto& instance : m_Instances)
    {
        if (!instance.active)
        {
            continue;
        }

        if (instance.spatial != SoundSpatial::SOUND_3D)
        {
            continue;
        }

        Update3DSound(instance);
    }
}

//=============================================================================
// サウンド位置更新
//=============================================================================
void CSoundManager::UpdateSoundPosition(SoundHandle handle, const D3DXVECTOR3& position)
{
    if (handle == INVALID_HANDLE)
    {
        return;
    }

    auto it = FindInstance(handle);

    if (it == m_Instances.end())
    {
        return;
    }

    SoundInstance& instance = *it;

    if (!instance.active)
    {
        return;
    }

    // 3D音源じゃなかったら
    if (instance.spatial != SoundSpatial::SOUND_3D)
    {
        return;
    }

    instance.position = position;

    Update3DSound(instance);
}

//=============================================================================
// サウンドインスタンス破棄
//=============================================================================
void CSoundManager::DestroyInstance(SoundInstance& instance)
{
    if (instance.pSourceVoice != nullptr)
    {
        //------------------------------------------------------------------------
        // 再生停止
        //------------------------------------------------------------------------
        instance.pSourceVoice->Stop(0);

        //------------------------------------------------------------------------
        // 保留中のバッファを削除
        //------------------------------------------------------------------------
        instance.pSourceVoice->FlushSourceBuffers();

        //------------------------------------------------------------------------
        // SourceVoice破棄
        //------------------------------------------------------------------------
        instance.pSourceVoice->DestroyVoice();
        instance.pSourceVoice = nullptr;
    }

    instance.active = false;
}

//=============================================================================
// サウンドインスタンス検索
//=============================================================================
std::vector<CSoundManager::SoundInstance>::iterator CSoundManager::FindInstance(SoundHandle handle)
{
    return std::find_if(
        m_Instances.begin(),
        m_Instances.end(),
        [handle](const SoundInstance& instance)
        {
            return instance.handle == handle;
        });
}

//=============================================================================
// 再生中のサウンド数取得
//=============================================================================
int CSoundManager::CountPlayingSounds(SOUND_LABEL label) const
{
    int count = 0;

    for (const auto& instance : m_Instances)
    {
        if (!instance.active)
        {
            continue;
        }

        if (instance.label == label)
        {
            ++count;
        }
    }

    return count;
}

//=============================================================================
// BGM再生
//=============================================================================
CSoundManager::SoundHandle CSoundManager::PlayBGM(SOUND_LABEL label)
{
    //------------------------------------------------------------------------
    // ラベルチェック
    //------------------------------------------------------------------------
    if (label < 0 || label >= SOUND_LABEL_MAX)
    {
        return INVALID_HANDLE;
    }

    //------------------------------------------------------------------------
    // BGMであることを確認
    //------------------------------------------------------------------------
    if (m_SoundInfo[label].category != SoundCategory::BGM)
    {
        return INVALID_HANDLE;
    }

    //------------------------------------------------------------------------
    // 既にBGMが再生されている場合は停止
    //------------------------------------------------------------------------
    if (m_BGMHandle != INVALID_HANDLE)
    {
        Stop(m_BGMHandle);
    }

    //------------------------------------------------------------------------
    // BGM再生
    //------------------------------------------------------------------------
    SoundHandle handle = PlayInternal(label, INIT_VEC3);

    if (handle == INVALID_HANDLE)
    {
        return INVALID_HANDLE;
    }

    //------------------------------------------------------------------------
    // 現在のBGMとして登録
    //------------------------------------------------------------------------
    m_BGMHandle = handle;

    return handle;
}

//=============================================================================
// 2Dサウンド再生
//=============================================================================
CSoundManager::SoundHandle CSoundManager::Play2D(SOUND_LABEL label)
{
    //------------------------------------------------------------------------
    // ラベルチェック
    //------------------------------------------------------------------------
    if (label < 0 || label >= SOUND_LABEL_MAX)
    {
        return INVALID_HANDLE;
    }

    //------------------------------------------------------------------------
    // 2Dサウンドであることを確認
    //------------------------------------------------------------------------
    if (m_SoundInfo[label].spatial != SoundSpatial::SOUND_2D)
    {
        return INVALID_HANDLE;
    }

    //------------------------------------------------------------------------
    // 2Dサウンド再生
    //------------------------------------------------------------------------
    return PlayInternal(label, INIT_VEC3);
}

//=============================================================================
// 3Dサウンド再生
//=============================================================================
CSoundManager::SoundHandle CSoundManager::Play3D(SOUND_LABEL label, const D3DXVECTOR3& position)
{
    //------------------------------------------------------------------------
    // ラベルチェック
    //------------------------------------------------------------------------
    if (label < 0 || label >= SOUND_LABEL_MAX)
    {
        return INVALID_HANDLE;
    }

    //------------------------------------------------------------------------
    // 3Dサウンドであることを確認
    //------------------------------------------------------------------------
    if (m_SoundInfo[label].spatial != SoundSpatial::SOUND_3D)
    {
        return INVALID_HANDLE;
    }

    //------------------------------------------------------------------------
    // 3Dサウンド再生
    //------------------------------------------------------------------------
    return PlayInternal(label, position);
}

//=============================================================================
// 一時停止
//=============================================================================
void CSoundManager::PauseAll(void)
{
    for (auto& inst : m_Instances)
    {
        if (inst.pSourceVoice)
        {
            inst.pSourceVoice->Stop(0); // フラグ0で一時停止
        }
    }
}
//=============================================================================
// 再開
//=============================================================================
void CSoundManager::ResumeAll(void)
{
    for (auto& inst : m_Instances)
    {
        if (inst.pSourceVoice)
        {
            inst.pSourceVoice->Start(0); // フラグ0で再開
        }
    }
}

//=============================================================================
// サウンド停止
//=============================================================================
void CSoundManager::Stop(SoundHandle handle)
{
    // インスタンス検索
    auto it = FindInstance(handle);

    if (it == m_Instances.end())
    {
        return;
    }

    // BGMか確認
    if (m_BGMHandle == handle)
    {
        m_BGMHandle = INVALID_HANDLE;
    }

    // SourceVoice破棄
    DestroyInstance(*it);

    // インスタンス削除
    m_Instances.erase(it);
}

//=============================================================================
// ラベル指定でサウンド停止
//=============================================================================
void CSoundManager::StopByLabel(SOUND_LABEL label)
{
    //------------------------------------------------------------------------
    // ラベルチェック
    //------------------------------------------------------------------------
    if (label < 0 || label >= SOUND_LABEL_MAX)
    {
        return;
    }

    //------------------------------------------------------------------------
    // 該当ラベルのインスタンスをすべて停止
    //------------------------------------------------------------------------
    for (auto it = m_Instances.begin(); it != m_Instances.end();)
    {
        if (it->label == label)
        {
            // BGMだったら
            if (m_BGMHandle == it->handle)
            {
                m_BGMHandle = INVALID_HANDLE;
            }

            DestroyInstance(*it);
            it = m_Instances.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

//=============================================================================
// 全サウンド停止
//=============================================================================
void CSoundManager::StopAll()
{
    //------------------------------------------------------------------------
    // 全インスタンス破棄
    //------------------------------------------------------------------------
    for (auto& instance : m_Instances)
    {
        DestroyInstance(instance);
    }

    m_Instances.clear();

    //------------------------------------------------------------------------
    // BGMハンドルリセット
    //------------------------------------------------------------------------
    m_BGMHandle = INVALID_HANDLE;
}

//=============================================================================
// BGM停止処理
//=============================================================================
void CSoundManager::StopBGM(void)
{
    if (m_BGMHandle == INVALID_HANDLE)
    {
        return;
    }

    Stop(m_BGMHandle);

    m_BGMHandle = INVALID_HANDLE;
}

//=============================================================================
// BGM一時停止
//=============================================================================
void CSoundManager::PauseBGM(void)
{
    //------------------------------------------------------------------------
    // BGMが再生されていない
    //------------------------------------------------------------------------
    if (m_BGMHandle == INVALID_HANDLE)
    {
        return;
    }

    //------------------------------------------------------------------------
    // BGMのインスタンスを検索
    //------------------------------------------------------------------------
    for (auto& instance : m_Instances)
    {
        if (instance.handle != m_BGMHandle)
        {
            continue;
        }

        //------------------------------------------------------------------------
        // 再生中なら一時停止
        //------------------------------------------------------------------------
        if (instance.pSourceVoice != nullptr &&
            instance.active)
        {
            instance.pSourceVoice->Stop();
        }

        return;
    }
}

//=============================================================================
// BGM再開
//=============================================================================
void CSoundManager::ResumeBGM(void)
{
    //------------------------------------------------------------------------
    // BGMが設定されていない
    //------------------------------------------------------------------------
    if (m_BGMHandle == INVALID_HANDLE)
    {
        return;
    }

    //------------------------------------------------------------------------
    // BGMのインスタンスを検索
    //------------------------------------------------------------------------
    for (auto& instance : m_Instances)
    {
        if (instance.handle != m_BGMHandle)
        {
            continue;
        }

        //------------------------------------------------------------------------
        // SourceVoiceが存在するなら再開
        //------------------------------------------------------------------------
        if (instance.pSourceVoice != nullptr &&
            instance.active)
        {
            instance.pSourceVoice->Start();
        }

        return;
    }
}

//=============================================================================
// サウンド情報JSON読み込み
//=============================================================================
HRESULT CSoundManager::LoadSoundInfo(const char* pFilename)
{
    // パスが無かったら
    if (pFilename == nullptr)
    {
        return E_INVALIDARG;
    }

    std::ifstream file(pFilename);

    // 開けなかったら
    if (!file.is_open())
    {
        MessageBox(nullptr, "サウンドリストの読み込みに失敗しました。", "エラー", MB_OK | MB_ICONERROR);
        return E_FAIL;
    }

    json root;

    try
    {
        file >> root;
    }
    catch (const json::exception&)
    {
        return E_FAIL;
    }

    if (!root.contains("sounds") || !root["sounds"].is_array())
    {
        return E_FAIL;
    }

    for (const auto& sound : root["sounds"])
    {
        if (!sound.contains("label") ||
            !sound.contains("filename"))
        {
            continue;
        }

        int labelIndex = sound["label"].get<int>();

        if (labelIndex < 0 || labelIndex >= SOUND_LABEL_MAX)
        {
            continue;
        }

        SOUND_LABEL label = static_cast<SOUND_LABEL>(labelIndex);

        SoundInfo& info = m_SoundInfo[label];

        // ファイル名
        info.filename = sound["filename"].get<std::string>();

        // カテゴリ
        if (sound.contains("category"))
        {
            info.category = static_cast<SoundCategory>(sound["category"].get<int>());
        }

        // 2D / 3D
        if (sound.contains("spatial"))
        {
            info.spatial = static_cast<SoundSpatial>(sound["spatial"].get<int>());
        }

        // ループ回数
        if (sound.contains("loopCount"))
        {
            info.loopCount = sound["loopCount"].get<int>();
        }

        // 最大同時再生数
        if (sound.contains("maxInstances"))
        {
            info.maxInstances = sound["maxInstances"].get<int>();
        }

        // 音量
        if (sound.contains("volume"))
        {
            info.volume = sound["volume"].get<float>();
        }

        // 3D距離
        if (sound.contains("minDistance"))
        {
            info.minDistance = sound["minDistance"].get<float>();
        }

        if (sound.contains("maxDistance"))
        {
            info.maxDistance = sound["maxDistance"].get<float>();
        }
    }

    return S_OK;
}

//=============================================================================
// サウンド再生内部処理
//=============================================================================
CSoundManager::SoundHandle CSoundManager::PlayInternal(SOUND_LABEL label, const D3DXVECTOR3& position)
{
    //------------------------------------------------------------------------
    // ラベルチェック
    //------------------------------------------------------------------------
    if (label < 0 || label >= SOUND_LABEL_MAX)
    {
        return INVALID_HANDLE;
    }

    //------------------------------------------------------------------------
    // サウンド情報取得
    //------------------------------------------------------------------------
    const SoundInfo& info = m_SoundInfo[label];

    if (info.filename.empty())
    {
        return INVALID_HANDLE;
    }

    //------------------------------------------------------------------------
    // 同時再生数チェック
    //------------------------------------------------------------------------
    if (info.maxInstances > 0)
    {
        if (CountPlayingSounds(label) >= info.maxInstances)
        {
            return INVALID_HANDLE;
        }
    }

    //------------------------------------------------------------------------
    // Waveデータ読み込み
    //------------------------------------------------------------------------
    if (m_SoundData[label].pAudioData == nullptr)
    {
        HRESULT hr = LoadWave(label);

        if (FAILED(hr))
        {
            return INVALID_HANDLE;
        }
    }

    //------------------------------------------------------------------------
    // インスタンス生成
    //------------------------------------------------------------------------
    SoundInstance instance = {};

    instance.handle = m_NextHandle++;
    instance.label = label;
    instance.spatial = info.spatial;
    instance.position = position;

    instance.minDistance = info.minDistance;
    instance.maxDistance = info.maxDistance;

    instance.volume = info.volume;
    instance.active = false;
    instance.looping = (info.loopCount < 0);

    //------------------------------------------------------------------------
    // SourceVoice生成＋再生
    //------------------------------------------------------------------------
    HRESULT hr = CreateAndStartVoice(label, instance);

    if (FAILED(hr))
    {
        return INVALID_HANDLE;
    }

    //------------------------------------------------------------------------
    // 3Dサウンドの場合
    //------------------------------------------------------------------------
    if (instance.spatial == SoundSpatial::SOUND_3D)
    {
        Update3DSound(instance);
    }

    //------------------------------------------------------------------------
    // インスタンス登録
    //------------------------------------------------------------------------
    m_Instances.push_back(instance);

    return instance.handle;
}

//=============================================================================
// SourceVoice生成＋再生処理
//=============================================================================
HRESULT CSoundManager::CreateAndStartVoice(SOUND_LABEL label, SoundInstance& instance)
{
    //------------------------------------------------------------------------ 
    // SoundData取得 
    //------------------------------------------------------------------------ 
    SoundData& soundData = m_SoundData[label]; 
    
    if (soundData.pAudioData == nullptr || soundData.audioBytes == 0)
    {
        return E_FAIL; 
    }

    //------------------------------------------------------------------------
    // SourceVoice作成 
    //------------------------------------------------------------------------ 
    IXAudio2SourceVoice* pSourceVoice = nullptr;

    HRESULT hr = m_pXAudio2->CreateSourceVoice(
        &pSourceVoice,
        &soundData.wfx.Format);

    // 失敗時
    if (FAILED(hr))
    {
        return hr;
    }

    //------------------------------------------------------------------------ 
    // 再生バッファ設定 
    //------------------------------------------------------------------------

    XAUDIO2_BUFFER buffer = {};
    buffer.AudioBytes = soundData.audioBytes;
    buffer.pAudioData = soundData.pAudioData;

    // ループ設定
    const SoundInfo& info = m_SoundInfo[label];

    if (info.loopCount < 0)
    {// ループ時
        buffer.LoopCount = XAUDIO2_LOOP_INFINITE;
    }
    else
    {// ループじゃない時
        // loopCount = 0 → 1回だけ再生 
        // loopCount = 1 → 1回ループして計2回再
        buffer.LoopCount = static_cast<BYTE>(info.loopCount);
        //buffer.Flags = XAUDIO2_END_OF_STREAM;
    }

    // バッファをSourceVoiceへ登録
    hr = pSourceVoice->SubmitSourceBuffer(&buffer);

    if (FAILED(hr))
    {
        pSourceVoice->DestroyVoice();
        return hr;
    }

    //------------------------------------------------------------------------ 
    // インスタンス情報設定 
    //------------------------------------------------------------------------ 
    instance.pSourceVoice = pSourceVoice; 
    instance.label = label; 
    instance.spatial = info.spatial;
    instance.looping = (info.loopCount < 0); 
    instance.active = true; 
    instance.volume = info.volume; 
    
    // 3D情報 
    instance.minDistance = info.minDistance; 
    instance.maxDistance = info.maxDistance; 

    //------------------------------------------------------------------------
    // 初期音量設定 
    //------------------------------------------------------------------------ 
    float volume = info.volume; 
    
    if (info.category == SoundCategory::BGM)
    { 
        volume *= m_BGMVolume; 
    } 
    else
    {
        volume *= m_SEVolume; 
    } 
    
    volume *= m_MasterVolume; 
    pSourceVoice->SetVolume(volume); 
    
    //------------------------------------------------------------------------ 
    // 再生開始 
    //------------------------------------------------------------------------ 
    hr = pSourceVoice->Start(0); 
    
    if (FAILED(hr))
    { 
        pSourceVoice->DestroyVoice(); 

        instance.pSourceVoice = nullptr; 
        instance.active = false; 

        return hr; 
    } 

    return S_OK;
}

//=============================================================================
// WAVEの読み込み
//=============================================================================
HRESULT CSoundManager::LoadWave(SOUND_LABEL label)
{
    if (label < 0 || label >= SOUND_LABEL_MAX)
    {
        return E_FAIL;
    }

    HANDLE hFile = CreateFileA(
        m_SoundInfo[label].filename.c_str(),
        GENERIC_READ, FILE_SHARE_READ,
        NULL, OPEN_EXISTING,
        0, NULL);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        return E_FAIL;
    }

    DWORD dwChunkSize;
    DWORD dwChunkPosition;

    // "RIFF" チャンクチェック
    if (FAILED(CheckChunk(hFile, 'FFIR', &dwChunkSize, &dwChunkPosition)))
    {
        CloseHandle(hFile);
        return E_FAIL;
    }

    // "fmt " チャンク
    if (FAILED(CheckChunk(hFile, ' tmf', &dwChunkSize, &dwChunkPosition)))
    {
        CloseHandle(hFile);
        return E_FAIL;
    }

    WAVEFORMATEXTENSIBLE wfx = {};
    if (FAILED(ReadChunkData(hFile, &wfx, dwChunkSize, dwChunkPosition)))
    {
        CloseHandle(hFile);
        return E_FAIL;
    }

    // "data" チャンク
    if (FAILED(CheckChunk(hFile, 'atad', &dwChunkSize, &dwChunkPosition)))
    {
        CloseHandle(hFile);
        return E_FAIL;
    }

    BYTE* pDataBuffer = new BYTE[dwChunkSize];
    if (FAILED(ReadChunkData(hFile, pDataBuffer, dwChunkSize, dwChunkPosition)))
    {
        delete[] pDataBuffer;
        CloseHandle(hFile);
        return E_FAIL;
    }

    // ソースボイス作成
    IXAudio2SourceVoice* pSourceVoice = nullptr;
    if (FAILED(m_pXAudio2->CreateSourceVoice(&pSourceVoice, (WAVEFORMATEX*)&wfx)))
    {
        delete[] pDataBuffer;
        CloseHandle(hFile);
        return E_FAIL;
    }

    // バッファ設定
    XAUDIO2_BUFFER buffer = {};
    buffer.AudioBytes = dwChunkSize;
    buffer.pAudioData = pDataBuffer;
    buffer.Flags = XAUDIO2_END_OF_STREAM;

    pSourceVoice->SubmitSourceBuffer(&buffer);

    // 保持
    m_SoundData[label].pAudioData = pDataBuffer;	// 読み込んだPCMデータのバッファ
    m_SoundData[label].audioBytes = dwChunkSize;	// データのバイト数
    m_SoundData[label].wfx = wfx;					// フォーマット情報

    CloseHandle(hFile);

    return S_OK;
}

//=============================================================================
// チャンクのチェック
//=============================================================================
HRESULT CSoundManager::CheckChunk(HANDLE hFile, DWORD format, DWORD* pChunkSize, DWORD* pChunkDataPosition)
{
    HRESULT hr = S_OK;
    DWORD dwRead;
    DWORD dwChunkType;
    DWORD dwChunkDataSize;
    DWORD dwRIFFDataSize = 0;
    DWORD dwFileType;
    DWORD dwBytesRead = 0;
    DWORD dwOffset = 0;

    if (SetFilePointer(hFile, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
    {// ファイルポインタを先頭に移動
        return HRESULT_FROM_WIN32(GetLastError());
    }

    while (hr == S_OK)
    {
        if (ReadFile(hFile, &dwChunkType, sizeof(DWORD), &dwRead, NULL) == 0)
        {// チャンクの読み込み
            hr = HRESULT_FROM_WIN32(GetLastError());
        }

        if (ReadFile(hFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, NULL) == 0)
        {// チャンクデータの読み込み
            hr = HRESULT_FROM_WIN32(GetLastError());
        }

        switch (dwChunkType)
        {
        case 'FFIR':
            dwRIFFDataSize = dwChunkDataSize;
            dwChunkDataSize = 4;
            if (ReadFile(hFile, &dwFileType, sizeof(DWORD), &dwRead, NULL) == 0)
            {// ファイルタイプの読み込み
                hr = HRESULT_FROM_WIN32(GetLastError());
            }
            break;

        default:
            if (SetFilePointer(hFile, dwChunkDataSize, NULL, FILE_CURRENT) == INVALID_SET_FILE_POINTER)
            {// ファイルポインタをチャンクデータ分移動
                return HRESULT_FROM_WIN32(GetLastError());
            }
        }

        dwOffset += sizeof(DWORD) * 2;
        if (dwChunkType == format)
        {
            *pChunkSize = dwChunkDataSize;
            *pChunkDataPosition = dwOffset;

            return S_OK;
        }

        dwOffset += dwChunkDataSize;
        if (dwBytesRead >= dwRIFFDataSize)
        {
            return S_FALSE;
        }
    }

    return E_FAIL;
}

//=============================================================================
// チャンクデータの読み込み
//=============================================================================
HRESULT CSoundManager::ReadChunkData(HANDLE hFile, void* pBuffer, DWORD dwBuffersize, DWORD dwBufferoffset)
{
    if (hFile == INVALID_HANDLE_VALUE || pBuffer == nullptr)
    {
        return E_INVALIDARG;
    }

    DWORD dwRead;

    if (SetFilePointer(hFile, dwBufferoffset, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
    {// ファイルポインタを指定位置まで移動
        return HRESULT_FROM_WIN32(GetLastError());
    }

    if (ReadFile(hFile, pBuffer, dwBuffersize, &dwRead, NULL) == 0)
    {// データの読み込み
        return HRESULT_FROM_WIN32(GetLastError());
    }

    return S_OK;
}