// client.h -- the client state (cl_main.c and friends)
//
// Include after quakedef.h and mathlib.h.

#ifndef CLIENT_H
#define CLIENT_H

#include "const.h"
#include "info.h"
#include "usercmd.h"
#include "entity_state.h"
#include "progs.h"
#include "cl_entity.h"
#include "dlight.h"
#include "custom.h"
#include "screenfade.h"
#include "net.h"
#include "event.h"
#include "consistency.h"
#include "delta_packet.h"

#define	MAX_DEMOS			32

#define MAX_CLIENTS			32

#define MAX_MODELS			768		// this engine build: cl.model_precache has 768 entries
#define MAX_SOUNDS			768		// and so has cl.sound_precache
#define MAX_RESOURCE_LIST	1792	// MAX_MODELS + MAX_SOUNDS + MAX_EVENTS

#define MAX_DLIGHTS			32
#define MAX_ELIGHTS			64
#define MAX_LIGHTSTYLES		64
#define MAX_STYLESTRING		64
#define MAX_EFRAGS			640
#define MAX_VISEDICTS		512
#define MAX_STATIC_ENTITIES	32
#define MAX_BASELINES		64

#define MAX_CL_STATS		32

#define	MULTIPLAYER_BACKUP	64		// how many data slots to use when in multiplayer (must be power of 2)

#define	SIGNONS				2		// signon messages to receive before connected

typedef struct cmd_s
{
	usercmd_t	cmd;
	float		senttime;		// Time sent
	float		receivedtime;	// Time ack'd
	float		frame_lerp;		// lerp value when packet was sent
	qboolean	processedfuncs;	// whether the frame's functions have been processed
	qboolean	heldback;		// held back by the client
	int			sendsize;		// size of the packet
} cmd_t;

typedef struct frame_s
{
	double				receivedtime;	// time message was received, or -1
	double				latency;
	qboolean			invalid;		// true if the packet_entities delta was invalid
	qboolean			choked;

	entity_state_t		playerstate[MAX_CLIENTS];

	double				time;
	clientdata_t		clientdata;
	weapon_data_t		weapondata[64];
	packet_entities_t	packet_entities;

	unsigned short		clientbytes;
	unsigned short		playerinfobytes;
	unsigned short		packetentitybytes;
	unsigned short		tentitybytes;
	unsigned short		soundbytes;
	unsigned short		eventbytes;
	unsigned short		usrbytes;
	unsigned short		voicebytes;
	unsigned short		msgbytes;
} frame_t;

typedef struct
{
	int		length;
	char	map[MAX_STYLESTRING];
} lightstyle_t;

typedef struct player_info_s
{
	// User id on server
	int				userid;

	// User info string
	char			userinfo[MAX_INFO_STRING];

	// Name
	char			name[32];

	// Spectator or not, unused
	int				spectator;

	int				ping;
	int				packet_loss;

	// skin information
	char			model[64];
	int				topcolor;
	int				bottomcolor;

	// last frame rendered
	int				renderframe;

	// Gait frame estimation
	int				gaitsequence;
	float			gaitframe;
	float			gaityaw;
	vec3_t			prevgaitorigin;

	customization_t	customdata;

	char			hashedcdkey[16];
	unsigned __int64	m_nSteamID;
} player_info_t;

typedef enum
{
	ca_dedicated,		// This is a dedicated server, client code is inactive
	ca_disconnected,	// full screen console with no connection
	ca_connecting,		// netchan_t established, waiting for svc_serverdata
	ca_connected,		// processing data lists, donwloading, etc
	ca_uninitialized,
	ca_active			// everything is in, so frames can be rendered
} cactive_t;

typedef struct
{
	qboolean	bUsed;
	float		fTime;
	int			nBytesRemaining;
} downloadtime_t;

typedef struct
{
	qboolean		doneregistering;
	int				percent;
	qboolean		downloadrequested;
	downloadtime_t	rgStats[8];
	int				nCurStat;
	int				nTotalSize;
	int				nTotalToTransfer;
	int				nRemainingToTransfer;
	float			fLastStatusUpdate;
	qboolean		custom;
} incomingtransfer_t;

