// ピクセルシェーダーのメイン関数　→　ポリゴンを描画するのに必要なピクセル数だけ呼び出される
// 各ピクセルの色を指定するのが役目
// 戻り値：　このピクセルを何色にしたいか、RGBAで指定する
// 引数inputPos：　ピクセルの座標。ウインドウの左上を原点とするピクセル座標。

// テクスチャを受け取るグローバル変数
Texture2D gTexture : register(t0);

// サンプラーのグローバル変数
SamplerState gSampler : register(s0);

// 定数バッファ
#include "cb.hlsl"

float4 ps_main(float4 inputPos : SV_POSITION,
	float2 uv : TEXCOORD, float4 col : COLOR, float3 normal : NORMAL) : SV_Target
{
	// テクスチャのテクセル色を出力ピクセル色にする
	float4 pixelColor = gTexture.Sample(gSampler, uv);

	// ディフューズマテリアルの色を混ぜる
	pixelColor *= gDiffuse;

	// ライトの強さが０→ライト処理不要
	if (gLightPower <= 0.0f)
	{
		// 完全透過ピクセルなら出力しない→Zバッファも更新されない
		if (pixelColor.a == 0.0f)
			discard;

		return pixelColor;
	}


	//平行光源の処理
	float4 finalColor = pixelColor;
	float3 normalVec = normalize(normal);	//正規化された法線ベクトル				
	float3 lightVec = normalize(gLightVector);	//正規化された光の法線ベクトル

	//光の方向ベクトルと法線ベクトルの内積を計算する
	float brightness = dot(lightVec, normalVec);

	//求まった明るさをピクセル色にかける
	finalColor = pixelColor * brightness;
	//マイナスで暗くなりすぎた場合は0に戻す
	finalColor = saturate(finalColor);
	//アルファの値だけ元に戻す
	finalColor.a = pixelColor.a;

	//環境光の処理
	//最終的な色 = 平行光源計算済み色 + 環境光計算済み色
	//環境光計算済み色 = 環境光 * ライティング前の色
	float4 ambient = gAmbient * pixelColor;
	finalColor = finalColor + ambient;

	return finalColor;
}
