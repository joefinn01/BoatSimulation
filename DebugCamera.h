#pragma once
#include "Camera.h"
class DebugCamera : public Camera
{
public:
	DebugCamera(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 at, DirectX::XMFLOAT3 up, float windowWidth, float windowHeight, float nearDepth, float farDepth) : Camera(position, at, up, windowWidth, windowHeight, nearDepth, farDepth)
	{

	}

	DebugCamera()
	{

	}

	void Update(float deltaTime);

private:

};

