// sound.h -- client sound i/o functions (snd_dma.c, snd_mix.c)

#ifndef SOUND_H
#define SOUND_H

// needs zone.h (cache_user_t)
typedef struct sfx_s
{
	char			name[64];
	cache_user_t	cache;
	int				servercount;
} sfx_t;

// sound flags of svc_sound
#define SND_VOLUME			(1<<0)		// a byte
#define SND_ATTENUATION		(1<<1)		// a byte
#define SND_LARGE_INDEX		(1<<2)		// a send sound as short
#define SND_PITCH			(1<<3)		// a byte
#define SND_SENTENCE		(1<<4)		// set if sound num is actually a sentence num
#define SND_STOP			(1<<5)		// stop the sound
#define SND_CHANGE_VOL		(1<<6)		// change sound vol
#define SND_CHANGE_PITCH	(1<<7)		// change sound pitch
#define SND_SPAWNING		(1<<8)		// we're spawing, used in some cases for ambients

#define DEFAULT_SOUND_PACKET_VOLUME			255
#define DEFAULT_SOUND_PACKET_ATTENUATION	1.0
#define DEFAULT_SOUND_PACKET_PITCH			100

#define CVOXFILESENTENCEMAX	1536		// max number of sentences in game. NOTE: this must match CVOXFILESENTENCEMAX in dlls\util.h!!!

void	S_StopAllSounds( qboolean clear );
void	S_StopSound( int entnum, int entchannel );
void	S_StartDynamicSound( int entnum, int entchannel, sfx_t *sfx, vec_t *origin, float fvol, float attenuation, int flags, int pitch );
void	S_StartStaticSound( int entnum, int entchannel, sfx_t *sfxin, vec_t *origin, float fvol, float attenuation, int flags, int pitch );

extern	char	*rgpszrawsentence[CVOXFILESENTENCEMAX];
void	S_BeginPrecaching( void );
void	S_EndPrecaching( void );
struct sfx_s	*S_PrecacheSound( char *name );

// snd_mix.c
void	SetCareerAudioState( int paused );

// sequence.cpp
void	Sequence_Init( void );
void	Sequence_OnLevelLoad( const char *mapName );

#endif // SOUND_H
