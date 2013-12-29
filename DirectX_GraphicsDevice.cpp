#include "DirectX_GraphicsDevice.h"
#include "WindowGlobals.h"          //ghMainWnd

#include <assert.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include <sstream>
#include "Vertex.h"
#include "Debug_Graphics.h"
#include "MeshResourcer.h"
#include "Mesh.h"
#include "Resources.h"

DirectX_GraphicsDevice::DirectX_GraphicsDevice(void) : 
    GraphicsDevice(DeviceAPI::DirectX11),
    driverType(D3D_DRIVER_TYPE_HARDWARE), 
    featureLevel(D3D_FEATURE_LEVEL_11_0),
    device(NULL),
    deviceContext(NULL),
    swapChain(NULL),
    renderTargetView(NULL),
    depthStencilView(NULL),
    depthStencilBuffer(NULL),
    Width(800), 
    Height(600),
    frameRateDenom(60),
    vsync_enabled(true),
    enable4xMsaa(false),
    samplerLinear(nullptr),
    samplerPoint(nullptr),
    blendNone(nullptr),
    blendAlpha(nullptr),
    s_vMeshColor(Vector4(0.9f, 0.0f, 0.9f,1.0f)) //bright purple
{
    
}


    

DirectX_GraphicsDevice::~DirectX_GraphicsDevice(void)
{
}

void DirectX_GraphicsDevice::Free()
{
    //clear states
    if( deviceContext ) deviceContext->ClearState();

    ShaderResourcer::Instance().Dispose();
    BufferResourcer::Instance().Dispose();
    TextureResourcer::Instance().Dispose();

    if(blendNone) blendNone->Release();
    if(blendAlpha) blendAlpha->Release();

    if(samplerLinear) samplerLinear->Release();
    if(samplerPoint) samplerPoint->Release();

    if(m_depthDisabledStencilState) m_depthDisabledStencilState->Release();
    if(m_depthStencilState) m_depthStencilState->Release() ;

    //release resources
    if( depthStencilBuffer ) depthStencilBuffer->Release();
    if( depthStencilView ) depthStencilView->Release();

    if( renderTargetView ) renderTargetView->Release();
    if( swapChain ) swapChain->Release();
    if( deviceContext ) deviceContext->Release();

#ifdef _DEBUG_LEAK_REPORTING
    ReportLiveObjects(device);
#endif

    //debugDev->Release();
    if( device ) device->Release();
}

int DirectX_GraphicsDevice::Init()
{
    HRESULT hr = S_OK;

    RECT rc;
    GetClientRect( ghMainWnd, &rc );
    Width = rc.right - rc.left;
    Height = rc.bottom - rc.top;

    // Create the device and device deviceContext.
    unsigned int createDeviceFlags = 0;
    #if defined(DEBUG) || defined(_DEBUG)  
        createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    #endif

    D3D_FEATURE_LEVEL featureLevel;
    hr = D3D11CreateDevice(
        0,                 // use default display adapter
        driverType,
        0,                 // no software device
        createDeviceFlags, 
        0, 0,              // default feature level array
        D3D11_SDK_VERSION,
        &device,
        &featureLevel,
        &deviceContext);

    DebugPrintf("GRAPHICS: Creating Graphics Device\n");
    CHECKHR(hr, "FAILED: D3D11CreateDevice not created .\n")
    
    if( featureLevel != D3D_FEATURE_LEVEL_11_0 )
    {
        DebugPrintf("ERROR: Direct3D Feature Level 11 unsupported.\n");
        MessageBox(0, L"Direct3D Feature Level 11 unsupported.", 0, 0);
        return 1;
    }

    hr = device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m4xMsaaQuality);
    CHECKHR(hr, "FAILED: Error Checking Multi Sample Quality Levels. \n") 

    DebugAssert( m4xMsaaQuality > 0, "M4xMsaaQuality is 0, Multisampling with the given format and sample count combination is not supported for the installed graphics adapter.");

    

    //======= Generate Swap Chain Description =============
    DXGI_SWAP_CHAIN_DESC sd;
    //describe backbuffer
    sd.BufferDesc.Width  = Width;
    sd.BufferDesc.Height = Height;
    sd.BufferDesc.RefreshRate.Numerator = 1;
    sd.BufferDesc.RefreshRate.Denominator = 60;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;  //backbuffer pixel format
    sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    sd.BufferUsage  = DXGI_USAGE_RENDER_TARGET_OUTPUT;  //use backbuffer as render target
    sd.BufferCount  = 1;                                //1 backbuffer, double buffering
    sd.OutputWindow = ghMainWnd;                        //give the window to render onto
    sd.Windowed     = true;                             //true = windowed, false = fullscreen
    sd.SwapEffect   = DXGI_SWAP_EFFECT_DISCARD;         //let the display driver select the most efficient presentation method
    sd.Flags        = 0;

    sd.SampleDesc.Count = enable4xMsaa ? 4 : 1;
    sd.SampleDesc.Quality = enable4xMsaa ? m4xMsaaQuality-1 : 0;

    if(vsync_enabled)
    {
        sd.BufferDesc.RefreshRate.Numerator = 1;
        sd.BufferDesc.RefreshRate.Denominator = frameRateDenom;
    }
    else
    {
        sd.BufferDesc.RefreshRate.Numerator = 0;
        sd.BufferDesc.RefreshRate.Denominator = 1;
    }

    //====== Create the Swap Chain ========
    {
        IDXGIDevice* dxgiDevice = 0;
        hr = device->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice);
        CHECKHR(hr, "FAILED: Could not retrieve dxgiInterface.\n") 

        IDXGIAdapter* dxgiAdapter = 0;
        hr = dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter);
        CHECKHR(hr, "FAILED: Could not retrieve dxgiAdapter.\n") 
        
        IDXGIFactory* dxgiFactory = 0;
        hr = dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory);
        CHECKHR(hr, "FAILED: Could not retrieve dxgiFactory.\n") 
    
        // Now, create the swap chain.
        hr = dxgiFactory->CreateSwapChain(device, &sd, &swapChain);
        CHECKHR(hr, "FAILED: Could not create SwapChain.\n") 
    
        // Release our acquired COM interfaces (because we are done with them).
        dxgiDevice->Release();
        dxgiAdapter->Release();
        dxgiFactory->Release();
    }

    


    if( hr = OnResize() )
    {
        return hr;
    }

    if( hr =  createTextureSampler() )
    {
        return hr;
    }

    if( hr = createBlendStates())
    {
        return hr;
    }

    g_Projection = g_View = g_World = DirectX::XMMatrixIdentity();

    return 0;
}

