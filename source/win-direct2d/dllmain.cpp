// dllmain.cpp : Defines the entry point for the DLL application.
#include "header.h"

#include "private_defs.h"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		dlPrivateInit();
		break;
    case DLL_PROCESS_DETACH:
		dlPrivateShutdown();
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
    }
    return TRUE;
}

