#pragma once
#include "BillBoard.h"
#include "Structures.h"

class PickUp : public BillBoard
{
private:
	SphereCollider mCollider;
	PICKUP mType;

public:
	PickUp(DirectX::XMFLOAT3 position, int width, int height, wchar_t* textureFilePath, Application* app, float radius, PICKUP type);

	DirectX::XMFLOAT3 GetPosition();

	SphereCollider* GetCollider();

	void SetPosition(DirectX::XMFLOAT3 pos);
};

