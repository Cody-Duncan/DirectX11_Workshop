#include "stdafx.h"
#include "DirectX_SpriteBatch.h"
#include "Resources.h"
#include "FileResourcer.h"

#define BatchSize 2000

DirectX_SpriteBatch::DirectX_SpriteBatch(GraphicsDevice* deviceIn) : SpriteBatch(DeviceAPI::DirectX11)
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


DirectX_SpriteBatch::~DirectX_SpriteBatch(void)
{
    
}



void DirectX_SpriteBatch::Init()
{
    //----- LOAD SHADERS AND FONT
    //texture shaders
    vertexShaderH = Resources::Instance().LoadVertexShaderFile("resources/Tutorial07.fx","VS", "vs_4_0");
                    Resources::Instance().VerifyVertexSize("resources/Tutorial07.fx", sizeof(Vertex));
    pixelShaderH  = Resources::Instance().LoadPixelShaderFile("resources/Tutorial07.fx","PS", "ps_4_0");

    LoadDebugFont("resources/DebugFont.fnt");

    //----- INITIALIZE SPRITE INDEX BUFFER
    BufferResourcer::Instance().createDynamicIndexBuffer(BatchSize*6, device->getDevice(), &batchIBuffer);

    unsigned int* indices = new unsigned int[BatchSize *6];
    int v = 0;
    for(int i = 0; i < BatchSize*6; (i+=6, v+=4))
    {
        indices[i] = v;
        indices[i+1] = v+1;
        indices[i+2] = v+2;

        indices[i+3] = v;
        indices[i+4] = v+2;
        indices[i+5] = v+3;
    }

    ID3D11DeviceContext* context = device->getContext();
    ID3D11Buffer* indexBuffer  = BufferResourcer::Instance().getIBufferData(batchIBuffer).getIndexBuffer();

    //update vertex buffer
    D3D11_MAPPED_SUBRESOURCE resource;
    context->Map(indexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);    //map the buffer to lock resource
        unsigned int* pI = (unsigned int*) resource.pData;                  //convert data to unisgned int* so it can be assigned
        DebugAssert(pI != 0, "Pointer to the dynamic index buffer is null. Error in allocating Buffer");
        memcpy(pI, indices, sizeof( unsigned int ) * BatchSize*6);          //memcopy the indices in
    context->Unmap(indexBuffer, 0);                                         //unmap to unlock resource

    delete[] indices;

    //----- SET OTHER VALUES
    device->setClearColor(Color(0.4f,0.6f,0.9f,1.0f)); // cornflower blue

   
}

void DirectX_SpriteBatch::Dispose()
{
    batchVBuffers.clear();

    for(auto iter = batch.begin(); iter != batch.end(); ++iter)
    {
        iter->second.clear();
    }

    batch.clear();

    //@NOTE: This clears the capacity of spritebatch batches. May need to get that memory back later.
    //std::unordered_map<TextureHandle, std::vector<Vertex> >().swap(batch);
}

void DirectX_SpriteBatch::addBatchBuffer(TextureHandle t)
{
    if(batchVBuffers.find(t) == batchVBuffers.end()) //if key nonexistent
    {
        DebugAssert(device, "Spritebatch's graphicsDevice is null");

        VertexBufferHandle quadBuffer;
        BufferResourcer::Instance().createDynamicVertexBuffer(BatchSize*4, device->getDevice(), &quadBuffer);

        batchVBuffers[t] = quadBuffer;
        batch[t].reserve(BatchSize*4);
    }
}

void DirectX_SpriteBatch::resetBatchBuffer(TextureHandle t)
{
    VertexBufferData& quadBufferData = BufferResourcer::Instance().getVBufferData(batchVBuffers[t]);
    quadBufferData.startVertex = 0;

    batch[t].clear();
}

void DirectX_SpriteBatch::resetAllBatchBuffers()
{
    for(auto iter = batchVBuffers.begin(); iter != batchVBuffers.end(); iter++)
    {
        resetBatchBuffer(iter->first);
    }
}


void DirectX_SpriteBatch::Begin(bool alphaBlend)
{   
    static Matrix m = Matrix::Identity();
    device->setWorld(m);

    Vector4 eye(0.0f, 0.0f, 10.0f, 1.0f);
    Vector4 at(0.0f, 0.0f, 0.0f, 1.0f);
    Vector4 up(0.0f, 1.0f, 0.0f, 1.0f );
    device->setView( eye, at, up );

    device->setOrthographicProjection();
    
    device->setBlend(alphaBlend);

    device->setVertexShader(vertexShaderH);
    device->setPixelShader(pixelShaderH);

    device->setTextureSampler(false);
    device->ToggleDepthBuffer(false);

    device->BeginDraw();
}



