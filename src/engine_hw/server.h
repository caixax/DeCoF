// server.h -- the local server (sv_main.c and friends)

#ifndef SERVER_H
#define SERVER_H

#include "custom.h"
#include "net.h"
#include "consistency.h"
#include "event.h"

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

typedef enum
{
	ss_dead,	// no map loaded
	ss_loading,	// spawning level edicts
	ss_active	// actively running
} server_state_t;

// server_t, checked against the Windows binary (sv at 0x025203A0). Doubles
// are 8 byte aligned here, unlike the Linux build: time at +0x10, worldmodel
// at +0x130. The precache limits of this build are larger than the Linux
// ones (768 models, sounds and generic files): num_resources at +0x3B948
// (SV_SendResources), models at +0x41D50 (SV_HullForBsp), edicts at +0x52160
// (SV_LinkEdict), signon at +0x54900.
struct model_s;
typedef struct edict_s edict_t;

typedef struct
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

	resource_t		resourcelist[MAX_MODELS + MAX_SOUNDS + MAX_EVENTS];	// 1792 in this build
	int				num_resources;

	consistency_t	consistency_list[MAX_CONSISTENCY_LIST];
	int				num_consistency;

	char			*model_precache[MAX_MODELS];
	struct model_s	*models[MAX_MODELS];
	unsigned char	model_precache_flags[MAX_MODELS];
	struct event_s	event_precache[MAX_EVENTS];

	char			*sound_precache[MAX_SOUNDS];
	short			sound_precache_hashedlookup[MAX_SOUNDS_HASHLOOKUP_SIZE];
	qboolean		sound_precache_hashedlookup_built;

	char			*generic_precache[MAX_GENERIC];
	char			generic_precache_names[MAX_GENERIC][64];
	int				num_generic_names;

	char			*lightstyles[MAX_LIGHTSTYLES];

	int				num_edicts;
	int				max_edicts;
	edict_t			*edicts;			// can NOT be array indexed, because
										// edict_t is variable sized, but can
										// be used to reference the world ent
	struct entity_state_s	*baselines;		// parallel array to edicts, storing entity baselines
	struct extra_baselines_s	*instance_baselines;

	server_state_t	state;				// some actions are only valid during load

	sizebuf_t		datagram;
	byte			datagram_buf[MAX_DATAGRAM];

	sizebuf_t		reliable_datagram;	// copied to all clients at end of frame
	byte			reliable_datagram_buf[MAX_DATAGRAM];

	sizebuf_t		multicast;			// Longest multicast message
	byte			multicast_buf[1024];

	sizebuf_t		spectator;			// Spectator channel
	byte			spectator_buf[1024];

	sizebuf_t		signon;
	byte			signon_data[32768];
} server_t;

// server_static_t, checked against the Windows binary (svs at 0x0251F320:
// maxclients +0x08, log +0x18, log.file +0x34, next_cleartime +0x38,
// stats +0x48, isSecure +0x80).
typedef struct
{
	qboolean	active;
	qboolean	net_log;
	netadr_t	net_address;
	void		*file;
} server_log_t;

typedef struct
{
	int		num_samples;
	int		at_capacity;
	int		at_empty;
	float	capacity_percent;
	float	empty_percent;
	int		minusers;
	int		maxusers;
	float	cumulative_occupancy;
	float	occupancy;
	int		num_sessions;
	float	cumulative_sessiontime;
	float	average_session_len;
	float	cumulative_latency;
	float	average_latency;
} server_stats_t;

typedef struct server_static_s
{
	qboolean		dll_initialized;	// Have we loaded the game dll.
	struct client_s	*clients;			// array of up to [maxclients] client slots.
	int				maxclients;			// Current max #
	int				maxclientslimit;	// Max allowed on server.
	int				spawncount;			// Number of servers spawned since start,
										// used to check late spawns (e.g., when d/l'ing lots of
										// data)
	int				serverflags;		// Episode completion information

	server_log_t	log;				// Log file information.

	double			next_cleartime;		// Next time to clear the stats
	double			next_sampletime;	// Next time to sample the stats
	server_stats_t	stats;				// Server statistics

	qboolean		isSecure;			// Is this a secure server?
} server_static_t;

extern	server_static_t	svs;			// persistant server info
extern	server_t		sv;				// local server

// pr_cmds.c
extern	int		g_groupmask;
extern	int		g_groupop;

#define GROUP_OP_AND	0
#define GROUP_OP_NAND	1

struct client_s;

extern	struct client_s	*host_client;	// current client

extern	cvar_t	sv_lan;
extern	cvar_t	sv_lan_rate;

void	SV_DropClient( struct client_s *cl, qboolean crash, char *fmt, ... );

#endif // SERVER_H
