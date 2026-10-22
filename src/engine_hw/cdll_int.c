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

typedef struct client_data_s
{
	// fields that cannot be modified  (ie. have no effect if changed)
	vec3_t	origin;

	// fields that can be changed by the cldll
	vec3_t	viewangles;
	int		iWeaponBits;
	float	fov;	// field of view
} client_data_t;

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

int		CL_IsSpectateOnly_guess( void );
void	SPR_Init_guess( void );
void	SPR_Shutdown_guess( void );
void	R_ResetStudio( void );
mnode_t	*PVSNode( mnode_t *node, vec3_t emins, vec3_t emaxs );
void	CL_WriteDLLUpdate( client_data_t *cdat );
void	CL_DemoAnim( int anim, int body );
void	ClientDLL_CheckStudioInterface( HMODULE hClientDLL );

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

	tri_GL_RenderMode( kRenderNormal );
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

	tri_GL_RenderMode( kRenderNormal );
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
	vec3_t	mins, maxs;
	int		i;

	if ( !cl_funcs.pAddEntity )
		return 0;

	if ( !cl_funcs.pAddEntity( type, ent, ent->model ? ent->model->name : "" ) )
		return 0;

	if ( CL_IsSpectateOnly_guess() && !CL_IsDevOverviewMode() && type != ET_TEMPENTITY && type != ET_BEAM && ent->curstate.movetype != MOVETYPE_FOLLOW )
	{
		for ( i = 0; i < 3; i++ )
		{
			mins[i] = ent->origin[i] + ent->model->mins[i];
			maxs[i] = ent->origin[i] + ent->model->maxs[i];
		}

		if ( !PVSNode( cl.worldmodel->nodes, mins, maxs ) )
			return 0;
	}

	if ( type == ET_BEAM )
	{
		if ( cl_numbeamentities >= MAX_BEAMENTS )
			return 0;

		cl_beamentities[cl_numbeamentities] = ent;
		cl_numbeamentities++;
	}
	else
	{
		if ( cl_numvisedicts >= MAX_VISEDICTS )
			return 0;

		cl_visedicts[cl_numvisedicts] = ent;
		cl_numvisedicts++;
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
	SPR_Shutdown_guess();

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

	SPR_Init_guess();

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

	if ( cls.state != ca_active || cls.demoplayback )
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
	if ( !cl_funcs.pHudUpdateClientDataFunc )
		return;

	if ( cl_funcs.pHudUpdateClientDataFunc( cdat, (float)cl.time ) && !cls.spectator )
	{
		VectorCopy( cdat->viewangles, cl.viewangles );
		scr_fov_value = cdat->fov;
	}
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
