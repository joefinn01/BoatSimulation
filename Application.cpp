#define _USE_MATH_DEFINES

#include "Application.h"
#include <string>
#include <iostream>
#include "OBJLoader.h"
#include "GameObject.h"
#include "Boat.h"
#include <math.h>
#include "Structures.h"
#include "Duck.h"
#include "BillBoard.h"
#include "SkyBox.h"
#include "PickUp.h"

using namespace rapidxml;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;

    switch (message)
    {
        case WM_PAINT:
            hdc = BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

Application::Application()
{
	_hInst = nullptr;
	_hWnd = nullptr;
	_driverType = D3D_DRIVER_TYPE_NULL;
	_featureLevel = D3D_FEATURE_LEVEL_11_0;
	_pd3dDevice = nullptr;
	_pImmediateContext = nullptr;
	_pSwapChain = nullptr;
	_pRenderTargetView = nullptr;
	_pVertexShader = nullptr;
	_pPixelShader = nullptr;
	_pVertexLayout = nullptr;
	_pConstantBuffer = nullptr;
	_pSamplerLinear = nullptr;

	mRenderToggle = false;
}

Application::~Application()
{
	Cleanup();
}

void Application::CreateWater(WORD width, WORD height)
{
	_waterVertexCount = width * height;
	_waterFaceCount = (width - 1) * (height - 1) * 2;

	_waterVertices = new SimpleVertex[_waterVertexCount];

	for (int i = 0; i < width; i++)	//Loop through and create grid of points as vertices doing height first
	{
		for (int j = 0; j < height; j++)
		{
			SimpleVertex temp;
			temp.Pos = XMFLOAT3((float)i, sin(j * 0.25f), (float)j);
			temp.Normal = XMFLOAT3(0.0f, 0.0f, 0.0f);
			temp.TexC = XMFLOAT2(0, 0);

			_waterVertices[i * height + j] = temp;
		}
	}

	_waterIndices = new WORD[_waterFaceCount * 3];

	UINT index = 0;

	for (WORD i = 0; i < width - 1; i++)
	{
		for (WORD j = 0; j < height - 1; j++)
		{
			//First tri for the square
			_waterIndices[index] = (unsigned short)(i * height + j);
			_waterIndices[index + 1] = (unsigned short)(i * height + j + 1);
			_waterIndices[index + 2] = (unsigned short)((i + 1) * height + j);

			//Second tri for the square
			_waterIndices[index + 3] = (unsigned short)((i + 1) * height + j);
			_waterIndices[index + 4] = (unsigned short)(i * height + j + 1);
			_waterIndices[index + 5] = (unsigned short)((i + 1) * height + j + 1);

			index += 6;
		}
	}

	for (int i = 0; i < _waterVertexCount - 1; i += 3)	//Creating normals for each vertex
	{
		XMVECTOR vector1, vector2;

		XMFLOAT3 point = _waterVertices[_waterIndices[i]].Pos;
		XMFLOAT3 point1 = _waterVertices[_waterIndices[i + 1]].Pos;
		XMFLOAT3 point2 = _waterVertices[_waterIndices[i + 2]].Pos;

		vector1 = XMVectorSubtract(XMLoadFloat3(&point1), XMLoadFloat3(&point));
		vector2 = XMVectorSubtract(XMLoadFloat3(&point2), XMLoadFloat3(&point));

		//Add this normal to each vertices normal
		XMStoreFloat3(&_waterVertices[i].Normal, XMVectorAdd(XMVector3Cross(vector1, vector2), XMLoadFloat3(&_waterVertices[i].Normal)));
		XMStoreFloat3(&_waterVertices[i + 1].Normal, XMVectorAdd(XMVector3Cross(vector1, vector2), XMLoadFloat3(&_waterVertices[i + 1].Normal)));
		XMStoreFloat3(&_waterVertices[i + 2].Normal, XMVectorAdd(XMVector3Cross(vector1, vector2), XMLoadFloat3(&_waterVertices[i + 2].Normal)));
	}

	for (int i = 0; i < _waterVertexCount; i++)	//Normalizing the normals to calculate the average
	{
		XMStoreFloat3(&_waterVertices[i].Normal, XMVector3Normalize(XMLoadFloat3(&_waterVertices[i].Normal)));
	}

	//Vertex buffer
	D3D11_BUFFER_DESC floorBufferDescVert;
	ZeroMemory(&floorBufferDescVert, sizeof(floorBufferDescVert));
	floorBufferDescVert.Usage = D3D11_USAGE_DYNAMIC;
	floorBufferDescVert.ByteWidth = sizeof(SimpleVertex) * _waterVertexCount;
	floorBufferDescVert.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	floorBufferDescVert.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA floorInitDataVert;
	ZeroMemory(&floorInitDataVert, sizeof(floorInitDataVert));
	floorInitDataVert.pSysMem = _waterVertices;
	HRESULT hr = _pd3dDevice->CreateBuffer(&floorBufferDescVert, &floorInitDataVert, &_pWaterVertexBuffer);

	//index buffer
	D3D11_BUFFER_DESC floorBufferDescIndices;
	ZeroMemory(&floorBufferDescIndices, sizeof(floorBufferDescIndices));
	floorBufferDescIndices.Usage = D3D11_USAGE_DEFAULT;
	floorBufferDescIndices.ByteWidth = sizeof(WORD) * _waterFaceCount * 3;
	floorBufferDescIndices.BindFlags = D3D11_BIND_INDEX_BUFFER;
	floorBufferDescIndices.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA floorInitDataIndices;
	ZeroMemory(&floorInitDataIndices, sizeof(floorInitDataIndices));
	floorInitDataIndices.pSysMem = _waterIndices;
	_pd3dDevice->CreateBuffer(&floorBufferDescIndices, &floorInitDataIndices, &_pWaterIndexBuffer);

	// Create the constant buffer
	D3D11_BUFFER_DESC bdCB;
	ZeroMemory(&bdCB, sizeof(bdCB));
	bdCB.Usage = D3D11_USAGE_DYNAMIC;
	bdCB.ByteWidth = sizeof(WaterConstantBuffer);
	bdCB.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bdCB.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	hr = _pd3dDevice->CreateBuffer(&bdCB, nullptr, &_pWaterConstantBuffer);

	ID3DBlob* pPSBlob = nullptr;
	hr = CompileShaderFromFile(L"waterShader.fx", "PS", "ps_4_0", &pPSBlob);

	if (FAILED(hr))
	{
		MessageBox(nullptr,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
	}

	// Create the vertex shader
	hr = _pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &_pWaterPixelShader);
	pPSBlob->Release();
}

void Application::UpdateWater(float deltaTime)
{
	for (int i = 0; i < _waterVertexCount; i++)
	{
		_waterVertices[i].Pos.y = sin(0.5f * (_gameTimer.TotalTime() + _waterVertices[i].Pos.z));
		
		XMFLOAT3 tempVector = XMFLOAT3(0.0f, 0.5f, -0.5f * cos(0.5f * (_gameTimer.TotalTime() + _waterVertices->Pos.z)));
		XMStoreFloat3(&_waterVertices[i].Normal, XMVector3Normalize(XMLoadFloat3(&tempVector)));
	}

	//Remapping the buffer
	D3D11_MAPPED_SUBRESOURCE resource;

	_pImmediateContext->Map(_pWaterVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	memcpy(resource.pData, _waterVertices, sizeof(SimpleVertex) * _waterVertexCount);
	_pImmediateContext->Unmap(_pWaterVertexBuffer, 0);
}

void Application::DrawWater()
{
	XMFLOAT4X4 temp;
	XMFLOAT3 waterPosition = XMFLOAT3(-0, 4, 0);
	XMFLOAT3 waterScale = XMFLOAT3(3.25f, 1, 3);
	XMStoreFloat4x4(&temp, XMMatrixScalingFromVector(XMLoadFloat3(&waterScale)) * XMMatrixTranslationFromVector(XMLoadFloat3(&waterPosition)));

	_waterCB.Cb.mWorld = XMMatrixTranspose(XMLoadFloat4x4(&temp));
	_waterCB.Cb.mView = XMMatrixTranspose(XMLoadFloat4x4(&mSelectedCamera->GetViewMatrix()));
	_waterCB.Cb.mProjection = XMMatrixTranspose(XMLoadFloat4x4(&mSelectedCamera->GetProjectionMatrix()));
	_waterCB.Cb.mMaterial.AmbientMaterial = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	_waterCB.Cb.mMaterial.DiffuseMaterial = XMFLOAT4(0.0f, 0.467f, 1.0f, 0.745f);
	_waterCB.Cb.mMaterial.SpecularMaterial = XMFLOAT4(0.5f, 0.5f, 0.5f, 10.0f);
	_waterCB.Cb.EyePosW = mSelectedCamera->GetPosition();

	temp._41 = 0.0f;
	temp._42 = 0.0f;
	temp._43 = 0.0f;
	temp._44 = 1.0f;

	_pCB->mWorldInverseTranspose = XMMatrixTranspose(XMMatrixInverse(&XMMatrixDeterminant(XMLoadFloat4x4(&temp)), XMLoadFloat4x4(&temp)));

	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;

	D3D11_MAPPED_SUBRESOURCE resource;

	_pImmediateContext->Map(_pWaterConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	memcpy(resource.pData, &_waterCB, sizeof(WaterConstantBuffer));
	_pImmediateContext->Unmap(_pWaterConstantBuffer, 0);

	_pImmediateContext->IASetVertexBuffers(0, 1, &_pWaterVertexBuffer, &stride, &offset);
	_pImmediateContext->IASetIndexBuffer(_pWaterIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	_pImmediateContext->VSSetShader(_pVertexShader, nullptr, 0);
	_pImmediateContext->PSSetShader(_pWaterPixelShader, nullptr, 0);

	_pImmediateContext->VSSetConstantBuffers(0, 1, &_pWaterConstantBuffer);
	_pImmediateContext->PSSetConstantBuffers(0, 1, &_pWaterConstantBuffer);

	_pImmediateContext->DrawIndexed(_waterFaceCount * 3, 0, 0);
}

XMFLOAT3 LoadXMLAttribute(xml_node<>* node)
{
	xml_attribute<>* attribute = node->first_attribute();

	node->remove_first_attribute();

	std::string tempString = std::string(attribute->value());
	std::string stringValues[3];
	

	for (int i = 0; i < 3; i++)
	{
		int delimIndex = tempString.find(",");
		stringValues[i] = tempString.substr(0, delimIndex);
		tempString.erase(0, delimIndex + 1);
	}

	return XMFLOAT3(stof(stringValues[0]), stof(stringValues[1]), stof(stringValues[2]));
}

HRESULT Application::Initialise(HINSTANCE hInstance, int nCmdShow)
{
    if (FAILED(InitWindow(hInstance, nCmdShow)))
	{
        return E_FAIL;
	}

    RECT rc;
    GetClientRect(_hWnd, &rc);
    _WindowWidth = rc.right - rc.left;
    _WindowHeight = rc.bottom - rc.top;

    if (FAILED(InitDevice()))
    {
        Cleanup();

        return E_FAIL;
    }

	_selectedRenderState = _solidFrame;

	_pickUpTimer.Start();

	srand(_gameTimer.TotalTime());

	_pCB = new ConstantBuffer();

	_pFog = new Fog();
	_pFog->FogColour = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	_pFog->FogRange = 50.0;
	_pFog->FogStart = 10.0f;

	_keyDown = false;

	//Open XML file and parse
	file<> xmlFile("SceneInit.xml");

	xml_document<> doc;
	doc.parse<0>(xmlFile.data());

	xml_node<>* parentNode = doc.first_node();
	xml_node<>* node = parentNode->first_node();

	XMFLOAT3 position = LoadXMLAttribute(node);
	XMFLOAT3 rotation = LoadXMLAttribute(node);
	XMFLOAT3 scale = LoadXMLAttribute(node);

	mBoat = new Boat(position, rotation, scale, "Models/shipTest.obj", L"Textures/Ship_COLOR.dds", this, _pCB, XMFLOAT3(0, 0, 1), XMFLOAT3(0, 1, 0), _WindowWidth, _WindowHeight, 0.1f, 1000.0f, 1.0f);
	mBoat->Load();

	node = node->next_sibling();
	position = LoadXMLAttribute(node);
	rotation = LoadXMLAttribute(node);
	scale = LoadXMLAttribute(node);

	mBath = new GameObject(position, rotation, scale, "Models/bathTest.obj", L"Textures/Hercules_COLOR.dds", this, _pCB);
	mBath->Load();

	node = node->next_sibling();
	position = LoadXMLAttribute(node);

	mPickUp = new PickUp(position, 5, 5, L"Textures/slowTimePickup.dds", this, 10.0f, SPEED);
	mPickUp->Load();

	mCamera = DebugCamera(XMFLOAT3(0.0f, 0.0f, -3.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), _WindowWidth, _WindowHeight, 0.1f, 1000.0f);

	_debugCamera = false;

	mSelectedCamera = mBoat->GetSelectedCamera();

	CreateWater(100, 100);

	mSkyBox = new SkyBox(XMFLOAT3(0, 0, 0), "Models/sphere.obj", L"Textures/skytextureSquare2.dds", this);
	mSkyBox->Load();

	//Cosntant buffer stuff
	_pCB->Lights[0].Direction = XMFLOAT4(0, -1, 0, 1);
	_pCB->Lights[0].Mat.AmbientMaterial = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	_pCB->Lights[0].Mat.DiffuseMaterial = XMFLOAT4(0.75f, 0.75f, 0.75f, 1.0f);
	_pCB->Lights[0].Mat.SpecularMaterial = XMFLOAT4(0.5f, 0.5f, 0.5f, 10.0f);
	_pCB->Lights[0].LightType = DIRECTIONAL_LIGHT;
	_pCB->Lights[0].Enabled = true;

	node = node->next_sibling();
	position = LoadXMLAttribute(node);

	_pCB->Lights[1].Position = XMFLOAT4(position.x, position.y, position.z, 1.0f);
	_pCB->Lights[1].Mat.AmbientMaterial = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	_pCB->Lights[1].Mat.DiffuseMaterial = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	_pCB->Lights[1].Mat.SpecularMaterial = XMFLOAT4(0.5f, 0.5f, 0.5f, 10.0f);
	_pCB->Lights[1].Attenuation = XMFLOAT3(0.2f, 0.09f, 0.0f);
	_pCB->Lights[1].LightType = POINT_LIGHT;
	_pCB->Lights[1].Enabled = 0;
	_pCB->Lights[1].Range = 100.0f;

	node = node->next_sibling();
	position = LoadXMLAttribute(node);

	_pCB->Lights[2].Position = XMFLOAT4(position.x, position.y, position.z, 1.0);
	_pCB->Lights[2].Direction = XMFLOAT4(0, 0, 1, 1);
	_pCB->Lights[2].Mat.AmbientMaterial = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	_pCB->Lights[2].Mat.DiffuseMaterial = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	_pCB->Lights[2].Mat.SpecularMaterial = XMFLOAT4(0.5f, 0.5f, 0.5f, 10.0f);
	_pCB->Lights[2].Attenuation = XMFLOAT3(0.2f, 0.09f, 0.0f);
	_pCB->Lights[2].SpotLightAngle = 45.0f;
	_pCB->Lights[2].LightType = SPOT_LIGHT;
	_pCB->Lights[2].Enabled = 0;
	_pCB->Lights[2].Range = 1000.0f;

	_pCB->FogEnabled = true;
	_pCB->FogStart = _pFog->FogStart;
	_pCB->FogRange = _pFog->FogRange;
	_pCB->FogColour = _pFog->FogColour;

	_waterCB.Cb = *_pCB;
	_waterCB.PickUp[0].Position = mPickUp->GetPosition();
	_waterCB.PickUp[0].Enabled = 1;

	SetCursorPos(_WindowWidth / 2, _WindowHeight / 2);

	return S_OK;
}

HRESULT Application::InitShadersAndInputLayout()
{
	HRESULT hr;

 //   // Compile the vertex shader
    ID3DBlob* pVSBlob = nullptr;
    hr = CompileShaderFromFile(L"DX11 Framework.fx", "VS", "vs_4_0", &pVSBlob);

    if (FAILED(hr))
    {
        MessageBox(nullptr,
                   L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return hr;
    }

	// Create the vertex shader
	hr = _pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &_pVertexShader);

	if (FAILED(hr))
	{	
		pVSBlob->Release();
        return hr;
	}

	// Compile the pixel shader
	ID3DBlob* pPSBlob = nullptr;
    hr = CompileShaderFromFile(L"DX11 Framework.fx", "PS", "ps_4_0", &pPSBlob);

    if (FAILED(hr))
    {
        MessageBox(nullptr,
                   L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return hr;
    }

	// Create the pixel shader
	hr = _pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &_pPixelShader);
	pPSBlob->Release();

    if (FAILED(hr))
        return hr;

    // Define the input layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	UINT numElements = ARRAYSIZE(layout);

    // Create the input layout
	hr = _pd3dDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
                                        pVSBlob->GetBufferSize(), &_pVertexLayout);
	pVSBlob->Release();

	if (FAILED(hr))
        return hr;

    // Set the input layout
    _pImmediateContext->IASetInputLayout(_pVertexLayout);

	return hr;
}

HRESULT Application::InitWindow(HINSTANCE hInstance, int nCmdShow)
{
    // Register class
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_TUTORIAL1);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW );
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = L"TutorialWindowClass";
    wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_TUTORIAL1);
    if (!RegisterClassEx(&wcex))
        return E_FAIL;

    // Create window
    _hInst = hInstance;
    //RECT rc = {0, 0, 640, 480};
    RECT rc = {0, 0, 3840, 2160};
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
    _hWnd = CreateWindow(L"TutorialWindowClass", L"DX11 Framework", WS_OVERLAPPEDWINDOW,
                         CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
                         nullptr);
    if (!_hWnd)
		return E_FAIL;

    ShowWindow(_hWnd, nCmdShow);

    return S_OK;
}

