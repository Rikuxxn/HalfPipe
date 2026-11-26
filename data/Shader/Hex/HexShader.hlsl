//******************************************
//
// 六角形ブラー処理 [hexBlur.hlsl]
// Author:Sasaki Soichiro
//
//******************************************
// ブラーをかけるテクスチャの幅
static const float BLUR_TEX_W = 1280.0f;
// ブラーをかけるテクスチャの高さ
static const float BLUR_TEX_H = 720.0f;
float BLUR_RADIUS = 1.0f;

// 垂直のテクスチャ
texture g_verticalTexture;
sampler verticalSampler = sampler_state
{
    texture = <g_verticalTexture>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};
// 対角線のテクスチャ
texture g_diagonalTexture;
sampler diagonalSampler = sampler_state
{
    texture = <g_diagonalTexture>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};
//**********************************
// 頂点シェーダのインプット情報
//**********************************
struct VS_IN
{
    float4 pos : POSITION;
    float2 uv : TEXCOORD0;
};

//***********************************
// 頂点シェーダのアウトプット情報
//***********************************
struct VS_OUT
{
    float4 pos : POSITION;
    float2 uv : TEXCOORD0;
};

//********************
// 頂点シェーダ
//********************
VS_OUT VS(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    Out = In;
    
    return Out;
}

//********************
// ピクセルシェーダ
//********************
float4 PS(VS_OUT In) : COLOR0
{
    float2 uvOffset;
    uvOffset.x = 0.86602f / BLUR_TEX_W;
    uvOffset.y = -0.5f / BLUR_TEX_H;
    // ブラーの半径からブラーステップの長さを求める
    float blurStepLen = BLUR_RADIUS / 4.0f;
    uvOffset *= blurStepLen;
    
    float4 color = tex2D(verticalSampler, In.uv + uvOffset);
    color += tex2D(verticalSampler, In.uv + uvOffset * 2);
    color += tex2D(verticalSampler, In.uv + uvOffset * 3);
    color += tex2D(verticalSampler, In.uv + uvOffset * 4);
    
    // 左斜め下方向へのuvオフセットを計算する
    uvOffset.x = -0.86602f / BLUR_TEX_W * blurStepLen;
    //左斜め下方向にカラーをサンプリングする
    color += tex2D(diagonalSampler, In.uv);
    color += tex2D(diagonalSampler, In.uv + uvOffset);
    color += tex2D(diagonalSampler, In.uv + uvOffset * 2);
    color += tex2D(diagonalSampler, In.uv + uvOffset * 3);
    color += tex2D(diagonalSampler, In.uv + uvOffset * 4);
    
    color /= 9.0f;

    return color;
}

technique Basic
{
    pass P0
    {
        VertexShader = compile vs_3_0 VS();
        PixelShader = compile ps_3_0 PS();
    }
}