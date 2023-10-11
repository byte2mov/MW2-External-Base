#pragma once
#include <d3d9.h>
#include <math.h>

#define M_PI 3.14159265358979323846264338327950288419716939937510

class Vector2
{
public:
	double x;
	double y;

	inline bool is_Zero() {
		return (x == 0) && (y == 0);
	}
};


struct FRotator
{
public:
	double Pitch;
	double Yaw;
	double Roll;
};



class Vector3
{
public:
	Vector3() : x(0.f), y(0.f), z(0.f)
	{

	}

	Vector3(double _x, double _y, double _z) : x(_x), y(_y), z(_z)
	{

	}
	~Vector3()
	{

	}

	double x;
	double y;
	double z;

	inline double Dot(Vector3 v)
	{
		return x * v.x + y * v.y + z * v.z;
	}

	inline double Distance(Vector3 v)
	{
		return double(sqrtf(powf(v.x - x, 2.0) + powf(v.y - y, 2.0) + powf(v.z - z, 2.0)));
	}

	inline double Length()
	{
		return sqrt(x * x + y * y + z * z);
	}

	Vector3 operator+(Vector3 v)
	{
		return Vector3(x + v.x, y + v.y, z + v.z);
	}

	Vector3 operator-(Vector3 v)
	{
		return Vector3(x - v.x, y - v.y, z - v.z);
	}

	Vector3 operator*(double number) const
	{
		return Vector3(x * number, y * number, z * number);
	}
};

struct FPlane : Vector3
{
	double W = 0;


	Vector3 ToVector3()
	{
		Vector3 value;
		value.x = this->x;
		value.y = this->y;
		value.z = this->y;

		return value;
	}
};

struct FQuat
{
	double x;
	double y;
	double z;
	double w;
};

struct FMatrix
{
	FPlane XPlane;
	FPlane YPlane;
	FPlane ZPlane;
	FPlane WPlane;
};