HRESULT Application::CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
    HRESULT hr = S_OK;

	UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    ID3DBlob* pErrorBlob;
    hr = D3DCompileFromFile(szFileName, nullptr, nullptr, szEntryPoint, szShaderModel, 
        dwShaderFlags, 0, ppBlobOut, &pErrorBlob);

    if (FAILED(hr))
    {
        if (pErrorBlob != nullptr)
            OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());

        if (pErrorBlob) pErrorBlob->Release();

        return hr;
    }

    if (pErrorBlob) pErrorBlob->Release();

    return S_OK;
}

HRESULT Application::InitDevice()
{
    HRESULT hr = S_OK;

    UINT createDeviceFlags = 0;

#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };

    UINT numDriverTypes = ARRAYSIZE(driverTypes);

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };

	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 1;
    sd.BufferDesc.Width = _WindowWidth;
    sd.BufferDesc.Height = _WindowHeight;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = _hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;

    for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
    {
        _driverType = driverTypes[driverTypeIndex];
        hr = D3D11CreateDeviceAndSwapChain(nullptr, _driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
                                           D3D11_SDK_VERSION, &sd, &_pSwapChain, &_pd3dDevice, &_featureLevel, &_pImmediateContext);
        if (SUCCEEDED(hr))
            break;
    }

    if (FAILED(hr))
        return hr;

    // Create a render target view
    ID3D11Texture2D* pBackBuffer = nullptr;
    hr = _pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

    if (FAILED(hr))
        return hr;

    hr = _pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &_pRenderTargetView);
    pBackBuffer->Release();

    if (FAILED(hr))
        return hr;

	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width = _WindowWidth;
	depthStencilDesc.Height = _WindowHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	_pd3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, &_depthStencilBuffer);
	_pd3dDevice->CreateDepthStencilView(_depthStencilBuffer, nullptr, &_depthStencilView);

    _pImmediateContext->OMSetRenderTargets(1, &_pRenderTargetView, _depthStencilView);

    // Setup the viewport
    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)_WindowWidth;
    vp.Height = (FLOAT)_WindowHeight;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    _pImmediateContext->RSSetViewports(1, &vp);

	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	//_pd3dDevice->CreateSamplerState(&sampDesc, &_pSkyMapSampler);
	_pd3dDevice->CreateSamplerState(&sampDesc, &_pSamplerLinear);
	
	_pImmediateContext->PSSetSamplers(0, 1, &_pSamplerLinear);

	InitShadersAndInputLayout();

    // Set primitive topology
    _pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Create the constant buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	//bd.Usage = D3D11_USAGE_DEFAULT;
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	//bd.CPUAccessFlags = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    hr = _pd3dDevice->CreateBuffer(&bd, nullptr, &_pConstantBuffer);

	if (FAILED(hr))
		return hr;

	if (FAILED(hr))
		return hr;

	//Createing rasterisation states

	//Wire frame
	D3D11_RASTERIZER_DESC wfdesc;
	ZeroMemory(&wfdesc, sizeof(D3D11_RASTERIZER_DESC));
	wfdesc.FillMode = D3D11_FILL_WIREFRAME;
	wfdesc.CullMode = D3D11_CULL_NONE;
	wfdesc.FrontCounterClockwise = true;
	hr = _pd3dDevice->CreateRasterizerState(&wfdesc, &_wireFrame);

	if (FAILED(hr))
		return hr;

	//solid frame
	D3D11_RASTERIZER_DESC sfdesc;
	ZeroMemory(&sfdesc, sizeof(D3D11_RASTERIZER_DESC));
	sfdesc.FillMode = D3D11_FILL_SOLID;
	sfdesc.CullMode = D3D11_CULL_BACK;
	sfdesc.FrontCounterClockwise = false;
	hr = _pd3dDevice->CreateRasterizerState(&sfdesc, &_solidFrame);

    if (FAILED(hr))
        return hr;

	//No cull raster state
	D3D11_RASTERIZER_DESC sbdesc;
	ZeroMemory(&sbdesc, sizeof(D3D11_RASTERIZER_DESC));
	sbdesc.FillMode = D3D11_FILL_SOLID;
	sbdesc.CullMode = D3D11_CULL_NONE;
	sbdesc.FrontCounterClockwise = false;
	hr = _pd3dDevice->CreateRasterizerState(&sbdesc, &_pRasterizationStateNoCull);

	//if (FAILED(hr))
	//	return hr;

	//D3D11_DEPTH_STENCIL_DESC dssDesc;
	//ZeroMemory(&dssDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	//dssDesc.DepthEnable = true;
	//dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	//dssDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	//_pd3dDevice->CreateDepthStencilState(&dssDesc, &_pDepthStencilState);

	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));

	D3D11_RENDER_TARGET_BLEND_DESC renderTargetBlendDesc;
	ZeroMemory(&renderTargetBlendDesc, sizeof(renderTargetBlendDesc));

	renderTargetBlendDesc.BlendEnable = true;
	renderTargetBlendDesc.SrcBlend = D3D11_BLEND_SRC_COLOR;
	renderTargetBlendDesc.DestBlend = D3D11_BLEND_BLEND_FACTOR;
	renderTargetBlendDesc.BlendOp = D3D11_BLEND_OP_ADD;
	renderTargetBlendDesc.SrcBlendAlpha = D3D11_BLEND_ONE;
	renderTargetBlendDesc.DestBlendAlpha = D3D11_BLEND_ZERO;
	renderTargetBlendDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	renderTargetBlendDesc.RenderTargetWriteMask = D3D10_COLOR_WRITE_ENABLE_ALL;

	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.RenderTarget[0] = renderTargetBlendDesc;

	_pd3dDevice->CreateBlendState(&blendDesc, &_pWaterBlendState);


    return S_OK;
}

