//******************************************
//
// 被写界深度処理 [DoF.hlsl]
// Author:Sasaki Soichiro
//
//******************************************

//数値
float g_fGaussStart; // ボケの始まり
float g_fGaussEnd; // 完全なボケ

// ガウシアンのテクスチャ
texture g_mainTexture;
sampler mainSampler = sampler_state
{
    texture = <g_mainTexture>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    AddressU = CLAMP;
    AddressV = CLAMP;
};


// ガウシアンのテクスチャ
texture g_gaussianTexture;
sampler gaussianSampler = sampler_state
{
    texture = <g_gaussianTexture>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    AddressU = CLAMP;
    AddressV = CLAMP;
};

// Z値のテクスチャ
texture g_depthTexture;
sampler depthSampler = sampler_state
{
    texture = <g_depthTexture>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    AddressU = CLAMP;
    AddressV = CLAMP;
};

//***********************************
// ピクセルシェーダのインプット情報
//***********************************
struct PS_IN
{
    float4 pos : POSITION;
    float2 uv : TEXCOORD0;
};

//********************
// ピクセルシェーダ
//********************
float4 PS(PS_IN In) : COLOR0
{
    float depth = tex2D(depthSampler, In.uv).r;

    float4 main = tex2D(mainSampler, In.uv);
    float4 gaussian = tex2D(gaussianSampler, In.uv);

    float blurAmount = saturate((depth - g_fGaussStart) / g_fGaussEnd);

    return lerp(main, gaussian, blurAmount);
}

technique Basic
{
    pass P0
    {
        PixelShader = compile ps_3_0 PS();
    }
}