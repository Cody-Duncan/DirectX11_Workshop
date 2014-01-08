#include "BufferResourcer.h"
#include "Debug_Graphics.h"

BufferResourcer& BufferResourcer::Instance()
{
    static BufferResourcer b;
    return b;
}

BufferResourcer::BufferResourcer(void)
{
    
}


BufferResourcer::~BufferResourcer(void)
{
    Dispose();
}

void BufferResourcer::SetDevice(ID3D11Device* _device, ID3D11DeviceContext* _context)
{
    device = _device;
    context = _context;
}

VertexBuffer& BufferResourcer::getVertexBuffer(VertexBufferHandle h)
{
   return VBuffers[h.ID];
}

IndexBuffer& BufferResourcer::getIndexBuffer(IndexBufferHandle h)
{
    return IBuffers[h.ID];
}

VertexBufferHandle BufferResourcer::CreateStaticVertexBuffer(void* vertices, int numVertices, int stride)
{
    //generate vertex buffer
    D3D11_BUFFER_DESC bd;
    ZeroMemory( &bd, sizeof(bd) );
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = stride * numVertices;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA InitData;
    ZeroMemory( &InitData, sizeof(InitData) );
    InitData.pSysMem = vertices;

    ID3D11Buffer* newVertexBuffer;
    HRESULT hr = device->CreateBuffer( &bd, &InitData, &newVertexBuffer );
    //#TODO report HR failure
    SetDebugObjectName(newVertexBuffer,"Vertex Buffer");

    // create data references
    VertexBuffer dVBuf =
    {
        stride,                                 //stride
        numVertices,                            //vertexLength
        D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,  //topology
        0,                                      //startVertex
        false,                                  //isDynamic
        newVertexBuffer
    };

    VBuffers.push_back(dVBuf);    //store it

    VertexBufferHandle hV = { VBuffers.size() - 1 };

    return hV;
}

IndexBufferHandle BufferResourcer::CreateStaticIndexBuffer(void* indices, int numIndices)
{
    //generate index buffer
    D3D11_BUFFER_DESC bd;
    ID3D11Buffer* newIndexBuffer;
    ZeroMemory( &bd, sizeof(bd) );
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof( unsigned int ) * numIndices;        // 36 vertices needed for 12 triangles in a triangle list
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA InitData;
    ZeroMemory( &InitData, sizeof(InitData) );
    InitData.pSysMem = indices;

    HRESULT hr = device->CreateBuffer( &bd, &InitData, &newIndexBuffer );
    //#TODO report HR failure
    SetDebugObjectName(newIndexBuffer,"Index Buffer");

    // create data references
    IndexBuffer dIBuf =
    {
        numIndices,                           //indexLength
        0,                                    //startIndex
        false,                                //isDynamic
        newIndexBuffer
    };

    IBuffers.push_back(dIBuf);       //store it

    //set handles 
    IndexBufferHandle hI = { IBuffers.size() - 1 };

    return hI;
}

VertexBufferHandle BufferResourcer::CreateDynamicVertexBuffer(int vertexLength)
{
    //vertex buffer
    D3D11_BUFFER_DESC bd;
    ZeroMemory( &bd, sizeof(bd) );
    bd.Usage = D3D11_USAGE_DYNAMIC;                 //must be dynamic
    bd.ByteWidth = sizeof( Vertex ) * vertexLength;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;     //dynamic requires write access

    ID3D11Buffer* newVertexBuffer;
    HRESULT hr = device->CreateBuffer( &bd, nullptr, &newVertexBuffer );
    //#TODO report HR failure

    SetDebugObjectName(newVertexBuffer,"dynamic vertex buffer");

     // create data references
    VertexBuffer dVBuf =
    {
        sizeof(Vertex),                         //stride
        vertexLength,                           //vertexLength
        D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,  //topology
        0,                                      //startVertex
        true,                                   //isDynamic
        newVertexBuffer
    };

    VBuffers.push_back(dVBuf);
    VertexBufferHandle hV = { VBuffers.size()-1 };   //create and send back handle

    return hV;
}

IndexBufferHandle BufferResourcer::CreateDynamicIndexBuffer(int indexLength)
{
    //index buffer
    D3D11_BUFFER_DESC bd;
    ZeroMemory( &bd, sizeof(bd) );
    bd.Usage = D3D11_USAGE_DYNAMIC;                 //must be dynamic
    bd.ByteWidth = sizeof( unsigned int ) * indexLength;       
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;     //dynamic requires write access

    ID3D11Buffer* newIndexBuffer;
    HRESULT hr = device->CreateBuffer( &bd, nullptr, &newIndexBuffer );
    //#TODO report HR failure

    SetDebugObjectName(newIndexBuffer,"Dynamic Index Buffer");

    IndexBuffer dIBuf =
    {
        indexLength,                          //indexLength
        0,                                    //startIndex
        false,                                //isDynamic
        newIndexBuffer
    };
   
    IBuffers.push_back(dIBuf);
    IndexBufferHandle hI  = { IBuffers.size()-1 };   //create and send back handle
    
    return hI;
}

int BufferResourcer::InsertToDynamicVertexBuffer(void* vertices, int numVertices, int stride, VertexBufferHandle& hV)
{
    VertexBuffer* VBuf= &VBuffers[ hV.ID ];
    ID3D11Buffer* vertexBuffer = VBuf->buffer;

    int startV = VBuf->lastIndex;

    //update vertex buffer
    D3D11_MAPPED_SUBRESOURCE resource;
    context->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);   //map the buffer to lock resource
        Vertex* pV = (Vertex*) resource.pData;                              //convert data to Vertex* so we can set
        memcpy(&pV[VBuf->lastIndex], vertices, stride * numVertices);       //memcopy the vertices in
    context->Unmap(vertexBuffer, 0);                                        //unmap to unlock resource

    VBuf->lastIndex += numVertices;  
}

int BufferResourcer::InsertToDynamicIndexBuffer(void* indices, int numIndices, IndexBufferHandle& hI)
{
    IndexBuffer* IBuf = &IBuffers[ hI.ID ];
    ID3D11Buffer* indexBuffer = IBuf->buffer;

    int startI = IBuf->lastIndex;

    //update index buffer
    D3D11_MAPPED_SUBRESOURCE resource;
    context->Map(indexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);                            //lock resource
        unsigned int* pI = (unsigned int *)resource.pData;                                          //convert to unsigned int*
        memcpy(&pI[IBuf->lastIndex], indices, sizeof( unsigned int ) * numIndices);     //copy indices in
    context->Unmap(indexBuffer, 0);                                                                 //unlock resource
    IBuf->lastIndex += numIndices;  
}

void BufferResourcer::Dispose()
{
    for( auto iter = VBuffers.begin(); iter != VBuffers.end(); iter++)
    {
        (*iter).buffer->Release();
    }

    for( auto iter = IBuffers.begin(); iter != IBuffers.end(); iter++)
    {
        (*iter).buffer->Release();
    }

    
    VBuffers.clear();
    IBuffers.clear();
}