typedef struct
{
	int		nStartPercent;
	int		nClientSoundFadePercent;
	double	soundFadeStartTime;
	int		soundFadeOutTime;
	int		soundFadeHoldTime;
	int		soundFadeInTime;
} soundfade_t;

// the client_static_t structure is persistant through an arbitrary number
// of server connections
//
// Checked against the Windows binary (cls at 0x02DCF280: netchan 0x02DCF288,
// netchan.message 0x02DCF2E4, datagram 0x02DCF7A8, connect_time 0x02DD2760,
// userinfo 0x02DD30E4, demoplayback 0x02DD33F4, dl.nCurStat 0x02DD3598,
// packet_loss 0x02DD35C0, soundfade 0x02DD35D8, md5_clientdll 0x02DD36F8,
// connect_stream 0x02DD371C, passive 0x02DD3730, spectator 0x02DD3734,
// GameServerSteamID 0x02DD3748, build_num 0x02DD3750). Between dl and
// packet_loss this build has 16 bytes the Linux build lacks: the state of
// the Steam resource wait of CL_MAIN.C (CL_WaitForResources_guess starts it
// with the file system's WaitForResources, CL_CheckWaitForResources_guess
// polls it, CL_WaitForResourcesDone_guess ends it, CL_Disconnect cancels it).
typedef struct client_static_s
{
	cactive_t			state;

	// network stuff
	netchan_t			netchan;

	// private userinfo for sending to masterless servers
	sizebuf_t			datagram;			// unreliable stuff gets sent in this
	byte				datagram_buf[MAX_DATAGRAM];

	double				connect_time;		// for connection retransmits
	int					connect_retry;

	int					challenge;

	byte				authprotocol;

	int					userid;

	char				trueaddress[32];

	float				slist_time;

	// connection information
	int					signon;				// 0 to SIGNONS

	char				servername[MAX_PATH];	// name of server from original connect
	char				mapstring[64];

	char				spawnparms[2048];

	// private userinfo for sending to masterless servers
	char				userinfo[MAX_INFO_STRING];

	float				nextcmdtime;		// When can we send the next command packet?
	int					lastoutgoingcommand;	// Sequence number of last outgoing command

	// demo loop control
	int					demonum;			// -1 = don't play demos
	char				demos[MAX_DEMOS][16];	// when not playing

	// demo recording info must be here, because record is started before
	// entering a map (and clearing client_state_t)
	qboolean			demorecording;
	qboolean			demoplayback;
	qboolean			timedemo;

	float				demostarttime;
	int					demostartframe;

	int					forcetrack;			// -1 = use normal cd track

	FileHandle_t		demofile;
	FileHandle_t		demoheader;			// For saving startup data to start playing a demo midstream.
	qboolean			demowaiting;		// don't record until a non-delta message is sent
	qboolean			demoappending;		// Appending to a previous recording.
	char				demofilename[MAX_PATH];
	int					demoframecount;

	int					td_lastframe;		// to meter out one message a frame
	int					td_startframe;		// host_framecount at start
	float				td_starttime;		// realtime at second frame of timedemo

	incomingtransfer_t	dl;					// Status of resource download

	qboolean			waitingforresources_guess;	// a Steam resource wait is running
	int					waitforresources_guess;		// its handle (WaitForResourcesHandle_t)
	double				waitforresourcestime_guess;	// Sys_FloatTime() when it started

	float				packet_loss;
	double				packet_loss_recalc_time;

	int					playerbits;			// player bits for the current player

	soundfade_t			soundfade;

	char				physinfo[MAX_PHYSINFO_STRING];	// Physics info string

	unsigned char		md5_clientdll[16];

	// Location of the last server we've connected to
	netadr_t			game_stream;
	// Location of the server we are connecting to
	netadr_t			connect_stream;

	qboolean			passive;
	qboolean			spectator;
	qboolean			director;
	qboolean			fSecureClient;
	qboolean			isVAC2Secure;

	unsigned __int64	GameServerSteamID;

	int					build_num;
} client_static_t;

extern	client_static_t	cls;

