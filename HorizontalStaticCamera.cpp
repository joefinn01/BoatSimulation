#include "HorizontalStaticCamera.h"
#include "Boat.h"

using namespace DirectX;

HorizontalStaticCamera::HorizontalStaticCamera(XMFLOAT3 position, XMFLOAT3 at, XMFLOAT3 up, float windowWidth, float windowHeight, float nearDepth, float farDepth, Boat* target) : Camera(position, at, up, windowWidth, windowHeight, nearDepth, farDepth)
{
	mTarget = target;

	XMStoreFloat3(&mVectorToTarget, XMVectorSubtract(XMLoadFloat3(&target->GetPosition()), XMLoadFloat3(&mEye)));

	XMStoreFloat(&mDistanceToTarget, XMVector3Length(XMLoadFloat3(&mVectorToTarget)));

	XMStoreFloat3(&mVectorToTarget, XMVector3Normalize(XMLoadFloat3(&mVectorToTarget)) * -1.0f);	//Normalize vector to target
}

HorizontalStaticCamera::~HorizontalStaticCamera()
{
	mTarget = nullptr;
}

void HorizontalStaticCamera::Update(float deltaTime)
{
	Camera::Update(deltaTime);

	mForwardVector = mTarget->GetForward();
	XMStoreFloat3(&mVectorToTarget, XMLoadFloat3(&mForwardVector) * -1.0f);	//Vector to target is same as forward vector just backwards

	mUpVector = mTarget->GetUp();

	XMStoreFloat3(&mEye, XMVectorAdd(XMLoadFloat3(&mTarget->GetPosition()), XMLoadFloat3(&mForwardVector) * mDistanceToTarget));	//Update position of camera
}
