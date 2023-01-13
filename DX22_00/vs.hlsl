
// 頂点シェーダーのメイン関数
// 役割：各頂点の対して行いたい移動・回転・縮小などを実行する
// 戻り値：このシェーダーの処理の結果を、頂点のデータとして戻す
// 引数 inputPos：VRAMに転送された頂点データ（のうち１つ）が渡される
//      C言語で設定した頂点座標と同じ座標系


// 定数バッファ
#include "cb.hlsl"

// return用の構造体
struct VS_OUTPUT
{
	float4 pos : SV_Position;
	float2 uv  : TEXCOORD;
	float4 col : COLOR;
	float4 nor : NORMAL;
};

// 頂点シェーダー　メイン関数（entrypoint）
// 戻り値：座標変換済みの頂点データ
VS_OUTPUT vs_main(float3 inputPos : POSITION, // xy
	/*float4 col : COLOR,*/ float2 uv : TEXCOORD, float3 normal : NORMAL) // uv
{
	VS_OUTPUT output;

	output.pos.xyz = inputPos.xyz;  // 渡されたxyとuvを変数に移す
	output.pos.w = 1.0f;
	output.uv = uv + gUvOffset.xy;
	output.col = float4(1,1,1,1);
	output.nor.xyz = normal;
	output.nor.w = 1.0f;

	// 画面比率で横長になってしまうのを調整する
	//output.pos.x *= 480.0f / 640.0f;

	// 頂点座標に行列をかける
	output.pos = mul(output.pos, gWorld); // ワールド変換行列
	output.pos = mul(output.pos, gView);
	output.pos = mul(output.pos, gProjection);

	//法線ベクトルに回転行列をかける。正規化も行う
	output.nor = normalize(mul(float4(normal.x, normal.y, normal.z, 1.0f), gWorldRotate));

	return output;
}