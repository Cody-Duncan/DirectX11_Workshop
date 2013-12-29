#include "DirectX_PrimitiveBatch.h"
#include "BufferResourcer.h"
#include "Resources.h"
#include "DirectX_GraphicsDevice.h"

#define BatchSize 4096
#define Invalid_Buffer_ID -1

DirectX_PrimitiveBatch::DirectX_PrimitiveBatch(GraphicsDevice* deviceIn) : 
    PrimitiveBatch(DeviceAPI::DirectX11)
{
    DirectX_GraphicsDevice* gDevice = dynamic_cast<DirectX_GraphicsDevice*>(deviceIn);
    if(!gDevice)
    {
        device = 0;
        DebugPrintf("Tried to use non-directX graphics device in directX spritebatch");
    }
    else
    {
        device = gDevice;
    }
}


DirectX_PrimitiveBatch::~DirectX_PrimitiveBatch(void)
{
    
}

//UTILITY FUNCTIONS

void FillIndexBuffer(unsigned int* indices, ID3D11Buffer* buffer, ID3D11DeviceContext* context)
{
    D3D11_MAPPED_SUBRESOURCE resource;
    context->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);         //map the buffer to lock resource
        unsigned int* pI = (unsigned int*) resource.pData;                  //convert data to unisgned int* so it can be assigned
        DebugAssert(pI != 0, "Pointer to the dynamic index buffer is null. Error in allocating Buffer");
        memcpy(pI, indices, sizeof( unsigned int ) * BatchSize*6);          //memcopy the indices in
    context->Unmap(buffer, 0);                                              //unmap to unlock resource
}

void FillVertexBuffer(std::vector<VertexCol>& vertices, int length, ID3D11Buffer* buffer, ID3D11DeviceContext* context)
{
    D3D11_MAPPED_SUBRESOURCE resource;
    context->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);         //map the buffer to lock resource
        VertexCol* pV = (VertexCol*) resource.pData;                        //convert data to Vertex* so we can set
        DebugAssert(pV != 0, "Pointer to Dynamic Vertex Buffer is null. Cannot modify vertices. Check allocation of buffer.");
        DebugAssert(vertices.size() <= BatchSize*4, "Too many vertices for buffer size. Crash now before we crash the graphics card. Tried to draw %d vertices", (unsigned int)vertices.size());
        memcpy(pV, vertices.data(), sizeof( VertexCol ) * length);          //memcopy the vertices in
    context->Unmap(buffer, 0);                                              //unmap to unlock resource
}



void DirectX_PrimitiveBatch::Init()
{
    //color shaders
    colorVertSH   = Resources::Instance().LoadVertexShaderFile("resources/ColorShader.fx","VS", "vs_4_0");
                    Resources::Instance().VerifyVertexSize("resources/ColorShader.fx", sizeof(VertexCol));
    colorPixSH    = Resources::Instance().LoadPixelShaderFile("resources/ColorShader.fx","PS", "ps_4_0");

    
    unsigned int* indices = new unsigned int[BatchSize *6];
    for(int i = 0; i < BatchSize*6; ++i)
    {
        indices[i] = i;
    }

    ID3D11DeviceContext* context = device->getContext();
    BufferResourcer& BR = BufferResourcer::Instance();

    //----- INITIALIZE TRIANGLE INDEX BUFFER
    BR.createDynamicIndexBuffer(BatchSize*6, device->getDevice(), &triBatchIBuffer[Tri]);
    ID3D11Buffer* TriIndexBuffer = BR.getIBufferData(triBatchIBuffer[Tri]).getIndexBuffer();
    FillIndexBuffer(indices, TriIndexBuffer, context);
    
    //----- INITIALIZE LINE INDEX BUFFER
    BR.createDynamicIndexBuffer(BatchSize*6, device->getDevice(), &triBatchIBuffer[Line]);
    ID3D11Buffer* LineIndexBuffer = BR.getIBufferData(triBatchIBuffer[Line]).getIndexBuffer();
    FillIndexBuffer(indices, LineIndexBuffer, context);                                       //unmap to unlock resource

     delete[] indices;

    //----- SET OTHER VALUES
    device->setClearColor(Color(0.4f,0.6f,0.9f,1.0f)); // cornflower blue
}

void DirectX_PrimitiveBatch::Dispose()
{
    for(int i = 0; i < 2; ++i)
    {
        triBatchVBuffers[i].clear();

        for(auto iter = triBatch[i].begin(); iter != triBatch[i].end(); ++iter)
        {
            iter->clear();
        }

        triBatch[i].clear();
    }
}

bool DirectX_PrimitiveBatch::hasBatchBuffer(unsigned int layer, PrimitiveType type)
{
    return 
        layer < triBatchVBuffers[type].size() &&                        //cannot have index higher than size
        triBatchVBuffers[type][layer].VbufferID != Invalid_Buffer_ID;   //vbuffer id must be valid
}

void DirectX_PrimitiveBatch::addBatchBuffer(unsigned int layer, PrimitiveType type)
{
    if(!hasBatchBuffer(layer, type))
    {
        DebugAssert(device, "Spritebatch's graphicsDevice is null");

        VertexBufferHandle quadBuffer;
        BufferResourcer::Instance().createDynamicVertexBuffer(BatchSize*4, device->getDevice(), &quadBuffer);
        VertexBufferData& vbufdata = BufferResourcer::Instance().getVBufferData(quadBuffer);
        vbufdata.stride = sizeof(VertexCol);

        switch(type)
        {
            case Tri:
                vbufdata.primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
                break;
            case Line:
                vbufdata.primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
                break;
        }
        
        //assign vBuffer
        VertexBufferHandle invalidHandle = {Invalid_Buffer_ID};
        triBatchVBuffers[type].resize(layer+1, invalidHandle);
        triBatchVBuffers[type][layer] = quadBuffer;

        //create a batch
        triBatch[type].resize(layer+1);
        triBatch[type][layer].reserve(BatchSize*4);
    }
}

