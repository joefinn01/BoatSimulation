#include "DynamicCamera.h"
#include "Boat.h"
#include "Winuser.h"

using namespace DirectX;

DynamicCamera::DynamicCamera(XMFLOAT3 position, XMFLOAT3 at, XMFLOAT3 up, float windowWidth, float windowHeight, float nearDepth, float farDepth, Boat* target) : Camera(position, at, up, windowWidth, windowHeight, nearDepth, farDepth)
{
	mTarget = target;

	XMStoreFloat3(&mForwardVector, XMVectorSubtract(XMLoadFloat3(&target->GetPosition()), XMLoadFloat3(&mEye)));	//Calc vec to target

	XMVECTOR tempVec = XMVector3Length(XMLoadFloat3(&mForwardVector));	//Calc distance to target
	XMStoreFloat(&mDistanceToTarget, tempVec);

	XMStoreFloat3(&mForwardVector, XMVector3Normalize(XMLoadFloat3(&mForwardVector)));	//Normalize vector
}

DynamicCamera::~DynamicCamera()
{
	mTarget = nullptr;
}

void DynamicCamera::Update(float deltaTime)	//ADD MOUSE SCROLL WHEEL
{
	Camera::Update(deltaTime);

	float sensitivity = 1.5f;
	POINT mousePos;

	GetCursorPos(&mousePos);

	XMFLOAT2 deltaMousePos = XMFLOAT2((mWindowWidth / 2.0f) - mousePos.x, (mWindowHeight / 2.0f) - mousePos.y);
	SetCursorPos(mWindowWidth / 2, mWindowHeight / 2);

	mEye = mTarget->GetPosition();	//Translate to target for rotation

	//PITCH
	XMMATRIX rotationMatrix = XMMatrixRotationAxis(XMLoadFloat3(&GetRight()), deltaMousePos.y * deltaTime * sensitivity);

	XMStoreFloat3(&mForwardVector, XMVector3TransformNormal(XMLoadFloat3(&mForwardVector), rotationMatrix));
	XMStoreFloat3(&mUpVector, XMVector3TransformNormal(XMLoadFloat3(&mUpVector), rotationMatrix));

	//YAW
	rotationMatrix = XMMatrixRotationY(deltaMousePos.x * deltaTime * -sensitivity);

	XMStoreFloat3(&mForwardVector, XMVector3TransformNormal(XMLoadFloat3(&mForwardVector), rotationMatrix));
	XMStoreFloat3(&mUpVector, XMVector3TransformNormal(XMLoadFloat3(&mUpVector), rotationMatrix));

	XMStoreFloat3(&mEye, XMVectorSubtract(XMLoadFloat3(&mEye), XMLoadFloat3(&mForwardVector) * mDistanceToTarget));	//Reset camera position to distance before
}