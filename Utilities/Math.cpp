#include "stdafx.h"
#include "Math.h"

const float Math::PI = 3.14159265f;
const float Math::EPSILON= 0.000001f;
float Math::Modulo(float val1, float val2)
{
	while (val1 - val2 >= 0)
		val1 -= val2;

	return val1;
}

float Math::ToRadian(float degree)
{
	return degree * PI / 180.0f;
}

float Math::ToDegree(float radian)
{
	return radian * 180.0f / PI;
}

float Math::Random(float r1, float r2)
{
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = r2 - r1;
	float val = random * diff;
	
	return r1 + val;
}

float Math::Clamp(float value, float min, float max)
{
	value = value > max ? max : value;
	value = value < min ? min : value;

	return value;
}

void Math::LerpMatrix(OUT D3DXMATRIX & out, D3DXMATRIX & m1, D3DXMATRIX & m2, float amount)
{
	out._11 = m1._11 + (m2._11 - m1._11) * amount;
	out._12 = m1._12 + (m2._12 - m1._12) * amount;
	out._13 = m1._13 + (m2._13 - m1._13) * amount;
	out._14 = m1._14 + (m2._14 - m1._14) * amount;

	out._21 = m1._21 + (m2._21 - m1._21) * amount;
	out._22 = m1._22 + (m2._22 - m1._22) * amount;
	out._23 = m1._23 + (m2._23 - m1._23) * amount;
	out._24 = m1._24 + (m2._24 - m1._24) * amount;

	out._31 = m1._31 + (m2._31 - m1._31) * amount;
	out._32 = m1._32 + (m2._32 - m1._32) * amount;
	out._33 = m1._33 + (m2._33 - m1._33) * amount;
	out._34 = m1._34 + (m2._34 - m1._34) * amount;

	out._41 = m1._41 + (m2._41 - m1._41) * amount;
	out._42 = m1._42 + (m2._42 - m1._42) * amount;
	out._43 = m1._43 + (m2._43 - m1._43) * amount;
	out._44 = m1._44 + (m2._44 - m1._44) * amount;
}

float Math::ClosestPtSegmentSegment(const D3DXVECTOR3 & p1, const D3DXVECTOR3 & q1, const D3DXVECTOR3 & p2, const D3DXVECTOR3 & q2, float & s, float & t, D3DXVECTOR3 & c1, D3DXVECTOR3 & c2)
{
	D3DXVECTOR3 d1 = q1 - p1; // Direction segment s1
	D3DXVECTOR3 d2 = q2 - p2; // Direction segment s2
	D3DXVECTOR3 r = p1 - p2;
	float a = D3DXVec3Dot(&d1, &d1); // Squared length of segment s1
	float e = D3DXVec3Dot(&d2, &d2); // Squared length of segment s2
	float f = D3DXVec3Dot(&d2, &r);

	// degenerate ÅðÈ­½ÃÅ°´Ù
	// Check if either or both segments degenerate into points
	if (a <= Math::EPSILON && e <= Math::EPSILON)
	{
		// Both segements degenerate into points
		s = t = 0.0f;
		c1 = p1;
		c2 = p2;

		return D3DXVec3Dot(&(c1 - c2), &(c1 - c2));
	}
	if (a <= Math::EPSILON)
	{
		// First segment degenerates into a point
		s = 0.0f;
		t = f / e; // s = 0 => (b*s + f) / e = f / e
		t = Math::Clamp(t, 0.0f, 1.0f);
	}
	else
	{
		float c = D3DXVec3Dot(&d1, &r);
		if (e <= Math::EPSILON)
		{
			// Second segement degenrates into a point
			t = 0.0f;
			s = Math::Clamp(-c / a, 0.0f, 1.0f); // t = 0 => s = (b*t - c) / a = -c / a
		}
		else
		{
			// The general nondegenerate case starts here
			float b = D3DXVec3Dot(&d1, &d2);
			float denom = a*e - b*b; // Always nonnegative

									 // If segments not parallel, compute closest point on L1 to L2 and
									 // clamp to segment S1, Else pick arbitrary s (here 0)
			if (denom != 0.0f)
				s = Math::Clamp((b*f - c*e) / denom, 0.0f, 1.0f);
			else
				s = 0.0f;

			// Compute point on L2 closest to S1(s) using
			// t = Dot((P1 + D1*s - P2,D2) / Dot(D2,D2) = (b*s + f) /e
			t = (b*s + f) / e;

			// If t in [0,1] done. Else clamp t, recompute s for the new value
			// of t using s = Dot((P2 + D2*t) - P1,D1) / Dot(D1,D1) = (t*b - c) / a
			// and clamp s to [0, 1]
			if (t < 0.0f) {
				t = 0.0f;
				s = Math::Clamp(-c / a, 0.0f, 1.0f);
			}
			else if (t > 1.0f) {
				t = 1.0f;
				s = Math::Clamp((b - c) / a, 0.0f, 1.0f);
			}
		}
	}

	c1 = p1 + d1 * s;
	c2 = p2 + d2 * t;

	return D3DXVec3Dot(&(c1 - c2), &(c1 - c2));
}

