// Cry of Fear pixel buffer (no build records the name of this file)

#include <string>
#include <list>

#include "hud.h"
#include "cl_util.h"
#include "glmanager.h"
#include "guess_TextureManager.h"
#include "guess_PBuffer.h"

//=========================================================
// Declarations this file takes from wglext.h
//=========================================================
#define WGL_DRAW_TO_PBUFFER_ARB			0x202D
#define WGL_PBUFFER_WIDTH_ARB			0x2034
#define WGL_PBUFFER_HEIGHT_ARB			0x2035
#define WGL_PBUFFER_LOST_ARB			0x2036
#define WGL_SUPPORT_OPENGL_ARB			0x2010
#define WGL_DOUBLE_BUFFER_ARB			0x2011
#define WGL_RED_BITS_ARB				0x2015
#define WGL_GREEN_BITS_ARB				0x2017
#define WGL_BLUE_BITS_ARB				0x2019
#define WGL_ALPHA_BITS_ARB				0x201B
#define WGL_DEPTH_BITS_ARB				0x2022
#define WGL_BIND_TO_TEXTURE_RGBA_ARB	0x2071
#define WGL_TEXTURE_FORMAT_ARB			0x2072
#define WGL_TEXTURE_TARGET_ARB			0x2073
#define WGL_TEXTURE_RGBA_ARB			0x2076
#define WGL_TEXTURE_2D_ARB				0x207A
#define WGL_FRONT_LEFT_ARB				0x2083

typedef const char *( WINAPI *PFNWGLGETEXTENSIONSSTRINGARBPROC )( HDC hdc );
typedef HPBUFFERARB ( WINAPI *PFNWGLCREATEPBUFFERARBPROC )( HDC hDC, int iPixelFormat, int iWidth, int iHeight, const int *piAttribList );
typedef HDC ( WINAPI *PFNWGLGETPBUFFERDCARBPROC )( HPBUFFERARB hPbuffer );
typedef int ( WINAPI *PFNWGLRELEASEPBUFFERDCARBPROC )( HPBUFFERARB hPbuffer, HDC hDC );
typedef BOOL ( WINAPI *PFNWGLDESTROYPBUFFERARBPROC )( HPBUFFERARB hPbuffer );
typedef BOOL ( WINAPI *PFNWGLQUERYPBUFFERARBPROC )( HPBUFFERARB hPbuffer, int iAttribute, int *piValue );
typedef BOOL ( WINAPI *PFNWGLGETPIXELFORMATATTRIBIVARBPROC )( HDC hdc, int iPixelFormat, int iLayerPlane, UINT nAttributes, const int *piAttributes, int *piValues );
typedef BOOL ( WINAPI *PFNWGLGETPIXELFORMATATTRIBFVARBPROC )( HDC hdc, int iPixelFormat, int iLayerPlane, UINT nAttributes, const int *piAttributes, FLOAT *pfValues );
typedef BOOL ( WINAPI *PFNWGLCHOOSEPIXELFORMATARBPROC )( HDC hdc, const int *piAttribIList, const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats );
typedef BOOL ( WINAPI *PFNWGLBINDTEXIMAGEARBPROC )( HPBUFFERARB hPbuffer, int iBuffer );
typedef BOOL ( WINAPI *PFNWGLRELEASETEXIMAGEARBPROC )( HPBUFFERARB hPbuffer, int iBuffer );

