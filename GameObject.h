#pragma once
#include "Structures.h"
#include "Vector3D.h"
#include <d3d11_1.h>
#include <directxmath.h>
#include "OBJLoader.h"

class Application;
struct ConstantBuffer;

class GameObject
{
protected:
	XMFLOAT3 mPosition;
	XMFLOAT3 mRotation;
	XMFLOAT3 mScale;

	char* mMeshFilePath;
	wchar_t* mTextureFilePath;

	MeshData mMesh;
	Material* mMaterial;

	Application* mApp;

	ConstantBuffer* mCB;

	ID3D11ShaderResourceView* mTexture;
public:
	GameObject(XMFLOAT3 position, XMFLOAT3 rotation, XMFLOAT3 scale, char* meshFilePath, wchar_t* textureFilePath, Application* app, ConstantBuffer* cb);
	~GameObject();

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetRotation();
	XMFLOAT3 GetScale();

	void SetPosition(XMFLOAT3 position);
	void SetRotation(XMFLOAT3 position);
	void SetScale(XMFLOAT3 position);

	virtual void Update(float deltaTime);
	virtual void Draw();
	virtual void Load();
};

