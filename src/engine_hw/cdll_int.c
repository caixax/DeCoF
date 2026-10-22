// cdll_int.c -- the engine's calls into the client dll

#include "quakedef.h"
#include "mathlib.h"
#include "cvar.h"
#include "cmd.h"
#include "host.h"
#include "client.h"
#include "net.h"
#include "GL_MODEL.H"
#include "cdll_int.h"
#include "cl_spectator.h"
#include "vgui_int.h"
#include "APIProxy.h"
#include "r_studioint.h"
#include "entity_types.h"
#include "triangleapi.h"
#include "event_flags.h"
#include "event_args.h"
#include "winquake.h"
#include "vid.h"
#include "pmove.h"
#include "view.h"
#include "info.h"

typedef struct client_data_s
{
	// fields that cannot be modified  (ie. have no effect if changed)
	vec3_t	origin;

	// fields that can be changed by the cldll
	vec3_t	viewangles;
	int		iWeaponBits;
	float	fov;	// field of view
} client_data_t;

#define CLDLL_INTERFACE_VERSION		7

typedef struct client_sprite_s
{
	char	szName[64];
	char	szSprite[64];
	int		hspr;
	int		iRes;
	wrect_t	rc;
} client_sprite_t;

// Function type declarations for client exports
typedef int (*INITIALIZE_FUNC)	( struct cl_enginefuncs_s*, int );
typedef void (*HUD_INIT_FUNC)		( void );
typedef int (*HUD_VIDINIT_FUNC)	( void );
typedef int (*HUD_REDRAW_FUNC)	( float, int );
typedef int (*HUD_UPDATECLIENTDATA_FUNC) ( struct client_data_s*, float );
typedef void (*HUD_RESET_FUNC)    ( void );
typedef void (*HUD_CLIENTMOVE_FUNC)( struct playermove_s *ppmove, qboolean server );
typedef void (*HUD_CLIENTMOVEINIT_FUNC)( struct playermove_s *ppmove );
typedef char (*HUD_TEXTURETYPE_FUNC)( char *name );
typedef void (*HUD_IN_ACTIVATEMOUSE_FUNC) ( void );
typedef void (*HUD_IN_DEACTIVATEMOUSE_FUNC)		( void );
typedef void (*HUD_IN_MOUSEEVENT_FUNC)		( int mstate );
typedef void (*HUD_IN_CLEARSTATES_FUNC)		( void );
typedef void (*HUD_IN_ACCUMULATE_FUNC ) ( void );
typedef void (*HUD_CL_CREATEMOVE_FUNC)		( float frametime, struct usercmd_s *cmd, int active );
typedef int (*HUD_CL_ISTHIRDPERSON_FUNC) ( void );
typedef void (*HUD_CL_GETCAMERAOFFSETS_FUNC )( float *ofs );
typedef struct kbutton_s * (*HUD_KB_FIND_FUNC) ( const char *name );
typedef void ( *HUD_CAMTHINK_FUNC )( void );
typedef void ( *HUD_CALCREF_FUNC ) ( struct ref_params_s *pparams );
typedef int	 ( *HUD_ADDENTITY_FUNC ) ( int type, struct cl_entity_s *ent, const char *modelname );
typedef void ( *HUD_CREATEENTITIES_FUNC ) ( void );
typedef void ( *HUD_DRAWNORMALTRIS_FUNC ) ( void );
typedef void ( *HUD_DRAWTRANSTRIS_FUNC ) ( void );
typedef void ( *HUD_STUDIOEVENT_FUNC ) ( const struct mstudioevent_s *event, const struct cl_entity_s *entity );
typedef void ( *HUD_POSTRUNCMD_FUNC ) ( struct local_state_s *from, struct local_state_s *to, struct usercmd_s *cmd, int runfuncs, double time, unsigned int random_seed );
typedef void ( *HUD_SHUTDOWN_FUNC ) ( void );
typedef void ( *HUD_TXFERLOCALOVERRIDES_FUNC )( struct entity_state_s *state, const struct clientdata_s *client );
typedef void ( *HUD_PROCESSPLAYERSTATE_FUNC )( struct entity_state_s *dst, const struct entity_state_s *src );
typedef void ( *HUD_TXFERPREDICTIONDATA_FUNC ) ( struct entity_state_s *ps, const struct entity_state_s *pps, struct clientdata_s *pcd, const struct clientdata_s *ppcd, struct weapon_data_s *wd, const struct weapon_data_s *pwd );
typedef void ( *HUD_DEMOREAD_FUNC ) ( int size, unsigned char *buffer );
typedef int ( *HUD_CONNECTIONLESS_FUNC )( const struct netadr_s *net_from, const char *args, char *response_buffer, int *response_buffer_size );
typedef	int	( *HUD_GETHULLBOUNDS_FUNC ) ( int hullnumber, float *mins, float *maxs );
typedef void (*HUD_FRAME_FUNC)		( double );
typedef int (*HUD_KEY_EVENT_FUNC ) ( int eventcode, int keynum, const char *pszCurrentBinding );
typedef void (*HUD_TEMPENTUPDATE_FUNC) ( double frametime, double client_time, double cl_gravity, struct tempent_s **ppTempEntFree, struct tempent_s **ppTempEntActive, 	int ( *Callback_AddVisibleEntity )( struct cl_entity_s *pEntity ),	void ( *Callback_TempEntPlaySound )( struct tempent_s *pTemp, float damp ) );
typedef struct cl_entity_s *(*HUD_GETUSERENTITY_FUNC ) ( int index );
typedef void (*HUD_VOICESTATUS_FUNC)(int entindex, qboolean bTalking);
typedef void (*HUD_DIRECTORMESSAGE_FUNC)( int iSize, void *pbuf );
typedef int ( *HUD_STUDIO_INTERFACE_FUNC )( int version, struct r_studio_interface_s **ppinterface, struct engine_studio_api_s *pstudio );
typedef void (*HUD_CHATINPUTPOSITION_FUNC)( int *x, int *y );
typedef int (*HUD_GETPLAYERTEAM)(int iplayer);
typedef void *(*CLIENTFACTORY)();

