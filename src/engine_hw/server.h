// server.h -- the local server (sv_main.c and friends)

#ifndef SERVER_H
#define SERVER_H

delta_t	*SV_LookupDelta( char *name );

extern	int		g_bIsCStrike;
extern	int		g_bIsCZero;

void	SetCStrikeFlags( void );

typedef enum
{
	RD_NONE,
	RD_CLIENT,
	RD_PACKET
} redirect_t;

extern	redirect_t	sv_redirected;
extern	char		outputbuf[1400];

void	SV_FlushRedirect( void );

// The leading members of server_t, checked against the Windows binary (sv at
// 0x025203A0: active read by Host_IsServerActive, time at 0x025203B0 read by
// DELTA_WriteMarkedFields). The members after clientdllmd5 (resourcelist,
// precache tables, edicts, message buffers) are declared together with the
// server files that use them.
#include "crc.h"

struct model_s;

typedef struct server_s
{
	qboolean		active;				// false if only a net client
	qboolean		paused;
	qboolean		loadgame;			// handle connections specially

	double			time;
	double			oldtime;

	int				lastcheck;			// used by PF_checkclient
	double			lastchecktime;		// for monster ai

	char			name[64];			// map name
	char			oldname[64];
	char			startspot[64];
	char			modelname[64];		// maps/<name>.bsp, for model_precache[0]

	struct model_s	*worldmodel;
	CRC32_t			worldmapCRC;		// For detecting that client has a hacked local copy of map, the client will be dropped if this occurs.
	unsigned char	clientdllmd5[16];	// For detecting that client has a hacked local copy of the client dll
} server_t;

extern	server_t	sv;

#endif // SERVER_H
