#pragma once
#include "Camera.h"

class Boat;

class DynamicCamera : public Camera
{
public:
	DynamicCamera(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 at, DirectX::XMFLOAT3 up, float windowWidth, float windowHeight, float nearDepth, float farDepth, Boat* target);
	~DynamicCamera();

	DynamicCamera()
	{

	}

	void Update(float deltaTime);

private:
	float mDistanceToTarget;
	Boat* mTarget;
};

