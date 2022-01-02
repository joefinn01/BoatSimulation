#pragma once
#include <windows.h>
#include "Vector3D.h"
#include <d3d11_1.h>
#include <directxmath.h>

class Camera
{
public:
	Camera(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 at, DirectX::XMFLOAT3 up, float windowWidth, float windowHeight, float nearDepth, float farDepth);
	Camera();
	~Camera();

	virtual void Update(float deltaTime);

	void SetPosition(DirectX::XMFLOAT3 position);
	void SetLookAt(DirectX::XMFLOAT3 targetPosition);
	void SetUp(DirectX::XMFLOAT3 up);



	DirectX::XMFLOAT3 GetPosition();
	void ConcatenatePosition(DirectX::XMFLOAT3 vec);

	DirectX::XMFLOAT3 GetLookAt();
	DirectX::XMFLOAT3 GetUp();
	DirectX::XMVECTOR GetUpVector();
	DirectX::XMVECTOR GetRightVector();
	DirectX::XMFLOAT3 GetRight();
	DirectX::XMVECTOR GetForwardVector();

	DirectX::XMFLOAT4X4 GetViewMatrix();
	DirectX::XMFLOAT4X4 GetProjectionMatrix();
	DirectX::XMFLOAT4X4 GetViewProjectionMatrix();

	void Reshape(float windowWidth, float windowHeight, float nearDepth, float farDepth);

protected:
	DirectX::XMFLOAT3 mEye;
	DirectX::XMFLOAT3 mAt;
	DirectX::XMFLOAT3 mUpVector;

	DirectX::XMFLOAT3 mForwardVector;

	DirectX::XMFLOAT3 mRotation;

	float mWindowHeight;
	float mWindowWidth;
	float mNearDepth;
	float mFarDepth;

	DirectX::XMFLOAT4X4 mViewMatrix;
	DirectX::XMFLOAT4X4 mProjectionMatrix;
};