D3DXQUATERNION Math::LookAt(const D3DXVECTOR3 & origin, const D3DXVECTOR3 & target, const D3DXVECTOR3 & up)
{
	//¹æÇâ
	D3DXVECTOR3 f = (origin - target);
	D3DXVec3Normalize(&f, &f);

	//¾÷º¤ÅÍ
	D3DXVECTOR3 s;
	D3DXVec3Cross(&s, &up, &f);
	D3DXVec3Normalize(&s, &s);

	//¿À¸¥ÂÊº¤ÅÍ
	D3DXVECTOR3 u;
	D3DXVec3Cross(&u, &f, &s);

	float z = 1.0f + s.x + u.y + f.z;
	float fd = 2.0f * sqrtf(z);

	D3DXQUATERNION result;

	if (z > Math::EPSILON)
	{
		result.w = 0.25f * fd;
		result.x = (f.y - u.z) / fd;
		result.y = (s.z - f.x) / fd;
		result.z = (u.x - s.y) / fd;
	}
	else if (s.x > u.y && s.x > f.z)
	{
		fd = 2.0f * sqrtf(1.0f + s.x - u.y - f.z);
		result.w = (f.y - u.z) / fd;
		result.x = 0.25f * fd;
		result.y = (u.x + s.y) / fd;
		result.z = (s.z + f.x) / fd;
	}
	else if (u.y > f.z)
	{
		fd = 2.0f * sqrtf(1.0f + u.y - s.x - f.z);
		result.w = (s.z - f.x) / fd;
		result.x = (u.x - s.y) / fd;
		result.y = 0.25f * fd;
		result.z = (f.y + u.z) / fd;
	}
	else
	{
		fd = 2.0f * sqrtf(1.0f + f.z - s.x - u.y);
		result.w = (u.x - s.y) / fd;
		result.x = (s.z + f.x) / fd;
		result.y = (f.y + u.z) / fd;
		result.z = 0.25f * fd;
	}

	return result;
}

void Math::toEulerAngle(const D3DXQUATERNION & q, float & pitch, float & yaw, float & roll)
{
	////pitch (x-axis rotation)
	//float sinr = +2.0f * (q.w * q.x + q.y * q.z);
	//float cosr = +1.0f - 2.0f * (q.x * q.x + q.y * q.y);
	//pitch = atan2f(sinr, cosr);

	//// yaw (y-axis rotation)
	//float sinp = +2.0f * (q.w * q.y - q.z * q.x);
	//if (fabs(sinp) >= 1)
	//	yaw = (float)copysign(D3DX_PI / 2.0f, sinp); // use 90 degrees if out of range
	//else
	//	yaw = asinf(sinp);

	//// roll (z-axis rotation)
	//float siny = +2.0f * (q.w * q.z + q.x * q.y);
	//float cosy = +1.0f - 2.0f * (q.y * q.y + q.z * q.z);
	//roll = atan2f(siny, cosy);

	float sqw = q.w*q.w;
	float sqx = q.x*q.x;
	float sqy = q.y*q.y;
	float sqz = q.z*q.z;

	pitch = asinf(2.0f * (q.w*q.x - q.y*q.z)); // rotation about x-axis
	yaw = atan2f(2.0f * (q.x*q.z + q.w*q.y), (-sqx - sqy + sqz + sqw)); // rotation about y-axis
	roll = atan2f(2.0f * (q.x*q.y + q.w*q.z), (-sqx + sqy - sqz + sqw)); // rotation about z-axis
}

void Math::toEulerAngle(const D3DXQUATERNION & q, D3DXVECTOR3 & out)
{
	toEulerAngle(q, out.x, out.y, out.z);
}

int Math::Random(int r1, int r2)
{
	return (int)(rand() % (r2 - r1 + 1)) + r1;
}