float Time = 0;

texture Texture0;
texture NoiseTexture;

sampler MainSampler = sampler_state
{
    Texture = <Texture0>;
};

sampler NoiseSampler = sampler_state
{
    Texture = <NoiseTexture>;
};

struct PS_INPUT
{
    float2 TexCoord : TEXCOORD0;
    float4 Color : COLOR0;
};

float4 PS(PS_INPUT input) : COLOR0
{
    //---------------------------------
    // ƒmƒCƒYUV
    //---------------------------------

    float2 noiseUV = input.TexCoord;

    noiseUV.y -= Time * 0.5;

    //---------------------------------
    // ƒmƒCƒYŽæ“¾
    //---------------------------------

    float2 noise = tex2D(NoiseSampler, noiseUV).rg;

    //---------------------------------
    // UV˜c‚Ý
    //---------------------------------

    float2 uv = input.TexCoord;

    float strength = input.TexCoord.y;

    uv.x += (noise.r - 0.5) * 0.15 * strength;
    uv.y += (noise.g - 0.5) * 0.03 * strength;

    float4 tex = tex2D(MainSampler, uv);

    tex.rgb *= lerp(1.5, 0.2, input.TexCoord.y);

    tex.a *= 1.0 - input.TexCoord.y;

    return tex * input.Color;
}

technique Technique0
{
    pass Pass0
    {
        PixelShader = compile ps_2_0 PS();
    }
}