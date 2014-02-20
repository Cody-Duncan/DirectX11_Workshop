#pragma once

#include <d3d11.h>
#include <d3dcommon.h>
#include <winnt.h>

#if defined(_DEBUG) || defined(PROFILE)

template<UINT TNameLength>
inline void SetDebugObjectName(_In_ ID3D11DeviceChild* resource, _In_ const char (&name)[TNameLength])
{
#if defined(_DEBUG) || defined(PROFILE)
    resource->SetPrivateData(WKPDID_D3DDebugObjectName, TNameLength - 1, name);
#else
    UNREFERENCED_PARAMETER(resource);
    UNREFERENCED_PARAMETER(name);
#endif
}

inline void ReportLiveObjects(ID3D11Device* device)
{
    if(device)
    {
        ID3D11Debug* debugDev;
        device->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&debugDev));
        debugDev->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
    }
}

#else

template<UINT TNameLength>
inline void SetDebugObjectName(_In_ ID3D11DeviceChild* resource, _In_z_ const char (&name)[TNameLength])
{}

inline void ReportLiveObjects(ID3D11Device* device)
{}


#endif

inline bool errorBox(HRESULT hr,  const wchar_t* message)
{
    if(FAILED(hr))									
    {										
        MessageBox(0, message, 0, 0);		
        return true;						
    }		
    return false;
}