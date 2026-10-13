//*****************************************************************************
//
// Water Shader
// Author : RIKU TANEKAWA
//
//*****************************************************************************

//*****************************************************************************
// 定数
//*****************************************************************************
#define PI          3.14159265359f
#define PI2         6.28318530718f


float4x4 World;         // ワールド行列
float4x4 View;          // ビュー行列
float4x4 Projection;    // プロジェクション行列

float3 CameraPos;       // カメラの位置

float Time;             // 0.0～1.0

//*****************************************************************************
// 水の色
//*****************************************************************************

float4 WaterColor = float4(0.03f, 0.30f, 0.38f, 1.0f);
float4 DeepWaterColor = float4(0.01f, 0.08f, 0.12f, 1.0f);


//*****************************************************************************
// 波パラメータ
//*****************************************************************************

// 波1
float2 WaveDirection1   = float2(1.0f, 0.2f);
float WaveAmplitude1    = 0.50f;
float WaveLength1       = 25.0f;
float WaveSpeed1        = 1.5f;

// 波2
float2 WaveDirection2   = float2(-0.3f, 1.0f);
float WaveAmplitude2    = 0.25f;
float WaveLength2       = 12.0f;
float WaveSpeed2        = 2.0f;

// 波3
float2 WaveDirection3   = float2(0.8f, -0.5f);
float WaveAmplitude3    = 0.10f;
float WaveLength3       = 5.0f;
float WaveSpeed3        = 3.0f;

//=============================================================================
// 泡
//=============================================================================

float3 FoamPosition;

float FoamRadius;
float FoamStrength;

//*****************************************************************************
// 法線用パラメータ
//*****************************************************************************

float NormalStrength = 0.35f;


//*****************************************************************************
// 太陽
//*****************************************************************************

float3 LightDirection;
float3 LightColor;
float LightIntensity;


//*****************************************************************************
// フレネル
//*****************************************************************************

float FresnelPower      = 5.0f;
float FresnelStrength   = 0.8f;

//*****************************************************************************
// 頂点インプット情報
//*****************************************************************************
struct VS_INPUT
{
    float4 Position : POSITION0;
    float3 Normal   : NORMAL0;
    float4 Color    : COLOR0;
    float2 TexCoord : TEXCOORD0;
};

//*****************************************************************************
// 頂点アウトプット情報
//*****************************************************************************
struct VS_OUTPUT
{
    float4 Position : POSITION0;

    float3 WorldPos : TEXCOORD0;
    float3 Normal   : TEXCOORD1;

    float2 TexCoord : TEXCOORD2;
};

//*****************************************************************************
// 波の計算
//*****************************************************************************
float CalculateWave(
    float3 worldPos,
    float2 direction,
    float amplitude,
    float wavelength,
    float speed)
{
    direction = normalize(direction);

    float frequency = PI2 / wavelength;

    float phase =
        dot(worldPos.xz, direction) * frequency
        + Time * speed;

    return sin(phase) * amplitude;
}

//*****************************************************************************
// 波の高さ
//*****************************************************************************
float CalculateWaveHeight(float3 worldPos)
{
    float height = 0.0f;

    height += CalculateWave(
        worldPos,
        WaveDirection1,
        WaveAmplitude1,
        WaveLength1,
        WaveSpeed1
    );

    height += CalculateWave(
        worldPos,
        WaveDirection2,
        WaveAmplitude2,
        WaveLength2,
        WaveSpeed2
    );

    height += CalculateWave(
        worldPos,
        WaveDirection3,
        WaveAmplitude3,
        WaveLength3,
        WaveSpeed3
    );

    return height;
}

//*****************************************************************************
// 
// 波による法線計算
// [高さを少しずらしてサンプリングし、X方向・Z方向の傾きを求める]
//
//*****************************************************************************
float3 CalculateWaveNormal(float3 worldPos)
{
    const float SAMPLE_DISTANCE = 0.1f;

    float centerHeight = CalculateWaveHeight(worldPos);

    float3 posX = worldPos;
    posX.x += SAMPLE_DISTANCE;

    float3 posZ = worldPos;
    posZ.z += SAMPLE_DISTANCE;

    float heightX = CalculateWaveHeight(posX);

    float heightZ = CalculateWaveHeight(posZ);

    float dx = (heightX - centerHeight) / SAMPLE_DISTANCE;
    float dz = (heightZ - centerHeight) / SAMPLE_DISTANCE;

    float3 normal;

    normal.x = -dx * NormalStrength;
    normal.y = 1.0f;
    normal.z = -dz * NormalStrength;

    return normalize(normal);
}

