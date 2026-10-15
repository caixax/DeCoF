// Cry of Fear texture manager: loads textures through DevIL (or as sprites
// in software mode) and keeps a list of them.

#ifndef GUESS_TEXTUREMANAGER_H
#define GUESS_TEXTUREMANAGER_H

#include <string>
#include <list>

// texture types
#define TEXTYPE_SPRITE_guess	1
#define TEXTYPE_IMAGE_guess		2

typedef struct texture_s_guess
{
	int			type_guess;
	std::string	name_guess;
	std::string	path_guess;
	int			texture_guess;		// GL texture name (sprite: the bound texture)
	struct model_s *pModel_guess;	// sprite model
	int			iFound_guess;
	int			iFormat_guess;
} texture_t_guess;

class CTextureManager_guess
{
public:
	CTextureManager_guess();
	~CTextureManager_guess();

	void CheckMipmapSupport_guess( void );
	void CreateTexture_guess( int width, int height, GLuint *texture, GLenum target, GLenum format );
	std::string GetPath_guess( const std::string &name, int type, int *found );
	std::string FindTexture_guess( const std::string &name, int *found );
	texture_t_guess *GetTexture_guess( const std::string &name );
	std::string FindNumberedTextures_guess( const std::string &name, int *count );
	std::string FindImageFile_guess( const std::string &path );
	bool IsHardware_guess( void );
	void Reset_guess( void );
	void UploadImage_guess( texture_t_guess *tex );
	texture_t_guess *GetTexture_guess( const char *name );
	texture_t_guess *LoadImage_guess( const std::string &name );
	texture_t_guess *LoadSprite_guess( const std::string &name );
	texture_t_guess *Load_guess( const std::string &name );
	bool LoadTexture_guess( int texture, const char *name, int format, bool check );
	void LoadPlayerShadow_guess( void );

	int		m_iMipmapMode_guess;
	int		m_iNextTexture_guess;
	std::list<texture_t_guess> m_Textures_guess;
};

extern CTextureManager_guess g_TextureManager_guess;

byte IsExtensionSupported_guess( const char *extension, const char *extensions );

#endif // GUESS_TEXTUREMANAGER_H
