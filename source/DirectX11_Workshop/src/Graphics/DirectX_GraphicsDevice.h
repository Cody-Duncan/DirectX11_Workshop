#pragma once
#include <dxgi.h>
#include <d3d11.h>
#include "WindowInput\WindowGlobals.h"

class DirectX_GraphicsDevice
{
private:
	

public:
	DirectX_GraphicsDevice(void);
	~DirectX_GraphicsDevice(void);

	virtual int Init();
	virtual int OnResize();
	virtual void clearRenderTarget();
	virtual void BeginDraw();
	virtual void EndDraw();
	virtual void SwapBuffer();
	virtual void Free();

	virtual void setClearColor(Color color);

	virtual void setWorld(Matrix&);
	virtual void setView(Vector4& eye, Vector4& at, Vector4& up);
	virtual void updateView(Vector4& eye, Vector4& at);
	virtual void setProjection(float fovAngleY = 1.570796371f, float nearClip = 0.01f, float farClip = 1000.0f);
	virtual void setOrthographicProjection(float nearClip = 0.01f, float farClip = 1000.0f);


	ID3D11Device* getDevice();
	ID3D11DeviceContext* getContext();
	int getWidth();
	int getHeight();

	void ToggleDepthBuffer(bool turnItOn);
	void setTextureSampler(bool isLinear = true);
	void setBlend(bool isBlendingAlpha = false);

protected:
	

	int createTextureSampler();
	int createBlendStates();


	
};