// Pointers to the exported client functions themselves
typedef struct
{
	INITIALIZE_FUNC						pInitFunc;
	HUD_INIT_FUNC						pHudInitFunc;
	HUD_VIDINIT_FUNC					pHudVidInitFunc;
	HUD_REDRAW_FUNC						pHudRedrawFunc;
	HUD_UPDATECLIENTDATA_FUNC			pHudUpdateClientDataFunc;
	HUD_RESET_FUNC						pHudResetFunc;
	HUD_CLIENTMOVE_FUNC					pClientMove;
	HUD_CLIENTMOVEINIT_FUNC				pClientMoveInit;
	HUD_TEXTURETYPE_FUNC				pClientTextureType;
	HUD_IN_ACTIVATEMOUSE_FUNC			pIN_ActivateMouse;
	HUD_IN_DEACTIVATEMOUSE_FUNC			pIN_DeactivateMouse;
	HUD_IN_MOUSEEVENT_FUNC				pIN_MouseEvent;
	HUD_IN_CLEARSTATES_FUNC				pIN_ClearStates;
	HUD_IN_ACCUMULATE_FUNC				pIN_Accumulate;
	HUD_CL_CREATEMOVE_FUNC				pCL_CreateMove;
	HUD_CL_ISTHIRDPERSON_FUNC			pCL_IsThirdPerson;
	HUD_CL_GETCAMERAOFFSETS_FUNC		pCL_GetCameraOffsets;
	HUD_KB_FIND_FUNC					pFindKey;
	HUD_CAMTHINK_FUNC					pCamThink;
	HUD_CALCREF_FUNC					pCalcRefdef;
	HUD_ADDENTITY_FUNC					pAddEntity;
	HUD_CREATEENTITIES_FUNC				pCreateEntities;
	HUD_DRAWNORMALTRIS_FUNC				pDrawNormalTriangles;
	HUD_DRAWTRANSTRIS_FUNC				pDrawTransparentTriangles;
	HUD_STUDIOEVENT_FUNC				pStudioEvent;
	HUD_POSTRUNCMD_FUNC					pPostRunCmd;
	HUD_SHUTDOWN_FUNC					pShutdown;
	HUD_TXFERLOCALOVERRIDES_FUNC		pTxferLocalOverrides;
	HUD_PROCESSPLAYERSTATE_FUNC			pProcessPlayerState;
	HUD_TXFERPREDICTIONDATA_FUNC		pTxferPredictionData;
	HUD_DEMOREAD_FUNC					pReadDemoBuffer;
	HUD_CONNECTIONLESS_FUNC				pConnectionlessPacket;
	HUD_GETHULLBOUNDS_FUNC				pGetHullBounds;
	HUD_FRAME_FUNC						pHudFrame;
	HUD_KEY_EVENT_FUNC					pKeyEvent;
	HUD_TEMPENTUPDATE_FUNC				pTempEntUpdate;
	HUD_GETUSERENTITY_FUNC				pGetUserEntity;
	HUD_VOICESTATUS_FUNC				pVoiceStatus;
	HUD_DIRECTORMESSAGE_FUNC			pDirectorMessage;
	HUD_STUDIO_INTERFACE_FUNC			pStudioInterface;
	HUD_CHATINPUTPOSITION_FUNC			pChatInputPosition;
	HUD_GETPLAYERTEAM					pGetPlayerTeam;
	CLIENTFACTORY						pClientFactory;
} cldll_func_t;

// LoadBlob.cpp
typedef struct BlobFootprint_s
{
	int		m_hDll;
} BlobFootprint_t;

void	FreeBlob( BlobFootprint_t *pblobfootprint );

#define MAX_BEAMENTS		64

extern	cldll_func_t		cl_funcs;
extern	BlobFootprint_t		g_blobfootprintClient_guess;
extern	qboolean			fClientLoaded;
extern	HMODULE				hClientDLL;
extern	cvar_t				cl_lw;
extern	cvar_t				*cl_righthand;
extern	float				scr_fov_value;
extern	int					cl_numbeamentities;
extern	cl_entity_t			*cl_beamentities[MAX_BEAMENTS];
extern	engine_studio_api_t	engine_studio_api;
extern	triangleapi_t		tri;

int		CL_IsSpectateOnly( void );
void	SPR_Init( void );
void	SPR_Shutdown( void );
void	R_ResetStudio( void );
mnode_t	*PVSNode( mnode_t *node, vec3_t emins, vec3_t emaxs );
void	CL_WriteDLLUpdate( client_data_t *cdat );
void	CL_DemoAnim( int anim, int body );
void	ClientDLL_CheckStudioInterface( HMODULE hClientDLL );
void	ClientDLL_Shutdown( void );
void	ClientDLL_ClientMoveInit( struct playermove_s *ppmove );
void	LoadSecureClient( char *pszDllName );
void	LoadInsecureClient( char *pszFullPathClientDLLName );

// LoadBlob.cpp
int		FIsBlob( char *pstFileName );
void	*NLoadBlobFile( char *pstFileName, BlobFootprint_t *pblobfootprint, void *pv, char fLoadAsDll );

// iga/IGA_Engine_guess.cpp
void	IGA_ClientFactory_guess( void *factory );

int		VGui_Key_Event( int down, int keynum, const char *pszCurrentBinding );
struct model_s	*SPR_GetModelPointer( HSPRITE hSprite );
void	CL_DemoEvent( int flags, int idx, float delay, event_args_t *pargs );
pfnUserMsgHook	HookServerMsg( const char *pszName, pfnUserMsgHook pfn );

extern	struct cl_enginefuncs_s		cl_enginefuncs;
extern	struct cldll_func_dst_s		g_cldstAddrs;
extern	int							g_bTamperTrap_guess;
extern	cmdalias_t					*cmd_alias;
extern	cvar_t						sv_gravity;
extern	int							window_center_x, window_center_y;

/*
================
GetAliasesList

================
*/
cmdalias_t *GetAliasesList( void )
{
	return cmd_alias;
}

/*
================
ClientDLL_TamperTrap_guess

Executes an invalid lock prefixed instruction when the flag is set.
================
*/
void ClientDLL_TamperTrap_guess( void )
{
	if ( g_bTamperTrap_guess )
	{
		__asm
		{
			pushfd
			_emit 0xf0
			_emit 0xa8
			_emit 0x36
			_emit 0x00
			_emit 0x00
			_emit 0x00
			_emit 0x00
		}
	}
}

/*
================
ClientDLL_Init

================
*/
void ClientDLL_Init( void )
{
	char	szDllName[512];
	char	szFullName_guess[512];

	_snprintf( szDllName, sizeof( szDllName ), "cl_dlls\\client.dll" );

	if ( fClientLoaded )
		ClientDLL_Shutdown();

	COM_FixSlashes( szDllName );

	ClientDLL_TamperTrap_guess();

	strcpy( szFullName_guess, szDllName );
	COM_ExpandFilename( szFullName_guess );
	COM_FixSlashes( szFullName_guess );

	if ( FIsBlob( szDllName ) )
	{
		cls.fSecureClient = true;
		LoadSecureClient( szDllName );
	}
	else
	{
		cls.fSecureClient = false;
		LoadInsecureClient( szFullName_guess );
	}

	HookServerMsg( "ScreenShake", V_ScreenShake );
	HookServerMsg( "ScreenFade", V_ScreenFade );

	cl_funcs.pInitFunc( &cl_enginefuncs, CLDLL_INTERFACE_VERSION );

	ClientDLL_ClientMoveInit( &g_clmove );

	CL_GetPlayerHulls();

	if ( cl_funcs.pClientFactory && cl_funcs.pClientFactory() )
	{
		IGA_ClientFactory_guess( cl_funcs.pClientFactory() );
	}
}

/*
================
LoadSecureClient

================
*/
void LoadSecureClient( char *pszDllName )
{
	// the first two slots pass the module functions and the destination addresses to the blob
	cl_funcs.pInitFunc = (INITIALIZE_FUNC)&g_modfuncs;
	cl_funcs.pHudVidInitFunc = (HUD_VIDINIT_FUNC)&g_cldstAddrs;

	NLoadBlobFile( pszDllName, &g_blobfootprintClient_guess, &cl_funcs, false );

	fClientLoaded = true;
}

