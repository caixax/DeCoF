// Cry of Fear texture manager (no build records the name of this file)

#include <string>
#include <list>

#include "hud.h"
#include "cl_util.h"
#include "const.h"
#include "com_model.h"
#include "triangleapi.h"
#include "studio_util.h"
#include "r_studioint.h"
#include "glmanager.h"
#include "guess_TextureManager.h"
#include <io.h>

//=========================================================
// Declarations this file takes from DevIL (IL/il.h)
//=========================================================
typedef unsigned int	ILenum;
typedef unsigned char	ILboolean;
typedef unsigned int	ILuint;
typedef int				ILint;
typedef unsigned char	ILubyte;

#define IL_IMAGE_WIDTH		0x0DE4
#define IL_IMAGE_HEIGHT		0x0DE5
#define IL_IMAGE_BPP		0x0DE8
#define IL_IMAGE_FORMAT		0x0DEA

extern "C"
{
__declspec( dllimport ) void __stdcall ilInit( void );
__declspec( dllimport ) void __stdcall ilBindImage( ILuint Image );
__declspec( dllimport ) ILboolean __stdcall ilLoadImage( const char *FileName );
__declspec( dllimport ) ILubyte * __stdcall ilGetData( void );
__declspec( dllimport ) ILint __stdcall ilGetInteger( ILenum Mode );
}

extern engine_studio_api_t IEngineStudio;

// the image types DevIL loads
char guess_szImageTypes[][5] =
{
	"bmp", "cut", "dds", "gif", "ico", "cur", "jpg", "jpe", "jpeg", "lbm",
	"lif", "lmp", "mdl", "mng", "pcd", "pcx", "pic", "pix", "png", "pbm",
	"pgm", "ppm", "pnm", "psd", "pxr", "sgi", "bw", "rgb", "rgba", "tga",
	"tif", "tiff", "wal", "xpm"
};

guess_CTextureManager guess_g_TextureManager;

guess_CTextureManager::guess_CTextureManager()
{
}

guess_CTextureManager::~guess_CTextureManager()
{
	guess_Reset();
}

void guess_CTextureManager::guess_CheckMipmapSupport( void )
{
	const char *extensions = strdup( (const char *)glGetString( GL_EXTENSIONS ) );
	float version = atof( (const char *)glGetString( GL_VERSION ) );

	if ( guess_IsExtensionSupported( "GL_SGIS_generate_mipmap", extensions ) || guess_IsExtensionSupported( "GL_EXT_generate_mipmap", extensions ) )
	{
		guess_m_iMipmapMode = 2;
	}
	else if ( version >= 1.4 )
	{
		guess_m_iMipmapMode = 1;
	}
	else
	{
		gEngfuncs.Con_Printf( "MipMap extension not found\n" );
		guess_m_iMipmapMode = 0;
	}
}

