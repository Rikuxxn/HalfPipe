//*********************************************************************
//
// Sky Shader
// Author : RIKU TANEKAWA
//
//*********************************************************************

// 定数
float4x4 World;         // ワールド行列
float4x4 View;          // ビュー行列
float4x4 Projection;    // プロジェクション行列

float3 CameraPos;       // カメラの位置

float TimeOfDay;        // 0.0～1.0

// 太陽
float SunRadius;        // 太陽の半径
float SunIntensity;     // 太陽の強さ

// 月
float MoonRadius;       // 月の半径
float MoonIntensity;    // 月の強さ

// 雲
float CloudTime;        // 時間

// 頂点インプット情報
struct VSInput
{
    float4 Pos : POSITION; // 位置
};

// 頂点アウトプット情報
struct VSOutput
{
    float4 Pos      : POSITION;
    float3 WorldPos : TEXCOORD0;
};

//*********************************************************************
// メイン関数
//*********************************************************************
VSOutput main(VSInput Input)
{
    VSOutput Output;

    float4 worldPos = mul(Input.Pos, World);
    
    Output.WorldPos = worldPos.xyz;
    
    Output.Pos = mul(worldPos, View);
    Output.Pos = mul(Output.Pos, Projection);

    return Output;
}

//*********************************************************
// 星用の乱数関数
//*********************************************************
float hash(float2 p)
{
    return frac(sin(dot(p, float2(12.9898, 78.233))) * 43758.5453);
}

//*********************************************************
// 3D雲用乱数
//*********************************************************
float HashNoise3D(float3 p)
{
    return frac(
        sin(dot(p, float3(
            12.9898,
            78.233,
            37.719
        )))
        * 43758.5453
    );
}

//*********************************************************
// 3D補間ノイズ関数
//*********************************************************
float Noise3D(float3 p)
{
    float3 i = floor(p);
    float3 f = frac(p);

    f = f * f * (3.0 - 2.0 * f);

    float n000 = HashNoise3D(i + float3(0, 0, 0));
    float n100 = HashNoise3D(i + float3(1, 0, 0));
    float n010 = HashNoise3D(i + float3(0, 1, 0));
    float n110 = HashNoise3D(i + float3(1, 1, 0));

    float n001 = HashNoise3D(i + float3(0, 0, 1));
    float n101 = HashNoise3D(i + float3(1, 0, 1));
    float n011 = HashNoise3D(i + float3(0, 1, 1));
    float n111 = HashNoise3D(i + float3(1, 1, 1));


    float x00 = lerp(n000, n100, f.x);
    float x10 = lerp(n010, n110, f.x);
    float x01 = lerp(n001, n101, f.x);
    float x11 = lerp(n011, n111, f.x);


    float y0 = lerp(x00, x10, f.y);
    float y1 = lerp(x01, x11, f.y);


    return lerp(y0, y1, f.z);
}

//*********************************************************
// 3D複数ノイズ合成(フラクタルブラウン運動)
//*********************************************************
float FBM3D(float3 p)
{
    float value = 0.0;

    value += Noise3D(p) * 0.7;

    p *= 2.0;

    value += Noise3D(p) * 0.2;

    p *= 2.0;

    value += Noise3D(p) * 0.1;

    return value;
}

//*********************************************************
// 雲関数
//*********************************************************
float GetCloud(float3 viewDir)
{
    float3 p = normalize(viewDir);

    //----------------------------------
    // 雲の移動
    //----------------------------------
    p.x += CloudTime * 0.02;
    p.z += CloudTime * 0.01;
    
    // 雲の大きさ
    p *= 1.5;

    float n = FBM3D(p);

    float cloud = smoothstep(0.5, 0.62, n);
    
    //----------------------------------
    // 雲の高さ制限
    //----------------------------------

    // 地平線付近から出す
    float lowerFade = smoothstep(0.05, 0.25, viewDir.y);

    // 天頂側で消す
    float upperFade = 1.0 - smoothstep(0.55, 0.85, viewDir.y);

    cloud *= lowerFade * upperFade;
    
    return cloud;
}

