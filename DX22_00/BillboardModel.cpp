#include "BillboardModel.h"
#include "Direct3D.h"

using namespace DirectX;

extern ID3D11Buffer* gpConstBuffer;
/*
void BillboardModel::Draw()
{
	DIRECT3D* d3d = Direct3D_Get();

	// 回転行列
	XMMATRIX mxRotY = XMMatrixRotationY(
		XMConvertToRadians(mRotate.y));  // Y軸回転
	XMMATRIX mxRotX = XMMatrixRotationX(
		XMConvertToRadians(mRotate.x));
	XMMATRIX mxRotZ = XMMatrixRotationZ(
		XMConvertToRadians(mRotate.z));

	// 平行移動行列
	XMMATRIX mxTrans = XMMatrixTranslation(
		mPos.x, mPos.y, mPos.z);  // 立体の現在位置を表す変数を使って、平行移動の行列を作る

	// 拡大縮小行列
	XMMATRIX mxScale = XMMatrixScaling(
		mScale.x, mScale.y, mScale.z);

	// ビルボード処理
	// ビュー変換行列を取得
	XMMATRIX mxView = Camera::mMainCamera->GetViewMatrix();
	// 逆行列を作る
	XMMATRIX mxInverse =
		XMMatrixInverse(nullptr, mxView);
	// 逆行列の移動情報を取り除く
	mxInverse.r[3].m128_f32[0] = 0.0f;// ４行１列
	mxInverse.r[3].m128_f32[1] = 0.0f;// ４行２列
	mxInverse.r[3].m128_f32[2] = 0.0f;// ４行３列

	// ワールド変換行列を作成
	XMMATRIX mxWorld = mxScale * mxInverse * mxTrans;

	// プロジェクション変換行列を作成
	// 1: 視野角
	// 2: アスペクト比　→　基本、ウインドウサイズの縦横比を書けばよい
	// 3: near 見える範囲の一番手前の視点からの距離
	// 4: far  見える範囲の一番奥の視点からの距離
	XMMATRIX mxProjection = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f), 16.0f / 9.0f, 0.1f, 100.0f);

	// 描画で使用するテクスチャを指定する関数呼び出し
	// ここで指定されたテクスチャが、ピクセルシェーダーのグローバル変数にセットされる
	d3d->context->PSSetShaderResources(0, 1, &mModelData.mSRV);

	// 描画に使う頂点バッファを指定する
	UINT strides = sizeof(VERTEX3D);
	UINT offsets = 0;
	// 第三引数：頂点バッファ
	d3d->context->IASetVertexBuffers(0, 1,
		&mModelData.mVertexBuffer, &strides, &offsets);

	// 定数バッファ更新　→　Ｃ言語からシェーダーにデータを転送する
	D3D11_MAPPED_SUBRESOURCE pData;  // 定数バッファで確保されたメモリ領域の情報が格納される構造体
	// 定数バッファのメモリ領域にアクセスして、データを転送する処理
	// 定数バッファのメモリにアクセスするために、そのメモリ領域にロックをかける

	// カメラオブジェクトからビュー変換行列を取得
	XMMATRIX mxView = Camera::mMainCamera->GetViewMatrix();

	ConstBufferData cbdata;
	cbdata.world = XMMatrixTranspose(mxWorld);
	cbdata.view = XMMatrixTranspose(mxView);
	cbdata.projection = XMMatrixTranspose(mxProjection);

	HRESULT hr = d3d->context->Map(gpConstBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData);
	if (SUCCEEDED(hr)) {
		

		// 1: コピーしたい先のアドレス
		// 2: コピー先の確保されているデータサイズ
		// 3: コピーしたいデータが入っている先頭アドレス
		// 4: コピーしたいデータのサイズ
		memcpy_s(pData.pData, pData.RowPitch, &cbdata, sizeof(cbdata));

		d3d->context->Unmap(gpConstBuffer, 0);  // ロックしたメモリを解除する
	}

	// 定数バッファをシェーダーからアクセスできるようにセットする
	d3d->context->VSSetConstantBuffers(1, 1, &gpConstBuffer);

	// これまでの設定で実際に描画する  第一引数：実際に描画する頂点数
	d3d->context->Draw(mModelData.mNumVertex, 0);
}
*/
void BillboardModel::GetWVPRMatrix(ConstBufferData & out)
{
	// ビルボード処理
	// ビュー変換行列を取得
	XMMATRIX mxView = Camera::mMainCamera->GetViewMatrix();
	// 逆行列を作る
	XMMATRIX mxInverse =
		XMMatrixInverse(nullptr, mxView);
	// 逆行列の移動情報を取り除く
	mxInverse.r[3].m128_f32[0] = 0.0f;// ４行１列
	mxInverse.r[3].m128_f32[1] = 0.0f;// ４行２列
	mxInverse.r[3].m128_f32[2] = 0.0f;// ４行３列

	// 平行移動行列
	XMMATRIX mxTrans = XMMatrixTranslation(
		mPos.x, mPos.y, mPos.z);  // 立体の現在位置を表す変数を使って、平行移動の行列を作る

	// 拡大縮小行列
	XMMATRIX mxScale = XMMatrixScaling(
		mScale.x, mScale.y, mScale.z);

	// ワールド変換行列を作成
	XMMATRIX mxRotate = mxInverse;
	XMMATRIX mxWorld = mxScale * mxRotate * mxTrans;

	// プロジェクション変換行列を作成
	// 1: 視野角
	// 2: アスペクト比　→　基本、ウインドウサイズの縦横比を書けばよい
	// 3: near 見える範囲の一番手前の視点からの距離
	// 4: far  見える範囲の一番奥の視点からの距離
	XMMATRIX mxProjection = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f), 16.0f / 9.0f, 0.1f, 100.0f);

	// 定数バッファの転送用構造体にコピー　※この際、行列は転置行列にする
	out.world = XMMatrixTranspose(mxWorld);
	out.view = XMMatrixTranspose(mxView);
	out.projection = XMMatrixTranspose(mxProjection);
	out.worldRotate = XMMatrixTranspose(mxRotate);
}