/*
================
LoadInsecureClient

================
*/
void LoadInsecureClient( char *pszFullPathClientDLLName )
{
	hClientDLL = FS_LoadLibrary( pszFullPathClientDLLName );
	if ( !hClientDLL )
		Sys_Error( "could not load library %s", pszFullPathClientDLLName );

	fClientLoaded = true;

	cl_funcs.pInitFunc = (INITIALIZE_FUNC)GetProcAddress( hClientDLL, "Initialize" );
	if ( !cl_funcs.pInitFunc )
		Sys_Error( "could not link client.dll function Initialize\n" );

	cl_funcs.pHudVidInitFunc = (HUD_VIDINIT_FUNC)GetProcAddress( hClientDLL, "HUD_VidInit" );
	if ( !cl_funcs.pHudVidInitFunc )
		Sys_Error( "could not link client.dll function HUD_VidInit\n" );

	cl_funcs.pHudInitFunc = (HUD_INIT_FUNC)GetProcAddress( hClientDLL, "HUD_Init" );
	if ( !cl_funcs.pHudInitFunc )
		Sys_Error( "could not link client.dll function HUD_Init\n" );

	cl_funcs.pHudRedrawFunc = (HUD_REDRAW_FUNC)GetProcAddress( hClientDLL, "HUD_Redraw" );
	if ( !cl_funcs.pHudRedrawFunc )
		Sys_Error( "could not link client.dll function HUD_Redraw\n" );

	cl_funcs.pHudUpdateClientDataFunc = (HUD_UPDATECLIENTDATA_FUNC)GetProcAddress( hClientDLL, "HUD_UpdateClientData" );
	if ( !cl_funcs.pHudUpdateClientDataFunc )
		Sys_Error( "could not link client.dll function HUD_UpdateClientData\n" );

	cl_funcs.pHudResetFunc = (HUD_RESET_FUNC)GetProcAddress( hClientDLL, "HUD_Reset" );
	if ( !cl_funcs.pHudResetFunc )
		Sys_Error( "could not link client.dll function HUD_Reset\n" );

	cl_funcs.pClientMove = (HUD_CLIENTMOVE_FUNC)GetProcAddress( hClientDLL, "HUD_PlayerMove" );
	if ( !cl_funcs.pClientMove )
		Sys_Error( "could not link client.dll function HUD_PlayerMove\n" );

	cl_funcs.pClientMoveInit = (HUD_CLIENTMOVEINIT_FUNC)GetProcAddress( hClientDLL, "HUD_PlayerMoveInit" );
	if ( !cl_funcs.pClientMoveInit )
		Sys_Error( "could not link client.dll function HUD_PlayerMoveInit\n" );

	cl_funcs.pClientTextureType = (HUD_TEXTURETYPE_FUNC)GetProcAddress( hClientDLL, "HUD_PlayerMoveTexture" );
	if ( !cl_funcs.pClientTextureType )
		Sys_Error( "could not link client.dll function HUD_PlayerMoveTexture\n" );

	cl_funcs.pIN_ActivateMouse = (HUD_IN_ACTIVATEMOUSE_FUNC)GetProcAddress( hClientDLL, "IN_ActivateMouse" );
	if ( !cl_funcs.pIN_ActivateMouse )
		Sys_Error( "could not link client.dll function IN_ActivateMouse\n" );

	cl_funcs.pIN_DeactivateMouse = (HUD_IN_DEACTIVATEMOUSE_FUNC)GetProcAddress( hClientDLL, "IN_DeactivateMouse" );
	if ( !cl_funcs.pIN_DeactivateMouse )
		Sys_Error( "could not link client.dll function IN_DeactivateMouse\n" );

	cl_funcs.pIN_MouseEvent = (HUD_IN_MOUSEEVENT_FUNC)GetProcAddress( hClientDLL, "IN_MouseEvent" );
	if ( !cl_funcs.pIN_MouseEvent )
		Sys_Error( "could not link client.dll function IN_MouseEvent\n" );

	cl_funcs.pIN_ClearStates = (HUD_IN_CLEARSTATES_FUNC)GetProcAddress( hClientDLL, "IN_ClearStates" );
	if ( !cl_funcs.pIN_ClearStates )
		Sys_Error( "could not link client.dll function IN_ClearStates\n" );

	cl_funcs.pIN_Accumulate = (HUD_IN_ACCUMULATE_FUNC)GetProcAddress( hClientDLL, "IN_Accumulate" );
	if ( !cl_funcs.pIN_Accumulate )
		Sys_Error( "could not link client.dll function IN_Accumulate\n" );

	cl_funcs.pCL_CreateMove = (HUD_CL_CREATEMOVE_FUNC)GetProcAddress( hClientDLL, "CL_CreateMove" );
	if ( !cl_funcs.pCL_CreateMove )
		Sys_Error( "could not link client.dll function CL_CreateMove\n" );

	cl_funcs.pCL_IsThirdPerson = (HUD_CL_ISTHIRDPERSON_FUNC)GetProcAddress( hClientDLL, "CL_IsThirdPerson" );
	if ( !cl_funcs.pCL_IsThirdPerson )
		Sys_Error( "could not link client.dll function CL_IsThirdPerson\n" );

	cl_funcs.pCL_GetCameraOffsets = (HUD_CL_GETCAMERAOFFSETS_FUNC)GetProcAddress( hClientDLL, "CL_CameraOffset" );
	if ( !cl_funcs.pCL_GetCameraOffsets )
		Sys_Error( "could not link client.dll function CL_CameraOffset\n" );

	cl_funcs.pCamThink = (HUD_CAMTHINK_FUNC)GetProcAddress( hClientDLL, "CAM_Think" );
	if ( !cl_funcs.pCamThink )
		Sys_Error( "could not link client.dll function CAM_Think\n" );

	cl_funcs.pFindKey = (HUD_KB_FIND_FUNC)GetProcAddress( hClientDLL, "KB_Find" );
	if ( !cl_funcs.pFindKey )
		Sys_Error( "could not link client.dll function KB_Find\n" );

	cl_funcs.pCalcRefdef = (HUD_CALCREF_FUNC)GetProcAddress( hClientDLL, "V_CalcRefdef" );
	if ( !cl_funcs.pCalcRefdef )
		Sys_Error( "could not link client.dll function V_CalcRefdef\n" );

	cl_funcs.pAddEntity = (HUD_ADDENTITY_FUNC)GetProcAddress( hClientDLL, "HUD_AddEntity" );
	if ( !cl_funcs.pAddEntity )
		Sys_Error( "could not link client.dll function HUD_AddEntity\n" );

	cl_funcs.pCreateEntities = (HUD_CREATEENTITIES_FUNC)GetProcAddress( hClientDLL, "HUD_CreateEntities" );
	if ( !cl_funcs.pCreateEntities )
		Sys_Error( "could not link client.dll function HUD_CreateEntities\n" );

	cl_funcs.pDrawNormalTriangles = (HUD_DRAWNORMALTRIS_FUNC)GetProcAddress( hClientDLL, "HUD_DrawNormalTriangles" );
	if ( !cl_funcs.pDrawNormalTriangles )
		Sys_Error( "could not link client.dll function HUD_DrawNormalTriangles\n" );

	cl_funcs.pDrawTransparentTriangles = (HUD_DRAWTRANSTRIS_FUNC)GetProcAddress( hClientDLL, "HUD_DrawTransparentTriangles" );
	if ( !cl_funcs.pDrawTransparentTriangles )
		Sys_Error( "could not link client.dll function HUD_DrawTransparentTriangles\n" );

	cl_funcs.pStudioEvent = (HUD_STUDIOEVENT_FUNC)GetProcAddress( hClientDLL, "HUD_StudioEvent" );
	if ( !cl_funcs.pStudioEvent )
		Sys_Error( "could not link client.dll function HUD_StudioEvent\n" );

	cl_funcs.pShutdown = (HUD_SHUTDOWN_FUNC)GetProcAddress( hClientDLL, "HUD_Shutdown" );
	if ( !cl_funcs.pShutdown )
		Sys_Error( "could not link client.dll function HUD_Shutdown\n" );

	cl_funcs.pTxferLocalOverrides = (HUD_TXFERLOCALOVERRIDES_FUNC)GetProcAddress( hClientDLL, "HUD_TxferLocalOverrides" );
	if ( !cl_funcs.pTxferLocalOverrides )
		Sys_Error( "could not link client.dll function HUD_TxferLocalOverrides\n" );

	cl_funcs.pProcessPlayerState = (HUD_PROCESSPLAYERSTATE_FUNC)GetProcAddress( hClientDLL, "HUD_ProcessPlayerState" );
	if ( !cl_funcs.pProcessPlayerState )
		Sys_Error( "could not link client.dll function HUD_ProcessPlayerState\n" );

	cl_funcs.pTxferPredictionData = (HUD_TXFERPREDICTIONDATA_FUNC)GetProcAddress( hClientDLL, "HUD_TxferPredictionData" );
	if ( !cl_funcs.pTxferPredictionData )
		Sys_Error( "could not link client.dll function HUD_TxferPredictionData\n" );

	cl_funcs.pReadDemoBuffer = (HUD_DEMOREAD_FUNC)GetProcAddress( hClientDLL, "Demo_ReadBuffer" );
	if ( !cl_funcs.pReadDemoBuffer )
		Sys_Error( "could not link client.dll function Demo_ReadBuffer\n" );

	cl_funcs.pConnectionlessPacket = (HUD_CONNECTIONLESS_FUNC)GetProcAddress( hClientDLL, "HUD_ConnectionlessPacket" );
	if ( !cl_funcs.pConnectionlessPacket )
		Sys_Error( "could not link client.dll function HUD_ConnectionlessPacket\n" );

	cl_funcs.pGetHullBounds = (HUD_GETHULLBOUNDS_FUNC)GetProcAddress( hClientDLL, "HUD_GetHullBounds" );
	if ( !cl_funcs.pGetHullBounds )
		Sys_Error( "could not link client.dll function HUD_GetHullBounds\n" );

	cl_funcs.pHudFrame = (HUD_FRAME_FUNC)GetProcAddress( hClientDLL, "HUD_Frame" );
	if ( !cl_funcs.pHudFrame )
		Sys_Error( "could not link client.dll function HUD_Frame\n" );

	cl_funcs.pKeyEvent = (HUD_KEY_EVENT_FUNC)GetProcAddress( hClientDLL, "HUD_Key_Event" );
	if ( !cl_funcs.pKeyEvent )
		Sys_Error( "could not link client.dll function HUD_Key_Event\n" );

	cl_funcs.pPostRunCmd = (HUD_POSTRUNCMD_FUNC)GetProcAddress( hClientDLL, "HUD_PostRunCmd" );
	if ( !cl_funcs.pPostRunCmd )
		Sys_Error( "could not link client.dll function HUD_PostRunCmd\n" );

	cl_funcs.pTempEntUpdate = (HUD_TEMPENTUPDATE_FUNC)GetProcAddress( hClientDLL, "HUD_TempEntUpdate" );
	if ( !cl_funcs.pTempEntUpdate )
		Sys_Error( "could not link client.dll function HUD_TempEntUpdate\n" );

	cl_funcs.pGetUserEntity = (HUD_GETUSERENTITY_FUNC)GetProcAddress( hClientDLL, "HUD_GetUserEntity" );
	if ( !cl_funcs.pGetUserEntity )
		Sys_Error( "could not link client.dll function HUD_GetUserEntity\n" );

	// optional exports
	cl_funcs.pVoiceStatus = (HUD_VOICESTATUS_FUNC)GetProcAddress( hClientDLL, "HUD_VoiceStatus" );
	cl_funcs.pDirectorMessage = (HUD_DIRECTORMESSAGE_FUNC)GetProcAddress( hClientDLL, "HUD_DirectorMessage" );
	cl_funcs.pStudioInterface = (HUD_STUDIO_INTERFACE_FUNC)GetProcAddress( hClientDLL, "HUD_GetStudioModelInterface" );
	cl_funcs.pChatInputPosition = (HUD_CHATINPUTPOSITION_FUNC)GetProcAddress( hClientDLL, "HUD_ChatInputPosition" );
	cl_funcs.pClientFactory = (CLIENTFACTORY)GetProcAddress( hClientDLL, "ClientFactory" );
}