int DirectX_GraphicsDevice::OnResize()
{
    DebugAssert(deviceContext, "Device Context is null.");
    DebugAssert(device, "Device is null. How did that happen?");
    DebugAssert(swapChain, "Swap chain is null. This is really bad.");

    HRESULT hr = S_OK;
    
    //create RenderTargetView
    ID3D11Texture2D* backBuffer;
    hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));
    CHECKHR(hr, "FAILED: Failed to get backbuffer from swap chain.\n") 
    device->CreateRenderTargetView(backBuffer, 0, &renderTargetView);
    backBuffer->Release();



    //Create DepthStencilView
    D3D11_TEXTURE2D_DESC depthStencilTextureDesc;
    depthStencilTextureDesc.Width = Width;
    depthStencilTextureDesc.Height = Height;
    depthStencilTextureDesc.MipLevels = 1;
    depthStencilTextureDesc.ArraySize = 1;
    depthStencilTextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilTextureDesc.Usage = D3D11_USAGE_DEFAULT;
    depthStencilTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthStencilTextureDesc.CPUAccessFlags = 0;
    depthStencilTextureDesc.MiscFlags = 0;
    // Use 4X MSAA? --must match swap chain MSAA values.
    depthStencilTextureDesc.SampleDesc.Count = enable4xMsaa ? 4 : 1;
    depthStencilTextureDesc.SampleDesc.Quality = enable4xMsaa ? m4xMsaaQuality-1 : 0;

    hr = device->CreateTexture2D(
        &depthStencilTextureDesc,		// Description of texture to create.
        0,
        &depthStencilBuffer		// Return pointer to depth/stencil buffer.
    );
    CHECKHR(hr, "FAILED: Failed to create depth stencil buffer.") 
    
    hr = device->CreateDepthStencilView(
        depthStencilBuffer,		// Resource we want to create a view to.
        0,
        &depthStencilView		// Return depth/stencil view
    ); 
    CHECKHR(hr, "FAILED: Failed to create depth stencil view.") 

    //Create Enabled depth stencil state.
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
    ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
    depthStencilDesc.DepthEnable = true;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
    depthStencilDesc.StencilEnable = true;
    depthStencilDesc.StencilReadMask = 0xFF;
    depthStencilDesc.StencilWriteMask = 0xFF;
    // Stencil operations if pixel is front-facing.
    depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    // Stencil operations if pixel is back-facing.
    depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Create the depth stencil state.
    hr = device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
    CHECKHR(hr, "FAILED: Failed to create depth stencil enabled state");

    //create disabled depth stencil state
    D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;
    ZeroMemory(&depthDisabledStencilDesc, sizeof(depthDisabledStencilDesc));
    depthDisabledStencilDesc.DepthEnable = false; //SET THIS TO FALSE
    depthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
    depthDisabledStencilDesc.StencilEnable = true;
    depthDisabledStencilDesc.StencilReadMask = 0xFF;
    depthDisabledStencilDesc.StencilWriteMask = 0xFF;
    depthDisabledStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    depthDisabledStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthDisabledStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    depthDisabledStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    depthDisabledStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthDisabledStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    
    hr = device->CreateDepthStencilState(&depthDisabledStencilDesc, &m_depthDisabledStencilState);
    CHECKHR(hr, "FAILED: Failed to create depth stencil disabled state");

    // Set the depth stencil state.
    //deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

    // Bind Render Target and Depth stencil to output merger
    deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);

    



    //Create Viewport
    screenViewport.TopLeftX = 0.0f;
    screenViewport.TopLeftY = 0.0f;
    screenViewport.Width = static_cast<float>(Width);
    screenViewport.Height = static_cast<float>(Height);
    screenViewport.MinDepth = 0.0f;
    screenViewport.MaxDepth = 1.0f;

    deviceContext->RSSetViewports(1, &screenViewport);

    return 0;
}
int DirectX_GraphicsDevice::createTextureSampler()
{
    // Create the linear sample state
    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory( &sampDesc, sizeof(sampDesc) );
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    
    HRESULT hr = device->CreateSamplerState( &sampDesc, &samplerLinear );
    CHECKHR(hr, "FAILED: Failed to create linear texture sampler.") 

    currentSampler = samplerLinear;

    // Create the point sample state
    ZeroMemory( &sampDesc, sizeof(sampDesc) );
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    
    hr = device->CreateSamplerState( &sampDesc, &samplerPoint );
    CHECKHR(hr, "FAILED: Failed to create point texture sampler.") 
    return hr;
}

