#ifndef VERTEX_H
#define VERTEX_H

#include <d3d11.h>
#include <xnamath.h>

struct CatClarkVertex
{
    XMFLOAT3 pos;
    XMFLOAT3 norm;
    XMFLOAT2 tex;
};


struct SimpleVertex {
    XMFLOAT3 norm;
    XMFLOAT2 tex;
    XMFLOAT3 pos;
};

#endif