// Cry of Fear Cg water: brush entities with the water contents are drawn
// with a refraction shader over a copy of the screen.

#ifndef GUESS_CGWATER_H
#define GUESS_CGWATER_H

#include "guess_CgContext.h"

class guess_CCgWater
{
public:
	guess_CCgWater();

	void guess_DrawBlackQuad( void );
	void guess_EndShaders( void );
	void guess_Init( void );
	void guess_PostDraw( void );
	void guess_PreDraw( void );
	void guess_Render( void );
	void guess_RenderQuad( void );
	void guess_SetBlackShader( void );
	void guess_UpdateEntities( void );

	PFNGLACTIVETEXTUREARBPROC	glActiveTextureARB;
	PFNGLMULTITEXCOORD2FARBPROC	glMultiTexCoord2fARB;
	bool	guess_m_bAllowTextureShader;
	bool	guess_m_bEnabled;
	bool	guess_m_bDrawn;
	float	guess_m_flTexWidth;
	float	guess_m_flTexHeight;
	GLuint	guess_m_iMaskTexture;
	GLuint	guess_m_iDistortTexture;
	GLuint	guess_m_iNoiseTexture;
	GLuint	guess_m_iSceneTexture;
	GLuint	guess_m_iOffsetTexture;
	guess_cgprogram_t *guess_m_pWaterFP;
	guess_cgprogram_t *guess_m_pWaterVP;
	guess_cgprogram_t *guess_m_pBlackFP;
	guess_cgprogram_t *guess_m_pBlackVP;
	CGparameter	guess_m_pBlackModelViewProj;
	CGparameter	guess_m_pModelViewProj;
	CGparameter	guess_m_pTexOffset;
	CGparameter	guess_m_pScreen;
	GLint	guess_m_iSavedTexture1;
	GLint	guess_m_iSavedTexture2;
};

extern guess_CCgWater guess_g_CgWater;

#endif // GUESS_CGWATER_H
