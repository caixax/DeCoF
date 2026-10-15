// net.h -- quake's interface to the networking layer (net_ws.c, net_chan.c)

#ifndef NET_H
#define NET_H

#include "netadr.h"

extern	netadr_t	net_local_adr;
extern	netadr_t	net_from;		// address of who sent the packet
extern	sizebuf_t	net_message;

#endif // NET_H
