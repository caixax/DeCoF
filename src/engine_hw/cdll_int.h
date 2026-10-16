// cdll_int.h -- the engine's calls into the client dll (cdll_int.c)

#ifndef CDLL_INT_H
#define CDLL_INT_H

void	ClientDLL_ChatInputPosition( int *x, int *y );
int		ClientDLL_Key_Event( int down, int keynum, const char *pszCurrentBinding );

extern	struct modfuncs_s	g_modfuncs;

struct netadr_s;
struct usercmd_s;

void	ClientDLL_HudInit( void );
void	ClientDLL_CreateMove( float frametime, struct usercmd_s *cmd, int active );
int		ClientDLL_ConnectionlessPacket( const struct netadr_s *net_from, const char *args, char *response_buffer, int *response_buffer_size );
int		ClientDLL_GetHullBounds( int hullnumber, float *mins, float *maxs );

// module.cpp
typedef struct module_s
{
	unsigned char	ucMD5Hash[16];	// hash over code
	qboolean		fLoaded;		// true if successfully loaded
} module_t;

extern	module_t	g_module;

void	CloseSecurityModule( void );
void	Module_ProcessExpired_guess( void );	// 0x01D8DBA4: frees the timed out entries of the list at 0x023CA308

#endif // CDLL_INT_H
