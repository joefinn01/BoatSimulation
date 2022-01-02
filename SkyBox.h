#pragma once
#include "Structures.h"
#include "Vector3D.h"
#include <d3d11_1.h>
#include <directxmath.h>
#include "OBJLoader.h"

class Application;
struct SkyBoxConstantBuffer;

class SkyBox
{
private:
	Application* mApp;

	SkyBoxConstantBuffer* mCB;

	ID3D11InputLayout* mInputLayout;

	MeshData mMesh;

	ID3D11Buffer* mConstantBuffer;

	ID3D11VertexShader* mVertexShader;
	ID3D11PixelShader* mPixelShader;

	ID3D11ShaderResourceView* mTexture;

	ID3D11DepthStencilState* mDepthStencilState;
	ID3D11RasterizerState* mRasterState;

	ID3D11SamplerState* mSamplerState;

	XMFLOAT3 mPosition;

	char* mMeshFilePath;
	wchar_t* mTextureFilePath;

public:
	SkyBox(XMFLOAT3 position, char* meshFilePath, wchar_t* textureFilePath, Application* app);
	~SkyBox();

	void Update();
	void Draw();

	void Load();

	void SetFogEnabled(bool enabled);
};

