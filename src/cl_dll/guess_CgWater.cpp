// Cry of Fear Cg water (no build records the name of this file)

#include <string>
#include <list>

#include "hud.h"
#include "cl_util.h"
#include "const.h"
#include "com_model.h"
#include "cl_entity.h"
#include "studio_util.h"
#include "r_studioint.h"
#include "glmanager.h"
#include "gl_texloader.h"
#include "guess_CgWater.h"

extern engine_studio_api_t IEngineStudio;
extern int current_ext_texture_id;
extern GLint guess_g_iWaterOldTexture;

guess_CCgWater guess_g_CgWater;

guess_CCgWater::guess_CCgWater()
{
	guess_m_bEnabled = false;
}

void guess_CCgWater::guess_DrawBlackQuad( void )
{
	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glLoadIdentity();
	glOrtho( 0, 1, 1, 0, 0.1f, 100 );

	glDisable( GL_DEPTH_TEST );
	glCullFace( GL_NONE );
	glDisable( GL_CULL_FACE );
	glDisable( GL_TEXTURE_2D );
	glColor4f( 0, 0, 0, 1 );

	glBegin( GL_QUADS );
	glVertex3f( 0, 1, -1 );
	glVertex3f( 1, 1, -1 );
	glVertex3f( 1, 0, -1 );
	glVertex3f( 0, 0, -1 );
	glEnd();

	glEnable( GL_DEPTH_TEST );
	glEnable( GL_CULL_FACE );
	glEnable( GL_TEXTURE_2D );

	glMatrixMode( GL_MODELVIEW );
	glPopMatrix();
	glMatrixMode( GL_PROJECTION );
	glPopMatrix();
	glMatrixMode( GL_MODELVIEW );
}

void guess_CCgWater::guess_Init( void )
{
	if ( IEngineStudio.IsHardware() != 1 )
		return;

	glActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC)wglGetProcAddress( "glActiveTextureARB" );
	glMultiTexCoord2fARB = (PFNGLMULTITEXCOORD2FARBPROC)wglGetProcAddress( "glMultiTexCoord2fARB" );

	guess_m_bAllowTextureShader = true;
	guess_m_bEnabled = true;
	guess_m_bDrawn = false;

	if ( !guess_g_TextureManager.guess_IsHardware() )
	{
		guess_m_bEnabled = false;
		return;
	}

	if ( !(int)gEngfuncs.pfnGetCvarFloat( "cg_water" ) )
	{
		gEngfuncs.Con_DPrintf( "Disabling CG Effects\n" );
		guess_m_bEnabled = false;
	}

	bool bTextureShader = true;
	if ( !guess_IsExtensionSupported( "GL_NV_texture_shader", strdup( (const char *)glGetString( GL_EXTENSIONS ) ) ) )
		bTextureShader = false;

	if ( !guess_m_bEnabled )
		return;

	guess_m_flTexWidth = 2;
	while ( guess_m_flTexWidth < ScreenWidth )
		guess_m_flTexWidth *= 2;

	guess_m_flTexHeight = 2;
	while ( guess_m_flTexHeight < ScreenHeight )
		guess_m_flTexHeight *= 2;

	guess_m_iSceneTexture = 256;
	guess_g_TextureManager.guess_CreateTexture( guess_m_flTexWidth, guess_m_flTexHeight, &guess_m_iSceneTexture, GL_TEXTURE_2D, GL_RGB );
	guess_g_TextureManager.guess_CreateTexture( guess_m_flTexWidth, guess_m_flTexHeight, &guess_m_iMaskTexture, GL_TEXTURE_2D, GL_RGB );
	guess_g_TextureManager.guess_CreateTexture( 256, 256, &guess_m_iOffsetTexture, GL_TEXTURE_2D, GL_RGB );
	guess_g_TextureManager.guess_CreateTexture( guess_m_flTexWidth, guess_m_flTexHeight, &guess_m_iDistortTexture, GL_TEXTURE_2D, GL_RGB );

	unsigned char offsets[256][256][2];

	for ( int t = 0; t < 256; t++ )
	{
		for ( int s = 0; s < 256; s++ )
		{
			offsets[t][s][0] = s;
			offsets[t][s][1] = t;
		}
	}

	glBindTexture( GL_TEXTURE_2D, guess_m_iOffsetTexture );

	if ( bTextureShader )
		glTexImage2D( GL_TEXTURE_2D, 0, GL_DSDT8_NV, 256, 256, 0, GL_DSDT_NV, GL_UNSIGNED_BYTE, offsets );
	else
		glTexImage2D( GL_TEXTURE_2D, 0, GL_LUMINANCE, 256, 256, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, offsets );

	if ( !guess_m_iNoiseTexture )
		guess_m_iNoiseTexture = current_ext_texture_id++;
	CreateTexture( "textures/noise.tga", 1, guess_m_iNoiseTexture );

	guess_m_pWaterVP = guess_g_CgContext.guess_AddProgram( "water_vp.cg", guess_CGTYPE_VERTEX );

	if ( guess_m_pWaterVP )
	{
		guess_m_pModelViewProj = cgGetNamedParameter( guess_m_pWaterVP->guess_program, "ModelViewProj" );
		guess_m_pScreen = cgGetNamedParameter( guess_m_pWaterVP->guess_program, "flScreen" );
	}
	else
		gEngfuncs.Con_Printf( "Error on vertex\n" );

	const char *water, *black;

	if ( guess_g_CgContext.guess_GetFragmentProfile() == CG_PROFILE_FP20 )
	{
		black = "fp20_black_fp.cg";
		water = "fp20_water_fp.cg";
	}
	else
	{
		black = "black_fp.cg";
		water = "water_fp.cg";
	}

	guess_m_pWaterFP = guess_g_CgContext.guess_AddProgram( water, guess_CGTYPE_FRAGMENT );
	guess_m_pBlackFP = guess_g_CgContext.guess_AddProgram( black, guess_CGTYPE_FRAGMENT );
	guess_m_pBlackVP = guess_g_CgContext.guess_AddProgram( "black_vp.cg", guess_CGTYPE_VERTEX );
	guess_m_pBlackModelViewProj = cgGetNamedParameter( guess_m_pBlackVP->guess_program, "ModelViewProj" );

	if ( !guess_m_pBlackFP )
		gEngfuncs.Con_Printf( "Error Loading Black Shader\n" );

	if ( guess_m_pWaterFP )
		guess_m_pTexOffset = cgGetNamedParameter( guess_m_pWaterFP->guess_program, "TexOffset" );
	else
		gEngfuncs.Con_Printf( "Error on fragment\n" );

	glActiveTextureARB( GL_TEXTURE0_ARB );
}

