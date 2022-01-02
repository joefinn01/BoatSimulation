#pragma once
#include "GameObject.h"

class Boat;
class SphereCollider;

class Duck : public GameObject
{
private:
	Boat* mTarget;

	XMFLOAT3 mForward;
	XMFLOAT3 mUp;

	float mSpeed;

	SphereCollider* mCollider;

	bool mActive;

public:
	Duck(XMFLOAT3 position, XMFLOAT3 rotation, XMFLOAT3 scale, char* meshFilePath, wchar_t* textureFilePath, Application* app, ConstantBuffer* cb, Boat* target);

	void Update(float deltaTime);

	void Load();

	void SetSpeed(float speed);

	void SetActive(bool active);
	bool IsActive();
};