void Application::Cleanup()
{
	if (_pd3dDevice) _pd3dDevice->Release();
	if (_pImmediateContext) _pImmediateContext->Release();
	if (_pSwapChain) _pSwapChain->Release();
	if (_pRenderTargetView) _pRenderTargetView->Release();
	if (_pVertexShader) _pVertexShader->Release();
	if (_pPixelShader) _pPixelShader->Release();
	if (_pVertexLayout) _pVertexLayout->Release();
	if (_pConstantBuffer) _pConstantBuffer->Release();
	if (_wireFrame) _wireFrame->Release();
	if (_solidFrame) _solidFrame->Release();
	if (_depthStencilView) _depthStencilView->Release();
	if (_depthStencilBuffer) _depthStencilBuffer->Release();
	if (_pSamplerLinear) _pSamplerLinear->Release();
	if (_pWaterVertexBuffer) _pWaterVertexBuffer->Release();
	if (_pWaterIndexBuffer) _pWaterIndexBuffer->Release();
	if (_pWaterBlendState) _pWaterBlendState->Release();
	if (_pWaterPixelShader) _pWaterPixelShader->Release();
	if (_pCB) delete _pCB; _pCB = nullptr;
	if (_pRasterizationStateNoCull) _pRasterizationStateNoCull->Release();
	if (_pFog) delete _pFog; _pFog = nullptr;
	if (_pWaterConstantBuffer) _pWaterConstantBuffer->Release();
	if (_waterVertices) delete _waterVertices; _waterVertices = nullptr;
	if (_waterIndices) delete _waterIndices; _waterIndices = nullptr;
	if (mBath) delete mBath; mBath = nullptr;
	if (mBoat) delete mBoat; mBoat = nullptr;
	if (mSkyBox) delete mSkyBox; mSkyBox = nullptr;
	if (mPickUp) delete mPickUp; mPickUp = nullptr;
}