/*
================
NCallDummy

================
*/
int NCallDummy( int ijump, int cnArg )
{
	return 0;
}

/*
================
ClientDLL_GetUserEntity

================
*/
cl_entity_t *ClientDLL_GetUserEntity( int index )
{
	if ( cl_funcs.pGetUserEntity )
		return cl_funcs.pGetUserEntity( index );

	return NULL;
}

/*
================
ClientDLL_VoiceStatus

================
*/
void ClientDLL_VoiceStatus( int entindex, qboolean bTalking )
{
	if ( cl_funcs.pVoiceStatus )
		cl_funcs.pVoiceStatus( entindex, bTalking );
}

/*
================
ClientDLL_ChatInputPosition

================
*/
void ClientDLL_ChatInputPosition( int *x, int *y )
{
	if ( cl_funcs.pChatInputPosition )
		cl_funcs.pChatInputPosition( x, y );
}

/*
================
ClientDLL_TempEntUpdate

================
*/
void ClientDLL_TempEntUpdate( double ft, double ct, double grav, struct tempent_s **ppFreeTE, struct tempent_s **ppActiveTE, int ( *addTEntity )( struct cl_entity_s *pEntity ), void ( *playTESound )( struct tempent_s *pTemp, float damp ) )
{
	if ( !cl_funcs.pTempEntUpdate )
		return;

	cl_funcs.pTempEntUpdate( ft, ct, grav, ppFreeTE, ppActiveTE, addTEntity, playTESound );
}

/*
================
ClientDLL_DirectorMessage

================
*/
void ClientDLL_DirectorMessage( int iSize, void *pbuf )
{
	if ( !cl_funcs.pDirectorMessage )
		return;

	cl_funcs.pDirectorMessage( iSize, pbuf );
}