// the client_state_t structure is wiped completely at every
// server signon
//
// Checked against the Windows binary (cl at 0x02DD5A80: resourcesonhand
// 0x02DD5A84, num_resources 0x02E11394, parsecount 0x02E115A8, viewangles
// 0x02E1166C, light_level 0x02E11A00, intermission 0x02E11A08, mtime
// 0x02E11A10, time 0x02E11A20, frames 0x02E11A30, commands 0x02F1CA30,
// predicted_frames 0x02F1DE30, playernum 0x02F82B34, model_precache
// 0x02F83B38, sound_precache 0x02F8473C, consistency_list 0x02F8533C,
// maxclients 0x02F8AB6C, viewent 0x02F8AB88, players 0x02F8B770,
// instanced_baseline 0x02F90170, events 0x02F95678, downloadUrl 0x02F96C78).
// Against the Linux layout: usercmd_t and clientdata_t have 4 more bytes,
// local_state_t keeps 64 weapons, the resource and precache tables are
// larger, and 4 bytes follow light_level.
typedef struct
{
	int				max_edicts;

	resource_t		resourcesonhand;
	resource_t		resourcesneeded;
	resource_t		resourcelist[MAX_RESOURCE_LIST];
	int				num_resources;

	qboolean		need_force_consistency_response;

	char			serverinfo[MAX_SERVERINFO_STRING];

	int				servercount;		// server identification for prespawns

	int				validsequence;		// this is the sequence number of the last good
										// world snapshot/update we got. If this is 0, we can't
										// render a frame yet

	int				parsecount;			// server message counter
	int				parsecountmod;		// modulo with network window

	int				stats[MAX_CL_STATS];	// health, etc
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
	int				cof_light_extra_guess;	// CoF engine: 4 more bytes here, no code reads them

	int				intermission;		// don't change view angle, full screen, et

	double			mtime[2];			// the timestamp of the last two messages
	double			time;				// clients view of time, time should be between
										// servertime and oldservertime to generate
										// a lerp point for other data
	double			oldtime;			// previous cl.time, time-oldtime is used
										// to decay light values and smooth step ups

	frame_t			frames[MULTIPLAYER_BACKUP];
	cmd_t			commands[MULTIPLAYER_BACKUP];
	local_state_t	predicted_frames[MULTIPLAYER_BACKUP];
	int				delta_sequence;

	int				playernum;
	event_t			event_precache[MAX_EVENTS];
	struct model_s	*model_precache[MAX_MODELS];
	int				model_precache_count;
	struct sfx_s	*sound_precache[MAX_SOUNDS];
	consistency_t	consistency_list[MAX_CONSISTENCY_LIST];
	int				num_consistency;

	int				highentity;

	char			levelname[40];		// for display on solo scoreboard
	int				maxclients;
	int				gametype;
	int				viewentity;

	// refresh related state
	struct model_s	*worldmodel;		// cl_entitites[0].model
	efrag_t			*free_efrags;
	int				num_entities;		// held in cl_entities array
	int				num_statics;		// held in cl_staticentities array
	cl_entity_t		viewent;			// the gun model

	int				cdtrack;			// cd audio
	int				looptrack;

	CRC32_t			serverCRC;			// To determine if client is playing hacked .map. (entities lump is skipped)
	unsigned char	clientdllmd5[16];

	float			weaponstarttime;
	int				weaponsequence;

	int				fPrecaching;

	dlight_t		*pLight;

	player_info_t	players[MAX_CLIENTS];
	entity_state_t	instanced_baseline[MAX_BASELINES];
	int				instanced_baseline_number;

	CRC32_t			mapCRC;

	event_state_t	events;

	char			downloadUrl[128];
} client_state_t;

extern	client_state_t	cl;

extern	efrag_t			cl_efrags[MAX_EFRAGS];
extern	cl_entity_t		*cl_entities;
extern	cl_entity_t		cl_static_entities[MAX_STATIC_ENTITIES];
extern	lightstyle_t	cl_lightstyle[MAX_LIGHTSTYLES];
extern	dlight_t		cl_dlights[MAX_DLIGHTS];
extern	dlight_t		cl_elights[MAX_ELIGHTS];

extern	int				cl_numvisedicts;
extern	cl_entity_t		*cl_visedicts[MAX_VISEDICTS];

//
// cl_main.c
//
void	CL_Disconnect_f( void );

#endif // CLIENT_H
