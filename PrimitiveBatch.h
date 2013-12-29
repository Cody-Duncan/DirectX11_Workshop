#pragma once

#include "DeviceAPI.h"
#include "GraphicsResourceHandles.h"

enum PrimitiveType
{
    Tri,
    Line
};

class ST_API PrimitiveBatch
{
public:
    PrimitiveBatch() {};
    PrimitiveBatch(DeviceAPI api) : API(api) {}
    virtual ~PrimitiveBatch(void) = 0 {};

    DeviceAPI API;

    virtual void Init() = 0;

    virtual void Begin(bool AlphaBlend = false) = 0;
    virtual void DrawTriangles(unsigned int layer, Vector2* points, int pointLength, Matrix transform, Color c) = 0;
    virtual void DrawLines(unsigned int layer, Vector2* points, int pointLength, Matrix transform, Color c) = 0;
    virtual void End() = 0;
   
    virtual void Dispose() = 0;

    virtual void DrawBatch(int layer, PrimitiveType topology) = 0;
};