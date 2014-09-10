#include "Graphics\GraphicsSystem.h"
#include <memory>	                 // shared_ptr 


GraphicsSystem::GraphicsSystem() 
{
}

GraphicsSystem::~GraphicsSystem()
{
	ASSERT_DEBUG(m_device == nullptr, "Did not Call DeInit().");
}

void GraphicsSystem::DeInit()
{
	if (m_shaderFactory)
		delete m_shaderFactory; 

	if(m_deviceContext)
		m_deviceContext->ClearState();

	//unset fullscreen
	//if( /* is fullscreen */ )
		//m_swapChain->SetFullscreenState(FALSE, NULL); 
	
	//Free the resources.
	 m_depthStencilBuffer = nullptr; 
	 m_depthStencilView   = nullptr;
	 m_renderTargetView   = nullptr;
	 m_swapChain          = nullptr;
	 m_commonStates       = nullptr; //Custom Deallocator
	 m_deviceContext      = nullptr; 
	 m_device             = nullptr;
}

void GraphicsSystem::Init(HWND ghMainWnd)
{
	HRESULT hr = S_OK;
	m_HWnd = ghMainWnd;

	// Get Window Size
	RECT rc;
	GetClientRect( m_HWnd, &rc );
	m_Window_Width = rc.right - rc.left;
	m_Window_Height = rc.bottom - rc.top;

	// Set Debug Flag
	unsigned int createDeviceFlags = 0;
	#if defined(DEBUG) || defined(_DEBUG)  
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	#endif
	
	//============= Create Device =============
	hr = D3D11CreateDevice(
		0,						// use default display adapter
		m_driverType,
		0,						// no software device
		createDeviceFlags, 
		0, 0,					// default feature level array
		D3D11_SDK_VERSION,
		&m_device,
		&m_featureLevel,
		&m_deviceContext
	);

	LOG("GRAPHICS: Creating Graphics Device\n");
	ASSERT_DEBUG(SUCCEEDED(hr), "D3D11CreateDevice not created.\n");
	
	if( m_featureLevel != D3D_FEATURE_LEVEL_11_0 )
	{
		ASSERT_DEBUG(false, "Direct3D Feature Level 11 unsupported.\n");
		return;
	}

	hr = m_device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m_4xMsaaQuality);
	ASSERT_DEBUG(SUCCEEDED(hr), "Error Checking Multi Sample Quality Levels. \n");
	ASSERT_WARNING( m_4xMsaaQuality > 0, "M4xMsaaQuality is 0, Multisampling with the given format and sample count combination is not supported for the installed graphics adapter.");

	//============= Create Shader Factory =============
	m_shaderFactory = new ShaderFactory(m_device.Get());

	//============= Create Common States =============
	using namespace DirectX;
	m_commonStates = std::unique_ptr<CommonStates, void(*)(CommonStates*)>
		( 
			new (m_commonStatesBuffer) CommonStates(m_device.Get()) ,	      // Allocate memory in local buffer.
			[](CommonStates* commonStates) { commonStates->~CommonStates(); } // Custom deallocation function. 
		);

	//======= Generate Swap Chain Description =============
	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferDesc.Width                   = m_Window_Width;
	sd.BufferDesc.Height                  = m_Window_Height;
	sd.BufferDesc.RefreshRate.Numerator   = 1;
	sd.BufferDesc.RefreshRate.Denominator = 60;
	sd.BufferDesc.Format                  = DXGI_FORMAT_R8G8B8A8_UNORM;  //backbuffer pixel format
	sd.BufferDesc.ScanlineOrdering	      = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling	              = DXGI_MODE_SCALING_UNSPECIFIED;

	sd.BufferUsage  = DXGI_USAGE_RENDER_TARGET_OUTPUT;  // use backbuffer as render target
	sd.BufferCount  = 1;                                // 1 backbuffer, double buffering
	sd.OutputWindow = m_HWnd;                           // give the window to render onto
	sd.Windowed     = true;                             // true = windowed, false = fullscreen
	sd.SwapEffect   = DXGI_SWAP_EFFECT_DISCARD;	        // let the display driver select the most efficient presentation method
	sd.Flags        = 0;

	sd.SampleDesc.Count   = m_enable4xMsaa ? 4 : 1;
	sd.SampleDesc.Quality = m_enable4xMsaa ? m_4xMsaaQuality-1 : 0;

	if(m_vsync_enabled)
	{
		sd.BufferDesc.RefreshRate.Numerator = 1;
		sd.BufferDesc.RefreshRate.Denominator = m_frameRateDenom;
	}
	else
	{
		sd.BufferDesc.RefreshRate.Numerator = 0;
		sd.BufferDesc.RefreshRate.Denominator = 1;
	}

	//====== Create the Swap Chain ========
	{
		ComPtr<IDXGIDevice> dxgiDevice;
		ComPtr<IDXGIAdapter> dxgiAdapter;
		ComPtr<IDXGIFactory> dxgiFactory;

		m_device.As(&dxgiDevice);
		ASSERT_DEBUG(SUCCEEDED(hr), "Could not retrieve dxgiInterface.\n");

		hr = dxgiDevice->GetParent(IID_PPV_ARGS(&dxgiAdapter));
		ASSERT_DEBUG(SUCCEEDED(hr), "Could not retrieve dxgiAdapter.\n");
		
		hr = dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory));
		ASSERT_DEBUG(SUCCEEDED(hr), "Could not retrieve dxgiFactory.\n");
		
		// Now, create the swap chain.
		hr = dxgiFactory->CreateSwapChain(m_device.Get(), &sd, &m_swapChain);
		ASSERT_DEBUG(SUCCEEDED(hr), "Could not create SwapChain.\n");
	}

	OnResize();
}

