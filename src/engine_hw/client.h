// client.h -- the client state (cl_main.c and friends)

#ifndef CLIENT_H
#define CLIENT_H

#include "custom.h"
#include "screenfade.h"

#define MAX_RESOURCE_LIST	1280

// The leading members of client_state_t, checked against the Windows binary
// (cl at 0x02DE6A84: parsecount 0x02E115A8, viewangles 0x02E1166C,
// intermission 0x02E11A08, mtime 0x02E11A10). usercmd_t is 4 bytes longer
// than in the Linux build (is_donator), so every member after cmd sits 4
// bytes further than the Linux offsets. The members after oldtime (frames,
// commands, ...) are declared together with the client files that use them.
typedef struct
{
	int				max_edicts;

	resource_t		resourcesonhand;
	resource_t		resourcesneeded;
	resource_t		resourcelist[MAX_RESOURCE_LIST];
	int				num_resources;

	qboolean		need_force_consistency_response;

	char			serverinfo[512];	// MAX_SERVERINFO_STRING

	int				servercount;		// server identification for prespawns

	int				validsequence;		// this is the sequence number of the last good
										// world snapshot/update we got. If this is 0, we can't
										// render a frame yet

	int				parsecount;			// server message counter
	int				parsecountmod;		// modulo with network window

	int				stats[32];			// health, etc
	int				weapons;			// bit flags

	usercmd_t		cmd;				// sent to server

	vec3_t			viewangles;
	vec3_t			punchangle;
	vec3_t			crosshairangle;
	vec3_t			simorg;
	vec3_t			simvel;
	vec3_t			simangles;
	vec_t			predicted_origins[64][3];
	vec3_t			prediction_error;
	float			idealpitch;
	vec3_t			viewheight;

	screenfade_t	sf;

	qboolean		paused;				// send over by server

	int				onground;
	int				moving;
	int				waterlevel;
	int				usehull;
	float			maxspeed;

	int				pushmsec;
	int				light_level;

	int				intermission;		// don't change view angle, full screen, et

	double			mtime[2];			// the timestamp of the last two messages
	double			time;				// clients view of time, time should be between
										// servertime and oldservertime to generate
										// a lerp point for other data
	double			oldtime;			// previous cl.time, time-oldtime is used
										// to decay light values and smooth step ups
} client_state_t;

extern	client_state_t	cl;

#endif // CLIENT_H
