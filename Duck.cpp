#include "Duck.h"
#include "Boat.h"
#include "Structures.h"
#include "OBJLoader.h"
#include "Application.h"

Duck::Duck(XMFLOAT3 position, XMFLOAT3 rotation, XMFLOAT3 scale, char* meshFilePath, wchar_t* textureFilePath, Application* app, ConstantBuffer* cb, Boat* target) : GameObject(position, rotation, scale, meshFilePath, textureFilePath, app, cb)
{
	mTarget = target;

	mUp = XMFLOAT3(0, 1, 0);

	XMStoreFloat3(&mForward, XMVector3Normalize(XMVectorSubtract(XMLoadFloat3(&mTarget->GetForward()), XMLoadFloat3(&mPosition))));

	mCollider = new SphereCollider();

	mSpeed = 5.0f;
}

void Duck::Update(float deltaTime)
{
	if (mActive)
	{
		XMStoreFloat3(&mForward, XMVector3Normalize(XMVectorSubtract(XMLoadFloat3(&mTarget->GetPosition()), XMLoadFloat3(&mPosition))));	//Recalculate vector to target

		mForward.y = 0.0f;

		XMStoreFloat3(&mPosition, XMVectorAdd(XMLoadFloat3(&mPosition), XMLoadFloat3(&mForward) * mSpeed * deltaTime * 0.5f));

		XMFLOAT3 tempForward = XMFLOAT3(0, 0, -1);

		XMStoreFloat(&mRotation.y, XMVector3AngleBetweenVectors(XMLoadFloat3(&mForward), XMLoadFloat3(&tempForward)));

		mPosition.y = sin(0.5f * (mApp->GetTotalTime() + mPosition.z / 3.0f)) + 4.0f;

		mCollider->Position = mPosition;

		if (mCollider->CollisionDetection(mTarget->GetCollider()))
		{
			XMFLOAT3 randomPos = XMFLOAT3(rand() % 130 + 100, 3, rand() % 60 + 60);
			mTarget->SetPosition(randomPos);
		}
	}
}

void Duck::Load()
{
	mMesh = OBJLoader::Load(mMeshFilePath, mApp->GetDevice(), true, mCollider);
	CreateDDSTextureFromFile(mApp->GetDevice(), mTextureFilePath, nullptr, &mTexture);

	mCollider->Position = mPosition;
	mCollider->Radius = 0.8;
}

void Duck::SetSpeed(float speed)
{
	mSpeed = speed;
}

void Duck::SetActive(bool active)
{
	mActive = active;
}