/*
================
ClientDLL_Key_Event

================
*/
int ClientDLL_Key_Event( int down, int keynum, const char *pszCurrentBinding )
{
	if ( !VGui_Key_Event( down, keynum, pszCurrentBinding ) )
		return 0;

	if ( cl_funcs.pKeyEvent )
		return cl_funcs.pKeyEvent( down, keynum, pszCurrentBinding );

	return 1;
}

/*
================
ClientDLL_GetHullBounds

================
*/
int ClientDLL_GetHullBounds( int hullnumber, float *mins, float *maxs )
{
	if ( cl_funcs.pGetHullBounds )
		return cl_funcs.pGetHullBounds( hullnumber, mins, maxs );

	return 0;
}

/*
================
ClientDLL_ConnectionlessPacket

================
*/
int ClientDLL_ConnectionlessPacket( const struct netadr_s *net_from, const char *args, char *response_buffer, int *response_buffer_size )
{
	if ( cl_funcs.pConnectionlessPacket )
		return cl_funcs.pConnectionlessPacket( net_from, args, response_buffer, response_buffer_size );

	return 0;
}

/*
================
ClientDLL_PostRunCmd

================
*/
void ClientDLL_PostRunCmd( struct local_state_s *from, struct local_state_s *to, struct usercmd_s *cmd, int runfuncs, double time, unsigned int random_seed )
{
	if ( cl_funcs.pPostRunCmd )
	{
		cl_funcs.pPostRunCmd( from, to, cmd, runfuncs, time, random_seed );
	}
	else
	{
		if ( cl_lw.value )
		{
			Cvar_SetValue( "cl_lw", 0 );
		}
	}
}

/*
================
ClientDLL_StudioEvent

================
*/
void ClientDLL_StudioEvent( const struct mstudioevent_s *event, const struct cl_entity_s *entity )
{
	if ( cl_funcs.pStudioEvent )
		cl_funcs.pStudioEvent( event, entity );
}

/*
================
ClientDLL_DrawNormalTriangles

================
*/
void ClientDLL_DrawNormalTriangles( void )
{
	if ( cl_funcs.pDrawNormalTriangles )
		cl_funcs.pDrawNormalTriangles();

	tri.RenderMode( kRenderNormal );
}

/*
================
ClientDLL_DrawTransparentTriangles

================
*/
void ClientDLL_DrawTransparentTriangles( void )
{
	if ( cl_funcs.pDrawTransparentTriangles )
		cl_funcs.pDrawTransparentTriangles();

	tri.RenderMode( kRenderNormal );
}

/*
================
ClientDLL_CreateEntities

================
*/
void ClientDLL_CreateEntities( void )
{
	if ( cl_funcs.pCreateEntities )
		cl_funcs.pCreateEntities();
}

/*
================
ClientDLL_AddEntity

================
*/
int ClientDLL_AddEntity( int type, struct cl_entity_s *ent )
{
	vec3_t	emins, emaxs;
	int		i;

	if ( !cl_funcs.pAddEntity )
		return 0;

	if ( !cl_funcs.pAddEntity( type, ent, ent->model ? ent->model->name : "" ) )
		return 0;

	if ( CL_IsSpectateOnly() && !CL_IsDevOverviewMode() && type != ET_TEMPENTITY && type != ET_BEAM && ent->curstate.movetype != MOVETYPE_FOLLOW )
	{
		for ( i = 0; i < 3; i++ )
		{
			emins[i] = ent->origin[i] + ent->model->mins[i];
			emaxs[i] = ent->origin[i] + ent->model->maxs[i];
		}

		if ( !PVSNode( cl.worldmodel->nodes, emins, emaxs ) )
			return 0;
	}

	switch ( type )
	{
	case ET_BEAM:
		if ( cl_numbeamentities < MAX_BEAMENTS )
		{
			cl_beamentities[cl_numbeamentities] = ent;
			cl_numbeamentities++;
		}
		else
		{
			return 0;
		}
		break;

	default:
		if ( cl_numvisedicts < MAX_VISEDICTS )
		{
			cl_visedicts[cl_numvisedicts] = ent;
			cl_numvisedicts++;
		}
		else
		{
			return 0;
		}
		break;
	}

	return 1;
}

/*
================
ClientDLL_CalcRefdef

================
*/
void ClientDLL_CalcRefdef( struct ref_params_s *pparams )
{
	if ( cl_funcs.pCalcRefdef )
		cl_funcs.pCalcRefdef( pparams );
}

/*
================
ClientDLL_CAM_Think

================
*/
void ClientDLL_CAM_Think( void )
{
	if ( cls.state == ca_dedicated || cls.state == ca_disconnected )
		return;

	if ( cl_funcs.pCamThink )
		cl_funcs.pCamThink();
}

/*
================
ClientDLL_FindKey

================
*/
struct kbutton_s *ClientDLL_FindKey( const char *name )
{
	if ( cl_funcs.pFindKey )
		return cl_funcs.pFindKey( name );

	return NULL;
}

/*
================
ClientDLL_GetCameraOffsets

================
*/
void ClientDLL_GetCameraOffsets( float *ofs )
{
	if ( cl_funcs.pCL_GetCameraOffsets )
		cl_funcs.pCL_GetCameraOffsets( ofs );
}

/*
================
ClientDLL_IsThirdPerson

================
*/
int ClientDLL_IsThirdPerson( void )
{
	if ( cl_funcs.pCL_IsThirdPerson )
		return cl_funcs.pCL_IsThirdPerson();

	return 0;
}

/*
================
ClientDLL_CreateMove

================
*/
void ClientDLL_CreateMove( float frametime, struct usercmd_s *cmd, int active )
{
	if ( cl_funcs.pCL_CreateMove )
		cl_funcs.pCL_CreateMove( frametime, cmd, active );
}

/*
================
ClientDLL_ActivateMouse

================
*/
void ClientDLL_ActivateMouse( void )
{
	if ( !fClientLoaded || !cl_funcs.pIN_ActivateMouse )
		return;

	cl_funcs.pIN_ActivateMouse();
}

/*
================
ClientDLL_DeactivateMouse

================
*/
void ClientDLL_DeactivateMouse( void )
{
	if ( !fClientLoaded || !cl_funcs.pIN_DeactivateMouse )
		return;

	cl_funcs.pIN_DeactivateMouse();
}

/*
================
ClientDLL_MouseEvent

================
*/
void ClientDLL_MouseEvent( int mstate )
{
	if ( !fClientLoaded || !cl_funcs.pIN_MouseEvent )
		return;

	cl_funcs.pIN_MouseEvent( mstate );
}

/*
================
ClientDLL_ClearStates

================
*/
void ClientDLL_ClearStates( void )
{
	if ( !fClientLoaded || !cl_funcs.pIN_ClearStates )
		return;

	cl_funcs.pIN_ClearStates();
}

/*
================
ClientDLL_IN_Accumulate

================
*/
void ClientDLL_IN_Accumulate( void )
{
	if ( !fClientLoaded || !cl_funcs.pIN_Accumulate )
		return;

	cl_funcs.pIN_Accumulate();
}

