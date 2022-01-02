#pragma once

#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include "resource.h"
#include "DDSTextureLoader.h"
#include "Structures.h"
#include "Camera.h"
#include "DebugCamera.h"
#include "Timer.h"
#include "rapidxml.hpp"
#include "rapidxml_utils.hpp"

using namespace DirectX;

class GameObject;
class Boat;
class Duck;
class BillBoard;
class SkyBox;
class PickUp;

class Application
{
private:
	HINSTANCE               _hInst;
	HWND                    _hWnd;
	D3D_DRIVER_TYPE         _driverType;
	D3D_FEATURE_LEVEL       _featureLevel;
	ID3D11Device*           _pd3dDevice;
	ID3D11DeviceContext*    _pImmediateContext;
	IDXGISwapChain*			 _pSwapChain;
	ID3D11RenderTargetView* _pRenderTargetView;
	ID3D11VertexShader*     _pVertexShader;
	ID3D11PixelShader*      _pPixelShader;
	ID3D11InputLayout*      _pVertexLayout;
	ID3D11Buffer*           _pConstantBuffer;
	ID3D11RasterizerState* _wireFrame;
	ID3D11RasterizerState* _solidFrame;
	ID3D11DepthStencilView* _depthStencilView;
	ID3D11Texture2D* _depthStencilBuffer;
	ID3D11SamplerState* _pSamplerLinear;

	ID3D11Buffer* _pWaterVertexBuffer;
	ID3D11Buffer* _pWaterIndexBuffer;
	ID3D11BlendState* _pWaterBlendState;

	ID3D11PixelShader* _pWaterPixelShader;

	ConstantBuffer* _pCB;

	ID3D11RasterizerState* _pRasterizationStateNoCull;

	Fog* _pFog;

	bool _debugCamera;

	ID3D11RasterizerState* _selectedRenderState;

private:
	HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
	HRESULT InitDevice();
	void Cleanup();

	HRESULT InitShadersAndInputLayout();

	void UserInput();

	float _WindowHeight;
	float _WindowWidth;

	//Water variables
	int _waterVertexCount;
	int	_waterFaceCount;

	void CreateWater(WORD _width, WORD _height);
	void UpdateWater(float deltaTime);
	void DrawWater();

	bool mRenderToggle;

	ID3D11Buffer* _pWaterConstantBuffer;
	WaterConstantBuffer _waterCB;

	SimpleVertex* _waterVertices;
	WORD* _waterIndices;

	GameObject* mBath;

	Boat* mBoat;

	SkyBox* mSkyBox;

	PickUp* mPickUp;

	Camera* mSelectedCamera;

	DebugCamera mCamera;
	
	Timer _gameTimer;
	Timer _pickUpTimer;

	 bool _keyDown;

public:
	Application();
	~Application();

	HRESULT Initialise(HINSTANCE hInstance, int nCmdShow);

	HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);

	void Update(float deltaTime);
	void Draw();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();
	ID3D11Buffer* GetConstantBuffer();

	ID3D11PixelShader* GetPixelShader();
	ID3D11VertexShader* GetVertexShader();

	ID3D11RasterizerState* GetNoCull();
	ID3D11RasterizerState* GetSolidState();
	ID3D11RasterizerState* GetRenderState();

	Camera* GetSelectedCamera();

	ID3D11SamplerState* GetSampleState();

	ID3D11InputLayout* GetInputLayout();

	float GetTotalTime();

	Fog* GetFog();
};

