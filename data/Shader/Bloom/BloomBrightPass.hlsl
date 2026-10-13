//*********************************************************************
//
// Bloom Bright Pass Shader
// Author : RIKU TANEKAWA
//
//*********************************************************************

// 頂点インプット情報
struct VSInput
{
    float4 Pos : POSITION; // 位置
    float2 Tex : TEXCOORD0; // UV座標
};

// 頂点アウトプット情報
struct VSOutput
{
    float4 Pos : POSITION; // 位置
    float2 Tex : TEXCOORD0; // UV座標
};

// グローバル変数
texture SceneTexture;


// サンプラ定義
sampler2D SceneSampler = sampler_state
{
    Texture = <SceneTexture>;

    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = NONE;
    AddressU = CLAMP;
    AddressV = CLAMP;
};

// パラメータ
float Threshold = 0.9f;

//*********************************************************************
// メイン関数
//*********************************************************************
VSOutput main(VSInput Input)
{
    VSOutput Output;

    Output.Pos = Input.Pos;
    Output.Tex = Input.Tex;

    return Output;
}

//*********************************************************************
// ピクセルシェーダー関数
//*********************************************************************
float4 PSmain(VSOutput Input) : COLOR0
{
    float4 color = tex2D(SceneSampler, Input.Tex);

    // 輝度計算
    float luminance = dot(color.rgb, float3(0.299f, 0.587f, 0.114f));

    // 明るい部分だけ残す
    if (luminance < Threshold)
    {
        color.rgb = 0.0f;
    }

    // 出力
    return color;
}

//*********************************************************
// テクニック宣言
//*********************************************************
technique Draw
{
	// 0
    pass P0
    {
        VertexShader = compile vs_3_0 main();
        PixelShader = compile ps_3_0 PSmain();
    }
}