void DirectX_SpriteBatch::Draw(TextureHandle texH, Matrix transform, Rectangle2 rect)
{
    //@@optimization: gets another good chunk of fps by avoiding call to getTextureData
    //Works when draw calls are sorted by texture.
    static TextureHandle lastTex = {texH.textureIndex-1}; //guarantee initial value is not the same
    static TextureData* texDataP = &TextureResourcer::Instance().getTextureData(texH); 

    if(lastTex.textureIndex != texH.textureIndex)
    {
        addBatchBuffer(texH);
        texDataP = &TextureResourcer::Instance().getTextureData(texH);
        lastTex.textureIndex = texH.textureIndex;
    }
    
    TextureData& texData = *texDataP; //I like using . for accessing class variables

    //calculate normalized UV coordinates
    Rectangle2 textureArea;
    textureArea.position = Vector2(rect.position.x / texData.width, rect.position.y / texData.height);
    textureArea.dimensions = Vector2(rect.dimensions.x / texData.width, rect.dimensions.y / texData.height);

    //calculate mesh vertex positions
    Vector2 topLeft  = Vector2::Transform( Vector2(0,0), transform);
    Vector2 topRight = Vector2::Transform( Vector2(rect.dimensions.x, 0), transform );
    Vector2 botLeft  = Vector2::Transform( Vector2(0, rect.dimensions.y), transform);
    Vector2 botRight = Vector2::Transform( Vector2(rect.dimensions.x, rect.dimensions.y), transform);

    //generate vertices and indices for quads
    //vertices need normalized UV coordinates based on the rect in texture Coordinates.

//#define CLOCKWISE_VERTS_SPRITEBATCH//
#if !defined(COUNTERCLOCKWISE_VERTS_SPRITEBATCH) && !defined(CLOCKWISE_VERTS_SPRITEBATCH)
#define CLOCKWISE_VERTS_SPRITEBATCH
#endif
#ifdef CLOCKWISE_VERTS_SPRITEBATCH
    //Note: These are clockwise bound, orthogonal transform makes them counterclockwise in vertex shader.
    Vertex vertices[] = 
    {
        //comments on the right note where it shows up visually (because y goes to bottom of screen)
        { Vector4(botLeft.x, botLeft.y, 0, 1), textureArea.botLeft() },     //0 TopLeft
        { Vector4(topLeft.x, topLeft.y, 0, 1), textureArea.topLeft() },     //1 BotLeft
        { Vector4(topRight.x, topRight.y, 0, 1), textureArea.topRight() },  //2 BotRight
        { Vector4(botRight.x, botRight.y, 0, 1), textureArea.botRight() },  //3 TopRight
    };
#endif
#ifdef  COUNTERCLOCKWISE_VERTS_SPRITEBATCH
    //clockwise
    Vertex vertices[] = 
    {
        { Vector4(botLeft.x, botLeft.y, 0, 1), textureArea.botLeft() },     //0 TopLeft
        { Vector4(botRight.x, botRight.y, 0, 1), textureArea.botRight() },  //3 TopRight
        { Vector4(topRight.x, topRight.y, 0, 1), textureArea.topRight() },  //2 BotRight
        { Vector4(topLeft.x, topLeft.y, 0, 1), textureArea.topLeft() },     //1 BotLeft
    };
#endif

    //copy to end of batch
    std::vector<Vertex>& batchRef = batch[texH];
    batchRef.push_back(vertices[0]);
    batchRef.push_back(vertices[1]);
    batchRef.push_back(vertices[2]);
    batchRef.push_back(vertices[3]);

    VertexBufferData& quadBufferData = BufferResourcer::Instance().getVBufferData(batchVBuffers[texH]);
    quadBufferData.startVertex += 4;
}

void DirectX_SpriteBatch::TextDraw(Vector2 position, const char* text)
{
    for(int i = 0; text[i] != '\0'; ++i)
    {
        char c = text[i];
        FontLetter& letter = fontTex.letters[c];
        
        Matrix m = Matrix::CreateTranslation(position.x + letter.xoffset, position.y + letter.yoffset, 0);
        Rectangle2 r((float)letter.x, (float)letter.y, (float)letter.width, (float)letter.height);
        Draw(fontTex.t, m, r);
        position.x += letter.xadvance;
    }
}