/*
================
ClientDLL_ClientMoveInit

================
*/
void ClientDLL_ClientMoveInit( struct playermove_s *ppmove )
{
	if ( cl_funcs.pClientMoveInit )
		cl_funcs.pClientMoveInit( ppmove );
}

/*
================
ClientDLL_ClientTextureType

================
*/
char ClientDLL_ClientTextureType( char *name )
{
	if ( cl_funcs.pClientTextureType )
		return cl_funcs.pClientTextureType( name );

	return 0;
}

/*
================
ClientDLL_MoveClient

================
*/
void ClientDLL_MoveClient( struct playermove_s *ppmove )
{
	if ( cl_funcs.pClientMove )
		cl_funcs.pClientMove( ppmove, false );
}

/*
================
ClientDLL_Shutdown

================
*/
void ClientDLL_Shutdown( void )
{
	Net_APIShutDown();
	SPR_Shutdown();

	if ( cl_funcs.pShutdown )
		cl_funcs.pShutdown();

	if ( cls.fSecureClient )
	{
		FreeBlob( &g_blobfootprintClient_guess );
	}
	else
	{
		FreeLibrary( hClientDLL );
		hClientDLL = NULL;
	}

	Q_memset( &cl_funcs, 0, sizeof( cl_funcs ) );

	Cvar_RemoveHudCvars();
	Cmd_RemoveHudCmds();
	CL_ShutDownUsrMessages();

	fClientLoaded = false;

	FreeLibrary( hClientDLL );
	hClientDLL = NULL;
}

/*
================
ClientDLL_HudVidInit

================
*/
void ClientDLL_HudVidInit( void )
{
	if ( !cl_funcs.pHudVidInitFunc )
		Sys_Error( "C:\\Documents and Settings\\XPMUser\\Desktop\\GoldSource\\engine\\cdll_int.c, line %d: could not link client DLL for HUD Vid initialization", 1074 );

	SPR_Init();

	cl_funcs.pHudVidInitFunc();
}

/*
================
ClientDLL_HudInit

================
*/
void ClientDLL_HudInit( void )
{
	if ( !cl_funcs.pHudInitFunc )
		Sys_Error( "C:\\Documents and Settings\\XPMUser\\Desktop\\GoldSource\\engine\\cdll_int.c, line %d: could not link client DLL for HUD initialization", 1093 );

	cl_funcs.pHudInitFunc();

	ClientDLL_CheckStudioInterface( hClientDLL );

	cl_righthand = Cvar_FindVar( "cl_righthand" );
}

/*
================
ClientDLL_HudRedraw

================
*/
void ClientDLL_HudRedraw( int intermission )
{
	if ( VGuiWrap2_IsGameUIVisible() )
		return;

	if ( cl_funcs.pHudRedrawFunc )
		cl_funcs.pHudRedrawFunc( (float)cl.time, intermission );
}

/*
================
ClientDLL_Frame

================
*/
void ClientDLL_Frame( double time )
{
	if ( !fClientLoaded || !cl_funcs.pHudFrame )
		return;

	cl_funcs.pHudFrame( time );
}

/*
================
ClientDLL_UpdateClientData

================
*/
void ClientDLL_UpdateClientData( void )
{
	client_data_t	oldcdat;
	client_data_t	cdat;

	if ( cls.state != ca_active )
		return;

	if ( cls.demoplayback )
		return;

	Q_memset( &cdat, 0, sizeof( cdat ) );

	VectorCopy( cl.viewangles, cdat.viewangles );
	VectorCopy( cl_entities[cl.viewentity].origin, cdat.origin );
	cdat.fov = scr_fov_value;
	cdat.iWeaponBits = cl.weapons;

	if ( cls.demorecording )
		oldcdat = cdat;

	if ( cl_funcs.pHudUpdateClientDataFunc && cl_funcs.pHudUpdateClientDataFunc( &cdat, cl.time ) )
	{
		VectorCopy( cdat.viewangles, cl.viewangles );
		scr_fov_value = cdat.fov;
	}

	if ( cls.demorecording )
		CL_WriteDLLUpdate( &oldcdat );
}

/*
================
ClientDLL_DemoUpdateClientData

================
*/
void ClientDLL_DemoUpdateClientData( client_data_t *cdat )
{
	if ( cl_funcs.pHudUpdateClientDataFunc )
	{
		if ( cl_funcs.pHudUpdateClientDataFunc( cdat, (float)cl.time ) && !cls.spectator )
		{
			VectorCopy( cdat->viewangles, cl.viewangles );
			scr_fov_value = cdat->fov;
		}
	}
}

/*
================
COM_ParseFile

================
*/
char *COM_ParseFile( char *data, char *token )
{
	char	*return_data;

	g_engdstAddrs.COM_ParseFile( &data, &token );

	return_data = COM_Parse( data );
	Q_strcpy( token, com_token );

	return return_data;
}

/*
================
ClientDLL_TxferLocalOverrides

================
*/
void ClientDLL_TxferLocalOverrides( struct entity_state_s *state, const struct clientdata_s *client )
{
	if ( cl_funcs.pTxferLocalOverrides )
		cl_funcs.pTxferLocalOverrides( state, client );
}

/*
================
ClientDLL_ProcessPlayerState

================
*/
void ClientDLL_ProcessPlayerState( struct entity_state_s *dst, const struct entity_state_s *src )
{
	if ( cl_funcs.pProcessPlayerState )
		cl_funcs.pProcessPlayerState( dst, src );
}

/*
================
ClientDLL_TxferPredictionData

================
*/
void ClientDLL_TxferPredictionData( struct entity_state_s *ps, const struct entity_state_s *pps, struct clientdata_s *pcd, const struct clientdata_s *ppcd, struct weapon_data_s *wd, const struct weapon_data_s *pwd )
{
	if ( cl_funcs.pTxferPredictionData )
		cl_funcs.pTxferPredictionData( ps, pps, pcd, ppcd, wd, pwd );
}

/*
================
ClientDLL_ReadDemoBuffer

================
*/
void ClientDLL_ReadDemoBuffer( int size, unsigned char *buffer )
{
	if ( cl_funcs.pReadDemoBuffer )
		cl_funcs.pReadDemoBuffer( size, buffer );
}

/*
================
SPR_GetList

================
*/
client_sprite_t *SPR_GetList( char *psz, int *piCount )
{
	char			*pfile;
	int				iCount;
	int				i;
	client_sprite_t	*ps, *pret;

	g_engdstAddrs.pfnSPR_GetList( &psz, &piCount );

	pfile = (char *)COM_LoadFile( psz, 2, NULL );
	if ( !pfile )
		return NULL;

	pfile = COM_Parse( pfile );
	iCount = atoi( com_token );
	if ( !iCount )
		return NULL;

	ps = (client_sprite_t *)calloc( iCount * sizeof( client_sprite_t ), 1 );
	if ( !ps )
		return ps;

	pret = ps;

	for ( i = 0; i < iCount; i++ )
	{
		pfile = COM_Parse( pfile );
		Q_strncpy( ps->szName, com_token, sizeof( ps->szName ) - 1 );
		ps->szName[sizeof( ps->szName ) - 1] = 0;

		pfile = COM_Parse( pfile );
		ps->iRes = atoi( com_token );

		pfile = COM_Parse( pfile );
		Q_strncpy( ps->szSprite, com_token, sizeof( ps->szSprite ) - 1 );
		ps->szSprite[sizeof( ps->szSprite ) - 1] = 0;

		pfile = COM_Parse( pfile );
		ps->rc.left = atoi( com_token );

		pfile = COM_Parse( pfile );
		ps->rc.top = atoi( com_token );

		pfile = COM_Parse( pfile );
		ps->rc.right = atoi( com_token ) + ps->rc.left;

		pfile = COM_Parse( pfile );
		ps->rc.bottom = atoi( com_token ) + ps->rc.top;

		ps++;
	}

	if ( piCount )
		*piCount = iCount;

	return pret;
}

