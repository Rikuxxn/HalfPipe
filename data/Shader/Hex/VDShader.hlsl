//******************************************
//
// 横ブラー処理 [xBlur.hlsl]
// Author:Sasaki Soichiro
//
//******************************************
// ブラーをかけるテクスチャの幅
static const float BLUR_TEX_W = 1280.0f;
// ブラーをかけるテクスチャの高さ
static const float BLUR_TEX_H = 720.0f;
float BLUR_RADIUS=8.0f;

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

//***********************************
// 頂点シェーダのインプット情報
//***********************************
struct VS_IN
{
    float4 pos : POSITION;
    float2 uv : TEXCOORD0;
};


//***********************************
// ピクセルシェーダのインプット情報
//***********************************
struct PS_IN
{
    float4 pos : POSITION;
    float2 uv : TEXCOORD0;
};

//***********************************
// ピクセルシェーダのアウトプット情報
//***********************************
struct PS_OUT
{
    float4 Vertical : COLOR0;
    float4 Diagonal : COLOR1;
};

//********************
// 頂点シェーダ
//********************
PS_IN VS(VS_IN In)
{
    PS_IN Out = (PS_IN) 0;
    Out = In;
    
    return Out;
}

//********************
// ピクセルシェーダ
//********************
PS_OUT PS(PS_IN In)
{
    PS_OUT psOut = (PS_OUT) 0;
    
    // ブラーの半径からブラーステップの長さを求める
    float blurStepLen = BLUR_RADIUS / 4.0f;
    float4 mainColor = tex2D(mainSampler, In.uv);
    
    // 垂直方向のuvオフセットを計算
    float2 uvOffset = float2(0.0f, 1.0f / BLUR_TEX_H);
    uvOffset *= blurStepLen;
    
    // 1ステップ進む
    psOut.Vertical += tex2D(mainSampler, In.uv + uvOffset);
    // 2ステップ進む
    psOut.Vertical += tex2D(mainSampler, In.uv + uvOffset * 2);
    // 3ステップ進む
    psOut.Vertical += tex2D(mainSampler, In.uv + uvOffset * 3);
    // 4ステップ進む
    psOut.Vertical += tex2D(mainSampler, In.uv + uvOffset * 4);
    
    // 平均化
    psOut.Vertical /= 4.0f;

    // 対角線方向のUVオフセットを計算
    uvOffset.x = 0.86602f / BLUR_TEX_W;
    uvOffset.y = -0.5f / BLUR_TEX_H;

    uvOffset *= blurStepLen;
    
    // 対角線方向にカラーをサンプリングして平均を求める
    psOut.Diagonal = tex2D(mainSampler, In.uv + uvOffset);
    psOut.Diagonal += tex2D(mainSampler, In.uv + uvOffset * 2);
    psOut.Diagonal += tex2D(mainSampler, In.uv + uvOffset * 3);
    psOut.Diagonal += tex2D(mainSampler, In.uv + uvOffset * 4);
    
    psOut.Diagonal += mainColor;
    psOut.Diagonal /= 5.0f;
    
    // 垂直方向に平均化した
    psOut.Diagonal += psOut.Vertical;
    psOut.Diagonal /= 2.0f;
    
    
    return psOut;
}

technique Basic
{
    pass P0
    {
        VertexShader = compile vs_3_0 VS();
        PixelShader = compile ps_3_0 PS();
    }
}