void DirectX_SpriteBatch::End()
{
    for(auto iter = batchVBuffers.begin(); iter != batchVBuffers.end(); iter++)
    {
        sentBatchToBuffers(iter->first);
        DrawBatch(iter->first);
    }

    resetAllBatchBuffers();
}



void DirectX_SpriteBatch::DrawBatch(TextureHandle t)
{
    VertexBufferData& quadBufferData = BufferResourcer::Instance().getVBufferData(batchVBuffers[t]);
    if(quadBufferData.startVertex > 0) //don't draw empty buffers
    {
        DebugAssert(batchIBuffer.IbufferID >= 0, "Batch Index Buffer ID is invalid. Value: %d", batchIBuffer.IbufferID );
        device->Draw(batchVBuffers[t], batchIBuffer, t);
    }
}


void DirectX_SpriteBatch::sentBatchToBuffers(TextureHandle t)
{
    VertexBufferData& quadBufferData = BufferResourcer::Instance().getVBufferData(batchVBuffers[t]);
    std::vector<Vertex>& currBatch = batch[t];
    

    //NOTE: try batching the vertices and indices, 
    //then putting them in the vertex/index buffer all at once

    //get context and buffers
    ID3D11DeviceContext* context = device->getContext();
    ID3D11Buffer* vertexBuffer = quadBufferData.getVertexBuffer();
    

    //update vertex buffer
    D3D11_MAPPED_SUBRESOURCE resource;
    context->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);                           //map the buffer to lock resource
        Vertex* pV = (Vertex*) resource.pData;                                                      //convert data to Vertex* so we can set
        DebugAssert(pV != 0, "Pointer to Dynamic Vertex Buffer is null. Cannot modify vertices. Check allocation of buffer.");
        DebugAssert(currBatch.size() <= BatchSize*4, "Too many vertices for buffer size. Crash now before we crash the graphics card. Tried to draw %d vertices", (unsigned int)currBatch.size());
        memcpy(pV, currBatch.data(), sizeof( Vertex ) * quadBufferData.startVertex);   //memcopy the vertices in
    context->Unmap(vertexBuffer, 0);                                                                //unmap to unlock resource

    //set index buffer
    IndexBufferData& indexBufData = BufferResourcer::Instance().getIBufferData(batchIBuffer);
    indexBufData.startIndex = quadBufferData.startVertex * 3 / 2; //6 indices per 4 vertices
}




void DirectX_SpriteBatch::LoadDebugFont(std::string filename)
{
    using namespace std;
    FileDataHandle h = Resources::Instance().LoadDataFile("DebugFontCfg", filename);
    std::iostream& dataStream = *FileResourcer::Instance().getFile(h);
    
    std::string line;
    dataStream >> line; //info

    //font face
    dataStream >> line; 
    int start = line.find('"');
    int end = line.find('"', start+1);
    fontTex.face = std::string(line, start+1, end-start-1);

    for(int i = 0 ; i < 24; ++i)
    {
        dataStream >> line;
    }

    //texture file name
    dataStream >> line;
    start = line.find('"');
    end = line.find('"', start+1);
    fontTex.textureFileName = line.substr(start+1,end-start-1);

    dataStream >> line; //chars
    dataStream >> line; //count=###
    int charCount = std::stoi( line.substr(line.find('=')+1) );

    for(int i = 0; i < charCount; ++i)
    {
        FontLetter let;
        //char id=32   x=180   y=49    width=1     height=1     xoffset=0     yoffset=0     xadvance=17    page=0  chnl=15
        dataStream >> line; //char
        dataStream >> line; //id=###
        let.id = stoi( line.substr(line.find('=')+1) ); dataStream >> line; 
        let.x = stoi( line.substr(line.find('=')+1) ); dataStream >> line; 
        let.y = stoi( line.substr(line.find('=')+1) ); dataStream >> line; 
        let.width = stoi( line.substr(line.find('=')+1) ); dataStream >> line; 
        let.height = stoi( line.substr(line.find('=')+1) ); dataStream >> line; 
        let.xoffset = stoi( line.substr(line.find('=')+1) ); dataStream >> line; 
        let.yoffset = stoi( line.substr(line.find('=')+1) ); dataStream >> line; 
        let.xadvance = stoi( line.substr(line.find('=')+1) ); 
        dataStream >> line; //page
        dataStream >> line; //chnl

        fontTex.letters[let.id] = let;
    }

    fontTex.t = Resources::Instance().LoadTextureFile("DebugFontTxt", fontTex.textureFileName);
    DebugAssert( fontTex.t.textureIndex >= 0, "Texture index is invalid. Value: %d", fontTex.t.textureIndex );
    
}