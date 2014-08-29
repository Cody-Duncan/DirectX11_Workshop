#pragma once

#include <dxgi.h>
#include <d3d11.h>
#include "Graphics\ComPtr.h"

class GraphicsSystem
{
public:
	GraphicsSystem();
	~GraphicsSystem();

	void Init(HWND ghMainWnd);
	void Update();
	void DeInit();
	int OnResize();

	void ClearRenderTarget();
	void SwapBuffer();
private:
	
	Matrix	  g_World;
	Matrix	  g_View;
	Matrix	  g_Projection;
	Vector4   g_Up;
	
	HWND	m_HWnd;

	ComPtr<ID3D11Device>            m_device;
	ComPtr<ID3D11DeviceContext>     m_deviceContext; 
	ComPtr<IDXGISwapChain>          m_swapChain;
	ComPtr<ID3D11RenderTargetView>  m_renderTargetView;
	ComPtr<ID3D11DepthStencilView>  m_depthStencilView;
	ComPtr<ID3D11Texture2D>         m_depthStencilBuffer;
	D3D11_VIEWPORT                  m_screenViewport;

	
	unsigned char m_commonStatesBuffer[sizeof(DirectX::CommonStates)];
	std::unique_ptr<DirectX::CommonStates, void(*)(DirectX::CommonStates*)> m_commonStates;

	unsigned int	   m_Window_Width   = 0;
	unsigned int	   m_Window_Height  = 0;
	D3D_DRIVER_TYPE    m_driverType     = D3D_DRIVER_TYPE_HARDWARE;
	D3D_FEATURE_LEVEL  m_featureLevel   = D3D_FEATURE_LEVEL_11_0;
	unsigned int       m_4xMsaaQuality  = 0;
	bool	           m_enable4xMsaa   = false;
	int	               m_frameRateDenom = 60;
	bool	           m_vsync_enabled  = true;

	Vector4	           m_ClearColor     = Vector4(0.1f, 0.1f, 0.1f, 1.0f);
	Vector4            m_MeshColor      = Vector4(0.9f, 0.0f, 0.9f, 1.0f);
	
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader*  m_pixelShader;
	ID3D11InputLayout*  m_inputLayout;
};