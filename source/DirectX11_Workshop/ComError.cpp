#include "ComError.h"
#include <windows.h>
#include <comdef.h>

#define ERRMSGBUFFERSIZE 256

TCHAR pBuffer[ERRMSGBUFFERSIZE];

LPCTSTR HresultGetMessage(HRESULT hresult)
{
	_com_error error(hresult);
	memcpy(pBuffer, error.ErrorMessage(), ERRMSGBUFFERSIZE);
	return pBuffer;
}