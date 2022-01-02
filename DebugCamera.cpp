#include "DebugCamera.h"

using namespace DirectX;

void DebugCamera::Update(float deltaTime)
{
	Camera::Update(deltaTime);

	POINT mousePos;

	float sensitivity = 1.5f;

	if (GetAsyncKeyState(VK_UP) || GetAsyncKeyState(0x57))	//W
	{
		XMStoreFloat3(&mEye, XMVectorAdd(XMLoadFloat3(&GetPosition()), GetForwardVector() * deltaTime * 25.0f));
	}
	if (GetAsyncKeyState(VK_DOWN) || GetAsyncKeyState(0x53)) //S
	{
		XMStoreFloat3(&mEye, XMVectorSubtract(XMLoadFloat3(&GetPosition()), GetForwardVector() * deltaTime * 25.0f));
	}
	if (GetAsyncKeyState(VK_LEFT) || GetAsyncKeyState(0x41)) //A
	{
		XMStoreFloat3(&mEye, XMVectorAdd(XMLoadFloat3(&GetPosition()), GetRightVector() * deltaTime * 25.0f));
	}
	if (GetAsyncKeyState(VK_RIGHT) || GetAsyncKeyState(0x44))	//D
	{
		XMStoreFloat3(&mEye, XMVectorSubtract(XMLoadFloat3(&GetPosition()), GetRightVector() * deltaTime * 25.0f));
	}

	GetCursorPos(&mousePos);

	XMFLOAT2 deltaMousePos = XMFLOAT2((mWindowWidth / 2.0f) - mousePos.x, (mWindowHeight / 2.0f) - mousePos.y);
	SetCursorPos(mWindowWidth / 2, mWindowHeight / 2);

	//PITCH
	XMMATRIX rotationMatrix = XMMatrixRotationAxis(XMLoadFloat3(&GetRight()), deltaMousePos.y * deltaTime * sensitivity);

	XMStoreFloat3(&mForwardVector, XMVector3TransformNormal(XMLoadFloat3(&mForwardVector), rotationMatrix));
	XMStoreFloat3(&mUpVector, XMVector3TransformNormal(XMLoadFloat3(&mUpVector), rotationMatrix));

	//YAW
	rotationMatrix = XMMatrixRotationY(deltaMousePos.x * deltaTime * -sensitivity);

	XMStoreFloat3(&mForwardVector, XMVector3TransformNormal(XMLoadFloat3(&mForwardVector), rotationMatrix));
	XMStoreFloat3(&mUpVector, XMVector3TransformNormal(XMLoadFloat3(&mUpVector), rotationMatrix));
}