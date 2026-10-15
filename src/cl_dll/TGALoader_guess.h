// Cry of Fear TGA loader (the loader of the NeHe lesson 33 tutorial, in a
// class). Nothing in the client calls it; the linker keeps it.

#ifndef GUESS_TGALOADER_H
#define GUESS_TGALOADER_H

typedef struct
{
	GLubyte	*guess_imageData;
	GLuint	guess_bpp;
	GLuint	guess_width;
	GLuint	guess_height;
	GLuint	guess_texID;
	GLuint	guess_type;
} guess_Texture;

typedef struct
{
	GLubyte	guess_Header[12];
} guess_TGAHeader;

typedef struct
{
	GLubyte	guess_header[6];
	GLuint	guess_bytesPerPixel;
	GLuint	guess_imageSize;
	GLuint	guess_temp;
	GLuint	guess_type;
	GLuint	guess_Height;
	GLuint	guess_Width;
	GLuint	guess_Bpp;
} guess_TGA;

class guess_CTGALoader
{
public:
	bool guess_LoadTGA( guess_Texture *texture, char *filename );
	bool guess_LoadUncompressedTGA( guess_Texture *texture, char *filename, FILE *fTGA );
	bool guess_LoadCompressedTGA( guess_Texture *texture, char *filename, FILE *fTGA );

	guess_TGAHeader	guess_tgaheader;
	guess_TGA		guess_tga;
	GLubyte			guess_uTGAcompare[12];
	GLubyte			guess_cTGAcompare[12];
};

#endif // GUESS_TGALOADER_H
