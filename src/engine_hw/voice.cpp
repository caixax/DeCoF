// voice.cpp -- voice communication: recording, codec and playback channels

extern "C"
{
#include "quakedef.h"
#include "cvar.h"
#include "cdll_int.h"
}

extern "C" void ClientDLL_VoiceStatus( int entindex, qboolean bTalking );

extern "C" cvar_t	voice_loopback;

qboolean	g_bLocalPlayerTalkingAck;
float		g_LocalPlayerTalkingTimeout;

extern "C" qboolean Voice_GetLoopback( void )
{
	return !!voice_loopback.value;
}

extern "C" void Voice_LocalPlayerTalkingAck( void )
{
	if ( !g_bLocalPlayerTalkingAck )
	{
		// Tell the client DLL when this changes.
		ClientDLL_VoiceStatus( -2, TRUE );
	}

	g_bLocalPlayerTalkingAck = true;
	g_LocalPlayerTalkingTimeout = 0;
}
