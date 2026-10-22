// sys_dll2.cpp -- the launcher, dedicated server and GameUI interfaces of the engine

#include <windows.h>
#include <winsock.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "interface.h"

extern "C"
{
#include "quakedef.h"
#include "common.h"
#include "cvardef.h"
#include "sys.h"
#include "host.h"
}

#define VENGINE_LAUNCHER_API_VERSION	"VENGINE_LAUNCHER_API_VERSION002"

class IEngineAPI;

extern "C"
{

const char *GetCurrentSteamAppName( void )
{
	if ( !_stricmp( com_gamedir, "cstrike" ) || !_stricmp( com_gamedir, "cstrike_beta" ) )
		return "Counter-Strike";
	else if ( !_stricmp( com_gamedir, "valve" ) )
		return "Half-Life";
	else if ( !_stricmp( com_gamedir, "ricochet" ) )
		return "Ricochet";
	else if ( !_stricmp( com_gamedir, "dod" ) )
		return "Day of Defeat";
	else if ( !_stricmp( com_gamedir, "tfc" ) )
		return "Team Fortress Classic";
	else if ( !_stricmp( com_gamedir, "dmc" ) )
		return "Deathmatch Classic";
	else if ( !_stricmp( com_gamedir, "czero" ) )
		return "Condition Zero";
	else if ( !_stricmp( com_gamedir, "cryoffear" ) )
		return "Cry of Fear";

	return "Cry of Fear";
}

void F( IEngineAPI **api )
{
	*api = (IEngineAPI *)Sys_GetFactoryThis()( VENGINE_LAUNCHER_API_VERSION, NULL );
}

int Sys_GetSurfaceCacheSize( int width, int height )
{
	int size, pix;
	int i;

	if ( i = COM_CheckParm( "-surfcachesize" ) )
	{
		size = atoi( com_argv[ i + 1 ] ) * 1024;
		return size;
	}

	size = 0x300000;

	pix = width * height * 2;
	if ( pix > 64000 )
		size += ( pix - 64000 ) * 3;

	return size;
}

void Sys_GetCDKey( char *pszCDKey, int *nLength, int *bDedicated )
{
	char key[65];
	char hostname[MAX_PATH];

	if ( !gethostname( hostname, sizeof( hostname ) ) )
	{
		struct hostent *hostinfo;

		hostinfo = gethostbyname( hostname );
		if ( hostinfo && hostinfo->h_length == 4 && *hostinfo->h_addr_list )
		{
			_snprintf( key, sizeof( key ), "%u.%u.%u.%u",
				(unsigned char)( *hostinfo->h_addr_list )[0],
				(unsigned char)( *hostinfo->h_addr_list )[1],
				(unsigned char)( *hostinfo->h_addr_list )[2],
				(unsigned char)( *hostinfo->h_addr_list )[3] );
			key[64] = 0;
		}
	}

	strcpy( pszCDKey, key );

	if ( nLength )
		*nLength = strlen( key );

	if ( bDedicated )
		*bDedicated = 0;
}

void Sys_InitMemory( void )
{
	int i;
	bool bDidDefault;

	i = COM_CheckParm( "-heapsize" );
	if ( i && i < com_argc - 1 )
		host_parms.memsize = atoi( com_argv[ i + 1 ] ) * 1024;

	bDidDefault = false;

	if ( host_parms.memsize < 0xE00000 )
	{
		MEMORYSTATUS lpBuffer;

		lpBuffer.dwLength = sizeof( MEMORYSTATUS );
		GlobalMemoryStatus( &lpBuffer );

		bDidDefault = true;

		if ( lpBuffer.dwTotalPhys > 0 && lpBuffer.dwTotalPhys < 0xF00000 )
			Sys_Error( "Available memory less than 15MB!!! %i\n", host_parms.memsize );

		if ( lpBuffer.dwTotalPhys <= 0 )
			host_parms.memsize = 0x8000000;
		else
			host_parms.memsize = lpBuffer.dwTotalPhys >> 1;
	}

	if ( host_parms.memsize < 0xE00000 )
		host_parms.memsize = 0xE00000;

	if ( !g_bIsDedicatedServer && host_parms.memsize > 0x8000000 )
		host_parms.memsize = 0x8000000;

	if ( g_bIsDedicatedServer && bDidDefault )
		host_parms.memsize = 0x2800000;

	if ( g_bIsDedicatedServer && host_parms.memsize > 0x8000000 )
		host_parms.memsize = 0x8000000;

	if ( COM_CheckParm( "-minmemory" ) )
		host_parms.memsize = 0xE00000;

	host_parms.membase = GlobalAlloc( GMEM_FIXED, host_parms.memsize );
	if ( !host_parms.membase )
		Sys_Error( "Unable to allocate %.2f MB\n", host_parms.memsize / ( 1024.0 * 1024.0 ) );
}

void Sys_ShutdownMemory( void )
{
	GlobalFree( host_parms.membase );
	host_parms.membase = NULL;
	host_parms.memsize = 0;
}

void Sys_InitAuthentication( void )
{
	Sys_Printf( "STEAM Auth Server\r\n" );
}

void Sys_ShutdownAuthentication( void )
{
}

}
