#include "Camera.h"

using namespace DirectX;

Camera::Camera(XMFLOAT3 position, XMFLOAT3 at, XMFLOAT3 up, float windowWidth, float windowHeight, float nearDepth, float farDepth)
{
	mEye = position;
	mAt = at;
	mUpVector = up;

	XMStoreFloat3(&mForwardVector, XMVector3Normalize(XMVectorSubtract(XMLoadFloat3(&mAt), XMLoadFloat3(&mEye))));

	mRotation = XMFLOAT3(0,0,0);

	mWindowWidth = windowWidth;
	mWindowHeight = windowHeight;

	mNearDepth = nearDepth;
	mFarDepth = farDepth;

	XMStoreFloat4x4(&mProjectionMatrix, XMMatrixPerspectiveFovLH(XM_PIDIV2, mWindowWidth / (FLOAT)mWindowHeight, nearDepth, farDepth));
	XMStoreFloat4x4(&mViewMatrix, XMMatrixLookToLH(XMLoadFloat3(&mEye), XMLoadFloat3(&mForwardVector), XMLoadFloat3(&mUpVector)));
}

Camera::Camera()
{

}

Camera::~Camera()
{

}

void Camera::SetPosition(XMFLOAT3 position)
{
	mEye = position;
}

void Camera::ConcatenatePosition(XMFLOAT3 vec)
{
	XMStoreFloat3(&mEye, XMVectorAdd(XMLoadFloat3(&vec), XMLoadFloat3(&mEye)));
}

void Camera::SetLookAt(XMFLOAT3 at)
{
	mAt = at;
}

void Camera::SetUp(XMFLOAT3 up)
{
	mUpVector = up;
}

XMFLOAT3 Camera::GetPosition()
{
	return mEye;
}

XMFLOAT3 Camera::GetLookAt()
{
	return mAt;
}

XMFLOAT3 Camera::GetUp()
{
	return mUpVector;
}

XMVECTOR Camera::GetUpVector()
{
	return XMLoadFloat3(&mUpVector);
}

XMVECTOR Camera::GetRightVector()
{
	return XMVector3Cross(GetForwardVector(), GetUpVector());
}

XMFLOAT3 Camera::GetRight()
{
	XMFLOAT3 temp;

	XMStoreFloat3(&temp, XMVector3Cross(GetForwardVector(), GetUpVector()));

	return temp;
}

XMFLOAT4X4 Camera::GetViewMatrix()
{
	return mViewMatrix;
}

XMFLOAT4X4 Camera::GetProjectionMatrix()
{
	return mProjectionMatrix;
}

XMFLOAT4X4 Camera::GetViewProjectionMatrix()
{
	XMFLOAT4X4 temp;

	return XMFLOAT4X4();
}

XMVECTOR Camera::GetForwardVector()
{
	return XMLoadFloat3(&mForwardVector);
}

void Camera::Update(float deltaTime)
{
	XMStoreFloat4x4(&mViewMatrix, XMMatrixLookToLH(XMLoadFloat3(&mEye), XMLoadFloat3(&mForwardVector), XMLoadFloat3(&mUpVector)));
}

void Camera::Reshape(float windowWidth, float windowHeight, float nearDepth, float farDepth)
{
	mWindowHeight = windowHeight;
	mWindowWidth = windowWidth;

	mNearDepth = nearDepth;
	mFarDepth = farDepth;

	XMStoreFloat4x4(&mProjectionMatrix, XMMatrixPerspectiveFovLH(XM_PIDIV2, mWindowWidth / (FLOAT)mWindowHeight, nearDepth, farDepth));
}


