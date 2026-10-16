// host.h -- the main loop (host.c)

#ifndef HOST_H
#define HOST_H

extern	double	realtime;			// not bounded in any way, changed at
									// start of every frame, never reset

extern	cvar_t	developer;

extern	double	host_frametime;		// time of the current frame

#endif // HOST_H