void guess_CTextureManager::guess_CreateTexture( int width, int height, GLuint *texture, GLenum target, GLenum format )
{
	unsigned char *data = new unsigned char[width * height * 4];

	*texture = guess_m_iNextTexture++;

	glBindTexture( target, *texture );
	glTexParameteri( target, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( target, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameteri( target, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( target, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glCopyTexImage2D( target, 0, format, 0, 0, width, height, 0 );

	guess_texture_t tex;
	tex.guess_type = guess_TEXTYPE_IMAGE;
	tex.guess_name = guess_m_iNextTexture;
	tex.guess_iFound = 0;
	tex.guess_iFormat = GL_RGBA;
	tex.guess_texture = *texture;
	guess_m_Textures.push_back( tex );

	delete[] data;
}

std::string guess_CTextureManager::guess_GetPath( const std::string &name, int type, int *found )
{
	std::string path;

	if ( type == guess_TEXTYPE_SPRITE )
	{
		path = "sprites/";
		path += name;
		path += ".spr";
	}
	else if ( guess_IsHardware() )
	{
		path = guess_FindTexture( name, found );
	}

	return path;
}

std::string guess_CTextureManager::guess_FindTexture( const std::string &name, int *found )
{
	std::string dir;
	std::string mapname;
	std::string result;
	std::string file;

	dir = gEngfuncs.pfnGetGameDirectory();
	dir += "/textures";

	mapname = gEngfuncs.pfnGetLevelName();
	mapname.erase( 0, 5 );
	mapname.erase( mapname.size() - 4, mapname.size() );

	result = guess_FindNumberedTextures( name, found );

	if ( *found )
		return result;

	std::string base = name;
	base.erase( base.size() - 1, base.size() );

	file = dir + "/" + base + "/" + name;
	result = guess_FindImageFile( file );

	if ( result.size() )
	{
		(*found)++;
		guess_m_iNextTexture++;
		return result;
	}

	for ( int i = 3; i < 6; i++ )
	{
		file = dir + "/" + mapname + "/";

		switch ( i )
		{
		case 0:
			file += "highest/";
			break;
		case 1:
			file += "high/";
			break;
		case 2:
			file += "medium/";
			break;
		case 3:
			file += "low/";
			break;
		case 5:
			file = dir + "/";
			break;
		}

		file += name;
		result = guess_FindImageFile( file );

		if ( result.size() )
		{
			(*found)++;
			guess_m_iNextTexture++;
			return result;
		}
	}

	return "";
}

guess_texture_t *guess_CTextureManager::guess_GetTexture( const std::string &name )
{
	for ( std::list<guess_texture_t>::iterator it = guess_m_Textures.begin(); it != guess_m_Textures.end(); ++it )
	{
		if ( it->guess_name == name )
			return &*it;
	}

	return guess_Load( name );
}

std::string guess_CTextureManager::guess_FindNumberedTextures( const std::string &name, int *count )
{
	std::string path;
	std::string base;
	std::string result;
	char number[8];

	*count = 0;

	path = gEngfuncs.pfnGetGameDirectory();
	path += "/textures/";
	path += name;
	base = path;
	path += "/";

	if ( !_access( path.c_str(), 0 ) )
	{
		path += name;
		base = path;
	}

	path = base;
	sprintf( number, "%i", *count );
	path += number;
	path = guess_FindImageFile( path );

	while ( path.size() )
	{
		guess_texture_t tex;

		(*count)++;
		tex.guess_texture = guess_m_iNextTexture++;
		tex.guess_path = path;
		result = path;
		guess_UploadImage( &tex );

		sprintf( number, "%i", *count );
		path = base;
		path += number;
		path = guess_FindImageFile( path );
	}

	return result;
}

std::string guess_CTextureManager::guess_FindImageFile( const std::string &path )
{
	ilBindImage( guess_m_iNextTexture );

	for ( int i = 0; i < 34; i++ )
	{
		std::string file = path;
		file = file + "." + guess_szImageTypes[i];

		if ( ilLoadImage( file.c_str() ) )
			return file;
	}

	return "";
}

bool guess_CTextureManager::guess_IsHardware( void )
{
	return IEngineStudio.IsHardware() == 1;
}

void guess_CTextureManager::guess_Reset( void )
{
	if ( guess_IsHardware() )
	{
		ilInit();
		guess_CheckMipmapSupport();
	}

	guess_m_iNextTexture = 2000;
	guess_m_Textures.clear();
}

void guess_CTextureManager::guess_UploadImage( guess_texture_t *tex )
{
	ilBindImage( tex->guess_texture );
	glBindTexture( GL_TEXTURE_2D, tex->guess_texture );

	if ( guess_m_iMipmapMode == 2 || guess_m_iMipmapMode == 1 )
	{
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE );
	}
	else
	{
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	}

	ilLoadImage( tex->guess_path.c_str() );
	glTexImage2D( GL_TEXTURE_2D, 0, ilGetInteger( IL_IMAGE_BPP ), ilGetInteger( IL_IMAGE_WIDTH ), ilGetInteger( IL_IMAGE_HEIGHT ), 0, ilGetInteger( IL_IMAGE_FORMAT ), GL_UNSIGNED_BYTE, ilGetData() );
}

guess_texture_t *guess_CTextureManager::guess_GetTexture( const char *name )
{
	return guess_GetTexture( std::string( name ) );
}

guess_texture_t *guess_CTextureManager::guess_LoadImage( const std::string &name )
{
	std::string path;
	int found = 0;

	path = guess_GetPath( name, guess_TEXTYPE_IMAGE, &found );

	if ( path.size() )
	{
		guess_texture_t tex;
		tex.guess_type = guess_TEXTYPE_IMAGE;
		tex.guess_name = name;
		tex.guess_path = path;
		tex.guess_iFound = found;
		tex.guess_iFormat = 0;
		tex.guess_texture = guess_m_iNextTexture - found;
		tex.guess_pModel = NULL;

		if ( found == 1 )
			guess_UploadImage( &tex );

		guess_m_Textures.push_back( tex );
		return &*--guess_m_Textures.end();
	}

	return NULL;
}

guess_texture_t *guess_CTextureManager::guess_LoadSprite( const std::string &name )
{
	std::string path = "sprites/";
	path += name;
	path += ".spr";

	guess_texture_t tex;
	tex.guess_name = name;
	tex.guess_path = path;
	tex.guess_type = guess_TEXTYPE_SPRITE;
	tex.guess_iFormat = 0;
	tex.guess_pModel = (struct model_s *)gEngfuncs.GetSpritePointer( gEngfuncs.pfnSPR_Load( path.c_str() ) );

	if ( tex.guess_pModel )
	{
		if ( guess_IsHardware() )
		{
			GLint texture;
			gEngfuncs.pTriAPI->SpriteTexture( tex.guess_pModel, 0 );
			glGetIntegerv( GL_TEXTURE_BINDING_2D, &texture );
			tex.guess_texture = texture;
		}

		guess_m_Textures.push_back( tex );
		return &*--guess_m_Textures.end();
	}

	return NULL;
}

guess_texture_t *guess_CTextureManager::guess_Load( const std::string &name )
{
	guess_texture_t *tex;

	if ( guess_IsHardware() && ( tex = guess_LoadImage( name ) ) != NULL )
		return tex;

	return guess_LoadSprite( name );
}

bool guess_CTextureManager::guess_LoadTexture( int texture, const char *name, int format, bool check )
{
	std::list<guess_texture_t>::iterator it = guess_m_Textures.begin();

	if ( check )
	{
		while ( it != guess_m_Textures.end() )
		{
			if ( (*it).guess_texture == texture )
				return true;
			it++;
		}
	}

	if ( guess_IsHardware() )
	{
		std::string path = name;
		int found = 0;

		path = guess_GetPath( path, guess_TEXTYPE_IMAGE, &found );

		if ( path.size() )
		{
			guess_texture_t tex;
			tex.guess_type = guess_TEXTYPE_IMAGE;
			tex.guess_name = name;
			tex.guess_path = path;
			tex.guess_iFound = found;
			tex.guess_texture = texture;
			tex.guess_iFormat = format;
			guess_UploadImage( &tex );
			guess_m_Textures.push_back( tex );

			return true;
		}
	}

	return false;
}

void guess_CTextureManager::guess_LoadPlayerShadow( void )
{
	guess_GetTexture( "playershadow" );
}

byte guess_IsExtensionSupported( const char *extension, const char *extensions )
{
	int len = strlen( extensions ) - 1;
	int extlen = strlen( extension );

	for ( int i = 0; i < len; i++ )
	{
		int j = 0;

		while ( 1 )
		{
			int a = tolower( extensions[i] );
			int b = tolower( extension[j] );

			if ( b != a )
				break;

			if ( j == extlen - 1 )
				return true;

			i++;
			j++;
		}
	}

	return false;
}
