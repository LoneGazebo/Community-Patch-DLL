#pragma once
#ifndef CVPOINT_H
#define CVPOINT_H

class CvPoint2
{
public:
	CvPoint2(): x(0), y(0) {}
	CvPoint2(float fx, float fy) : x(fx),y(fy) {}

	float x, y;
};


class CvPoint3
{
public:
	CvPoint3(): x(0), y(0), z(0) {}
	CvPoint3(float fx, float fy, float fz) : x(fx),y(fy),z(fz) {} 

	bool operator== (const CvPoint3& pt) const
	{	return (x == pt.x && y == pt.y && z == pt.z);	}

	inline CvPoint3 operator+ (const CvPoint3& pt) const
	{	return CvPoint3(x+pt.x,y+pt.y,z+pt.z);	}

	inline CvPoint3 operator- (const CvPoint3& pt) const
	{	return CvPoint3(x-pt.x,y-pt.y,z-pt.z);	}

	inline float operator* (const CvPoint3& pt) const
	{	return x*pt.x+y*pt.y+z*pt.z;	}

	inline CvPoint3 operator* (float fScalar) const
	{	return CvPoint3(fScalar*x,fScalar*y,fScalar*z);	}

	inline CvPoint3 operator/ (float fScalar) const
	{
		float fInvScalar = 1.0f/fScalar;
		return CvPoint3(fInvScalar*x,fInvScalar*y,fInvScalar*z);
	}

	inline CvPoint3 operator- () const
	{	return CvPoint3(-x,-y,-z);	}

	inline float Length() const
	{ return sqrt(x * x + y * y + z * z); }

	inline float Unitize()
	{
		float length = Length();
		if(length != 0)
		{
			x /= length;
			y /= length;
			z /= length;
		}
		return length;
	}
	
	float x, y, z;
};

#endif //CVPOINT_H