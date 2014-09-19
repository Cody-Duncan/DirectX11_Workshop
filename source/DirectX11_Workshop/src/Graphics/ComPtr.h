#pragma once

#include <wrl/client.h> // ComPtr

template<typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;
