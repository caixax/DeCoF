// Cry of Fear Cg shader manager: the Cg context, the latest vertex and
// fragment profiles and a list of the programs loaded from cgshaders/.

#ifndef GUESS_CGCONTEXT_H
#define GUESS_CGCONTEXT_H

#include "guess_TextureManager.h"

//=========================================================
// Declarations this file takes from the NVIDIA Cg toolkit
// (Cg/cg.h, Cg/cgGL.h)
//=========================================================
typedef struct _CGcontext *CGcontext;
typedef struct _CGprogram *CGprogram;
typedef struct _CGparameter *CGparameter;

typedef enum
{
	CG_PROFILE_START = 6144,
	CG_PROFILE_UNKNOWN,
	CG_PROFILE_VP20,
	CG_PROFILE_FP20,
	CG_PROFILE_VP30,
	CG_PROFILE_FP30,
	CG_PROFILE_ARBVP1,
	CG_PROFILE_FP40,
	CG_PROFILE_ARBFP1 = 7000,
	CG_PROFILE_VP40 = 7001,
	CG_PROFILE_MAX = 7100,
} CGprofile;

typedef enum
{
	CG_UNKNOWN = 4096,
	CG_IN,
	CG_OUT,
	CG_INOUT,
	CG_MIXED,
	CG_VARYING,
	CG_UNIFORM,
	CG_CONSTANT,
	CG_PROGRAM_SOURCE,
	CG_PROGRAM_ENTRY,
	CG_COMPILED_PROGRAM,
	CG_PROGRAM_PROFILE,
	CG_GLOBAL,
	CG_PROGRAM,
	CG_DEFAULT,
	CG_ERROR,
	CG_SOURCE,
	CG_OBJECT,
} CGenum;

typedef int CGerror;

typedef enum
{
	CG_GL_MATRIX_IDENTITY = 0,
	CG_GL_MATRIX_TRANSPOSE = 1,
	CG_GL_MATRIX_INVERSE = 2,
	CG_GL_MATRIX_INVERSE_TRANSPOSE = 3,

	CG_GL_MODELVIEW_MATRIX,
	CG_GL_PROJECTION_MATRIX,
	CG_GL_TEXTURE_MATRIX,
	CG_GL_MODELVIEW_PROJECTION_MATRIX,

	CG_GL_VERTEX,
	CG_GL_FRAGMENT,
} CGGLenum;

extern "C"
{
CGcontext __cdecl cgCreateContext( void );
CGprogram __cdecl cgCreateProgramFromFile( CGcontext ctx, CGenum program_type, const char *program_file, CGprofile profile, const char *entry, const char **args );
CGerror __cdecl cgGetError( void );
const char * __cdecl cgGetErrorString( CGerror error );
CGparameter __cdecl cgGetNamedParameter( CGprogram prog, const char *name );

CGprofile __cdecl cgGLGetLatestProfile( CGGLenum profile_type );
void __cdecl cgGLSetOptimalOptions( CGprofile profile );
void __cdecl cgGLLoadProgram( CGprogram program );
void __cdecl cgGLEnableProfile( CGprofile profile );
void __cdecl cgGLDisableProfile( CGprofile profile );
void __cdecl cgGLBindProgram( CGprogram program );
void __cdecl cgGLSetParameter1f( CGparameter param, float x );
void __cdecl cgGLSetParameter4fv( CGparameter param, const float *v );
void __cdecl cgGLSetStateMatrixParameter( CGparameter param, CGGLenum matrix, CGGLenum transform );
}

// program types
#define guess_CGTYPE_FRAGMENT	1
#define guess_CGTYPE_VERTEX		2

typedef struct guess_cgprogram_s
{
	char		guess_name[32];
	int			guess_type;
	CGprogram	guess_program;
	struct guess_cgprogram_s *guess_next;
	struct guess_cgprogram_s *guess_prev;
} guess_cgprogram_t;

class guess_CCgContext
{
public:
	guess_CCgContext();
	~guess_CCgContext();

	guess_cgprogram_t *guess_AddProgram( const char *name, int type );
	guess_cgprogram_t *guess_FindProgram( const char *name );
	CGprofile guess_GetFragmentProfile( void );
	bool guess_IsProgramValid( const char *path, int type );
	guess_cgprogram_t *guess_RemoveProgram( guess_cgprogram_t *program );
	void guess_Shutdown( void );
	bool guess_Init( void );

	void guess_EnableVertexProfile( void )
	{
		if ( guess_g_TextureManager.guess_IsHardware() )
			cgGLEnableProfile( guess_m_vertexProfile );
	}
	void guess_EnableFragmentProfile( void )
	{
		if ( guess_g_TextureManager.guess_IsHardware() )
			cgGLEnableProfile( guess_m_fragmentProfile );
	}
	void guess_DisableVertexProfile( void )
	{
		if ( guess_g_TextureManager.guess_IsHardware() )
			cgGLDisableProfile( guess_m_vertexProfile );
	}
	void guess_DisableFragmentProfile( void )
	{
		if ( guess_g_TextureManager.guess_IsHardware() )
			cgGLDisableProfile( guess_m_fragmentProfile );
	}
	void guess_BindProgram( guess_cgprogram_t *program )
	{
		if ( guess_g_TextureManager.guess_IsHardware() )
			cgGLBindProgram( program->guess_program );
	}

	CGcontext	guess_m_context;
	CGprofile	guess_m_vertexProfile;
	CGprofile	guess_m_fragmentProfile;
	guess_cgprogram_t *guess_m_pPrograms;
	bool		guess_m_bInitialized;
};

extern guess_CCgContext guess_g_CgContext;

#endif // GUESS_CGCONTEXT_H
