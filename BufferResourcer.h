#pragma once

#include <d3d11_1.h>
#include "vertex.h"
#include <vector>
#include <unordered_map>
#include <string>


class Mesh;
class BufferResourcer;

struct VertexBufferHandle
{
    int ID;
};

struct IndexBufferHandle
{
    int ID;
};

struct VertexBuffer
{
     int Stride;
     int Length;
     D3D11_PRIMITIVE_TOPOLOGY primitiveTopology;
     int lastIndex;

     bool isDynamic;
     ID3D11Buffer* buffer;
};

struct IndexBuffer
{
    int Length;
    int lastIndex;

    bool isDynamic;
    ID3D11Buffer* buffer;
};



class BufferResourcer
{
public:
    void SetDevice(ID3D11Device* _device, ID3D11DeviceContext* _context);

    VertexBufferHandle CreateStaticVertexBuffer(void* vertices, int numVertices, int stride);
    IndexBufferHandle CreateStaticIndexBuffer(void* indices, int numIndices);

    VertexBufferHandle CreateDynamicVertexBuffer(int vertexLength);
    IndexBufferHandle CreateDynamicIndexBuffer(int indexLength);
    int InsertToDynamicVertexBuffer(void* vertices, int numVertices, int stride, VertexBufferHandle& hV);
    int InsertToDynamicIndexBuffer(void* indices, int numIndices, IndexBufferHandle& hI);

    VertexBuffer& getVertexBuffer(VertexBufferHandle h);
    IndexBuffer&  getIndexBuffer(IndexBufferHandle h);

    void Dispose();

    static BufferResourcer& Instance();

private:

    std::vector<VertexBuffer> VBuffers;
    std::vector<IndexBuffer> IBuffers;

    ID3D11Device* device;
    ID3D11DeviceContext* context;

    //Singleton
    BufferResourcer(void);
    ~BufferResourcer(void);
    BufferResourcer(BufferResourcer const&);              
    void operator=(BufferResourcer const&); 
};

