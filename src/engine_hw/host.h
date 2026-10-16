// host.h -- the main loop (host.c)

#ifndef HOST_H
#define HOST_H

extern	double	realtime;			// not bounded in any way, changed at
									// start of every frame, never reset

extern	cvar_t	developer;

extern	double	host_frametime;		// time of the current frame

extern	cvar_t	host_framerate;
extern	cvar_t	host_limitlocal;
extern	cvar_t	sys_timescale;

qboolean	Host_IsServerActive( void );
qboolean	Host_IsSinglePlayerGame( void );
void	Host_ShutdownServer( qboolean crash );
void	Host_ClearMemory( qboolean bQuiet );

void	CL_AddVoiceToDatagram( qboolean bFinal );

#endif // HOST_H