void guess_CCgWater::guess_PostDraw( void )
{
	if ( !guess_g_TextureManager.guess_IsHardware() )
		return;

	glActiveTextureARB( GL_TEXTURE1_ARB );
	glDisable( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, guess_m_iSavedTexture1 );

	glActiveTextureARB( GL_TEXTURE2_ARB );
	glDisable( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, guess_m_iSavedTexture2 );

	glActiveTextureARB( GL_TEXTURE3_ARB );
	glDisable( GL_TEXTURE_2D );

	glActiveTextureARB( GL_TEXTURE0_ARB );
}

void guess_CCgWater::guess_PreDraw( void )
{
	if ( !guess_g_TextureManager.guess_IsHardware() )
		return;

	if ( !guess_m_iSavedTexture1 || !guess_m_iSavedTexture2 )
		return;

	glActiveTextureARB( GL_TEXTURE1_ARB );
	glGetIntegerv( GL_TEXTURE_BINDING_2D, &guess_m_iSavedTexture1 );

	glActiveTextureARB( GL_TEXTURE2_ARB );
	glGetIntegerv( GL_TEXTURE_BINDING_2D, &guess_m_iSavedTexture2 );

	glActiveTextureARB( GL_TEXTURE0_ARB );
}

void guess_CCgWater::guess_Render( void )
{
	if ( !guess_m_bEnabled )
		guess_Init();

	guess_m_bDrawn = true;

	if ( !guess_m_bEnabled )
		return;

	glGetIntegerv( GL_TEXTURE_BINDING_2D, &guess_g_iWaterOldTexture );

	glDisable( GL_BLEND );
	glEnable( GL_TEXTURE_2D );

	// the scene
	glBindTexture( GL_TEXTURE_2D, guess_m_iSceneTexture );
	glCopyTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, 0, 0, ScreenWidth, ScreenHeight );
	guess_DrawBlackQuad();

	// the water surfaces with the scrolling noise texture
	glDisable( GL_CULL_FACE );
	glDepthMask( GL_FALSE );
	glBindTexture( GL_TEXTURE_2D, guess_m_iNoiseTexture );
	glColor4f( 1, 1, 1, 1 );

	int i;
	cl_entity_t *ent;

	for ( i = 0; i < 1024; i++ )
	{
		ent = gEngfuncs.GetEntityByIndex( i );

		if ( !ent || ent->curstate.skin != CONTENTS_WATER )
			continue;

		float maxs1 = ent->curstate.maxs[1];
		float mins1 = ent->curstate.mins[1];
		float maxs0 = ent->curstate.maxs[0];
		float mins0 = ent->curstate.mins[0];

		float s = gEngfuncs.pfnGetCvarFloat( "cl_waterspeed_x" );
		s = gEngfuncs.GetClientTime() / s;
		float t = gEngfuncs.pfnGetCvarFloat( "cl_waterspeed_y" );
		t = cos( gEngfuncs.GetClientTime() ) / t;

		float z = ent->curstate.maxs[2] + 1.0f;

		vec3_t v1, v2, v3, v4;
		v1[0] = ent->curstate.mins[0];
		v1[1] = ent->curstate.mins[1];
		v1[2] = z;
		v2[0] = ent->curstate.mins[0];
		v2[1] = ent->curstate.maxs[1];
		v2[2] = z;
		v3[0] = ent->curstate.maxs[0];
		v3[1] = ent->curstate.maxs[1];
		v3[2] = z;
		v4[0] = ent->curstate.maxs[0];
		v4[1] = ent->curstate.mins[1];
		v4[2] = z;

		glBegin( GL_QUADS );

		vec3_t tc1, tc2, tc3, tc4;
		float s0 = s + 0;
		float t0 = t + 0;
		float r0 = t0 + 0;
		tc1[0] = s0;
		tc1[1] = t0;
		tc1[2] = r0;
		glTexCoord2fv( tc1 );
		glVertex3fv( v1 );

		float t1 = ( maxs1 - mins1 ) * ( 1.0f / 128 ) + t;
		tc2[0] = s0;
		tc2[1] = t1;
		tc2[2] = r0;
		glTexCoord2fv( tc2 );
		glVertex3fv( v2 );

		float s1 = ( maxs0 - mins0 ) * ( 1.0f / 128 ) + s;
		tc3[0] = s1;
		tc3[1] = t1;
		tc3[2] = r0;
		glTexCoord2fv( tc3 );
		glVertex3fv( v3 );

		tc4[0] = s1;
		tc4[1] = t0;
		tc4[2] = r0;
		glTexCoord2fv( tc4 );
		glVertex3fv( v4 );

		glEnd();
	}

	// distorted copy
	glEnable( GL_CULL_FACE );
	glDepthMask( GL_TRUE );
	glBindTexture( GL_TEXTURE_2D, guess_m_iDistortTexture );
	glCopyTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, 0, 0, ScreenWidth, ScreenHeight );
	guess_DrawBlackQuad();

	// the water mask
	glDisable( GL_TEXTURE_2D );
	glColor4f( 1, 1, 1, 1 );
	glDisable( GL_CULL_FACE );

	for ( i = 0; i < 1024; i++ )
	{
		ent = gEngfuncs.GetEntityByIndex( i );

		if ( !ent || ent->curstate.skin != CONTENTS_WATER )
			continue;

		vec3_t v1, v2, v3, v4;
		v1[0] = ent->curstate.mins[0];
		v1[1] = ent->curstate.mins[1];
		v1[2] = ent->curstate.maxs[2];
		v2[0] = ent->curstate.mins[0];
		v2[1] = ent->curstate.maxs[1];
		v2[2] = ent->curstate.maxs[2];
		v3[0] = ent->curstate.maxs[0];
		v3[1] = ent->curstate.maxs[1];
		v3[2] = ent->curstate.maxs[2];
		v4[0] = ent->curstate.maxs[0];
		v4[1] = ent->curstate.mins[1];
		v4[2] = ent->curstate.maxs[2];

		glBegin( GL_QUADS );
		glVertex3fv( v1 );
		glVertex3fv( v2 );
		glVertex3fv( v3 );
		glVertex3fv( v4 );
		glEnd();
	}

	glEnable( GL_CULL_FACE );
	glDepthMask( GL_FALSE );
	glBindTexture( GL_TEXTURE_2D, guess_m_iMaskTexture );
	glCopyTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, 0, 0, ScreenWidth, ScreenHeight );
	guess_DrawBlackQuad();

	// put the scene back
	glBindTexture( GL_TEXTURE_2D, guess_m_iSceneTexture );
	guess_RenderQuad();

	guess_g_CgContext.guess_EnableVertexProfile();
	guess_g_CgContext.guess_EnableFragmentProfile();

	if ( guess_m_pWaterVP )
		guess_g_CgContext.guess_BindProgram( guess_m_pWaterVP );
	else
		gEngfuncs.Con_Printf( "Error on Vertex\n" );

	if ( guess_m_pWaterFP )
		guess_g_CgContext.guess_BindProgram( guess_m_pWaterFP );
	else
		gEngfuncs.Con_Printf( "Error on fragment\n" );

	glActiveTextureARB( GL_TEXTURE0_ARB );
	glEnable( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, guess_m_iDistortTexture );

	glActiveTextureARB( GL_TEXTURE1_ARB );
	glEnable( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, guess_m_iOffsetTexture );

	glActiveTextureARB( GL_TEXTURE2_ARB );
	glEnable( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, guess_m_iSceneTexture );

	glActiveTextureARB( GL_TEXTURE3_ARB );
	glEnable( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, guess_m_iMaskTexture );

	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glLoadIdentity();
	glOrtho( 0, 1, 1, 0, 0.1f, 4096 );

	cgGLSetStateMatrixParameter( guess_m_pModelViewProj, CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY );

	float offset[4] = { 0, 0.05f, 0, 0.05f };
	cgGLSetParameter4fv( guess_m_pTexOffset, offset );
	cgGLSetParameter1f( guess_m_pScreen, 0.965f );

	glDisable( GL_DEPTH_TEST );
	glDisable( GL_CULL_FACE );
	glColor4f( 1, 1, 1, 1 );

	glBegin( GL_QUADS );
	glTexCoord2f( 0, 0 );
	glVertex3f( 0, 1, -1 );
	glTexCoord2f( ScreenWidth / guess_m_flTexWidth, 0 );
	glVertex3f( 1, 1, -1 );
	glTexCoord2f( ScreenWidth / guess_m_flTexWidth, ScreenHeight / guess_m_flTexHeight );
	glVertex3f( 1, 0, -1 );
	glTexCoord2f( 0, ScreenHeight / guess_m_flTexHeight );
	glVertex3f( 0, 0, -1 );
	glEnd();

	glMatrixMode( GL_PROJECTION );
	glPopMatrix();
	glMatrixMode( GL_MODELVIEW );
	glPopMatrix();

	glActiveTextureARB( GL_TEXTURE1_ARB );
	glDisable( GL_TEXTURE_2D );
	glActiveTextureARB( GL_TEXTURE2_ARB );
	glDisable( GL_TEXTURE_2D );
	glActiveTextureARB( GL_TEXTURE3_ARB );
	glDisable( GL_TEXTURE_2D );
	glActiveTextureARB( GL_TEXTURE0_ARB );

	guess_g_CgContext.guess_DisableFragmentProfile();
	guess_g_CgContext.guess_DisableVertexProfile();

	glEnable( GL_DEPTH_TEST );
	glEnable( GL_CULL_FACE );
	glDisable( GL_BLEND );
	glBindTexture( GL_TEXTURE_2D, guess_g_iWaterOldTexture );
}

