#pragma once
#include "Camera.h"

class Boat;

//CHANGE NAME TO TOP DOWN STATIC CAMERA

class TopDownStaticCamera : public Camera
{
private:
	Boat* mTarget;

	float mDistanceToTarget;

	DirectX::XMFLOAT3 mVectorToTarget;

public:
	TopDownStaticCamera(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 at, DirectX::XMFLOAT3 up, float windowWidth, float windowHeight, float nearDepth, float farDepth, Boat* target);
	~TopDownStaticCamera();

	void Update(float deltaTime);
};