//*********************************************************
// 時間から太陽の方向を計算して返す
//*********************************************************
float3 GetSunDirection()
{
    float angle = TimeOfDay * 6.2831853 - 1.5707963;

    float3 dir;

    dir.x = cos(angle);
    dir.y = sin(angle);
    dir.z = -0.2;

    return normalize(dir);
}

//*********************************************************
// 時間から月の方向を計算して返す
//*********************************************************
float3 GetMoonDirection()
{
    float angle = TimeOfDay * 6.2831853 + 3.1415926 - 1.5707963;

    float3 dir;

    dir.x = cos(angle);
    dir.y = sin(angle);
    dir.z = -0.2;

    return normalize(dir);
}

//*********************************************************
// 空の色をの方向を計算して返す
//*********************************************************
float3 GetSkyColor(float3 viewDir, float3 sunDir)
{
    float day = saturate(sunDir.y * 0.5 + 0.5);

    float sunsetFactor = 1.0 - abs(sunDir.y);

    sunsetFactor = saturate(sunsetFactor * 2.0);

    float nightFade = saturate(sunDir.y * 5.0 + 1.0);

    sunsetFactor *= nightFade;

    sunsetFactor = pow(sunsetFactor, 2.0);


    float3 horizonDay   = float3(0.85, 0.92, 1.0);
    float3 middleDay    = float3(0.45, 0.70, 1.0);
    float3 zenithDay    = float3(0.08, 0.25, 0.75);


    float3 horizonNight = float3(0.02, 0.02, 0.04);
    float3 middleNight  = float3(0.01, 0.02, 0.08);
    float3 zenithNight  = float3(0, 0, 0.02);


    float height = saturate(viewDir.y * 0.5 + 0.5);

    float3 horizon  = lerp(horizonNight, horizonDay, day);
    float3 middle   = lerp(middleNight, middleDay, day);
    float3 zenith   = lerp(zenithNight, zenithDay, day);

    horizon = lerp(horizon, float3(1, 0.35, 0.15), sunsetFactor);
    middle  = lerp(middle, float3(0.7, 0.35, 0.3), sunsetFactor * 0.5);
    zenith  = lerp(zenith, float3(0.25, 0.15, 0.35), sunsetFactor * 0.3);

    if (height < 0.5)
    {
        return lerp(horizon, middle, height * 2);
    }
        
    return lerp(middle, zenith, (height - 0.5) * 2);
}

