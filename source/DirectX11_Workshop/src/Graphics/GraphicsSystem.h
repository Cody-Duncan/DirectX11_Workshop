#pragma once

#include <dxgi.h>
#include <d3d11.h>
#include "Graphics\ComPtr.h"
#include "Graphics\ShaderFactory.h"


class ShaderFactory;
class Geometry;

class GraphicsSystem
{
public:
	GraphicsSystem();
	~GraphicsSystem();

	void Init(HWND ghMainWnd);
	int OnResize();
	void Update();
	void DeInit();

	void ClearRenderTarget();
	void SwapBuffers();

	void LoadCompiledShader(std::string shaderFilename, D3D11_SHADER_VERSION_TYPE shaderType);
	void LoadSourceShader(std::string shaderFilename, std::string EntryPoint, std::string ShaderModel);
	void LoadTexture(std::string filename);
	
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

	typedef std::unique_ptr<DirectX::CommonStates, void(*)(DirectX::CommonStates*)> UniqPtr_CommonStates;
	unsigned char m_commonStatesBuffer[sizeof(DirectX::CommonStates)];
	UniqPtr_CommonStates m_commonStates = UniqPtr_CommonStates(nullptr, nullptr);

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
	
	ShaderFactory* m_shaderFactory      = nullptr;

	Shader* m_pVertexShader;
	Shader* m_pPixelShader;

	std::unique_ptr<Geometry> shape;
	ComPtr<ID3D11Buffer> shape_vertices;
	ComPtr<ID3D11Buffer> shape_indices;
	ComPtr<ID3D11Buffer> shader_worldBuffer;

	ComPtr<ID3D11Texture2D> m_texture;
	ComPtr<ID3D11ShaderResourceView> m_textureView;

	void _SetShader(Shader* shader);
	float GetAspectRatio() { return (float)m_Window_Width / (float)m_Window_Height; }
};