#include "TopDownStaticCamera.h"
#include "Boat.h"

using namespace DirectX;

TopDownStaticCamera::TopDownStaticCamera(XMFLOAT3 position, XMFLOAT3 at, XMFLOAT3 up, float windowWidth, float windowHeight, float nearDepth, float farDepth, Boat* target) : Camera(position, at, up, windowWidth, windowHeight, nearDepth, farDepth)
{
	mTarget = target;

	XMStoreFloat3(&mVectorToTarget, XMVectorSubtract(XMLoadFloat3(&target->GetPosition()), XMLoadFloat3(&mEye)));

	XMStoreFloat(&mDistanceToTarget, XMVector3Length(XMLoadFloat3(&mVectorToTarget)));

	XMStoreFloat3(&mVectorToTarget,  -1.0f * XMVector3Normalize(XMLoadFloat3(&mVectorToTarget)));	//Normalize vector to target
}

TopDownStaticCamera::~TopDownStaticCamera()
{
	mTarget = nullptr;
}

void TopDownStaticCamera::Update(float deltaTime)
{
	Camera::Update(deltaTime);

	XMStoreFloat3(&mVectorToTarget, mTarget->GetUpVector() * -1.0f);
	mForwardVector = mVectorToTarget;

	mUpVector = mTarget->GetForward();

	XMStoreFloat3(&mEye, XMVectorAdd(XMLoadFloat3(&mTarget->GetPosition()), XMLoadFloat3(&mVectorToTarget) * mDistanceToTarget * -1.0f));	//Update position of camera
}