int GraphicsSystem::OnResize()
{
	ASSERT_DEBUG(m_deviceContext, "Device Context is null.");
	ASSERT_DEBUG(m_device       , "Device is null. How did that happen?");
	ASSERT_DEBUG(m_swapChain    , "Swap chain is null. This is really bad.");

	HRESULT hr = S_OK;

	//****************************** FREE RESOURCES ******************************//

	// Release the RenderTargets
	m_depthStencilView = nullptr;
	m_renderTargetView = nullptr;
	m_deviceContext->OMSetRenderTargets(0, 0, 0);
	m_deviceContext->ClearState();

	// Resize Swap Chain buffers
	hr = m_swapChain->ResizeBuffers(1, m_Window_Width, m_Window_Height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
	ASSERT_DEBUG(SUCCEEDED(hr), "Failed to resize swap chain buffers.");




	//****************************** CREATE RENDERTARGETS ******************************//
	// Create RenderTargetView
	ComPtr<ID3D11Texture2D> backBuffer;
	hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &backBuffer);
	ASSERT_DEBUG(SUCCEEDED(hr), "FAILED: Failed to get backbuffer from swap chain.\n");
	m_device->CreateRenderTargetView(backBuffer.Get(), 0, &m_renderTargetView);

	
	// Create DepthStencilView
	D3D11_TEXTURE2D_DESC depthStencilTextureDesc;
	depthStencilTextureDesc.Width              = m_Window_Width;
	depthStencilTextureDesc.Height             = m_Window_Height;
	depthStencilTextureDesc.MipLevels          = 1;
	depthStencilTextureDesc.ArraySize          = 1;
	depthStencilTextureDesc.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilTextureDesc.Usage              = D3D11_USAGE_DEFAULT;
	depthStencilTextureDesc.BindFlags          = D3D11_BIND_DEPTH_STENCIL;
	depthStencilTextureDesc.CPUAccessFlags     = 0;
	depthStencilTextureDesc.MiscFlags          = 0;
	// Use 4X MSAA? --must match swap chain MSAA values.
	depthStencilTextureDesc.SampleDesc.Count   = m_enable4xMsaa ? 4 : 1;
	depthStencilTextureDesc.SampleDesc.Quality = m_enable4xMsaa ? m_4xMsaaQuality-1 : 0;

	hr = m_device->CreateTexture2D(&depthStencilTextureDesc, 0, &m_depthStencilBuffer);
	ASSERT_DEBUG(SUCCEEDED(hr), "FAILED: Failed to create depth stencil buffer.");
	
	hr = m_device->CreateDepthStencilView(m_depthStencilBuffer.Get(), 0, &m_depthStencilView); 
	ASSERT_DEBUG(SUCCEEDED(hr), "FAILED: Failed to create depth stencil view.");



	//****************************** BIND RENDERTARGETS ******************************//
	// Create Depth Stencil State
	m_deviceContext->OMSetDepthStencilState(m_commonStates.get()->DepthDefault(), 1);

	// Bind Render Target and Depth stencil 
	m_deviceContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());


	//****************************** CREATE VIEWPORT ******************************//
	//Create Viewport
	m_screenViewport.TopLeftX = 0.0f;
	m_screenViewport.TopLeftY = 0.0f;
	m_screenViewport.Width = static_cast<float>(m_Window_Width);
	m_screenViewport.Height = static_cast<float>(m_Window_Height);
	m_screenViewport.MinDepth = 0.0f;
	m_screenViewport.MaxDepth = 1.0f;

	m_deviceContext->RSSetViewports(1, &m_screenViewport);

	return 0;
}

void GraphicsSystem::LoadCompiledShader(std::string shaderFilename, D3D11_SHADER_VERSION_TYPE shaderType)
{
	Shader* shader = m_shaderFactory->BuildCompiledShader(shaderFilename, shaderType);
	_SetShader(shader);
}

void GraphicsSystem::LoadSourceShader(std::string shaderFilename, std::string EntryPoint, std::string ShaderModel)
{
	Shader* shader = m_shaderFactory->BuildSourceShader(shaderFilename, EntryPoint, ShaderModel);
	_SetShader(shader);
}

void GraphicsSystem::ClearRenderTarget()
{
	m_deviceContext->ClearRenderTargetView(m_renderTargetView.Get(), (float*)&m_ClearColor);
	m_deviceContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void GraphicsSystem::Update()
{
	using namespace DirectX;

	ASSERT_DEBUG(m_deviceContext, "Device Context is null.");
	ASSERT_DEBUG(m_swapChain, "Swap chain is null.");

	//========== BEGIN DRAW PHASE ==========//
	//m_deviceContext->IASetInputLayout();
	//m_deviceContext->VSSetShader();
	//m_deviceContext->PSSetShader();

	float blendFactor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	m_deviceContext->OMSetBlendState(m_commonStates.get()->Opaque(), blendFactor, 0xffffffff);

	//========== DRAW PHASE ==========//
	ClearRenderTarget();


	//========== END DRAW PHASE ==========//
	SwapBuffers();
}

void GraphicsSystem::SwapBuffers()
{
	HRESULT hr = m_swapChain->Present(m_vsync_enabled ? 1 : 0, 0);
	ASSERT_DEBUG(SUCCEEDED(hr), "Swap chain swapping failed.");
}

void GraphicsSystem::_SetShader(Shader* shader)
{
	switch (shader->GetShaderType())
	{
	case Shader::SHADER_VERTEX:
		this->m_deviceContext->VSSetShader(shader->m_vertexShader, nullptr, 0);
		break;

	case Shader::SHADER_PIXEL:
		this->m_deviceContext->PSSetShader(shader->m_pixelShader, nullptr, 0);
		break;
	}
}