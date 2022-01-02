#pragma once
#include "GameObject.h"

class Camera;

enum CAMERAS
{
	DYNAMIC = 0,
	HORZIONTAL,
	TOP_DOWN
};

struct ConstantBuffer;
struct SphereCollider;

class Boat : public GameObject
{
private:
	XMFLOAT3 mForwardVector;
	XMFLOAT3 mUpVector;

	Camera* mSelectedCamera;
	Camera* mCameras[3];

	SphereCollider* mCollider;

	float mWaterLevel;
	float mChangeInWaterLevel;

	float mVelocity;

	bool mAccelerating;

	float mSpeedMultiplyer;

public:
	Boat(XMFLOAT3 position, XMFLOAT3 rotation, XMFLOAT3 scale, char* meshFilePath, wchar_t* textureFilePath, Application* app, ConstantBuffer* cb, XMFLOAT3 forwardVector, XMFLOAT3 upVector, float windowWidth, float windowHeight, float nearCull, float farCull, float speedMult);
	~Boat();

	XMFLOAT3 GetForward();
	XMFLOAT3 GetRight();
	XMFLOAT3 GetUp();

	XMVECTOR GetUpVector();

	Camera* GetSelectedCamera();

	void SetSpeed(float speedMult);

	SphereCollider* GetCollider();

	void Update(float deltaTime);
	void Load();
};