/*
================
hudCenterX

================
*/
int hudCenterX( void )
{
	g_engdstAddrs.GetWindowCenterX();

	return window_center_x;
}

/*
================
hudCenterY

================
*/
int hudCenterY( void )
{
	g_engdstAddrs.GetWindowCenterY();

	return window_center_y;
}

/*
================
hudGetViewAngles

================
*/
void hudGetViewAngles( float *va )
{
	g_engdstAddrs.GetViewAngles( &va );

	VectorCopy( cl.viewangles, va );
}

/*
================
hudSetViewAngles

================
*/
void hudSetViewAngles( float *va )
{
	g_engdstAddrs.SetViewAngles( &va );

	VectorCopy( va, cl.viewangles );
}

/*
================
hudGetMaxClients

================
*/
int hudGetMaxClients( void )
{
	g_engdstAddrs.GetMaxClients();

	return cl.maxclients;
}

/*
================
hudPhysInfo_ValueForKey

================
*/
const char *hudPhysInfo_ValueForKey( const char *key )
{
	g_engdstAddrs.PhysInfo_ValueForKey( &key );

	return Info_ValueForKey( cls.physinfo, key );
}

/*
================
hudServerInfo_ValueForKey

================
*/
const char *hudServerInfo_ValueForKey( const char *key )
{
	g_engdstAddrs.ServerInfo_ValueForKey( &key );

	return Info_ValueForKey( cl.serverinfo, key );
}

/*
================
hudGetMousePosition

================
*/
void hudGetMousePosition( int *mx, int *my )
{
	POINT	current_pos_guess;

	g_engdstAddrs.GetMousePosition( &mx, &my );

	GetCursorPos( &current_pos_guess );
	ScreenToClient( *pmainwindow, &current_pos_guess );

	*mx = current_pos_guess.x;
	*my = current_pos_guess.y;
}

/*
================
hudIsNoClipping

================
*/
int hudIsNoClipping( void )
{
	g_engdstAddrs.IsNoClipping();

	return cl.frames[cl.parsecountmod].playerstate[cl.playernum].movetype == MOVETYPE_NOCLIP;
}

/*
================
hudGetLocalPlayer

================
*/
struct cl_entity_s *hudGetLocalPlayer( void )
{
	g_engdstAddrs.GetLocalPlayer();

	return &cl_entities[cl.playernum + 1];
}

/*
================
hudGetViewModel

================
*/
struct cl_entity_s *hudGetViewModel( void )
{
	g_engdstAddrs.GetViewModel();

	return &cl.viewent;
}

/*
================
hudGetEntityByIndex

================
*/
struct cl_entity_s *hudGetEntityByIndex( int idx )
{
	g_engdstAddrs.GetEntityByIndex( &idx );

	if ( idx < 0 || idx >= cl.max_edicts )
		return NULL;

	return &cl_entities[idx];
}

/*
================
hudGetClientTime

================
*/
float hudGetClientTime( void )
{
	g_engdstAddrs.GetClientTime();

	return cl.time;
}

/*
================
hudGetClientOldTime

================
*/
float hudGetClientOldTime( void )
{
	return cl.oldtime;
}

/*
================
hudGetServerGravityValue

================
*/
float hudGetServerGravityValue( void )
{
	return sv_gravity.value;
}

/*
================
hudGetModelByIndex

================
*/
struct model_s *hudGetModelByIndex( const int index )
{
	return CL_GetModelByIndex( index );
}

/*
================
CL_LoadModel

================
*/
struct model_s *CL_LoadModel( const char *modelname, int *index )
{
	int		i = 1;

	g_engdstAddrs.CL_LoadModel( &modelname, &index );

	while ( 1 )
	{
		model_t	*mod = cl.model_precache[i];

		if ( !mod )
			break;

		if ( !Q_stricmp( modelname, mod->name ) )
		{
			mod = CL_GetModelByIndex( i );
			if ( index )
				*index = i;
			return mod;
		}

		i++;
	}

	if ( index )
		*index = -1;

	return NULL;
}

/*
================
CL_CreateVisibleEntity

================
*/
int CL_CreateVisibleEntity( int type, struct cl_entity_s *ent )
{
	int		iret = 0;

	g_engdstAddrs.CL_CreateVisibleEntity( &type, &ent );

	switch ( type )
	{
	case ET_BEAM:
		if ( cl_numbeamentities < MAX_BEAMENTS )
		{
			iret = 1;
			cl_beamentities[cl_numbeamentities] = ent;
			cl_numbeamentities++;
		}
		break;
	default:
		if ( cl_numvisedicts < MAX_VISEDICTS )
		{
			iret = 1;
			cl_visedicts[cl_numvisedicts] = ent;
			cl_numvisedicts++;
		}
		break;
	}

	return iret;
}

/*
================
hudGetSpritePointer

================
*/
const struct model_s *hudGetSpritePointer( HSPRITE hSprite )
{
	g_engdstAddrs.GetSpritePointer( &hSprite );

	return SPR_GetModelPointer( hSprite );
}

/*
================
hudPrecacheEvent

================
*/
unsigned short hudPrecacheEvent( int type, const char *psz )
{
	int		i;

	g_engdstAddrs.pfnPrecacheEvent( &type, &psz );

	// the loop condition never fails, the loop ends at the first free slot
	for ( i = 1; MAX_EVENTS; i++ )
	{
		if ( !cl.event_precache[i].filename )
			break;

		if ( !Q_stricmp( cl.event_precache[i].filename, psz ) )
			return (unsigned short)i;
	}

	return 0;
}

/*
================
hudPlaybackEvent

================
*/
void hudPlaybackEvent( int flags, const struct edict_s *pInvoker, unsigned short eventindex, float delay, float *origin, float *angles, float fparam1, float fparam2, int iparam1, int iparam2, int bparam1, int bparam2 )
{
	event_args_t	eargs;

	g_engdstAddrs.pfnPlaybackEvent( &flags, &pInvoker, &eventindex, &delay, &origin, &angles, &fparam1, &fparam2, &iparam1, &iparam2, &bparam1, &bparam2 );

	if ( flags & FEV_SERVER )
		return;

	Q_memset( &eargs, 0, sizeof( eargs ) );

	VectorCopy( origin, eargs.origin );
	VectorCopy( angles, eargs.angles );
	VectorCopy( cl.simvel, eargs.velocity );

	eargs.fparam1 = fparam1;
	eargs.fparam2 = fparam2;
	eargs.iparam1 = iparam1;
	eargs.iparam2 = iparam2;
	eargs.bparam1 = bparam1;
	eargs.bparam2 = bparam2;

	eargs.entindex = cl.playernum + 1;
	eargs.ducking = ( cl.usehull == 1 );

	CL_QueueEvent( flags, eventindex, delay, &eargs );

	if ( cls.demorecording )
		CL_DemoEvent( flags, eventindex, delay, &eargs );
}

