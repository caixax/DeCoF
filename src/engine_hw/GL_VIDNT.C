// gl_vidnt.c -- NT GL vid component

#include "quakedef.h"
#include "mathlib.h"
#include "cvar.h"
#include "cmd.h"
#include "GL_MODEL.H"
#include "client.h"
#include "delta.h"
#include "server.h"
#include "vid.h"
#include "render.h"
#include "GLQUAKE.H"
#include "gl_cvars.h"
#include "qgl.h"

extern int		gD3DMode;
extern int		atismoothing;

extern void		( *VID_FlipScreen_guess )( void );

RECT		WindowRect;

const char	*gl_vendor;
const char	*gl_renderer;
const char	*gl_version;
const char	*gl_extensions;

void	( APIENTRY * qglPNTrianglesiATI )( GLenum pname, GLint param );

cvar_t	vid_d3d = { "vid_d3d", "0" };
cvar_t	gl_d3dflip = { "gl_d3dflip", "0" };

/*
================
CheckATINPatchExtensions
================
*/
void CheckATINPatchExtensions( void )
{
	if ( Q_strstr( gl_extensions, "GL_ATI_pn_triangles" ) )
	{
		qglPNTrianglesiATI = (void *)qwglGetProcAddress( "glPNTrianglesiATI" );
		atismoothing = qglPNTrianglesiATI != NULL;

		if ( atismoothing )
			Con_Printf( "ATI N-Patch Extensions found\n" );
	}
	else
	{
		atismoothing = FALSE;
	}
}

/*
=================
GL_BeginRendering

=================
*/
void GL_BeginRendering( int *x, int *y, int *width, int *height )
{
	*x = *y = 0;
	*width = WindowRect.right - WindowRect.left;
	*height = WindowRect.bottom - WindowRect.top;

	vid.width = vid.conwidth = *width;
	vid.height = vid.conheight = *height;

	GLimp_LogNewFrame();
}

/*
=================
GL_EndRendering
=================
*/
void GL_EndRendering( void )
{
	VID_FlipScreen_guess();
}

void VID_ShiftPalette( unsigned char *palette )
{
}

/*
=================
GL_Shutdown
=================
*/
void GL_Shutdown( HWND hwnd, HDC hdc, HGLRC hglrc )
{
	qwglMakeCurrent( NULL, NULL );

	if ( hglrc )
		qwglDeleteContext( hglrc );

	ReleaseDC( hwnd, hdc );
}

/*
=================
VID_Init
=================
*/
int VID_Init( unsigned short *palette )
{
	int		i_guess;

	Cvar_RegisterVariable( &gl_ztrick );
	Cvar_RegisterVariable( &vid_d3d );

	if ( gD3DMode )
		Cvar_Set( "vid_d3d", "1" );

	Cvar_RegisterVariable( &gl_d3dflip );

	i_guess = COM_CheckParm( "-gl_log" );
	if ( i_guess )
		Cmd_AddCommand( "gl_log", GLimp_EnableLogging );

	return 1;
}

/*
=================
VID_WriteBuffer
=================
*/
void VID_WriteBuffer( const char *pFilename )
{
	char			szFileName[MAX_PATH];
	static char		basefilename_guess[MAX_PATH];
	static int		filenumber_guess;

	if ( pFilename )
	{
		Q_strncpy( basefilename_guess, pFilename, sizeof( basefilename_guess ) - 1 );
		szFileName[sizeof( szFileName ) - 1] = 0;
		filenumber_guess = 1;
	}

	_snprintf( szFileName, sizeof( szFileName ) - 1, "%s%05d.bmp", basefilename_guess, filenumber_guess );
	szFileName[sizeof( szFileName ) - 1] = 0;

	VID_TakeSnapshot( szFileName );
	filenumber_guess++;
}
