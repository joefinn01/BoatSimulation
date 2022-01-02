#include "PickUp.h"

using namespace DirectX;

PickUp::PickUp(XMFLOAT3 position, int width, int height, wchar_t* textureFilePath, Application* app, float radius, PICKUP type) : BillBoard(position, width, height, textureFilePath, app)
{
	mCollider.Position = position;
	mCollider.Radius = radius;

	mType = type;
}

XMFLOAT3 PickUp::GetPosition()
{
	return mPosition;
}

SphereCollider* PickUp::GetCollider()
{
	return &mCollider;
}

void PickUp::SetPosition(DirectX::XMFLOAT3 pos)
{
	mPosition = pos;
}
