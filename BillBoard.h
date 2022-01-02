#pragma once
#include <d3d11_1.h>
#include <directxmath.h>

class Application;
struct BillBoardConstantBuffer;

class BillBoard
{
protected:
	DirectX::XMFLOAT3 mPosition;
	int mWidth;
	int mHeight;
	Application* mApp;

	wchar_t* mTextureFilePath;

	ID3D11ShaderResourceView* mTexture;

	ID3D11PixelShader* mPixelShader;
	ID3D11VertexShader* mVertexShader;
	ID3D11GeometryShader* mGeometryShader;

	BillBoardConstantBuffer* mCB;

	ID3D11Buffer* mConstantBuffer;
	ID3D11Buffer* mVertexBuffer;

	ID3D11InputLayout* mInputLayout;

	UINT mStride;
	UINT mOffset;


public:
	BillBoard(DirectX::XMFLOAT3 position, int width, int height, wchar_t* textureFilePath, Application* app);
	~BillBoard();

	void Load();

	void Draw();

	void SetFogEnabled(bool enabled);
};

