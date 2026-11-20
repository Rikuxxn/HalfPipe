//=============================================================================
// 
// ModelInstancing.fx
// Author : RIKU TANEKAWA
// 
//=============================================================================

float4x4 View;          // ビュー
float4x4 Projection;    // プロジェクション

// シャドウ
float4x4 LightView;
float4x4 LightProjection;

texture ShadowTexture;

float4 MatColor;    // マテリアルカラー
texture Texture0;   // テクスチャ

bool HasTexture;    // テクスチャを持っているか

float3 LightDir;
float4 Ambient = float4(0.25f, 0.25f, 0.25f, 1.0f);


sampler ShadowSampler = sampler_state
{
    Texture = <ShadowTexture>;

    MinFilter = LINEAR;
    MagFilter = LINEAR;

    AddressU = CLAMP;
    AddressV = CLAMP;
};

sampler TextureSampler = sampler_state
{
    Texture = <Texture0>;

    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = LINEAR;

    AddressU = CLAMP;
    AddressV = CLAMP;
    AddressW = CLAMP;
};

// 頂点インプット情報
struct VS_INPUT
{
    // Stream0(メッシュ)
    float3 Position : POSITION0;
    float3 Normal   : NORMAL0;
    float2 Tex      : TEXCOORD0;

    // Stream1(インスタンス)
    float4 World0 : TEXCOORD1;
    float4 World1 : TEXCOORD2;
    float4 World2 : TEXCOORD3;
    float4 World3 : TEXCOORD4;
};

// 頂点アウトプット情報
struct VS_OUTPUT
{
    float4 Pos      : POSITION0;
    float3 Normal   : NORMAL0;
    float2 Tex      : TEXCOORD0;
    
    float4 posInLVP : TEXCOORD1;
    float depthInView : TEXCOORD2;
};

struct PS_OUTPUT
{
    float4 color : COLOR0;
    float4 depth : COLOR1;
};


//=============================================================================
// 頂点メインエントリー
//=============================================================================
VS_OUTPUT VS_Main(VS_INPUT In)
{
    VS_OUTPUT Out;

    //----------------------------------
    // ワールド行列
    //----------------------------------

    float4x4 World =
    float4x4(
        In.World0,
        In.World1,
        In.World2,
        In.World3);

    //----------------------------------
    // ワールド位置
    //----------------------------------

    float4 worldPos = mul(float4(In.Position, 1.0f), World);
    
    //----------------------------------
    // Shadow座標
    //----------------------------------

    float4 shadowPos = mul(worldPos, LightView);

    shadowPos = mul(shadowPos, LightProjection);

    Out.posInLVP = shadowPos;
    
    //----------------------------------
    // View
    //----------------------------------

    float4 viewPos = mul(worldPos, View);
    
    Out.depthInView = viewPos.z;

    //----------------------------------
    // Projection
    //----------------------------------

    Out.Pos = mul(viewPos, Projection);

    //----------------------------------
    // 法線
    //----------------------------------

    float3x3 world3 = (float3x3) World;
    Out.Normal = normalize(mul(In.Normal, world3));

    //----------------------------------
    // UV
    //----------------------------------

    Out.Tex = In.Tex;

    return Out;
}

//=============================================================================
// ピクセルメインエントリー
//=============================================================================
PS_OUTPUT PS_Main(VS_OUTPUT Out) : COLOR0
{
    PS_OUTPUT psOut;
    
    float4 color = HasTexture ? tex2D(TextureSampler, Out.Tex) : MatColor;

    float3 N = normalize(Out.Normal);

    float diffuse = saturate(dot(N, -LightDir));

    float shadow = 1.0f;

    //---------------------------------
    // Shadow座標
    //---------------------------------
    
    // 透視除算
    float2 shadowMapUV =
    Out.posInLVP.xy /
    Out.posInLVP.w;

    //// -1～1 → 0～1
    //shadowMapUV.xy = shadowMapUV.xy * 0.5f + 0.5f;

    //// Y反転
    //shadowMapUV.y = 1.0f - shadowMapUV.y;
    
    shadowMapUV *= float2(0.5f, -0.5f);
    shadowMapUV += 0.5f;
    
    //---------------------------------
    // 現在深度
    //---------------------------------

    float zInLVP = Out.posInLVP.z / Out.posInLVP.w;
    
    //---------------------------------
    // 範囲外チェック
    //---------------------------------

    if (shadowMapUV.x > 0.0f &&
        shadowMapUV.x < 1.0f &&
        shadowMapUV.y > 0.0f &&
        shadowMapUV.y < 1.0f)
    {
        //---------------------------------
        // ShadowMap深度
        //---------------------------------

        float shadowMap = tex2D(ShadowSampler, shadowMapUV).r;

        //---------------------------------
        // バイアス
        //---------------------------------

        float3 lightNormal = normalize(-LightDir);

        float bias = 0.005f * (1.0f - saturate(dot(N, lightNormal)));

        bias = max(bias, 0.001f);

        //---------------------------------
        // 影判定
        //---------------------------------

        if (zInLVP > shadowMap)
        {
            shadow = 0.5f;
        }
    }

    //---------------------------------
    // ライティング
    //---------------------------------
    
    float4 light = Ambient + diffuse;

    float4 result = color * saturate(light);

    // RGBだけ暗くする
    result.rgb *= shadow;
    
    // アルファ維持
    result.a = color.a;

    psOut.color = result;
    psOut.depth = Out.depthInView;
    
    return psOut;
}

//=============================================================================
// テクニック宣言
//=============================================================================
technique ModelInstancing
{
    pass P0
    {
        VertexShader = compile vs_3_0 VS_Main();
        PixelShader = compile ps_3_0 PS_Main();
    }
}