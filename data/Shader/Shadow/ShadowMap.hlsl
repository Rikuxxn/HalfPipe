//*********************************************************************
//
// ShadowMap Shader
// Author : RIKU TANEKAWA
//
//*********************************************************************

// 定数
float4x4 View;          // ビュー
float4x4 Projection;    // プロジェクション

// 頂点インプット情報
struct VS_INPUT
{
    float4 Position : POSITION0;

    float4 World0 : TEXCOORD1;
    float4 World1 : TEXCOORD2;
    float4 World2 : TEXCOORD3;
    float4 World3 : TEXCOORD4;
};

// 頂点アウトプット情報
struct VS_OUTPUT
{
    float4 Position : POSITION0;
    float Depth : TEXCOORD0;
};

//*********************************************************************
// 頂点シェーダメイン関数
//*********************************************************************
VS_OUTPUT VSMain(VS_INPUT input)
{
    VS_OUTPUT output;

    float4x4 world =
    {
        input.World0,
        input.World1,
        input.World2,
        input.World3
    };


    float4 pos = mul(input.Position, world);

    pos = mul(pos, View);

    pos = mul(pos, Projection);


    output.Position = pos;

    output.Depth = pos.z / pos.w;

    return output;
}

//*********************************************************************
// 深度エンコート
//*********************************************************************
float4 EncodeDepth(float depth)
{
    float4 enc = frac(depth * float4(
        1.0,
        255.0,
        65025.0,
        16581375.0));

    enc -= enc.yzww *
        float4(
            1.0 / 255.0,
            1.0 / 255.0,
            1.0 / 255.0,
            0);

    return enc;
}

//*********************************************************************
// ピクセルシェーダメイン関数
//*********************************************************************
float4 PSMain(VS_OUTPUT input) : COLOR0
{
    float depth = input.Depth * 0.5f + 0.5f;

    return float4(
        depth,
        depth,
        depth,
        1);
}

//*********************************************************************
// テクニック宣言
//*********************************************************************
technique Shadow
{
    pass P0
    {
        ZEnable = TRUE;
        ZWriteEnable = TRUE;

        CullMode = CW;

        VertexShader = compile vs_3_0 VSMain();
        PixelShader = compile ps_3_0 PSMain();
    }
}