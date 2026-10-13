//*********************************************************
// 
// FBX描画用ピクセルシェーダー
// Author Tetsuji Yamamoto
// 
//*********************************************************
#include "FBXVS.hlsl"

//*********************************************************
// ピクセルシェーダー
//*********************************************************
float4 PS_main(PSIn Input) : COLOR
{
	// マテリアルのAmbient取得
	float FactorAmbient = MaterialAmbient.w;
	float FactorDiffuse = MaterialDiffuse.w;
	float FactorEmissive = MaterialEmissive.w;
	float FactorTransParentColor = MaterialTransParentColor.w;
	
	// Factorでマテリアルカラー調節
	float4 ColorAmbient = MaterialAmbient * FactorAmbient;
	float4 ColorDiffuse = MaterialDiffuse * FactorDiffuse;
	float4 ColorEmissive = MaterialEmissive * FactorEmissive;
	float4 ColorTransParentColor = MaterialTransParentColor * FactorTransParentColor;

	// テクスチャカラーを求める
	float4 ColorTex = tex2D(TextureSampler, Input.Tex);
	
	// 内積計算で明るさを求める
	float3 LightVecN = normalize(LightVector.xyz);
	float fDot = max(dot(Input.Nor.xyz, LightVecN), 0.0f);
	
	// ベースカラー作成
	float4 ColorBace = Input.Col * ColorTex;
	
	// 拡散反射光作成
	float4 DiffusePart = ColorBace * ColorDiffuse * LightColor * fDot;
	
	// 環境光作成
	float4 AmbientPart = ColorBace * ColorAmbient;
	
	// 出力色作成
	float4 Output = (float4) 0;
	Output = DiffusePart + AmbientPart;
	Output.a = Input.Col.a * ColorTex.a;
	
	// 出力
	return Output;
}