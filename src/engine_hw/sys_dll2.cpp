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
#include "sys.h"
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

void Sys_InitAuthentication( void )
{
	Sys_Printf( "STEAM Auth Server\r\n" );
}

}