void guess_CCgWater::guess_RenderQuad( void )
{
	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glLoadIdentity();
	glOrtho( 0, 1, 1, 0, 0.1f, 100 );

	glDisable( GL_DEPTH_TEST );
	glCullFace( GL_NONE );
	glDisable( GL_CULL_FACE );
	glColor4f( 1, 1, 1, 1 );

	glBegin( GL_QUADS );
	glTexCoord2f( 0, 0 );
	glVertex3f( 0, 1, -1 );
	glTexCoord2f( ScreenWidth / guess_m_flTexWidth, 0 );
	glVertex3f( 1, 1, -1 );
	glTexCoord2f( ScreenWidth / guess_m_flTexWidth, ScreenHeight / guess_m_flTexHeight );
	glVertex3f( 1, 0, -1 );
	glTexCoord2f( 0, ScreenHeight / guess_m_flTexHeight );
	glVertex3f( 0, 0, -1 );
	glEnd();

	glEnable( GL_DEPTH_TEST );
	glEnable( GL_CULL_FACE );

	glMatrixMode( GL_MODELVIEW );
	glPopMatrix();
	glMatrixMode( GL_PROJECTION );
	glPopMatrix();
	glMatrixMode( GL_MODELVIEW );
}

void guess_CCgWater::guess_SetBlackShader( void )
{
	guess_g_CgContext.guess_EnableFragmentProfile();
	guess_g_CgContext.guess_EnableVertexProfile();

	if ( guess_m_pBlackFP )
		guess_g_CgContext.guess_BindProgram( guess_m_pBlackFP );

	if ( guess_m_pBlackVP )
		guess_g_CgContext.guess_BindProgram( guess_m_pBlackVP );

	cgGLSetStateMatrixParameter( guess_m_pBlackModelViewProj, CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY );
}

void guess_CCgWater::guess_UpdateEntities( void )
{
	if ( !(int)gEngfuncs.pfnGetCvarFloat( "cg_water" ) )
	{
		guess_m_bEnabled = false;
		return;
	}

	if ( !guess_m_bEnabled )
		return;

	for ( int i = 0; i < 1024; i++ )
	{
		cl_entity_t *ent = gEngfuncs.GetEntityByIndex( i );

		if ( !ent || ent->curstate.skin != CONTENTS_WATER )
			continue;

		ent->baseline.effects = EF_NODRAW;
		ent->baseline.renderamt = 0;
		ent->baseline.rendermode = kRenderTransAdd;
		ent->prevstate.effects = EF_NODRAW;
		ent->prevstate.renderamt = 0;
		ent->prevstate.rendermode = kRenderTransAdd;
		ent->curstate.effects = EF_NODRAW;
		ent->curstate.renderamt = 0;
		ent->curstate.rendermode = kRenderTransAdd;
	}
}
