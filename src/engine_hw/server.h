// server.h -- the local server (sv_main.c and friends)

#ifndef SERVER_H
#define SERVER_H

#include "mathlib.h"
#include "const.h"
#include "progs.h"
#include "usercmd.h"
#include "delta_packet.h"
#include "custom.h"
#include "net.h"
#include "consistency.h"
#include "event.h"
#include "userid.h"
#include "info.h"
#include "cvar.h"

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

// client_t, checked against the Windows binary (svs.clients stride 0x5018):
// netchan at +0x20, fakeclient +0x2548, edict +0x4B9C, name +0x4D08,
// resourcesonhand +0x4D34, resourcesneeded +0x4DBC, customdata +0x4E4C.
// usercmd_t is 4 bytes longer than in the Linux build and doubles are 8
// byte aligned, so the members sit further than the Linux offsets.
typedef struct
{
	double			senttime;
	float			ping_time;
	clientdata_t	clientdata;
	weapon_data_t	weapondata[64];
	packet_entities_t	entities;
} client_frame_t;

typedef struct client_s
{
	qboolean		active;				// false = client is free
	qboolean		spawned;			// false = don't send datagrams
	qboolean		fully_connected;	// true = client has fully connected, set after sendents command is received
	qboolean		connected;			// On server, but not spawned.
	qboolean		uploading;			// true = client uploading custom resources
	qboolean		hasusrmsgs;			// Whether this client has received the list of user messages
	qboolean		has_force_unmodified;	// true = mp_consistency is set and at least one file is forced to be consistent

	netchan_t		netchan;			// The client's net connection.

	int				chokecount;			// amount of choke since last client message
	int				delta_sequence;		// -1 = no compression.  This is where the server is creating the
										// compressed info from.

	qboolean		fakeclient;			// Bot
	qboolean		proxy;				// HLTV proxy

	usercmd_t		lastcmd;			// for filling in big drops and partial predictions

	double			connecttime;		// Time client connected
	double			cmdtime;			// Time we received last command
	double			ignorecmdtime;		// Time we should start accepting commands again

	float			latency;			// Average latency
	float			packet_loss;		// Average packet loss

	double			localtime;			// of last message
	double			nextping;			// next time to recalculate ping
	double			svtimebase;			// Server time base for this client

	// the datagram is written to after every frame, but only cleared
	// when it is sent out to the client.  overflow is tolerated.
	sizebuf_t		datagram;
	byte			datagram_buf[MAX_DATAGRAM];

	double			connection_started;	// Or time of disconnect for zombies
	double			next_messagetime;	// Time when we should send next world state update
	double			next_messageinterval;	// Default time to wait for next message

	qboolean		send_message;		// false - only send messages if the client has sent one
										//  and the bandwidth is not choked
	qboolean		skip_message;		// Defer message sending for one frame

	client_frame_t	*frames;			// updates can be deltad from here

	event_state_t	events;				// Per edict events

	edict_t			*edict;				// EDICT_NUM(clientnum+1)
	const edict_t	*pViewEntity;		// View Entity (camera or the client itself)

	int				userid;				// identifying number
	USERID_t		network_userid;

	char			userinfo[MAX_INFO_STRING];	// infostring
	qboolean		sendinfo;			// at end of frame, send info to all
										// this prevents malicious multiple broadcasts
	float			sendinfo_time;		// Time when userinfo was sent

	char			hashedcdkey[64];	// MD5 hash is 32 hex #'s, plus trailing 0
	char			name[32];			// for printing to other people
										// extracted from userinfo
	int				topcolor;
	int				bottomcolor;

	int				entityId;

	resource_t		resourcesonhand;	// Downloaded
	resource_t		resourcesneeded;	// Need to download
	FileHandle_t	upload;				// Handle for file upload
	qboolean		uploaddoneregistering;	// If client files have finished uploading
	customization_t	customdata;			// Head of custom (client) data list

	int				crcValue;			// For consistency checking

	int				lw;					// If user is predicting weapons locally
	int				lc;					//  If user is lag compensating

	char			physinfo[MAX_PHYSINFO_STRING];	// Physics info string

	qboolean		m_bLoopback;		// Does this client want to hear his own voice?
	uint32			m_VoiceStreams[2];	// Which other clients does this guy's voice stream go to?

	double			m_lastvoicetime;

	int				m_sendrescount;		// Count of times we've sent the resources to the client
	qboolean		m_bSentNewResponse;
} client_t;

// sv_main.c
extern	cvar_t	sv_allow_upload;
extern	cvar_t	sv_max_upload;

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

// sv_main.c
void	SV_PropagateCustomizations( void );
void	SV_BroadcastPrintf( char *fmt, ... );
void	SV_BroadcastCommand( char *fmt, ... );

extern	int		num_servers;
extern	qboolean	allow_cheats;

// sv_steam3.cpp
const char	*Steam_GetCommunityName( void );
int		Steam_GSInitiateGameConnection( void *pData, int cbMaxData, unsigned __int64 steamID, unsigned int unIPServer, unsigned short usPortServer, qboolean bSecure );
void	Steam_GSTerminateGameConnection( unsigned int unIPServer, unsigned short usPortServer );
void	Steam_SetCVar( const char *pchKey, const char *pchValue );

#endif // SERVER_H