//*********************************************************************
// 頂点シェーダー関数
//*********************************************************************
VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;

    //=========================================================================
    // ローカルからワールド座標
    //=========================================================================
    float4 worldPosition = mul(input.Position, World);

    //-------------------------------------------------------------------------
    // 波
    //-------------------------------------------------------------------------

    float waveHeight = CalculateWaveHeight(worldPosition.xyz);

    worldPosition.y += waveHeight;

    //-------------------------------------------------------------------------
    // 波を加えたWorld座標
    //-------------------------------------------------------------------------

    output.WorldPos = worldPosition.xyz;

    //-------------------------------------------------------------------------
    // 波の法線
    //-------------------------------------------------------------------------

    output.Normal = CalculateWaveNormal(worldPosition.xyz);

    //-------------------------------------------------------------------------
    // UV
    //-------------------------------------------------------------------------

    output.TexCoord = input.TexCoord;

    //-------------------------------------------------------------------------
    // ビュー / プロジェクション
    //-------------------------------------------------------------------------

    float4 viewPosition = mul(worldPosition, View);

    output.Position = mul(viewPosition, Projection);


    return output;
}

//*********************************************************************
// フレネル
//*********************************************************************
float CalculateFresnel(float3 normal, float3 viewDir)
{
    float NdotV = saturate(dot(normal, viewDir));

    float fresnel = pow(1.0f - NdotV, FresnelPower);

    return fresnel * FresnelStrength;
}

//*********************************************************************
// ピクセルシェーダー関数
//*********************************************************************
float4 PSmain(VS_OUTPUT input) : COLOR0
{
    //-------------------------------------------------------------------------
    // 法線
    //-------------------------------------------------------------------------

    float3 normal = normalize(input.Normal);

    //-------------------------------------------------------------------------
    // カメラ → サーフェス
    //-------------------------------------------------------------------------

    float3 viewDir = normalize(CameraPos - input.WorldPos);

    //-------------------------------------------------------------------------
    // フレネル
    //-------------------------------------------------------------------------

    float fresnel = CalculateFresnel(normal, viewDir);

    //-------------------------------------------------------------------------
    // 水面の基本色
    //-------------------------------------------------------------------------

    float3 baseColor = lerp(WaterColor.rgb, DeepWaterColor.rgb, 0.5f);

    //-------------------------------------------------------------------------
    // 太陽方向
    //-------------------------------------------------------------------------

    float3 sunDir = normalize(LightDirection);

    //-------------------------------------------------------------------------
    // スペキュラー
    //-------------------------------------------------------------------------

    float3 halfVector = normalize(sunDir + viewDir);

    float specular = pow(saturate(dot(normal, halfVector)), 64.0f);

    specular *= LightIntensity;

    //-------------------------------------------------------------------------
    // Fresnelによる反射色
    //-------------------------------------------------------------------------

    float3 reflectionColor = LightColor * fresnel;

    //-------------------------------------------------------------------------
    // 水の色 + 反射
    //-------------------------------------------------------------------------

    float3 finalColor = baseColor;

    finalColor = lerp(finalColor, reflectionColor, fresnel);

    //-------------------------------------------------------------------------
    // 太陽の反射
    //-------------------------------------------------------------------------

    finalColor += LightColor * specular;

    //-------------------------------------------------------------------------
    // 泡
    //-------------------------------------------------------------------------
    float2 surfacePos = input.WorldPos.xz;
    float2 foamPos = FoamPosition.xz;

    float distanceToFoam = distance(surfacePos, foamPos);

    float foam =
    1.0f - smoothstep(
        FoamRadius * 0.3f,
        FoamRadius,
        distanceToFoam
    );

    foam *= FoamStrength;
    
    //-------------------------------------------------------------------------
    // 泡を合成
    //-------------------------------------------------------------------------
    finalColor = lerp(
        finalColor,
        float3(1.0f, 1.0f, 1.0f),
        saturate(foam)
    );
    
    //-------------------------------------------------------------------------
    // 最終カラー
    //-------------------------------------------------------------------------

    return float4(saturate(finalColor), 0.85f);
}

//*********************************************************
// テクニック宣言
//*********************************************************
technique Water
{
	// Pass 0
    pass P0
    {
        VertexShader    = compile vs_3_0 main();
        PixelShader     = compile ps_3_0 PSmain();
    }
}