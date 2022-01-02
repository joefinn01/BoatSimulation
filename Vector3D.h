#pragma once
class Vector3D
{
public:
	float x, y, z;

	//Constructors
	Vector3D();
	Vector3D(float _x, float _y, float _z);
	Vector3D(const Vector3D& vec);

	//Operations
	Vector3D operator+(const Vector3D vec);
	Vector3D operator-(const Vector3D vec);
	Vector3D operator*(const float value);
	Vector3D operator/(const float value);

	Vector3D& operator+=(const Vector3D vec);
	Vector3D& operator-=(const Vector3D vec);
	Vector3D& operator*=(const float value);
	Vector3D& operator/=(const float value);
	Vector3D& operator=(const Vector3D vec);

	//Vector operations
	float DotProduct(const Vector3D& vec);
	Vector3D CrossProduct(const Vector3D& vec);
	Vector3D Normalize();
	float Magnitude();

private:

};