void DirectX_PrimitiveBatch::resetBatchBuffer(unsigned int layer, PrimitiveType type)
{
    if(hasBatchBuffer(layer, type))
    {
        VertexBufferHandle triVBH = triBatchVBuffers[type][layer];

        //reset startVertex on VBuffer data
        VertexBufferData& triBufferData = BufferResourcer::Instance().getVBufferData(triVBH);
        triBufferData.startVertex = 0;

        //reset batch
        triBatch[type][layer].clear();
    }
}

void DirectX_PrimitiveBatch::resetAllBatchBuffers()
{
    for(unsigned int i = 0; i < triBatch[Tri].size(); ++i)
    {
        resetBatchBuffer(i, Tri);
    }

    for(unsigned int i = 0; i < triBatch[Line].size(); ++i)
    {
        resetBatchBuffer(i, Line);
    }
}


void DirectX_PrimitiveBatch::DrawTriangles(unsigned int layer, Vector2* points, int pointLength, Matrix transform, Color c)
{
    DrawPrimitive(layer, points, pointLength, transform, c, Tri);
}

void DirectX_PrimitiveBatch::DrawLines(unsigned int layer, Vector2* points, int pointLength, Matrix transform, Color c)
{
    DrawPrimitive(layer, points, pointLength, transform, c, Line);
}

void DirectX_PrimitiveBatch::DrawPrimitive(unsigned int layer, Vector2* points, int pointLength, Matrix transform, Color c, PrimitiveType type)
{
    if(!hasBatchBuffer(layer, type))
        addBatchBuffer(layer, type);

    std::vector<VertexCol>& batchRef = triBatch[type][layer];
    for(int i = 0; i < pointLength; ++i)
    {
        Vector2 temp = Vector2::Transform(points[i], transform);
        batchRef.push_back( VertexCol(Vector4(temp.x, temp.y, 0, 1), c) );
    }

    VertexBufferData& triBufferData = BufferResourcer::Instance().getVBufferData(triBatchVBuffers[type][layer]);
    triBufferData.startVertex += pointLength;
}


void DirectX_PrimitiveBatch::DrawBatch(int layer, PrimitiveType type)
{
    VertexBufferHandle triVBH = triBatchVBuffers[type][layer];
    if(triVBH.VbufferID == Invalid_Buffer_ID) //do not draw invalid buffers
        return;

    VertexBufferData& quadBufferData = BufferResourcer::Instance().getVBufferData(triVBH);
    if(quadBufferData.startVertex > 0) //don't draw empty buffers
    {
        DebugAssert(triBatchIBuffer[type].IbufferID >= 0, "Batch Index Buffer ID is invalid. Value: %d", triBatchIBuffer[type].IbufferID );
        device->Draw(triVBH, triBatchIBuffer[type]);
    }
}


void DirectX_PrimitiveBatch::sendBatchToBuffers(unsigned int layer, PrimitiveType type)
{
    VertexBufferHandle triVBH = triBatchVBuffers[type][layer];
    if(triVBH.VbufferID == Invalid_Buffer_ID) //do not draw invalid buffers
        return;

    VertexBufferData& triBufferData = BufferResourcer::Instance().getVBufferData(triVBH);
    std::vector<VertexCol>& currBatch = triBatch[type][layer];
    if(triBufferData.startVertex == 0)
        return;

    ID3D11DeviceContext* context = device->getContext();
    ID3D11Buffer* vertexBuffer = triBufferData.getVertexBuffer();

    //update vertex buffer
    FillVertexBuffer(currBatch, triBufferData.startVertex, vertexBuffer, context);

    //set index buffer
    IndexBufferData& indexBufData = BufferResourcer::Instance().getIBufferData(triBatchIBuffer[type]);
    indexBufData.startIndex = currBatch.size(); //6 indices per 4 vertices
}

void DirectX_PrimitiveBatch::Begin(bool alphaBlend)
{   
    static Matrix m = Matrix::Identity();
    device->setWorld(m);

    Vector4 eye(0.0f, 0.0f, 10.0f, 1.0f);
    Vector4 at(0.0f, 0.0f, 0.0f, 1.0f);
    Vector4 up(0.0f, 1.0f, 0.0f, 1.0f );
    device->setView( eye, at, up );

    device->setOrthographicProjection();
    
    device->setBlend(alphaBlend);

    device->setVertexShader(colorVertSH);
    device->setPixelShader(colorPixSH);

    device->setTextureSampler(false);
    device->ToggleDepthBuffer(false);

    device->BeginDraw();
}

void DirectX_PrimitiveBatch::End()
{
    for(unsigned int i = 0; i < triBatchVBuffers[Tri].size(); ++i)
    {
        sendBatchToBuffers(i, Tri);
        DrawBatch(i, Tri);
    }

    for(unsigned int i = 0; i < triBatchVBuffers[Line].size(); ++i)
    {
        sendBatchToBuffers(i, Line);
        DrawBatch(i, Line);
    }

    resetAllBatchBuffers();
}