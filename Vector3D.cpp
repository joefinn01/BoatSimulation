#include "Vector3D.h"
#include <assert.h>
#include <math.h>

Vector3D::Vector3D()
{
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
}

Vector3D::Vector3D(float _x, float _y, float _z)
{
	x = _x;
	y = _y;
	z = _z;
}

Vector3D::Vector3D(const Vector3D& vec)
{
	x = vec.x;
	y = vec.y;
	z = vec.z;
}

Vector3D Vector3D::operator+(const Vector3D vec)
{
	return Vector3D(x + vec.x, y + vec.y, z + vec.z);
}

Vector3D Vector3D::operator-(const Vector3D vec)
{
	return Vector3D(x - vec.x, y - vec.y, z - vec.z);
}

Vector3D Vector3D::operator*(const float value)
{
	return Vector3D(x * value, y * value, z * value);
}

Vector3D Vector3D::operator/(const float value)
{
	return Vector3D(x / value, y / value, z / value);
}

Vector3D& Vector3D::operator+=(const Vector3D vec)
{
	x += vec.x;
	y += vec.y;
	z += vec.z;

	return *this;
}

Vector3D& Vector3D::operator-=(const Vector3D vec)
{
	x -= vec.x;
	y -= vec.y;
	z -= vec.z;

	return *this;
}

Vector3D& Vector3D::operator*=(const float value)
{
	x *= value;
	y *= value;
	z *= value;

	return *this;
}

Vector3D& Vector3D::operator/=(const float value)
{
	assert(value != 0);	//If value == 0 error

	x /= value;
	y /= value;
	z /= value;

	return *this;
}

Vector3D& Vector3D::operator=(const Vector3D vec)
{
	x = vec.x;
	y = vec.y;
	z = vec.z;

	return *this;
}

float Vector3D::DotProduct(const Vector3D& vec)
{
	return (x * vec.x) + (y * vec.y) + (z * vec.z);
}

Vector3D Vector3D::CrossProduct(const Vector3D& vec)
{
	return Vector3D(y * vec.z - z * vec.y, z * vec.x - x * vec.z, x * vec.y - y * vec.x);
}

Vector3D Vector3D::Normalize()
{
	*this /= Magnitude();

	return *this / Magnitude();
}

float Vector3D::Magnitude()
{
	return sqrtf((x * x + y * y + z * z));
}