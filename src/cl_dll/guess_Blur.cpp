// Cry of Fear motion blur through a rectangle texture
// (no build records the name of this file; its position in the link
// order is between battery.cpp and cdll_int.cpp. Nothing calls this code
// in 1.6a: CHud::DrawBlurTest in hud_redraw.cpp draws the blur instead)

#include "hud.h"
#include "cl_util.h"
#include "const.h"
#include "com_model.h"
#include "glmanager.h"
#include <mmsystem.h>

extern cvar_t *cv_blurtest;

class guess_CBlurTexture
{
public:
	guess_CBlurTexture();

	void guess_Init( int width, int height );
	void guess_BindTexture( int width, int height );
	void guess_DrawQuad( int width, int height, int of );
	void guess_Draw( int width, int height );

	unsigned int	guess_m_uiTexture;
	float			guess_m_flAlpha;
	float			guess_m_flRed;
	float			guess_m_flGreen;
	float			guess_m_flBlue;
};

class guess_CBlur
{
public:
	void guess_InitScreen( void );
	void guess_DrawBlur( void );
	bool guess_AnimateNextFrame( int desiredFrameRate );

	int					guess_m_iActive;
	guess_CBlurTexture	guess_m_Output;
};

guess_CBlurTexture::guess_CBlurTexture()
{
}

void guess_CBlurTexture::guess_Init( int width, int height )
{
	// a load of blank pixels to create the texture with
	unsigned char *pBlankTex = new unsigned char[width * height * 3];
	memset( pBlankTex, 0, width * height * 3 );

	glGenTextures( 1, &guess_m_uiTexture );
	glBindTexture( GL_TEXTURE_RECTANGLE_NV, guess_m_uiTexture );
	glTexParameteri( GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexImage2D( GL_TEXTURE_RECTANGLE_NV, 0, GL_RGBA8, width, height, 0, GL_RGBA8, GL_UNSIGNED_BYTE, NULL );

	delete[] pBlankTex;
}

void guess_CBlurTexture::guess_BindTexture( int width, int height )
{
	glBindTexture( GL_TEXTURE_RECTANGLE_NV, guess_m_uiTexture );
	glCopyTexImage2D( GL_TEXTURE_RECTANGLE_NV, 0, GL_RGBA8, 0, 0, ScreenWidth, ScreenHeight, 0 );
}

void guess_CBlurTexture::guess_DrawQuad( int width, int height, int of )
{
	glTexCoord2f( -of, -of );
	glVertex3f( 0, 1, -1 );
	glTexCoord2f( -of, height + of );
	glVertex3f( 0, 0, -1 );
	glTexCoord2f( width + of, height + of );
	glVertex3f( 1, 0, -1 );
	glTexCoord2f( width + of, -of );
	glVertex3f( 1, 1, -1 );
}

void guess_CBlurTexture::guess_Draw( int width, int height )
{
	glEnable( GL_TEXTURE_RECTANGLE_NV );
	glColor3f( 1, 1, 1 );
	glDisable( GL_DEPTH_TEST );

	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glLoadIdentity();
	glOrtho( 0, 1, 1, 0, 0.1, 100 );

	glBindTexture( GL_TEXTURE_RECTANGLE_NV, guess_m_uiTexture );
	glColor4f( guess_m_flRed, guess_m_flGreen, guess_m_flBlue, guess_m_flAlpha );

	if ( cv_blurtest->value )
	{
		glBegin( GL_QUADS );
		guess_DrawQuad( ScreenWidth, ScreenHeight, 0 );
		glEnd();
	}

	glMatrixMode( GL_PROJECTION );
	glPopMatrix();
	glMatrixMode( GL_MODELVIEW );
	glPopMatrix();

	glDisable( GL_TEXTURE_RECTANGLE_NV );
	glEnable( GL_DEPTH_TEST );
}

void guess_CBlur::guess_InitScreen( void )
{
	guess_m_iActive = 1;
	guess_m_Output.guess_Init( 320, 240 );
	guess_m_Output.guess_m_flRed = 1.0;
	guess_m_Output.guess_m_flGreen = 1.0;
	guess_m_Output.guess_m_flBlue = 1.0;
}

void guess_CBlur::guess_DrawBlur( void )
{
	if ( cv_blurtest->value )
	{
		glBlendFunc( GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA );
		glEnable( GL_BLEND );

		guess_m_Output.guess_m_flAlpha = 0.4;
		guess_m_Output.guess_m_flRed = 1.0;
		guess_m_Output.guess_m_flGreen = 1.0;
		guess_m_Output.guess_m_flBlue = 1.0;
		guess_m_Output.guess_Draw( ScreenWidth, ScreenHeight );
		guess_m_Output.guess_BindTexture( ScreenWidth, ScreenHeight );

		glDisable( GL_BLEND );
	}
}

bool guess_CBlur::guess_AnimateNextFrame( int desiredFrameRate )
{
	static float lastTime = 0.0f;
	float elapsedTime = 0.0;

	float currentTime = timeGetTime() * 0.001f;
	elapsedTime = currentTime - lastTime;

	if ( elapsedTime > ( 1.0f / desiredFrameRate ) )
	{
		lastTime = currentTime;
		return true;
	}

	return false;
}
