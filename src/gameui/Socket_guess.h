//========= Copyright (c) 1996-2003, Valve LLC, All rights reserved. ============
//
// Purpose: The tracker master server socket. Only its declaration is known in
//			this module: no object of it is ever created here, and the names of
//			its methods are not recorded anywhere.
//
// $NoKeywords: $
//=============================================================================

#ifndef SOCKET_GUESS_H
#define SOCKET_GUESS_H

#ifdef _WIN32
#pragma once
#endif

struct CSocket
{
	virtual ~CSocket();
	virtual void AddMessageHandler_guess( void *handler );
	virtual void RemoveMessageHandler_guess( void *handler );
	virtual int SendMessage_guess( void *to, void *msg );
	virtual int Broadcast_guess( int port, void *msg );
	virtual void *GetSendBuffer_guess();
	virtual void Frame_guess();	// called once per frame to check the socket for data
};

#endif // SOCKET_GUESS_H
