//*********************************************************************
//
// Bloom Horizontal Blur Shader
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

// パラメータ
// 1.0f / テクスチャ幅
// 使い方 : effect->SetFloat("TexelWidth", 1.0f / renderTargetWidth);
float TexelWidth;

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
    float2 uv = Input.Tex;

    float4 color = float4(0, 0, 0, 0);

    // 横方向ガウシアンブラー
    color += tex2D(SceneSampler, uv + float2(-2.0 * TexelWidth, 0.0)) * 0.0545;
    color += tex2D(SceneSampler, uv + float2(-1.0 * TexelWidth, 0.0)) * 0.2442;
    color += tex2D(SceneSampler, uv) * 0.4026;
    color += tex2D(SceneSampler, uv + float2(1.0 * TexelWidth, 0.0)) * 0.2442;
    color += tex2D(SceneSampler, uv + float2(2.0 * TexelWidth, 0.0)) * 0.0545;

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