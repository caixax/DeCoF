// Cry of Fear Cg shader manager (no build records the name of this file)

#include <string>
#include <list>

#include "hud.h"
#include "cl_util.h"
#include "const.h"
#include "com_model.h"
#include "studio_util.h"
#include "r_studioint.h"
#include "glmanager.h"
#include "guess_CgContext.h"

extern engine_studio_api_t IEngineStudio;

guess_CCgContext guess_g_CgContext;

guess_CCgContext::guess_CCgContext()
{
	guess_m_pPrograms = NULL;
	guess_m_context = NULL;
	guess_m_bInitialized = false;
}

guess_CCgContext::~guess_CCgContext()
{
	guess_Shutdown();
	guess_m_context = NULL;
	guess_m_bInitialized = false;
}

guess_cgprogram_t *guess_CCgContext::guess_AddProgram( const char *name, int type )
{
	if ( IEngineStudio.IsHardware() != 1 )
		return NULL;

	guess_cgprogram_t *program = guess_FindProgram( name );

	if ( !guess_m_context )
		guess_Init();

	if ( !program )
	{
		char path[128];
		sprintf( path, "%s/cgshaders/%s", gEngfuncs.pfnGetGameDirectory(), name );

		if ( guess_IsProgramValid( path, type ) )
		{
			program = new guess_cgprogram_t;
			strcpy( program->guess_name, name );

			if ( type == guess_CGTYPE_FRAGMENT )
				program->guess_program = cgCreateProgramFromFile( guess_m_context, CG_SOURCE, path, guess_m_fragmentProfile, "main", NULL );
			else if ( type == guess_CGTYPE_VERTEX )
				program->guess_program = cgCreateProgramFromFile( guess_m_context, CG_SOURCE, path, guess_m_vertexProfile, "main", NULL );

			program->guess_type = type;
			program->guess_prev = NULL;
			program->guess_next = guess_m_pPrograms;
			if ( guess_m_pPrograms )
				guess_m_pPrograms->guess_prev = program;
			guess_m_pPrograms = program;

			cgGLLoadProgram( program->guess_program );
		}
	}

	return program;
}

guess_cgprogram_t *guess_CCgContext::guess_FindProgram( const char *name )
{
	for ( guess_cgprogram_t *program = guess_m_pPrograms; program; program = program->guess_next )
	{
		if ( !stricmp( name, program->guess_name ) )
			return program;
	}

	return NULL;
}

CGprofile guess_CCgContext::guess_GetFragmentProfile( void )
{
	return guess_m_fragmentProfile;
}

bool guess_CCgContext::guess_IsInitialized( void )
{
	return guess_m_bInitialized;
}

bool guess_CCgContext::guess_IsProgramValid( const char *path, int type )
{
	CGprogram program;

	if ( type == guess_CGTYPE_FRAGMENT )
		program = cgCreateProgramFromFile( guess_m_context, CG_SOURCE, path, guess_m_fragmentProfile, "main", NULL );
	else if ( type == guess_CGTYPE_VERTEX )
		program = cgCreateProgramFromFile( guess_m_context, CG_SOURCE, path, guess_m_vertexProfile, "main", NULL );

	if ( !program )
	{
		gEngfuncs.Con_Printf( "%s\n", cgGetErrorString( cgGetError() ) );
		return false;
	}

	return true;
}

guess_cgprogram_t *guess_CCgContext::guess_RemoveProgram( guess_cgprogram_t *program )
{
	guess_cgprogram_t *next = program->guess_next;

	if ( program == guess_m_pPrograms )
		guess_m_pPrograms = program->guess_next;

	if ( program->guess_next )
		program->guess_next->guess_prev = program->guess_prev;

	if ( program->guess_prev )
		program->guess_prev->guess_next = program->guess_next;

	delete program;

	return next;
}

void guess_CCgContext::guess_Shutdown( void )
{
	guess_cgprogram_t *program = guess_m_pPrograms;

	while ( program )
		program = guess_RemoveProgram( program );

	guess_m_pPrograms = NULL;
	guess_m_bInitialized = false;
}

bool guess_CCgContext::guess_Init( void )
{
	bool result;

	guess_m_context = cgCreateContext();

	if ( guess_m_context )
	{
		guess_m_vertexProfile = cgGLGetLatestProfile( CG_GL_VERTEX );

		if ( guess_m_vertexProfile != CG_PROFILE_UNKNOWN )
		{
			cgGLSetOptimalOptions( guess_m_vertexProfile );
			guess_m_fragmentProfile = cgGLGetLatestProfile( CG_GL_FRAGMENT );

			if ( guess_m_fragmentProfile != CG_PROFILE_UNKNOWN )
			{
				cgGLSetOptimalOptions( guess_m_fragmentProfile );
				result = true;
			}
			else
				result = false;
		}
		else
			result = false;
	}
	else
		result = false;

	guess_m_bInitialized = result;
	return result;
}
