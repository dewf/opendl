#pragma once

#include <Windows.h>
#include <strsafe.h>

#define APPLICATION_TITLE "OPENDL"
#define FAILURE_LOCATION L"\r\nFunction: " TEXT(__FUNCTION__) L"\r\nLine: " TEXT(STRINGIZE_(__LINE__))
inline void HR(HRESULT const result)
{
	// Displays an error message for API failures,
	// returning the very same error code that came in.
	if (FAILED(result))
	{
		const wchar_t* format = L"%s\r\nError code = %X";

		wchar_t buffer[1000];
		buffer[0] = '\0';

		StringCchPrintf(buffer, ARRAYSIZE(buffer), format, L"COM method failure", result);
		MessageBox(NULL, buffer, TEXT(APPLICATION_TITLE), MB_OK | MB_ICONEXCLAMATION | MB_TASKMODAL);
	}
}

// SafeRelease inline function.
template <class T> inline void SafeRelease(T **ppT)
{
	if (*ppT)
	{
		(*ppT)->Release();
		*ppT = NULL;
	}
}
