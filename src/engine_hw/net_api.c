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
