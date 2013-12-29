#pragma once

#include "SpriteBatch.h"
#include "GraphicsDevice.h"
#include "DirectX_GraphicsDevice.h"

struct FontLetter
{
    int id;
    int x;
    int y;
    int width;
    int height;
    int xoffset;
    int yoffset;
    int xadvance;
};

class SpriteFont
{
public:
    TextureHandle t;
    std::unordered_map<int, FontLetter> letters;

    std::string face;
    std::string textureFileName;
};

class DirectX_SpriteBatch : public SpriteBatch
{
public:
    DirectX_SpriteBatch(GraphicsDevice* device);
    ~DirectX_SpriteBatch(void);

    void Init();
    void LoadDebugFont(std::string filename);

    void Begin(bool AlphaBlend = false);
    void Draw(TextureHandle texH, Matrix transform, Rectangle2 rect);
    void TextDraw(Vector2 position, const char* text);
    void End();
    
    void Dispose(); 

    void DrawBatch(TextureHandle t);

private:
    DirectX_GraphicsDevice* device;

    VertexShaderHandle vertexShaderH;
    PixelShaderHandle pixelShaderH;

    void addBatchBuffer(TextureHandle t);
    void resetBatchBuffer(TextureHandle t);
    void resetAllBatchBuffers();

    void sentBatchToBuffers(TextureHandle t);
    
    std::unordered_map<TextureHandle, VertexBufferHandle> batchVBuffers;
    IndexBufferHandle batchIBuffer;
    std::unordered_map<TextureHandle, std::vector<Vertex> > batch;

    SpriteFont fontTex;
};

