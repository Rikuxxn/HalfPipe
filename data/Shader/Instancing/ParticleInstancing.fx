//=============================================================================
// 
// ParticleInstancing.fx
// Author : RIKU TANEKAWA
// 
//=============================================================================

float4x4 View;
float4x4 Projection;

texture Texture0;

sampler TextureSampler =
sampler_state
{
    Texture = <Texture0>;

    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = LINEAR;

    AddressU = CLAMP;
    AddressV = CLAMP;
};

struct VS_INPUT
{
    // Stream0
    float3 QuadPos : POSITION0;
    float2 Tex : TEXCOORD0;

    // Stream1
    float3 ParticlePos : TEXCOORD1;
    float Size : TEXCOORD2;

    float4 Color : COLOR0;
};

struct VS_OUTPUT
{
    float4 Pos : POSITION0;
    float2 Tex : TEXCOORD0;
    float4 Color : COLOR0;
};

VS_OUTPUT VS_Main(VS_INPUT In)
{
    VS_OUTPUT Out;

    //----------------------------------
    // View行列からカメラ軸取得
    //----------------------------------

    float3 right = float3(View[0][0], View[1][0], View[2][0]);

    float3 up = float3(View[0][1], View[1][1], View[2][1]);

    //----------------------------------
    // ビルボード頂点生成
    //----------------------------------

    float3 worldPos = In.ParticlePos +
        right * In.QuadPos.x * In.Size +
        up * In.QuadPos.y * In.Size;

    //----------------------------------
    // View変換
    //----------------------------------

    float4 viewPos = mul(float4(worldPos, 1.0f), View);

    //----------------------------------
    // Projection変換
    //----------------------------------

    Out.Pos = mul(viewPos, Projection);

    Out.Tex = In.Tex;
    Out.Color = In.Color;

    return Out;
}

float4 PS_Main(VS_OUTPUT In) : COLOR0
{
    float4 tex = tex2D(TextureSampler, In.Tex);

    return tex * In.Color;
}

//=============================================================================
// テクニック宣言
//=============================================================================
technique ParticleInstancing
{
    pass P0
    {
        ZEnable = TRUE;
        ZWriteEnable = FALSE;

        AlphaBlendEnable = TRUE;

        SrcBlend = SRCALPHA;
        DestBlend = ONE;


        VertexShader = compile vs_3_0 VS_Main();

        PixelShader = compile ps_3_0 PS_Main();
    }
}