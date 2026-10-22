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

extern int		TEXTURE0_SGIS;
extern int		TEXTURE1_SGIS;
extern int		TEXTURE2_SGIS_guess;
extern GLenum	oldtarget;
extern int		gl_mtexable;

void	GL_SelectTexture( GLenum target );
void	DT_Initialize( void );
void	GL_Config( void );

#define GL_TEXTURE0_SGIS			0x835E
#define GL_TEXTURE1_SGIS			0x835F
#define GL_TEXTURE2_SGIS			0x8360
#define GL_TEXTURE0_ARB				0x84C0
#define GL_TEXTURE1_ARB				0x84C1
#define GL_TEXTURE2_ARB				0x84C2
#define GL_MAX_TEXTURE_UNITS_ARB	0x84E2

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
CheckTextureExtensions
================
*/
void CheckTextureExtensions( void )
{
	qboolean	texture_ext = FALSE;

	if ( vid_d3d.value )
		return;

	if ( Q_strstr( gl_extensions, "GL_EXT_paletted_texture" ) && Q_strstr( gl_extensions, "GL_EXT_shared_texture_palette" ) )
	{
		qglColorTableEXT = (void *)qwglGetProcAddress( "glColorTableEXT" );
		Con_Printf( "Found paletted texture extension.\n" );
	}
	else
	{
		qglColorTableEXT = NULL;
	}

	if ( Q_strstr( gl_extensions, "GL_EXT_texture_object " ) )
		texture_ext = TRUE;

	if ( !texture_ext || COM_CheckParm( "-gl11" ) )
		return;

	qglBindTexture = (void *)qwglGetProcAddress( "glBindTextureEXT" );
	if ( !qglBindTexture )
		Sys_Error( "GetProcAddress for BindTextureEXT failed" );
}

/*
================
CheckArrayExtensions
================
*/
void CheckArrayExtensions( void )
{
	char	*tmp;

	// check for texture extension
	tmp = (char *)qglGetString( GL_EXTENSIONS );
	while ( *tmp )
	{
		if ( Q_strncmp( tmp, "GL_EXT_vertex_array", Q_strlen( "GL_EXT_vertex_array" ) ) == 0 )
		{
			if ( 0 )
			{
				Sys_Error( "GetProcAddress for vertex extension failed" );
				return;
			}
			return;
		}
		tmp++;
	}

	Sys_Error( "Vertex array extension not present" );
}

/*
================
CheckMultiTextureExtensions
================
*/
void CheckMultiTextureExtensions( void )
{
	int		num_guess;

	if ( vid_d3d.value )
		return;

	if ( Q_strstr( gl_extensions, "GL_ARB_multitexture " ) )
	{
		Con_Printf( "ARB Multitexture extensions found.\n" );

		qglMTexCoord2fSGIS = (void *)qwglGetProcAddress( "glMultiTexCoord2fARB" );
		qglSelectTextureSGIS = (void *)qwglGetProcAddress( "glActiveTextureARB" );

		TEXTURE0_SGIS = GL_TEXTURE0_ARB;
		TEXTURE1_SGIS = GL_TEXTURE1_ARB;
		TEXTURE2_SGIS_guess = GL_TEXTURE2_ARB;

		oldtarget = GL_TEXTURE0_ARB;
		GL_SelectTexture( GL_TEXTURE0_ARB );
		gl_mtexable = 2;

		if ( Q_strstr( gl_extensions, "GL_ARB_texture_env_combine " ) || Q_strstr( gl_extensions, "GL_EXT_texture_env_combine " ) )
		{
			qglGetIntegerv( GL_MAX_TEXTURE_UNITS_ARB, &num_guess );

			if ( num_guess > 2 )
			{
				Con_Printf( "%d texture units.  Detail texture supported.\n", num_guess );
				gl_mtexable = num_guess;
				DT_Initialize();
			}
		}
	}
	else if ( Q_strstr( gl_extensions, "GL_SGIS_multitexture " ) )
	{
		Con_Printf( "Multitexture extensions found.\n" );

		qglMTexCoord2fSGIS = (void *)qwglGetProcAddress( "glMTexCoord2fSGIS" );
		qglSelectTextureSGIS = (void *)qwglGetProcAddress( "glSelectTextureSGIS" );

		TEXTURE0_SGIS = GL_TEXTURE0_SGIS;
		TEXTURE1_SGIS = GL_TEXTURE1_SGIS;
		TEXTURE2_SGIS_guess = GL_TEXTURE2_SGIS;

		oldtarget = GL_TEXTURE0_SGIS;
		gl_mtexable = 2;
		GL_SelectTexture( GL_TEXTURE0_SGIS );
	}
	else
	{
		Con_Printf( "NO Multitexture extensions found.\n" );
	}
}

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
===============
GL_Init
===============
*/
void GL_Init( void )
{
	gl_vendor = (const char *)qglGetString( GL_VENDOR );
	Con_DPrintf( "GL_VENDOR: %s\n", gl_vendor );
	gl_renderer = (const char *)qglGetString( GL_RENDERER );
	Con_DPrintf( "GL_RENDERER: %s\n", gl_renderer );
	gl_version = (const char *)qglGetString( GL_VERSION );
	Con_DPrintf( "GL_VERSION: %s\n", gl_version );
	gl_extensions = (const char *)qglGetString( GL_EXTENSIONS );

	CheckTextureExtensions();
	CheckMultiTextureExtensions();
	CheckATINPatchExtensions();

	qglClearColor( 1, 0, 0, 0 );
	qglCullFace( GL_FRONT );
	qglEnable( GL_TEXTURE_2D );

	qglEnable( GL_ALPHA_TEST );
	qglAlphaFunc( GL_NOTEQUAL, 0 );

	qglPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	qglShadeModel( GL_FLAT );

	qglTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	qglTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	qglTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	qglTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

	qglBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	qglTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );

	GL_Config();
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
