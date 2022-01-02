#include "BillBoard.h"
#include "Application.h"
#include "Structures.h"

using namespace DirectX;

BillBoard::BillBoard(DirectX::XMFLOAT3 position, int width, int height, wchar_t* textureFilePath, Application* app)
{
	mPosition = position;
	mWidth = width;
	mHeight = height;
	mTextureFilePath = textureFilePath;
	mApp = app;

	mStride = sizeof(XMFLOAT3);
	mOffset = 0;

	mCB = new BillBoardConstantBuffer;

	mCB->Size = XMFLOAT2(mWidth, mHeight);

	mCB->FogEnabled = true;
	mCB->FogStart = mApp->GetFog()->FogStart;
	mCB->FogRange = mApp->GetFog()->FogRange;
	mCB->FogColour = mApp->GetFog()->FogColour;

	mStride = sizeof(SimpleVertex);
	mOffset = 0;
}

BillBoard::~BillBoard()
{
	//mApp = nullptr;
	//mTextureFilePath = nullptr;
	//if (mTexture) mTexture->Release();
	//if (mPixelShader) mPixelShader->Release();
	//if (mVertexShader) mVertexShader->Release();
	//if (mGeometryShader) mGeometryShader->Release();
	//if (mCB) delete mCB; mCB = nullptr;
	//if (mConstantBuffer) mConstantBuffer->Release();
	////if (mVertexBuffer) mVertexBuffer->Release();
}

void BillBoard::Load()
{
	HRESULT hr = CreateDDSTextureFromFile(mApp->GetDevice(), mTextureFilePath, nullptr, &mTexture);

	if (FAILED(hr))
	{
		MessageBox(nullptr,
			L"The texture couldn't be loaded.", L"Error", MB_OK);
	}

	// Create the constant buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(BillBoardConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	hr = mApp->GetDevice()->CreateBuffer(&bd, nullptr, &mConstantBuffer);

	//Compile vertex shader
	ID3DBlob* pVSBlob = nullptr;
	hr = mApp->CompileShaderFromFile(L"Billboarding.fx", "VS", "vs_4_0", &pVSBlob);

	if (FAILED(hr))
	{
		MessageBox(nullptr,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
	}

	mApp->GetDevice()->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &mVertexShader);

	//Compile Geometry shader
	ID3DBlob* pGSBlob = nullptr;
	hr = mApp->CompileShaderFromFile(L"Billboarding.fx", "GS", "gs_4_0", &pGSBlob);

	if (FAILED(hr))
	{
		MessageBox(nullptr,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
	}

	hr = mApp->GetDevice()->CreateGeometryShader(pGSBlob->GetBufferPointer(), pGSBlob->GetBufferSize(), nullptr, &mGeometryShader);
	pGSBlob->Release();

	//Compile the pixel shader
	ID3DBlob* pPSBlob = nullptr;
	hr = mApp->CompileShaderFromFile(L"Billboarding.fx", "PS", "ps_4_0", &pPSBlob);

	if (FAILED(hr))
	{
		MessageBox(nullptr,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
	}

	hr = mApp->GetDevice()->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &mPixelShader);
	pPSBlob->Release();

	// Define the input layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	UINT numElements = ARRAYSIZE(layout);

	// Create the input layout
	hr = mApp->GetDevice()->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &mInputLayout);
	pVSBlob->Release();

	D3D11_BUFFER_DESC vertexBD;
	ZeroMemory(&vertexBD, sizeof(vertexBD));
	vertexBD.Usage = D3D11_USAGE_DEFAULT;
	vertexBD.ByteWidth = sizeof(SimpleVertex);
	vertexBD.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBD.CPUAccessFlags = 0;

	SimpleVertex vertices[] =
	{
		{XMFLOAT3(0,0,0), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f)}
	};

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = vertices;
	hr = mApp->GetDevice()->CreateBuffer(&vertexBD, &InitData, &mVertexBuffer);
}

void BillBoard::Draw()
{
	mApp->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	mApp->GetDeviceContext()->IAGetInputLayout(&mInputLayout);

	XMFLOAT4X4 transformationMatrix;
	XMStoreFloat4x4(&transformationMatrix, XMMatrixTranslationFromVector(XMLoadFloat3(&mPosition)));

	mCB->World = XMMatrixTranspose(XMLoadFloat4x4(&transformationMatrix));
	mCB->View = XMMatrixTranspose(XMLoadFloat4x4(&mApp->GetSelectedCamera()->GetViewMatrix()));
	mCB->Projection = XMMatrixTranspose(XMLoadFloat4x4(&mApp->GetSelectedCamera()->GetProjectionMatrix()));
	mCB->CameraPositon = mApp->GetSelectedCamera()->GetPosition();

	//Map constant buffer
	D3D11_MAPPED_SUBRESOURCE resource;

	mApp->GetDeviceContext()->Map(mConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	memcpy(resource.pData, mCB, sizeof(BillBoardConstantBuffer));
	mApp->GetDeviceContext()->Unmap(mConstantBuffer, 0);

	mApp->GetDeviceContext()->PSSetShaderResources(0, 1, &mTexture);

	mApp->GetDeviceContext()->IASetVertexBuffers(0, 1, &mVertexBuffer, &mStride, &mOffset);

	mApp->GetDeviceContext()->VSSetShader(mVertexShader, nullptr, 0);
	mApp->GetDeviceContext()->GSSetShader(mGeometryShader, nullptr, 0);
	mApp->GetDeviceContext()->PSSetShader(mPixelShader, nullptr, 0);

	mApp->GetDeviceContext()->VSSetConstantBuffers(0, 1, &mConstantBuffer);
	mApp->GetDeviceContext()->GSSetConstantBuffers(0, 1, &mConstantBuffer);
	mApp->GetDeviceContext()->PSSetConstantBuffers(0, 1, &mConstantBuffer);

	mApp->GetDeviceContext()->RSSetState(mApp->GetNoCull());
	mApp->GetDeviceContext()->Draw(1, 0);

	//Reset changed states
	ID3D11InputLayout* tempLayout = mApp->GetInputLayout();

	mApp->GetDeviceContext()->GSSetShader(NULL, 0, 0);
	mApp->GetDeviceContext()->RSSetState(mApp->GetRenderState());
	mApp->GetDeviceContext()->IAGetInputLayout(&tempLayout);
	mApp->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void BillBoard::SetFogEnabled(bool enabled)
{
	mCB->FogEnabled = enabled;
}
