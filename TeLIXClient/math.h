#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <windows.h>
#include <d2d1_3.h>
#include <dwrite.h>
#include <unordered_map>
#include <ctype.h>

struct float2
{
    union { float r; float x; };
    union { float g; float y; };
    float2() = default;
    float2(float _x, float _y) : x(_x), y(_y) {}
    float2(const D2D1_POINT_2F& p) : x(p.x), y(p.y) {}
    operator D2D1_POINT_2F() const { return { x,y }; }
    float* operator&() { return &x; }
    const float* operator&() const { return &x; }
};

struct float4
{
    union { float r; float x; };
    union { float g; float y; };
    union { float b; float z; };
    union { float a; float w; };
    float4() = default;
    float4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
    float4(const D2D1_COLOR_F& c) : r(c.r), g(c.g), b(c.b), a(c.a) {}
    operator D2D1_COLOR_F() const { return { r,g,b,a }; }
    float* operator&() { return &x; }
    const float* operator&() const { return &x; }
}; 