void Application::UserInput()
{
	if (GetAsyncKeyState(0x31) || GetAsyncKeyState(0x32) || GetAsyncKeyState(0x33))	//1, 2 and 3
	{
		_debugCamera = false;
	}
	else if (GetAsyncKeyState(0x34))	//4
	{
		_debugCamera = true;
		mSelectedCamera = &mCamera;
	}
	else if (GetAsyncKeyState(0x35))	//5		Turn directional light on and off
	{
		if (!_keyDown)
		{
			_pCB->Lights[0].Enabled = !_pCB->Lights[0].Enabled;
			_waterCB.Cb.Lights[0].Enabled = _pCB->Lights[0].Enabled;

			_keyDown = true;
		}
	}
	else if (GetAsyncKeyState(0x36))	//6		Turn point light on and off
	{
		if (!_keyDown)
		{
			_pCB->Lights[1].Enabled = !_pCB->Lights[1].Enabled;
			_waterCB.Cb.Lights[1].Enabled = _pCB->Lights[1].Enabled;

			_keyDown = true;
		}
	}
	else if (GetAsyncKeyState(0x37))	//7		Turn spotlight on and off
	{
		if (!_keyDown)
		{
			_pCB->Lights[2].Enabled = !_pCB->Lights[2].Enabled;
			_waterCB.Cb.Lights[2].Enabled = _pCB->Lights[2].Enabled;

			_keyDown = true;
		}
	}
	else if (GetAsyncKeyState(0x38))	//8		Turn fog on and off
	{
		if (!_keyDown)
		{
			_pCB->FogEnabled = !_pCB->FogEnabled;

			_waterCB.Cb.FogEnabled = _pCB->FogEnabled;
			mSkyBox->SetFogEnabled(_pCB->FogEnabled);
			mPickUp->SetFogEnabled(_pCB->FogEnabled);

			_keyDown = true;
		}
	}
	else if (GetAsyncKeyState(VK_SPACE))
	{
		if (!_keyDown)
		{
    		if (mRenderToggle)
			{
				_pImmediateContext->RSSetState(_wireFrame);
				_selectedRenderState = _wireFrame;
			}
			else
			{
				_pImmediateContext->RSSetState(_solidFrame);
				_selectedRenderState = _solidFrame;
			}

			mRenderToggle = !mRenderToggle;

			_keyDown = true;
		}
	}
	else
	{
		_keyDown = false;
	}
}