/*
================
hudWeaponAnim

================
*/
void hudWeaponAnim( int iAnim, int body )
{
	g_engdstAddrs.pfnWeaponAnim( &iAnim, &body );

	cl.weaponsequence = iAnim;
	cl.weaponstarttime = 0;
	cl.viewent.curstate.body = body;

	if ( cls.demorecording )
		CL_DemoAnim( iAnim, body );
}

/*
================
hudGetClientMaxspeed

================
*/
float hudGetClientMaxspeed( void )
{
	g_engdstAddrs.GetClientMaxspeed();

	return cl.maxspeed;
}

/*
================
hudGetGameDir

================
*/
const char *hudGetGameDir( void )
{
	g_engdstAddrs.pfnGetGameDirectory();

	return com_gamedir;
}

/*
================
hudGetLevelName

================
*/
const char *hudGetLevelName( void )
{
	g_engdstAddrs.pfnGetLevelName();

	if ( cls.state >= ca_connected )
		return cl.levelname;

	return "";
}

/*
================
hudGetScreenFade

================
*/
void hudGetScreenFade( struct screenfade_s *fade )
{
	g_engdstAddrs.pfnGetScreenFade( &fade );

	*fade = cl.sf;
}

/*
================
hudSetScreenFade

================
*/
void hudSetScreenFade( struct screenfade_s *fade )
{
	g_engdstAddrs.pfnSetScreenFade( &fade );

	cl.sf = *fade;
}

/*
================
ClientDLL_CheckStudioInterface

================
*/
void ClientDLL_CheckStudioInterface( HMODULE hClientDLL )
{
	HUD_STUDIO_INTERFACE_FUNC	pStudioInterface;

	R_ResetStudio();

	if ( !fClientLoaded )
		return;

	pStudioInterface = (HUD_STUDIO_INTERFACE_FUNC)GetProcAddress( hClientDLL, "HUD_GetStudioModelInterface" );
	if ( cl_funcs.pStudioInterface )
	{
		if ( !cl_funcs.pStudioInterface( STUDIO_INTERFACE_VERSION, &pStudioAPI, &engine_studio_api ) )
		{
			Con_DPrintf( "Couldn't get client .dll studio model rendering interface.  Version mismatch?\n" );
			R_ResetStudio();
		}
	}
}

/*
================
COM_AddAppDirectoryToSearchPath

================
*/
void COM_AddAppDirectoryToSearchPath( const char *pszBaseDir, const char *appName )
{
	g_engdstAddrs.COM_AddAppDirectoryToSearchPath( &pszBaseDir, &appName );

	COM_AddAppDirectory( pszBaseDir, appName );
}

/*
================
ClientDLL_ExpandFileName

================
*/
int ClientDLL_ExpandFileName( const char *fileName, char *nameOutBuffer, int nameOutBufferSize )
{
	char	buf[512];

	g_engdstAddrs.COM_ExpandFilename( &fileName, &nameOutBuffer, &nameOutBufferSize );

	Q_strncpy( buf, fileName, sizeof( buf ) - 1 );
	buf[sizeof( buf ) - 1] = 0;

	if ( !COM_ExpandFilename( buf ) )
		return 0;

	COM_FixSlashes( buf );
	Q_strncpy( nameOutBuffer, buf, nameOutBufferSize );
	nameOutBuffer[nameOutBufferSize - 1] = 0;

	return 1;
}

/*
================
PlayerInfo_ValueForKey

================
*/
const char *PlayerInfo_ValueForKey( int playerNum, const char *key )
{
	g_engdstAddrs.PlayerInfo_ValueForKey( &playerNum, &key );

	if ( playerNum > cl.maxclients || playerNum < 1 || !cl.players[playerNum - 1].name || !cl.players[playerNum - 1].name[0] )
		return NULL;

	return Info_ValueForKey( cl.players[playerNum - 1].userinfo, key );
}

/*
================
LocalPlayerInfo_ValueForKey

================
*/
const char *LocalPlayerInfo_ValueForKey( const char *key )
{
	g_engdstAddrs.LocalPlayerInfo_ValueForKey( &key );

	return Info_ValueForKey( cls.userinfo, key );
}

/*
================
PlayerInfo_SetValueForKey

================
*/
void PlayerInfo_SetValueForKey( const char *key, const char *value )
{
	if ( !strcmp( Info_ValueForKey( cls.userinfo, key ), value ) )
		return;

	g_engdstAddrs.PlayerInfo_SetValueForKey( &key, &value );

	Info_SetValueForStarKey( cls.userinfo, key, value, MAX_INFO_STRING );

	if ( cls.state != ca_dedicated && cls.state >= ca_connected )
	{
		MSG_WriteByte( &cls.netchan.message, clc_stringcmd );
		SZ_Print( &cls.netchan.message, va( "setinfo \"%s\" \"%s\"\n", key, value ) );
	}
}

/*
================
GetPlayerUniqueID

================
*/
qboolean GetPlayerUniqueID( int iPlayer, char *playerID )
{
	g_engdstAddrs.GetPlayerUniqueID( &iPlayer, &playerID );

	iPlayer--;

	if ( iPlayer < 0 || iPlayer >= cl.maxclients )
		return false;

	if ( !cl.players[iPlayer].userinfo[0] || !cl.players[iPlayer].name[0] )
		return false;

	memcpy( playerID, cl.players[iPlayer].hashedcdkey, 16 );
	return true;
}

/*
================
GetTrackerIDForPlayer

================
*/
int GetTrackerIDForPlayer( int playerSlot )
{
	g_engdstAddrs.GetTrackerIDForPlayer( &playerSlot );

	return 0;
}

/*
================
GetPlayerForTrackerID

================
*/
int GetPlayerForTrackerID( int trackerID )
{
	g_engdstAddrs.GetPlayerForTrackerID( &trackerID );

	return 0;
}

/*
================
GetFirstCVarPtr

================
*/
struct cvar_s *GetFirstCVarPtr( void )
{
	return cvar_vars;
}

/*
================
GetFirstCmdFunctionHandle

================
*/
unsigned int GetFirstCmdFunctionHandle( void )
{
	return (unsigned int)Cmd_GetFirstCmd();
}

/*
================
GetNextCmdFunctionHandle

================
*/
unsigned int GetNextCmdFunctionHandle( unsigned int cmdhandle )
{
	cmd_function_t	*cmd;

	cmd = (cmd_function_t *)cmdhandle;
	return (unsigned int)cmd->next;
}

/*
================
GetCmdFunctionName

================
*/
const char *GetCmdFunctionName( unsigned int cmdhandle )
{
	cmd_function_t	*cmd;

	cmd = (cmd_function_t *)cmdhandle;
	return cmd->name;
}