int DirectX_GraphicsDevice::createBlendStates()
{
    D3D11_BLEND_DESC BlendState;
    ZeroMemory(&BlendState, sizeof(D3D11_BLEND_DESC));
    BlendState.RenderTarget[0].BlendEnable = FALSE;
    BlendState.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    device->CreateBlendState(&BlendState, &blendNone);

    currentBlend = blendNone;

    ZeroMemory(&BlendState, sizeof(D3D11_BLEND_DESC));
    BlendState.RenderTarget[0].BlendEnable = TRUE;
    BlendState.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    BlendState.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    BlendState.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    BlendState.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    BlendState.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
    BlendState.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    BlendState.RenderTarget[0].RenderTargetWriteMask = 0x0f;
    device->CreateBlendState(&BlendState, &blendAlpha);

    return 0;
}

void DirectX_GraphicsDevice::clearRenderTarget()
{
    deviceContext->ClearRenderTargetView(renderTargetView, clearColor);
    deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void DirectX_GraphicsDevice::BeginDraw()
{
    using namespace DirectX;
    DebugAssert(deviceContext, "Device Context is null.");
    DebugAssert(swapChain, "Swap chain is null. This is really bad.");

    //set shaders and resources
    vertexShader = ShaderResourcer::Instance().getVertexShader(toDraw_vertexShader);
    pixelShader  = ShaderResourcer::Instance().getPixelShader(toDraw_pixelShader);
    inputLayout = ShaderResourcer::Instance().getInputLayout(toDraw_vertexShader);
    constantBuffer = ShaderResourcer::Instance().getConstantBuffer(toDraw_vertexShader);

    deviceContext->IASetInputLayout(inputLayout);
    deviceContext->VSSetShader( vertexShader, nullptr, 0 );
    deviceContext->PSSetShader( pixelShader, nullptr, 0 );

    //set it to the current vertexShader's infrequent buffer
    CBChangesInfrequently newProjBuffer = {g_Projection};
    deviceContext->UpdateSubresource( constantBuffer.infrequentBuffer, 0, nullptr, &newProjBuffer, 0, 0 );
    deviceContext->VSSetConstantBuffers( 0, 1, &constantBuffer.infrequentBuffer );
    
    float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    UINT sampleMask   = 0xffffffff;
    deviceContext->OMSetBlendState(currentBlend, blendFactor, sampleMask);
    
}

void DirectX_GraphicsDevice::Draw(MeshHandle& hMesh, TextureHandle& hTex)
{
    DebugAssert(hMesh.meshID >= 0, "Mesh ID is invalid. (Cannot be negative)");
    DebugAssert(hTex.textureIndex >= 0, "Texture ID is invalid (Cannot be negative)");

    //Setup the world/view matrices
    CBChangesEveryFrame cb_Frame;
    cb_Frame.mWorld = g_World;
    cb_Frame.mView = g_View;
    cb_Frame.vMeshColor = s_vMeshColor;

    //get mesh data;
    Mesh& mesh = (* MeshResourcer::Instance().getMesh(hMesh));

    IndexBufferData& IndexData = BufferResourcer::Instance().getIBufferData(mesh.hIBuffer);
    VertexBufferData& VertexData = BufferResourcer::Instance().getVBufferData(mesh.hVBuffer);

    ID3D11Buffer* vertexBuffer = VertexData.getVertexBuffer();
    ID3D11Buffer* indexBuffer = IndexData.getIndexBuffer();
    unsigned int stride = VertexData.stride;
    unsigned int vertexoffset = mesh.vertexOffset;
    unsigned int indexOffset = mesh.indexOffset;
    unsigned int vertexCount = VertexData.vertexLength;
    unsigned int indexCount = IndexData.indexLength;

    //get texture data
    ID3D11ShaderResourceView* texture = TextureResourcer::Instance().getTextureData(hTex).textureView;

    //input assembler for mesh data
    deviceContext->IASetVertexBuffers( 0, 1, &vertexBuffer, &stride, &vertexoffset );
    deviceContext->IASetIndexBuffer( indexBuffer, DXGI_FORMAT_R32_UINT, 0 );
    deviceContext->IASetPrimitiveTopology( VertexData.primitiveTopology );

    //set constant buffer for world/view matrix
    deviceContext->UpdateSubresource( constantBuffer.frameBuffer, 0, nullptr, &cb_Frame, 0, 0 );
    deviceContext->VSSetConstantBuffers( 1, 1, &constantBuffer.frameBuffer );

    //set texture resources on pixel shader
    deviceContext->PSSetShaderResources( 0, 1, &texture );
    deviceContext->PSSetSamplers( 0, 1, &currentSampler );
    deviceContext->PSSetConstantBuffers( 1, 1, &constantBuffer.frameBuffer );

    
    //draw
    deviceContext->DrawIndexed( indexCount, indexOffset, vertexoffset ); 
}

void DirectX_GraphicsDevice::Draw(VertexBufferHandle& hVBuf, IndexBufferHandle& hIBuf, const TextureHandle& hTex)
{
    DebugAssert(hTex.textureIndex >= 0, "Texture ID is invalid (Cannot be negative)");

    //get texture data
    ID3D11ShaderResourceView* texture = TextureResourcer::Instance().getTextureData(hTex).textureView;
    deviceContext->PSSetShaderResources( 0, 1, &texture );
    Draw(hVBuf,  hIBuf);
}

void DirectX_GraphicsDevice::Draw(VertexBufferHandle& hVBuf, IndexBufferHandle& hIBuf)
{
    DebugAssert(hVBuf.VbufferID >= 0, "Vertex Buffer ID is invalid (Cannot be negative)");
    DebugAssert(hIBuf.IbufferID >= 0, "Index Buffer ID is invalid (Cannot be negative)");

    //Setup the world/view matrices
    CBChangesEveryFrame cb_Frame;
    cb_Frame.mWorld = g_World;
    cb_Frame.mView = g_View;
    cb_Frame.vMeshColor = s_vMeshColor;

    //get mesh data;
    IndexBufferData& IndexData = BufferResourcer::Instance().getIBufferData(hIBuf);
    VertexBufferData& VertexData = BufferResourcer::Instance().getVBufferData(hVBuf);
    ID3D11Buffer* vertexBuffer = VertexData.getVertexBuffer();
    ID3D11Buffer* indexBuffer = IndexData.getIndexBuffer();
    unsigned int stride = VertexData.stride;
    unsigned int vertexoffset = 0;
    unsigned int indexOffset = 0;
    unsigned int vertexCount = VertexData.startVertex; //startVertex is the first free vertex
    unsigned int indexCount = IndexData.startIndex;   //startIndex is the first free index

    DebugAssert(indexCount > 0, "Trying to draw 0 indices?");
    DebugAssert(vertexCount > 0, "Trying to draw 0 vertices?");


    //input assembler for mesh data
    deviceContext->IASetVertexBuffers( 0, 1, &vertexBuffer, &stride, &vertexoffset );
    deviceContext->IASetIndexBuffer( indexBuffer, DXGI_FORMAT_R32_UINT, 0 );
    deviceContext->IASetPrimitiveTopology( VertexData.primitiveTopology );

    //set constant buffer for world/view matrix
    deviceContext->UpdateSubresource( constantBuffer.frameBuffer, 0, nullptr, &cb_Frame, 0, 0 );
    deviceContext->VSSetConstantBuffers( 1, 1, &constantBuffer.frameBuffer );

    //set texture resources on pixel shader
    deviceContext->PSSetSamplers( 0, 1, &currentSampler );
    deviceContext->PSSetConstantBuffers( 1, 1, &constantBuffer.frameBuffer );

    //draw
    deviceContext->DrawIndexed( indexCount, indexOffset, vertexoffset ); 
}

void DirectX_GraphicsDevice::EndDraw()
{

}

//======================= Public Methods ======================================
//=============================================================================
//=============================================================================

int DirectX_GraphicsDevice::createVertexIndexBuffer(Mesh* mesh, VertexBufferHandle* hVBuf, IndexBufferHandle* hIBuf)
{
    BufferResourcer& resourceVertexBuf = BufferResourcer::Instance();
    int result = resourceVertexBuf.createStaticBuffers(*mesh, device, hVBuf, hIBuf);
    if(result)
        return result;

    return 0;
}



void DirectX_GraphicsDevice::setClearColor(Color color)
{
    clearColor[0] = color.x;
    clearColor[1] = color.y; 
    clearColor[2] = color.z;
    clearColor [3] = color.w;
}

void DirectX_GraphicsDevice::setVertexShader(VertexShaderHandle& hVertexShader)
{
    toDraw_vertexShader = hVertexShader;
}


void DirectX_GraphicsDevice::setPixelShader(PixelShaderHandle& hPixelShader)
{
    toDraw_pixelShader = hPixelShader;
}


void DirectX_GraphicsDevice::setWorld(Matrix& m)
{
    g_World = XMLoadFloat4x4(&m) ;
}


void DirectX_GraphicsDevice::setView(Vector4& eye, Vector4& at, Vector4& up)
{
    using namespace DirectX;

    g_Up = up;
    updateView(eye, at);
}

void DirectX_GraphicsDevice::updateView(Vector4& eyePosition, Vector4& focusPosition)
{
    using namespace DirectX;

    // Initialize the view matrix
    XMVECTOR EyePosition = XMLoadFloat4( &eyePosition );
    XMVECTOR FocusPosition = XMLoadFloat4( &focusPosition );
    g_View = XMMatrixLookAtRH( EyePosition, FocusPosition, XMLoadFloat4( &g_Up )  );
}


void DirectX_GraphicsDevice::setProjection(float fovAngleY, float nearClip, float farClip)
{
    g_Projection = DirectX::XMMatrixPerspectiveFovRH( fovAngleY, Width / (float)Height, nearClip, farClip);
}

void DirectX_GraphicsDevice::setOrthographicProjection(float nearClip, float farClip)
{
    g_Projection = DirectX::XMMatrixOrthographicOffCenterRH(0, (float)Width, (float)Height, 0, nearClip, farClip);
}

void DirectX_GraphicsDevice::SwapBuffer()
{
    if(vsync_enabled)
    {
        if(swapChain->Present(1, 0) < 0)
        {
            MessageBox(0, L"Swap Chain swapping broke.", 0, 0);
        }
    }
    else
    {
        if(swapChain->Present(0, 0) < 0)
        {
            MessageBox(0, L"Swap Chain swapping broke.", 0, 0);
        }
    }
}

ID3D11Device* DirectX_GraphicsDevice::getDevice()
{
    return device;
}

ID3D11DeviceContext* DirectX_GraphicsDevice::getContext()
{
    return deviceContext;
}

int DirectX_GraphicsDevice::getWidth()
{
    return Width;
}
int DirectX_GraphicsDevice::getHeight()
{
    return Height;
}

void DirectX_GraphicsDevice::ToggleDepthBuffer(bool turnItOn)
{
    if(turnItOn)
    {
        deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);
    }
    else
    {
        deviceContext->OMSetDepthStencilState(m_depthDisabledStencilState, 1);
    }
}

void DirectX_GraphicsDevice::setTextureSampler(bool isLinear)
{
    if(isLinear)
    {
        currentSampler = samplerLinear;
    }
    else
    {
        currentSampler = samplerPoint;
    }
}

void DirectX_GraphicsDevice::setBlend(bool isBlendingAlpha)
{
    if(isBlendingAlpha)
    {
        currentBlend = blendAlpha;
    }
    else
    {
        currentBlend = blendNone;
    }
}