PFNWGLGETEXTENSIONSSTRINGARBPROC	wglGetExtensionsStringARB = NULL;
PFNWGLCREATEPBUFFERARBPROC			wglCreatePbufferARB = NULL;
PFNWGLGETPBUFFERDCARBPROC			wglGetPbufferDCARB = NULL;
PFNWGLRELEASEPBUFFERDCARBPROC		wglReleasePbufferDCARB = NULL;
PFNWGLDESTROYPBUFFERARBPROC			wglDestroyPbufferARB = NULL;
PFNWGLQUERYPBUFFERARBPROC			wglQueryPbufferARB = NULL;
PFNWGLGETPIXELFORMATATTRIBIVARBPROC	wglGetPixelFormatAttribivARB = NULL;
PFNWGLGETPIXELFORMATATTRIBFVARBPROC	wglGetPixelFormatAttribfvARB = NULL;
PFNWGLCHOOSEPIXELFORMATARBPROC		wglChoosePixelFormatARB = NULL;
PFNWGLBINDTEXIMAGEARBPROC			wglBindTexImageARB = NULL;
PFNWGLRELEASETEXIMAGEARBPROC		wglReleaseTexImageARB = NULL;

// the string is not stringized: the message prints "#funcname()" (original bug)
#define guess_INIT_ENTRY_POINT( funcname, type )	\
	funcname = (type)wglGetProcAddress( #funcname );	\
	if ( !funcname )	\
		gEngfuncs.Con_Printf( "#funcname() not Initialized!\n" );

guess_CPBuffer guess_g_PBuffer;

guess_CPBuffer::guess_CPBuffer()
{
	guess_m_bValid = true;
}

guess_CPBuffer::~guess_CPBuffer()
{
}

void guess_CPBuffer::guess_BindTexture( void )
{
	if ( !wglBindTexImageARB( guess_m_hPBuffer, WGL_FRONT_LEFT_ARB ) )
		gEngfuncs.Con_Printf( "Couldnt bind pbuffer to render texture\n" );
}

void guess_CPBuffer::guess_Create( void )
{
	guess_m_hWindowDC = wglGetCurrentDC();
	guess_m_hWindowRC = wglGetCurrentContext();

	int attribs[] =
	{
		WGL_SUPPORT_OPENGL_ARB, TRUE,
		WGL_DRAW_TO_PBUFFER_ARB, TRUE,
		WGL_BIND_TO_TEXTURE_RGBA_ARB, TRUE,
		WGL_RED_BITS_ARB, 8,
		WGL_GREEN_BITS_ARB, 8,
		WGL_BLUE_BITS_ARB, 8,
		WGL_ALPHA_BITS_ARB, 8,
		WGL_DEPTH_BITS_ARB, 16,
		WGL_DOUBLE_BUFFER_ARB, FALSE,
		0
	};
	int format;
	UINT count = 0;

	wglChoosePixelFormatARB( guess_m_hWindowDC, attribs, NULL, 1, &format, &count );

	if ( !count )
	{
		gEngfuncs.Con_Printf( "Problem Finding Pixel Format\n" );
		guess_m_bValid = false;
	}

	int pbattribs[] =
	{
		WGL_TEXTURE_FORMAT_ARB, WGL_TEXTURE_RGBA_ARB,
		WGL_TEXTURE_TARGET_ARB, WGL_TEXTURE_2D_ARB,
		0
	};

	guess_m_iWidth = 1;
	while ( guess_m_iWidth < ScreenWidth )
		guess_m_iWidth *= 2;
	guess_m_iHeight = guess_m_iWidth;

	guess_g_TextureManager.guess_CreateTexture( guess_m_iWidth, guess_m_iHeight, &guess_m_iTexture, GL_TEXTURE_2D, GL_RGBA );

	guess_m_hPBuffer = wglCreatePbufferARB( guess_m_hWindowDC, format, guess_m_iWidth, guess_m_iHeight, pbattribs );
	guess_m_hDC = wglGetPbufferDCARB( guess_m_hPBuffer );
	guess_m_hRC = wglCreateContext( guess_m_hDC );

	if ( !guess_m_hPBuffer )
	{
		gEngfuncs.Con_Printf( "Problem Creating PBuffer\n" );
		guess_m_bValid = false;
	}

	int width, height;
	wglQueryPbufferARB( guess_m_hPBuffer, WGL_PBUFFER_WIDTH_ARB, &width );
	wglQueryPbufferARB( guess_m_hPBuffer, WGL_PBUFFER_HEIGHT_ARB, &height );

	if ( height != guess_m_iHeight || width != guess_m_iWidth )
	{
		gEngfuncs.Con_Printf( "Pbuffer Size doesnt match\n" );
		guess_m_bValid = false;
	}

	if ( !wglMakeCurrent( guess_m_hDC, guess_m_hRC ) )
	{
		gEngfuncs.Con_Printf( "Couldnt make Pbuffer Current\n" );
		guess_m_bValid = false;
	}

	guess_SwitchToWindow();
}

void guess_CPBuffer::guess_MakeCurrent( void )
{
	int lost = 0;

	wglQueryPbufferARB( guess_m_hPBuffer, WGL_PBUFFER_LOST_ARB, &lost );

	if ( lost )
		gEngfuncs.Con_Printf( "Pbuffer was Lost\n" );

	if ( !wglMakeCurrent( guess_m_hDC, guess_m_hRC ) )
		gEngfuncs.Con_Printf( "Couldnt make Pbuffer Current\n" );
}

void guess_CPBuffer::guess_Prepare( void )
{
	wglGetExtensionsStringARB = (PFNWGLGETEXTENSIONSSTRINGARBPROC)wglGetProcAddress( "wglGetExtensionsStringARB" );

	if ( !wglGetExtensionsStringARB )
		return;

	const char *extensions = wglGetExtensionsStringARB( wglGetCurrentDC() );

	if ( !guess_IsExtensionSupported( "WGL_ARB_pixel_format", extensions ) || !guess_IsExtensionSupported( "WGL_ARB_pbuffer", extensions ) )
	{
		guess_m_bValid = false;
		return;
	}

	if ( !guess_m_bValid )
		return;

	guess_INIT_ENTRY_POINT( wglCreatePbufferARB, PFNWGLCREATEPBUFFERARBPROC );
	guess_INIT_ENTRY_POINT( wglGetPbufferDCARB, PFNWGLGETPBUFFERDCARBPROC );
	guess_INIT_ENTRY_POINT( wglReleasePbufferDCARB, PFNWGLRELEASEPBUFFERDCARBPROC );
	guess_INIT_ENTRY_POINT( wglDestroyPbufferARB, PFNWGLDESTROYPBUFFERARBPROC );
	guess_INIT_ENTRY_POINT( wglQueryPbufferARB, PFNWGLQUERYPBUFFERARBPROC );
	guess_INIT_ENTRY_POINT( wglGetPixelFormatAttribivARB, PFNWGLGETPIXELFORMATATTRIBIVARBPROC );
	guess_INIT_ENTRY_POINT( wglGetPixelFormatAttribfvARB, PFNWGLGETPIXELFORMATATTRIBFVARBPROC );
	guess_INIT_ENTRY_POINT( wglChoosePixelFormatARB, PFNWGLCHOOSEPIXELFORMATARBPROC );
	guess_INIT_ENTRY_POINT( wglBindTexImageARB, PFNWGLBINDTEXIMAGEARBPROC );
	guess_INIT_ENTRY_POINT( wglReleaseTexImageARB, PFNWGLRELEASETEXIMAGEARBPROC );

	guess_Create();
}

void guess_CPBuffer::guess_ReleaseTexture( void )
{
	if ( !wglReleaseTexImageARB( guess_m_hPBuffer, WGL_FRONT_LEFT_ARB ) )
		gEngfuncs.Con_Printf( "Couldnt release pbuffer to render texture\n" );
}

void guess_CPBuffer::guess_Shutdown( void )
{
}

void guess_CPBuffer::guess_SwitchToWindow( void )
{
	if ( !wglMakeCurrent( guess_m_hWindowDC, guess_m_hWindowRC ) )
		gEngfuncs.Con_Printf( "Couldnt Restore Renderer\n" );
}
