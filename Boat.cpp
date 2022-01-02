#include "Boat.h"
#include "DynamicCamera.h"
#include "HorizontalStaticCamera.h"
#include "TopDownStaticCamera.h"
#include "Camera.h"
#include "Structures.h"
#include "Application.h"
#include "OBJLoader.h"

Boat::Boat(XMFLOAT3 position, XMFLOAT3 rotation, XMFLOAT3 scale, char* meshFilePath, wchar_t* textureFilePath, Application* app, ConstantBuffer* cb, XMFLOAT3 forwardVector, XMFLOAT3 upVector, float windowWidth, float windowHeight, float nearCull, float farCull, float speedMult) : GameObject(position, rotation, scale, meshFilePath, textureFilePath, app, cb)
{
	mForwardVector = forwardVector;
	mUpVector = upVector;

	//Init cameras
	mCameras[DYNAMIC] = new DynamicCamera(XMFLOAT3(position.x, position.y, position.z - 5.0f), XMFLOAT3(0, 0, 1), upVector, windowWidth, windowHeight, nearCull, farCull, this);
	mCameras[HORZIONTAL] = new HorizontalStaticCamera(XMFLOAT3(position.x, position.y, position.z + 2.0f), XMFLOAT3(0, 0, 1), upVector, windowWidth, windowHeight, nearCull, farCull, this);
	mCameras[TOP_DOWN] = new TopDownStaticCamera(XMFLOAT3(position.x, position.y + 5.0f, position.z), XMFLOAT3(0, -1, 0), mForwardVector, windowWidth, windowHeight, nearCull, farCull, this);

	mSelectedCamera = mCameras[DYNAMIC];	//Set to dynamic camera by default

	mChangeInWaterLevel = 0.0f;
	mWaterLevel = 0.0f;

	mVelocity = 0.0f;

	mSpeedMultiplyer = speedMult;

	mCollider = new SphereCollider();
	mCollider->Position = XMFLOAT3(0, 0, 0);
}

Boat::~Boat()
{
	for (int i = 0; i < 3; i++)
	{
		delete mCameras[i];
		mCameras[i] = nullptr;
	}

	delete mCollider; mCollider = nullptr;

}

XMFLOAT3 Boat::GetForward()
{
	return mForwardVector;
}

XMFLOAT3 Boat::GetRight()
{
	return XMFLOAT3();
}

XMFLOAT3 Boat::GetUp()
{
	return mUpVector;
}

XMVECTOR Boat::GetUpVector()
{
	return XMLoadFloat3(&mUpVector);
}

Camera* Boat::GetSelectedCamera()
{
	return mSelectedCamera;
}

void Boat::SetSpeed(float speedMult)
{
	mSpeedMultiplyer = speedMult;
}

void Boat::Update(float deltaTime)
{
	mAccelerating = false;

	if (mSelectedCamera != nullptr)
	{
		if (GetAsyncKeyState(0x57))	//W
		{
			mAccelerating = true;
		}
		if (GetAsyncKeyState(0x44))	//D
		{
			mRotation.y += deltaTime;

			XMMATRIX rotationMatrix = XMMatrixRotationAxis(XMLoadFloat3(&mUpVector), deltaTime);

			XMStoreFloat3(&mForwardVector, XMVector3TransformNormal(XMLoadFloat3(&mForwardVector), rotationMatrix));
		}
		if (GetAsyncKeyState(0x41))	//A
		{
			mRotation.y -= deltaTime;

			XMMATRIX rotationMatrix = XMMatrixRotationAxis(XMLoadFloat3(&mUpVector), -deltaTime);

			XMStoreFloat3(&mForwardVector, XMVector3TransformNormal(XMLoadFloat3(&mForwardVector), rotationMatrix));
		}
	}

	//Switching cameras
	if (GetAsyncKeyState(0x31))	//1
	{
		mSelectedCamera = mCameras[DYNAMIC];
	}
	else if (GetAsyncKeyState(0x32))	//2
	{
		mSelectedCamera = mCameras[HORZIONTAL];
	}
	else if (GetAsyncKeyState(0x33))	//3
	{
		mSelectedCamera = mCameras[TOP_DOWN];
	}
	else if (GetAsyncKeyState(0x34))	//4
	{
		mSelectedCamera = nullptr;
	}

	if (mSelectedCamera != nullptr)
	{
		mSelectedCamera->Update(deltaTime);
	}

	if (mAccelerating)	//Calculating velocity and clamping
	{
		if (mVelocity + 2.0f * deltaTime > 7.0f)
		{
			mVelocity = 7.0f;
		}
		else
		{
			mVelocity += 2.0f * deltaTime;
		}
	}
	else
	{
		if (mVelocity - 2.0f * deltaTime < 0.0f)
		{
			mVelocity = 0.0f;
		}
		else
		{
			mVelocity -= 2.0f * deltaTime;
		}
	}

	XMStoreFloat3(&mPosition, XMVectorAdd(XMLoadFloat3(&mPosition), XMLoadFloat3(&mForwardVector) * deltaTime * mVelocity * mSpeedMultiplyer));


	mPosition.y = sin(0.5f * (mApp->GetTotalTime() + mPosition.z / 3.0f)) + 4.0f;

	mCollider->Position = mPosition;

	//Update up vector and calculate new rotation

	//XMFLOAT3 newUpVector = XMFLOAT3(0.0f, 0.5f, -0.5f * cos(0.5f * (mApp->GetTotalTime() + mPosition.z / 3.0f)));

	//XMStoreFloat3(&newUpVector, XMVector3Normalize(XMLoadFloat3(&newUpVector)));

	//XMStoreFloat(&mRotation.x, XMVector2AngleBetweenNormals(XMLoadFloat3(&mUpVector), XMLoadFloat3(&newUpVector)));

	//mUpVector = newUpVector;
}

void Boat::Load()
{
	mMesh = OBJLoader::Load(mMeshFilePath, mApp->GetDevice(), true, mCollider);
	CreateDDSTextureFromFile(mApp->GetDevice(), mTextureFilePath, nullptr, &mTexture);
}

SphereCollider* Boat::GetCollider()
{
	return mCollider;
}