// ----------------------------------------------------------------------------
//  SwAutoplayFix :: (C) 2007 Roman Switch` Dzieciol
// ----------------------------------------------------------------------------

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include "detours.h"

#pragma comment(lib,"detours.lib")


DETOUR_TRAMPOLINE(BOOL WINAPI
	Real_CreateProcessW(
		IN LPCWSTR lpApplicationName,
		IN LPWSTR lpCommandLine,
		IN LPSECURITY_ATTRIBUTES lpProcessAttributes,
		IN LPSECURITY_ATTRIBUTES lpThreadAttributes,
		IN BOOL bInheritHandles,
		IN DWORD dwCreationFlags,
		IN LPVOID lpEnvironment,
		IN LPCWSTR lpCurrentDirectory,
		IN LPSTARTUPINFOW lpStartupInfo,
		OUT LPPROCESS_INFORMATION lpProcessInformation
		), 
		CreateProcessW);

DETOUR_TRAMPOLINE(BOOL WINAPI
	Real_CreateProcessA(
		IN LPCSTR lpApplicationName,
		IN LPSTR lpCommandLine,
		IN LPSECURITY_ATTRIBUTES lpProcessAttributes,
		IN LPSECURITY_ATTRIBUTES lpThreadAttributes,
		IN BOOL bInheritHandles,
		IN DWORD dwCreationFlags,
		IN LPVOID lpEnvironment,
		IN LPCSTR lpCurrentDirectory,
		IN LPSTARTUPINFO lpStartupInfo,
		OUT LPPROCESS_INFORMATION lpProcessInformation
		), 
		CreateProcessA);




LPWSTR GetGlobalModPath()
{		
	HMODULE m = GetModuleHandle(TEXT("Core.dll"));
	if( m )
	{
		//MessageBox( NULL, TEXT("GetModuleHandle"), TEXT("SwAutoplayFix"), MB_OK );	
		FARPROC p = GetProcAddress( m, TEXT("?GModPath@@3PAGA") );
		if( p )
		{
			//MessageBoxW( NULL, (LPWSTR)p, L"SwAutoplayFix", MB_OK );	
			return (LPWSTR)p;
		}
	}
	return NULL;
}

void FixAutoplay( LPCWSTR wszApplicationName, LPCWSTR wszCommandLine, LPCWSTR wszCurrentDirectory )
{
	LPCWSTR uppApplicationName = _wcsupr( _wcsdup( wszApplicationName ) );

	// only UT2004
	if( wcsstr(uppApplicationName,L"UT2004.EXE") == NULL )
		return;

	// read mod name from commandline, must be specified
	LPCWSTR uppCommandLine = _wcsupr( _wcsdup( wszCommandLine ) );
	LPWSTR pb = wcsstr(uppCommandLine,L"-MOD=");
	if( pb == NULL )
		return;
	
	// mod name must be valid
	LPWSTR ps = pb + wcslen(L"-MOD=");
	LPWSTR token = wcstok( ps, L" " );
	if( token == NULL )
		return;

	// mod directory must be valid
	if( !SetCurrentDirectoryW(wszCurrentDirectory)
	||	!SetCurrentDirectoryW(L"..")
	||	!SetCurrentDirectoryW(token) )
		return;

	// copy Autoplay.ut2
	if( !CopyFileW( L"..\\Maps\\Autoplay.ut2", L"Maps\\Autoplay.ut2", FALSE ) )
		return;

	//MessageBox( NULL, TEXT("Copy OK"), TEXT("SwAutoplayFix"), MB_OK );
}

BOOL WINAPI CreateProcessW_Detour(LPCWSTR lpApplicationName, LPWSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes,BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory, LPSTARTUPINFOW lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation)
{
	FixAutoplay( lpApplicationName, lpCommandLine, lpCurrentDirectory );

	return Real_CreateProcessW(lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
}

BOOL WINAPI CreateProcessA_Detour(LPCSTR lpApplicationName, LPSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes,BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment, LPCSTR lpCurrentDirectory, LPSTARTUPINFOA lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation)
{
	WCHAR wszApplicationName[1024];
	WCHAR wszCommandLine[1024];
	WCHAR wszCurrentDirectory[1024];

	MultiByteToWideChar( CP_ACP, 0, lpApplicationName, strlen(lpApplicationName)+1, wszApplicationName, sizeof(wszApplicationName)/sizeof(wszApplicationName[0]) );
	MultiByteToWideChar( CP_ACP, 0, lpCommandLine, strlen(lpCommandLine)+1, wszCommandLine, sizeof(wszCommandLine)/sizeof(wszCommandLine[0]) );
	MultiByteToWideChar( CP_ACP, 0, lpCurrentDirectory, strlen(lpCurrentDirectory)+1, wszCurrentDirectory, sizeof(wszCurrentDirectory)/sizeof(wszCurrentDirectory[0]) );

	FixAutoplay( wszApplicationName, wszCommandLine, wszCurrentDirectory );

	return Real_CreateProcessA(lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
}


BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  reason, 
                       LPVOID lpReserved
					 )
{
	if( reason == DLL_PROCESS_ATTACH )
	{
		//MessageBox( NULL, TEXT("DLL_PROCESS_ATTACH"), TEXT("SwAutoplayFix"), MB_OK );
		DetourFunctionWithTrampoline((PBYTE)Real_CreateProcessW, (PBYTE)CreateProcessW_Detour);			
		DetourFunctionWithTrampoline((PBYTE)Real_CreateProcessA, (PBYTE)CreateProcessA_Detour);
	}
	else if( reason == DLL_PROCESS_DETACH )
	{
		//MessageBox( NULL, TEXT("DLL_PROCESS_DETACH"), TEXT("SwAutoplayFix"), MB_OK );		
		DetourRemoveWithTrampoline((PBYTE)Real_CreateProcessW, (PBYTE)CreateProcessW_Detour);			
		DetourRemoveWithTrampoline((PBYTE)Real_CreateProcessA, (PBYTE)CreateProcessA_Detour);
	}

    return TRUE;
}

