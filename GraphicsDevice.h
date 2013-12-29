#pragma once

#include "SimpleMath.h"
#include <string>
#include "Vertex.h"


enum DeviceAPI
{
    DirectX,
    OpenGL
};

class GraphicsDevice
{
public:
    GraphicsDevice(void);
    GraphicsDevice(DeviceAPI api) : api(api) { }
    ~GraphicsDevice(void);

    DeviceAPI api;

    virtual int Init() = 0;
    virtual int OnResize() = 0;
    virtual void BeginDraw() = 0;
    virtual void Draw(MeshHandle& hMesh, TextureHandle& hTex) = 0;
    virtual void EndDraw() = 0;
    virtual void SwapBuffer() = 0;
    virtual void Free() = 0;

    virtual int createVertexIndexBuffer(Mesh* mesh, VertexBufferHandle* hVBuf, IndexBufferHandle* hIBuf) = 0;

    virtual void setClearColor(Color color) = 0;
    virtual void clearRenderTarget() = 0;

    virtual void setVertexShader(VertexShaderHandle&) = 0;
    virtual void setPixelShader(PixelShaderHandle&) = 0;

    virtual void setWorld(Matrix&) = 0;
    virtual void setView(Vector4& eye, Vector4& at, Vector4& up) = 0;
    virtual void updateView(Vector4& eye, Vector4& at) = 0;
    virtual void setProjection(float fovAngleY = 1.570796371f, float nearClip = 0.01f, float farClip = 1000.0f) = 0;
    virtual void setOrthographicProjection(float nearClip = 0.01f, float farClip = 1000.0f) =0;
    

    
};

