#include "SkyBox.h"
#include "Application.h"

SkyBox::SkyBox(XMFLOAT3 position, char* meshFilePath, wchar_t* textureFilePath, Application* app)
{
	mPosition = position;

	mMeshFilePath = meshFilePath;
	mTextureFilePath = textureFilePath;
	
	mApp = app;

	mCB = new SkyBoxConstantBuffer();

	mCB->FogEnabled = true;
	mCB->FogStart = mApp->GetFog()->FogStart;
	mCB->FogRange = mApp->GetFog()->FogRange;
	mCB->FogColour = mApp->GetFog()->FogColour;

	mInputLayout = nullptr;
	mPixelShader = nullptr;
	mVertexShader = nullptr;
	mTexture = nullptr;

	//Create CB
	D3D11_BUFFER_DESC skyBoxbd;
	ZeroMemory(&skyBoxbd, sizeof(skyBoxbd));
	skyBoxbd.Usage = D3D11_USAGE_DYNAMIC;
	skyBoxbd.ByteWidth = sizeof(SkyBoxConstantBuffer);
	skyBoxbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	skyBoxbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	mApp->GetDevice()->CreateBuffer(&skyBoxbd, nullptr, &mConstantBuffer);

	//Create Depth stencil state
	D3D11_DEPTH_STENCIL_DESC dssDesc;
	ZeroMemory(&dssDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	dssDesc.DepthEnable = true;
	dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dssDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	mApp->GetDevice()->CreateDepthStencilState(&dssDesc, &mDepthStencilState);

	//Create raster state
	D3D11_RASTERIZER_DESC sbdesc;
	ZeroMemory(&sbdesc, sizeof(D3D11_RASTERIZER_DESC));
	sbdesc.FillMode = D3D11_FILL_SOLID;
	sbdesc.CullMode = D3D11_CULL_NONE;
	mApp->GetDevice()->CreateRasterizerState(&sbdesc, &mRasterState);

	//Create sampler
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	mApp->GetDevice()->CreateSamplerState(&sampDesc, &mSamplerState);
}

SkyBox::~SkyBox()
{
	mApp = nullptr;
	delete mCB; mCB = nullptr;
	if (mInputLayout) mInputLayout->Release();
	if (mConstantBuffer) mConstantBuffer->Release();
	if (mVertexShader) mVertexShader->Release();
	if (mPixelShader) mPixelShader->Release();
	if (mTexture) mTexture->Release();
	if (mDepthStencilState) mDepthStencilState->Release();
	if (mRasterState) mRasterState->Release();
	if (mSamplerState) mSamplerState->Release();
}

void SkyBox::Update()
{
	mPosition = mApp->GetSelectedCamera()->GetPosition();
	mCB->CameraPositon = mPosition;
}

void SkyBox::Draw()
{
	//Setting up constant buffer

	XMFLOAT4X4 transformationMatrix;
	XMStoreFloat4x4(&transformationMatrix,XMMatrixScaling(1000.0f, 1000.0f, 1000.0f) * XMMatrixTranslationFromVector(XMLoadFloat3(&mPosition)));

	mCB->world = XMMatrixTranspose(XMLoadFloat4x4(&transformationMatrix));
	mCB->view = XMMatrixTranspose(XMLoadFloat4x4(&mApp->GetSelectedCamera()->GetViewMatrix()));
	mCB->Projection = XMMatrixTranspose(XMLoadFloat4x4(&mApp->GetSelectedCamera()->GetProjectionMatrix()));

	//Remap constant buffer
	D3D11_MAPPED_SUBRESOURCE skyBoxResource;

	mApp->GetDeviceContext()->Map(mConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &skyBoxResource);
	memcpy(skyBoxResource.pData, mCB, sizeof(SkyBoxConstantBuffer));
	mApp->GetDeviceContext()->Unmap(mConstantBuffer, 0);

	//Set resources
	mApp->GetDeviceContext()->IASetInputLayout(mInputLayout);

	mApp->GetDeviceContext()->PSSetShaderResources(0, 1, &mTexture);

	mApp->GetDeviceContext()->IASetVertexBuffers(0, 1, &mMesh.VertexBuffer, &mMesh.VBStride, &mMesh.VBOffset);
	mApp->GetDeviceContext()->IASetIndexBuffer(mMesh.IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	mApp->GetDeviceContext()->VSSetShader(mVertexShader, nullptr, 0);
	mApp->GetDeviceContext()->PSSetShader(mPixelShader, nullptr, 0);

	mApp->GetDeviceContext()->OMSetDepthStencilState(mDepthStencilState, 0);
	mApp->GetDeviceContext()->RSSetState(mRasterState);

	mApp->GetDeviceContext()->VSSetConstantBuffers(0, 1, &mConstantBuffer);
	mApp->GetDeviceContext()->PSSetConstantBuffers(0, 1, &mConstantBuffer);

	mApp->GetDeviceContext()->PSSetSamplers(0, 1, &mSamplerState);

	mApp->GetDeviceContext()->DrawIndexed(mMesh.IndexCount, 0, 0);

	mApp->GetDeviceContext()->OMSetDepthStencilState(NULL, 0);
	mApp->GetDeviceContext()->RSSetState(mApp->GetRenderState());
	mApp->GetDeviceContext()->IASetInputLayout(mApp->GetInputLayout());
}

void SkyBox::Load()
{
	HRESULT hr;

	mMesh = OBJLoader::Load(mMeshFilePath, mApp->GetDevice());
	CreateDDSTextureFromFile(mApp->GetDevice(), mTextureFilePath, nullptr, &mTexture);

	//Create vertex shader
	ID3DBlob* VSBlob;

	hr = mApp->CompileShaderFromFile(L"SkyBox.fx", "VS", "vs_4_0", &VSBlob);

	if (FAILED(hr))
	{
		MessageBox(nullptr,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
	}

	hr = mApp->GetDevice()->CreateVertexShader(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), nullptr, &mVertexShader);

	//Create pixel shader
	ID3D10Blob* PSBlob;

	hr = mApp->CompileShaderFromFile(L"SkyBox.fx", "PS", "ps_4_0", &PSBlob);

	if (FAILED(hr))
	{
		MessageBox(nullptr,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
	}

	// Create the pixel shader
	hr = mApp->GetDevice()->CreatePixelShader(PSBlob->GetBufferPointer(), PSBlob->GetBufferSize(), nullptr, &mPixelShader);
	PSBlob->Release();

	//Create input layout
	D3D11_INPUT_ELEMENT_DESC skyBoxLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	int numElements = ARRAYSIZE(skyBoxLayout);

	hr = mApp->GetDevice()->CreateInputLayout(skyBoxLayout, numElements, VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), &mInputLayout);

	VSBlob->Release();
}

void SkyBox::SetFogEnabled(bool enabled)
{
	mCB->FogEnabled = enabled;
}
