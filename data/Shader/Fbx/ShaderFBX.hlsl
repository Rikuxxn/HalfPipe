//=============================================================================
// 
// ShaderFBX.hlsl
// Author : RIKU TANEKAWA
// 
//=============================================================================

// 頂点インプット情報
struct VSInput
{
	float4 Pos		: POSITION;		// 位置
	float4 Nor		: NORMAL;		// 法線
	float4 Col		: COLOR;		// 色
	float2 Tex		: TEXCOORD0;	// UV座標
	float2 IdxBone	: TEXCOORD1;	// ボーンインデックス
	float2 Weight	: TEXCOORD2;	// 重さ
};

// 頂点シェーダ入力情報
struct PSIn
{
	float4 Pos : POSITION;	// 位置
	float4 Nor : NORMAL;	// 法線
	float4 Col : COLOR;		// 色
	float2 Tex : TEXCOORD;	// UV座標
};

// 頂点アウトプット情報
struct VSOutput
{
	float4 Pos : POSITION;	// 位置
	float4 Nor : NORMAL;	// 法線
	float4 Col : COLOR;		// 色
	float2 Tex : TEXCOORD;	// UV座標
};

// グローバル変数
float4x4	MtxWorld;					// ワールド変換行列
float4x4	MtxView;					// ビュー変換行列
float4x4	MtxProj;					// プロジェクションマトリックス(透視射影変換行列)
float4		LightVector;				// ライトベクトル
float4		LightColor;					// ライトカラー
float4		MaterialAmbient;			// Ambient
float4		MaterialDiffuse;			// Diffuse
float4		MaterialEmissive;			// Emissive
float4		MaterialTransParentColor;	// TransParentColor
texture		Texture0;					// テクスチャ
float4x4	MtxMeshInit;				// メッシュの初期行列
float4x4	MtxBones[30];				// ボーンマトリックス
float4x4	MtxBonesInit[30];			// ボーン初期マトリックス
bool		HasTexture;					// テクスチャがあるか
	
// サンプラ定義
sampler TextureSampler = sampler_state
{
	Texture = <Texture0>;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = None;
	AddressU = Wrap;
	AddressV = Wrap;
};

//=============================================================================
// 頂点シェーダーボーンあり
//=============================================================================
VSOutput main(VSInput Input)
{
    VSOutput Output;

    Output.Pos = float4(0, 0, 0, 1);
    Output.Nor = float4(0, 0, 0, 0);

    int nIdxBoneA = (int) Input.IdxBone.x;
    int nIdxBoneB = (int) Input.IdxBone.y;

    float4 PosInit = mul(Input.Pos, MtxMeshInit);

    float4 PosInversA = float4(0, 0, 0, 1);
    float4 PosInversB = float4(0, 0, 0, 1);

    PosInversA.xyz = PosInit.xyz - MtxBonesInit[nIdxBoneA]._41_42_43;
    PosInversB.xyz = PosInit.xyz - MtxBonesInit[nIdxBoneB]._41_42_43;

    //==================================================
    // ボーン2本をウェイトでブレンド
    //==================================================

    float4 PosA = mul(PosInversA, MtxBones[nIdxBoneA]);
    float4 PosB = mul(PosInversB, MtxBones[nIdxBoneB]);

    Output.Pos = PosA * Input.Weight.x + PosB * Input.Weight.y;

    //==================================================
    // World / View / Projection
    //==================================================

    Output.Pos = mul(Output.Pos, MtxWorld);
    Output.Pos = mul(Output.Pos, MtxView);
    Output.Pos = mul(Output.Pos, MtxProj);

    //==================================================
    // 法線
    //==================================================

    float3 MeshInitNor = mul(Input.Nor.xyz, (float3x3) MtxMeshInit);
	
    float3 NorA = mul(MeshInitNor, (float3x3) MtxBones[nIdxBoneA]);
    float3 NorB = mul(MeshInitNor, (float3x3) MtxBones[nIdxBoneB]);

    Output.Nor.xyz = NorA * Input.Weight.x + NorB * Input.Weight.y;
    Output.Nor.xyz = mul(Output.Nor.xyz, (float3x3) MtxWorld);
    Output.Nor.xyz = normalize(Output.Nor.xyz);
    Output.Nor.w = 0.0f;

    Output.Col = Input.Col;
    Output.Tex = Input.Tex;

    return Output;
}