void Application::Update(float deltaTime)
{
    // Update our time
    static float t = 0.0f;

    if (_driverType == D3D_DRIVER_TYPE_REFERENCE)
    {
        t += (float) XM_PI * 0.0125f;
    }
    else
    {
        static DWORD dwTimeStart = 0;
        DWORD dwTimeCur = GetTickCount();

		if (dwTimeStart == 0)
		{
			dwTimeStart = dwTimeCur;
		}

        t = (dwTimeCur - dwTimeStart) / 1000.0f;
    }

	_gameTimer.Tick();
	_pickUpTimer.Tick();

	if (!_debugCamera)
	{
		mSelectedCamera = mBoat->GetSelectedCamera();
	}

	UserInput();

	if (_gameTimer.DeltaTime() < 10000.0f)
	{
		mSelectedCamera->Update(_gameTimer.DeltaTime());

		mBath->Update(_gameTimer.DeltaTime());
		mBoat->Update(_gameTimer.DeltaTime());

		UpdateWater(_gameTimer.DeltaTime());

		mSkyBox->Update();

		if (mPickUp->GetCollider()->CollisionDetection(mBoat->GetCollider()))
		{
			mBoat->SetSpeed(2.0f);
			_waterCB.PickUp[0].Enabled = 0;

			mPickUp->SetPosition(XMFLOAT3(0, -1010, 0));

			_pickUpTimer.Start();
		}

		if (_pickUpTimer.TotalTime() > 5.0f)
		{
			_pickUpTimer.Reset();
			_pickUpTimer.Stop();

			mBoat->SetSpeed(1.0f);

			XMFLOAT3 randomPos = XMFLOAT3(rand() % 130 + 100, 7, rand() % 60 + 60);
			mPickUp->SetPosition(randomPos);

			_waterCB.PickUp[0].Position = mPickUp->GetPosition();
			_waterCB.PickUp[0].Enabled = 1;
			mPickUp->GetCollider()->Position = randomPos;
		}
	}
}

