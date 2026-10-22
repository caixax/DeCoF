// net_api.c -- network services exported to the client dll

#include "quakedef.h"
#include "mathlib.h"
#include "cvar.h"
#include "host.h"
#include "client.h"
#include "net.h"
#include "net_api.h"

extern	cvar_t	rate;
extern	double	g_flLatency;

/*
=================
NET_ParseRulesResponse

=================
*/
void NET_ParseRulesResponse( char *info )
{
	char	*val;
	char	value[1024];
	char	*s;
	int	i;
	char	key[64];
	int	c;

	info[0] = 0;

	MSG_BeginReading();
	MSG_ReadLong();
	MSG_ReadByte();
	c = MSG_ReadShort();

	Info_SetValueForKey( info, "rules", va( "%i", c ), 2048 );

	for ( i = 0; i < c; i++ )
	{
		s = MSG_ReadString();
		if ( !s || !s[0] )
			break;

		Q_strncpy( key, s, sizeof( key ) - 1 );
		key[sizeof( key ) - 1] = 0;

		val = MSG_ReadString();
		if ( !val )
			break;

		Q_strncpy( value, val, sizeof( value ) - 1 );
		value[sizeof( value ) - 1] = 0;

		Info_SetValueForKey( info, key, value, 2048 );
	}
}

/*
=================
Net_AdrToString

=================
*/
char *Net_AdrToString( struct netadr_s *a )
{
	return NET_AdrToString( *a );
}

/*
=================
Net_StringToAdr

=================
*/
int Net_StringToAdr( char *s, struct netadr_s *a )
{
	return NET_StringToAdr( s, a );
}

/*
=================
Net_InitNetworking

=================
*/
void Net_InitNetworking( void )
{
	NET_Config( true );
}

/*
=================
Net_CompareAdr

=================
*/
int Net_CompareAdr( struct netadr_s *a, struct netadr_s *b )
{
	return NET_CompareAdr( *a, *b );
}

/*
=================
Net_GetPacketLoss

=================
*/
int Net_GetPacketLoss( void )
{
	int packet_loss = 0;

	if ( cls.state == ca_active )
	{
		packet_loss = (int)cls.packet_loss;
		packet_loss = ( packet_loss < 0 ) ? 0 : packet_loss;
		packet_loss = ( packet_loss > 100 ) ? 100 : packet_loss;
	}

	return packet_loss;
}

/*
=================
Net_Status

=================
*/
void Net_Status( struct net_status_s *status )
{
	int connected;

	if ( !status )
		return;

	connected = ( cls.state != ca_disconnected ) ? 1 : 0;

	status->connected = connected;
	status->local_address = net_local_adr;
	status->remote_address = cls.netchan.remote_address;
	status->packet_loss = connected ? Net_GetPacketLoss() : 0;
	status->latency = connected ? g_flLatency : 0.0;
	status->connection_time = connected ? realtime - cls.netchan.connect_time : 0.0;
	status->rate = rate.value;
}