//*********************************************************************
// ピクセルシェーダー関数
//*********************************************************************
float4 PSmain(VSOutput Input) : COLOR0
{
    //----------------------------------
    // 視線方向
    //----------------------------------

    float3 viewDir = normalize(Input.WorldPos - CameraPos);

    //----------------------------------
    // 太陽方向を取得
    //----------------------------------

    float3 SunDir = GetSunDirection();

    //----------------------------------
    // 月方向を取得
    //----------------------------------

    float3 MoonDir = GetMoonDirection();

    //----------------------------------
    // 太陽の色
    //----------------------------------

    float sunset = saturate(1.0 - abs(SunDir.y));
    sunset = pow(sunset, 2.0);

    // 太陽が沈んだ後は消す
    sunset *= saturate(SunDir.y * 5.0 + 1.0);

    float3 SunColor = lerp(float3(1.0, 0.98, 0.95), float3(1.0, 0.45, 0.15), sunset);
    
    //----------------------------------
    // 月の色
    //----------------------------------
    
    float3 MoonColor = float3(0.6, 0.6, 0.3);
    
    //----------------------------------
    // 空色
    //----------------------------------

    float3 color = GetSkyColor(viewDir, SunDir);
    
    //----------------------------------
    // 雲
    //----------------------------------

    float cloud = GetCloud(viewDir);

    float3 cloudColor = float3(0.92, 0.92, 0.92);

    // 昼夜で雲の色変更
    float cloudBrightness = saturate(SunDir.y * 0.5 + 0.5);

    cloudColor *= lerp(
    float3(0.15, 0.15, 0.2),
    float3(1, 1, 1),
    cloudBrightness
    );

    color = lerp(
    color,
    cloudColor,
    cloud * 0.95
    );
    
    //----------------------------------
    // 星
    //----------------------------------

    float3 starDir = normalize(viewDir);

    // 空を2D化
    float2 starUV;

    starUV.x = atan2(starDir.z, starDir.x);
    starUV.y = asin(starDir.y);

    // 密度調整
    starUV *= 60.0;

    // グリッド化
    float2 cell = floor(starUV);

    float2 cellUV = frac(starUV) - 0.5;
    
    float dist = length(cellUV);
    
    float star = hash(cell);

    float starShape = smoothstep(0.12, 0.0, dist);
    
    // 星を点にする
    star *= step(0.995, star);
    star *= starShape;
    
    //----------------------------------
    // 太陽との角度
    //----------------------------------

    float cosTheta = dot(viewDir, normalize(SunDir));

    //----------------------------------
    // 月との角度
    //----------------------------------

    float moonTheta = dot(viewDir, MoonDir);

    //----------------------------------
    // 月ディスク
    //----------------------------------

    float moon = smoothstep(
        1.0 - MoonRadius,
        1.0 - MoonRadius + 0.0005,
        moonTheta);

    //----------------------------------
    // 月の中心
    //----------------------------------

    float moonCore = pow(saturate(moonTheta), 128.0);
    
    //----------------------------------
    // 太陽ディスク
    //----------------------------------

    float edge = cos(SunRadius);

    float sun = smoothstep(edge, edge + 0.0005, cosTheta);

    //----------------------------------
    // ハロー
    //----------------------------------

    float halo = pow(saturate(cosTheta), 512.0);

    halo += pow(saturate(cosTheta), 128.0) * 0.05;

    // 地平線付近では弱める
    halo *= smoothstep(-0.3, 0.4, SunDir.y);
    
    //----------------------------------
    // 中心だけより明るく
    //----------------------------------

    float core = pow(saturate(cosTheta), 2048.0);
    
    //----------------------------------
    // 夜は太陽を消して、月を出す
    //----------------------------------

    float visible = smoothstep(-0.05, 0.05, SunDir.y);
    
    float moonVisible = smoothstep(-0.05, 0.05, MoonDir.y);
    
    //----------------------------------
    // 合成
    //----------------------------------

    color += SunColor * sun * SunIntensity * visible;
    color += SunColor * halo * 2.5 * visible;
    color += SunColor * core * (SunIntensity * 2.0 * visible);

    //----------------------------------
    // 月
    //----------------------------------
    
    color += MoonColor * moon * MoonIntensity * moonVisible;
    color += MoonColor * moonCore * MoonIntensity * moonVisible;
    
    //----------------------------------
    // 星の描画
    //----------------------------------

    float3 starColor = float3(1.0, 1.0, 0.9);
    float starVisible = saturate(viewDir.y * 0.5 + 0.5);

    starVisible = pow(starVisible, 0.5);
    
    float starRandom = hash(cell);

    float brightness = lerp(0.3, 1.5, starRandom);
    
    color += starColor * star * brightness * moonVisible * starVisible * 1.5;
    
    //----------------------------------
    // 雲に色を合成
    //----------------------------------
    
    return float4(color, 1);
}

//*********************************************************
// テクニック宣言
//*********************************************************
technique Sky
{
	// 0
    pass P0
    {
        VertexShader    = compile vs_3_0 main();
        PixelShader     = compile ps_3_0 PSmain();
    }
}