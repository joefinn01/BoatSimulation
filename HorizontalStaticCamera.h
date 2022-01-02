#pragma once
#include "Camera.h"

class Boat;

class HorizontalStaticCamera : public Camera
{
private:
	Boat* mTarget;

	float mDistanceToTarget;

	DirectX::XMFLOAT3 mVectorToTarget;

public:
	HorizontalStaticCamera(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 at, DirectX::XMFLOAT3 up, float windowWidth, float windowHeight, float nearDepth, float farDepth, Boat* target);
	~HorizontalStaticCamera();

	void Update(float deltaTime);
};