//=============================================================================
// 頂点シェーダーボーン無し
//=============================================================================
VSOutput NoBone(VSInput Input)
{
	// 出力用変数
	VSOutput Output;
	
	// 座標変換(ワールド,ビュー,プロジェクション)
	Output.Pos = mul(Input.Pos, MtxMeshInit);
	Output.Pos = mul(Output.Pos, MtxWorld);
	Output.Pos = mul(Output.Pos, MtxView);
	Output.Pos = mul(Output.Pos, MtxProj);

	// ワールド座標へ変換した法線の算出
	float3 Normal = mul(Input.Nor.xyz, (float3x3) MtxMeshInit);
	Normal = mul(Normal, (float3x3) MtxWorld);
	Normal = normalize(Normal);
	Output.Nor.xyz = Normal;
	Output.Nor.w = 0.0f;
	
	// 色設定
	Output.Col = Input.Col;
	
	// テクスチャ座標設定
	Output.Tex = Input.Tex;
	
	// 出力
	return Output;
}

//=============================================================================
// ピクセルシェーダー
//=============================================================================
float4 PS_main(PSIn Input) : COLOR
{
    float3 Normal = normalize(Input.Nor.xyz);
    float3 LightVecN = normalize(LightVector.xyz);

    float fDot = saturate(dot(Normal, LightVecN));

    //=========================================================
    // ベースカラー
    //=========================================================

    float4 TexColor = float4(1.0f, 1.0f, 1.0f, 1.0f);

    if (HasTexture)
    {
        TexColor = tex2D(TextureSampler, Input.Tex);
    }

    float3 BaseColor =
        TexColor.rgb *
        MaterialDiffuse.rgb;

    //=========================================================
    // Diffuse
    //=========================================================

    float3 Diffuse =
        BaseColor *
        LightColor.rgb *
        fDot;

    //=========================================================
    // Ambient
    //=========================================================

    float3 Ambient =
        BaseColor *
        MaterialAmbient.rgb;

    //=========================================================
    // Emissive
    //=========================================================

    float3 Emissive =
        MaterialEmissive.rgb;

    //=========================================================
    // 最終カラー
    //=========================================================

    float3 FinalColor =
        Diffuse +
        Ambient +
        Emissive;

    return float4(
        saturate(FinalColor),
        TexColor.a
    );
	
    //return MaterialDiffuse;
}

//=============================================================================
// ピクセルシェーダー(ライティング無し)
//=============================================================================
float4 PS_LightingNone(PSIn Input) : COLOR
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
	
	// ベースカラー作成
	float4 ColorBace = Input.Col * ColorTex;
	
	// 拡散反射光作成
	float4 DiffusePart = ColorBace * ColorDiffuse * LightColor;
	
	// 環境光作成
	float4 AmbientPart = ColorBace * ColorAmbient;
	
	// 出力色作成
	float4 Output = (float4) 0;
	Output = DiffusePart + AmbientPart;
	Output.a = Input.Col.a * ColorTex.a;
	
	// 出力
	return Output;
}

//=============================================================================
// テクニック宣言
//=============================================================================
technique Draw
{
	// ボーンありライティングあり
	pass P0
	{
		VertexShader = compile vs_3_0 main();
		PixelShader = compile ps_3_0 PS_main();
	}
	
	// ボーン無しライティングあり
	pass P1
	{
		VertexShader = compile vs_3_0 NoBone();
		PixelShader = compile ps_3_0 PS_main();
	}
	
	// ボーンありライティング無し
	pass P2
	{
		VertexShader = compile vs_3_0 main();
		PixelShader = compile ps_3_0 PS_LightingNone();
	}
	
	// ボーンなしライティング無し
	pass P3
	{
		VertexShader = compile vs_3_0 NoBone();
		PixelShader = compile ps_3_0 PS_LightingNone();
	}
}
