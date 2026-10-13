//*********************************************************************
//
// Bloom Combine Shader
// Author : RIKU TANEKAWA
//
//*********************************************************************

// 頂点インプット情報
struct VSInput
{
    float4 Pos : POSITION;  // 位置
    float2 Tex : TEXCOORD0; // UV座標
};

// 頂点アウトプット情報
struct VSOutput
{
    float4 Pos : POSITION;  // 位置
    float2 Tex : TEXCOORD0; // UV座標
};

// グローバル変数
texture SceneTexture;
texture BloomTexture;

// サンプラ定義
sampler2D SceneSampler = sampler_state
{
    Texture = <SceneTexture>;

    MinFilter   = LINEAR;
    MagFilter   = LINEAR;
    MipFilter   = NONE;
    AddressU    = CLAMP;
    AddressV    = CLAMP;
};
sampler2D BloomSampler = sampler_state
{
    Texture = <BloomTexture>;

    MinFilter   = LINEAR;
    MagFilter   = LINEAR;
    MipFilter   = NONE;
    AddressU    = CLAMP;
    AddressV    = CLAMP;
};

// ブルームの強さ
float BloomPower = 1.0f;

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
    float4 scene = tex2D(SceneSampler, Input.Tex);

    float4 bloom = tex2D(BloomSampler, Input.Tex);

    // 加算合成
    float4 color = scene + bloom * BloomPower;

    // 0～1に収める
    color = saturate(color);

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