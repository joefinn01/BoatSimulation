#pragma once

#include <windows.h>
#include "Vector3D.h"
#include <d3d11_1.h>
#include <directxmath.h>
#include "Constants.h"

using namespace DirectX;

enum LIGHTS
{
	DIRECTIONAL_LIGHT = 0,
	POINT_LIGHT,
	SPOT_LIGHT
};

enum PICKUP
{
	SPEED = 0,
	SLOW_TIME
};

struct SimpleVertex
{
	XMFLOAT3 Pos;
	XMFLOAT3 Normal;
	XMFLOAT2 TexC;

	bool operator<(const SimpleVertex other) const
	{
		return memcmp((void*)this, (void*)&other, sizeof(SimpleVertex)) > 0;
	};
};

struct MeshData
{
	ID3D11Buffer* VertexBuffer;
	ID3D11Buffer* IndexBuffer;
	UINT VBStride;
	UINT VBOffset;
	UINT IndexCount;
};

struct Material
{
	XMFLOAT4 AmbientMaterial;
	XMFLOAT4 DiffuseMaterial;	//4th float is the alpha
	XMFLOAT4 SpecularMaterial;	//4th float is the specular power
};

struct Light
{
	XMFLOAT4 Position;
	XMFLOAT4 Direction;
	Material Mat;

	XMFLOAT3 Attenuation;
	float SpotLightAngle;

	int LightType;
	int Enabled;
	float Range;
	float Pad;

	Light()
	{
		ZeroMemory(this, sizeof(this));
	}
};

struct PickUpCB
{
	XMFLOAT3 Position;
	int Enabled;

	PickUpCB()
	{
		ZeroMemory(this, sizeof(this));
	}
};

struct ConstantBuffer
{
	XMMATRIX mWorld;
	XMMATRIX mWorldInverseTranspose;
	XMMATRIX mView;
	XMMATRIX mProjection;

	Material mMaterial;

	Light Lights[MAX_LIGHTS];

	XMFLOAT3 EyePosW;
	float FogEnabled;

	float FogStart;
	float FogRange;
	XMFLOAT2 Pad;

	XMFLOAT4 FogColour;

	ConstantBuffer()
	{
		ZeroMemory(this, sizeof(this));

		for (int i = 0; i < MAX_LIGHTS; i++)
		{
			Lights[i] = Light();
			Lights[i].Enabled = 0;
		}
	}
};

struct WaterConstantBuffer
{
	ConstantBuffer Cb;
	PickUpCB PickUp[MAX_PICKUPS];

	WaterConstantBuffer()
	{
		ZeroMemory(this, sizeof(PickUpCB) * MAX_PICKUPS);

		for (int i = 0; i < MAX_PICKUPS; i++)
		{
			PickUp[i] = PickUpCB();
			PickUp[i].Enabled = 0;
		}
	}

};

struct SkyBoxConstantBuffer
{
	XMMATRIX world;
	XMMATRIX view;
	XMMATRIX Projection;

	XMFLOAT3 CameraPositon;
	float FogEnabled;

	float FogStart;
	float FogRange;
	XMFLOAT2 Pad;

	XMFLOAT4 FogColour;

	SkyBoxConstantBuffer()
	{
		ZeroMemory(this, sizeof(this));
	}
};

struct SphereCollider
{
	float Radius;
	XMFLOAT3 Position;

	SphereCollider()
	{
		ZeroMemory(this, sizeof(this));
	}

	bool CollisionDetection(SphereCollider* collider)
	{
		float distanceBetween;

		XMStoreFloat(&distanceBetween, XMVector3Length(XMVectorSubtract(XMLoadFloat3(&Position), XMLoadFloat3(&collider->Position))));

		return Radius + collider->Radius > distanceBetween;
	}
};

struct BillBoardConstantBuffer
{
	XMMATRIX World;
	XMMATRIX View;
	XMMATRIX Projection;

	XMFLOAT3 CameraPositon;
	float FogEnabled;

	XMFLOAT2 Size;
	float FogStart;
	float FogRange;

	XMFLOAT4 FogColour;

	BillBoardConstantBuffer()
	{
		ZeroMemory(this, sizeof(this));
	}
};

struct BillBoardVertex
{
	XMFLOAT3 Position;
};

struct Fog
{
	float FogStart;
	float FogRange;
	XMFLOAT4 FogColour;

	Fog()
	{
		ZeroMemory(this, sizeof(this));
	}
};