void Application::Draw()
{
    // Clear the back buffer
    float ClearColor[4] = {0.0f, 0.125f, 0.3f, 1.0f}; // red,green,blue,alpha
    _pImmediateContext->ClearRenderTargetView(_pRenderTargetView, ClearColor);
	_pImmediateContext->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//Opaque Objects
	_pImmediateContext->OMSetBlendState(0, 0, 0xffffffff);

	mBath->Draw();
	mBoat->Draw();
	
	mPickUp->Draw();

	mSkyBox->Draw();

	//Transparent Objects
	float blendFactor[] = { 0.5f, 0.5f, 0.9f, 1.0f };

	_pImmediateContext->OMSetBlendState(_pWaterBlendState, blendFactor, 0xffffffff);

	DrawWater();

	//present back buffer to front
    _pSwapChain->Present(0, 0);
}

ID3D11Device* Application::GetDevice()
{
	return _pd3dDevice;
}

ID3D11DeviceContext* Application::GetDeviceContext()
{
	return _pImmediateContext;
}

ID3D11Buffer* Application::GetConstantBuffer()
{
	return _pConstantBuffer;
}

ID3D11PixelShader* Application::GetPixelShader()
{
	return _pPixelShader;
}

ID3D11VertexShader* Application::GetVertexShader()
{
	return _pVertexShader;
}

ID3D11RasterizerState* Application::GetNoCull()
{
	return _pRasterizationStateNoCull;
}

ID3D11RasterizerState* Application::GetSolidState()
{
	return _solidFrame;
}

ID3D11RasterizerState* Application::GetRenderState()
{
	return _selectedRenderState;
}

Camera* Application::GetSelectedCamera()
{
	return mSelectedCamera;
}

ID3D11SamplerState* Application::GetSampleState()
{
	return _pSamplerLinear;
}

ID3D11InputLayout* Application::GetInputLayout()
{
	return _pVertexLayout;
}

float Application::GetTotalTime()
{
	return _gameTimer.TotalTime();
}

Fog* Application::GetFog()
{
	return _pFog;
}
