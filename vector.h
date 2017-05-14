#ifndef VECTOR_H
#define VECTOR_H

#include <d3d11.h>
#include <xnamath.h>


XMFLOAT3 Vec3Cross(XMFLOAT3 a, XMFLOAT3 b)
{
    XMFLOAT3 c = XMFLOAT3(a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x);
    return c;
}

float Vec3Dot(XMFLOAT3 a, XMFLOAT3 b)
{
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

float Vec3Length(const XMFLOAT3 &v)
{
    return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}


XMFLOAT3 Vec3normalize(const  XMFLOAT3 &a)
{
    XMFLOAT3 c = a;
    float len = Vec3Length(a);
    c.x /= len;
    c.y /= len;
    c.z /= len;
    return c;
}

XMFLOAT3 operator+(const XMFLOAT3 lhs, const XMFLOAT3 rhs)
{
    XMFLOAT3 c = XMFLOAT3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
    return c;
}


XMFLOAT3 operator-(XMFLOAT3 in)
{
    return XMFLOAT3(
        -in.x,
        -in.y,
        -in.z);
}

XMFLOAT3 operator-(const XMFLOAT3 lhs, const XMFLOAT3 rhs)
{
    XMFLOAT3 c = XMFLOAT3(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
    return c;
}

#endif