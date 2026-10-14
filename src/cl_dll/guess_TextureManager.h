// Cry of Fear texture manager: loads textures through DevIL (or as sprites
// in software mode) and keeps a list of them.

#ifndef GUESS_TEXTUREMANAGER_H
#define GUESS_TEXTUREMANAGER_H

#include <string>
#include <list>

// texture types
#define guess_TEXTYPE_SPRITE	1
#define guess_TEXTYPE_IMAGE		2

typedef struct guess_texture_s
{
	int			guess_type;
	std::string	guess_name;
	std::string	guess_path;
	int			guess_texture;		// GL texture name (sprite: the bound texture)
	struct model_s *guess_pModel;	// sprite model
	int			guess_iFound;
	int			guess_iFormat;
} guess_texture_t;

class guess_CTextureManager
{
public:
	guess_CTextureManager();
	~guess_CTextureManager();

	void guess_CheckMipmapSupport( void );
	void guess_CreateTexture( int width, int height, GLuint *texture, GLenum target, GLenum format );
	std::string guess_GetPath( const std::string &name, int type, int *found );
	std::string guess_FindTexture( const std::string &name, int *found );
	guess_texture_t *guess_GetTexture( const std::string &name );
	std::string guess_FindNumberedTextures( const std::string &name, int *count );
	std::string guess_FindImageFile( const std::string &path );
	bool guess_IsHardware( void );
	void guess_Reset( void );
	void guess_UploadImage( guess_texture_t *tex );
	guess_texture_t *guess_GetTexture( const char *name );
	guess_texture_t *guess_LoadImage( const std::string &name );
	guess_texture_t *guess_LoadSprite( const std::string &name );
	guess_texture_t *guess_Load( const std::string &name );
	bool guess_LoadTexture( int texture, const char *name, int format, bool check );
	void guess_LoadPlayerShadow( void );

	int		guess_m_iMipmapMode;
	int		guess_m_iNextTexture;
	std::list<guess_texture_t> guess_m_Textures;
};

extern guess_CTextureManager guess_g_TextureManager;

byte guess_IsExtensionSupported( const char *extension, const char *extensions );

#endif // GUESS_TEXTUREMANAGER_H
