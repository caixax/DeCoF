// Cry of Fear TGA loader (the loader of the NeHe lesson 33 tutorial by
// Evan Pipho, in a class, printing to the console). Nothing in the client
// calls it.

#include <windows.h>
#include <gl/gl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hud.h"
#include "cl_util.h"
#include "guess_TGALoader.h"

bool guess_CTGALoader::guess_LoadTGA( guess_Texture *texture, char *filename )
{
	FILE *fTGA;
	fTGA = fopen( filename, "rb" );

	if ( fTGA == NULL )
	{
		gEngfuncs.Con_Printf( "TGA LOAD: Could not open texture file\n" );
		return false;
	}

	if ( fread( &guess_tgaheader, sizeof( guess_TGAHeader ), 1, fTGA ) == 0 )
	{
		gEngfuncs.Con_Printf( "TGA LOAD: Could not read file header\n" );
		if ( fTGA != NULL )
			fclose( fTGA );
		return false;
	}

	if ( memcmp( guess_uTGAcompare, &guess_tgaheader, sizeof( guess_tgaheader ) ) == 0 )
	{
		guess_LoadUncompressedTGA( texture, filename, fTGA );
	}
	else if ( memcmp( guess_cTGAcompare, &guess_tgaheader, sizeof( guess_tgaheader ) ) == 0 )
	{
		guess_LoadCompressedTGA( texture, filename, fTGA );
	}
	else
	{
		gEngfuncs.Con_Printf( "TGA LOAD: TGA file must be type 2 or type 10\n" );
		fclose( fTGA );
		return false;
	}

	return true;
}

bool guess_CTGALoader::guess_LoadUncompressedTGA( guess_Texture *texture, char *filename, FILE *fTGA )
{
	if ( fread( guess_tga.guess_header, sizeof( guess_tga.guess_header ), 1, fTGA ) == 0 )
	{
		gEngfuncs.Con_Printf( "TGA LOAD: Could not read info header\n" );
		if ( fTGA != NULL )
			fclose( fTGA );
		return false;
	}

	texture->guess_width	= guess_tga.guess_header[1] * 256 + guess_tga.guess_header[0];
	texture->guess_height	= guess_tga.guess_header[3] * 256 + guess_tga.guess_header[2];
	texture->guess_bpp		= guess_tga.guess_header[4];
	guess_tga.guess_Width	= texture->guess_width;
	guess_tga.guess_Height	= texture->guess_height;
	guess_tga.guess_Bpp		= texture->guess_bpp;

	if ( ( texture->guess_width <= 0 ) || ( texture->guess_height <= 0 ) || ( ( texture->guess_bpp != 24 ) && ( texture->guess_bpp != 32 ) ) )
	{
		gEngfuncs.Con_Printf( "TGA LOAD: Invalid texture information\n" );
		if ( fTGA != NULL )
			fclose( fTGA );
		return false;
	}

	if ( texture->guess_bpp == 24 )
		texture->guess_type = GL_RGB;
	else
		texture->guess_type = GL_RGBA;

	guess_tga.guess_bytesPerPixel	= ( guess_tga.guess_Bpp / 8 );
	guess_tga.guess_imageSize		= ( guess_tga.guess_bytesPerPixel * guess_tga.guess_Width * guess_tga.guess_Height );
	texture->guess_imageData		= (GLubyte *)malloc( guess_tga.guess_imageSize );

	if ( texture->guess_imageData == NULL )
	{
		gEngfuncs.Con_Printf( "TGA LOAD: Could not allocate memory for image\n" );
		fclose( fTGA );
		return false;
	}

	if ( fread( texture->guess_imageData, 1, guess_tga.guess_imageSize, fTGA ) != guess_tga.guess_imageSize )
	{
		gEngfuncs.Con_Printf( "TGA LOAD: Could not read image data\n" );
		if ( texture->guess_imageData != NULL )
			free( texture->guess_imageData );
		fclose( fTGA );
		return false;
	}

	for ( GLuint cswap = 0; cswap < (int)guess_tga.guess_imageSize; cswap += guess_tga.guess_bytesPerPixel )
	{
		texture->guess_imageData[cswap] ^= texture->guess_imageData[cswap + 2] ^=
		texture->guess_imageData[cswap] ^= texture->guess_imageData[cswap + 2];
	}

	fclose( fTGA );
	return true;
}

bool guess_CTGALoader::guess_LoadCompressedTGA( guess_Texture *texture, char *filename, FILE *fTGA )
{
	if ( fread( guess_tga.guess_header, sizeof( guess_tga.guess_header ), 1, fTGA ) == 0 )
	{
		gEngfuncs.Con_Printf( "TGA LOAD: Could not read info header\n" );
		if ( fTGA != NULL )
			fclose( fTGA );
		return false;
	}

	texture->guess_width	= guess_tga.guess_header[1] * 256 + guess_tga.guess_header[0];
	texture->guess_height	= guess_tga.guess_header[3] * 256 + guess_tga.guess_header[2];
	texture->guess_bpp		= guess_tga.guess_header[4];
	guess_tga.guess_Width	= texture->guess_width;
	guess_tga.guess_Height	= texture->guess_height;
	guess_tga.guess_Bpp		= texture->guess_bpp;

	if ( ( texture->guess_width <= 0 ) || ( texture->guess_height <= 0 ) || ( ( texture->guess_bpp != 24 ) && ( texture->guess_bpp != 32 ) ) )
	{
		gEngfuncs.Con_Printf( "TGA LOAD: Invalid texture information\n" );
		if ( fTGA != NULL )
			fclose( fTGA );
		return false;
	}

	if ( texture->guess_bpp == 24 )
		texture->guess_type = GL_RGB;
	else
		texture->guess_type = GL_RGBA;

	guess_tga.guess_bytesPerPixel	= ( guess_tga.guess_Bpp / 8 );
	guess_tga.guess_imageSize		= ( guess_tga.guess_bytesPerPixel * guess_tga.guess_Width * guess_tga.guess_Height );
	texture->guess_imageData		= (GLubyte *)malloc( guess_tga.guess_imageSize );

	if ( texture->guess_imageData == NULL )
	{
		gEngfuncs.Con_Printf( "TGA LOAD: Could not allocate memory for image\n" );
		fclose( fTGA );
		return false;
	}

	GLuint pixelcount		= guess_tga.guess_Height * guess_tga.guess_Width;
	GLuint currentpixel		= 0;
	GLuint currentbyte		= 0;
	GLubyte *colorbuffer	= (GLubyte *)malloc( guess_tga.guess_bytesPerPixel );

	do
	{
		GLubyte chunkheader = 0;

		if ( fread( &chunkheader, sizeof( GLubyte ), 1, fTGA ) == 0 )
		{
			gEngfuncs.Con_Printf( "TGA LOAD: Could not read RLE header\n" );
			if ( fTGA != NULL )
				fclose( fTGA );
			if ( texture->guess_imageData != NULL )
				free( texture->guess_imageData );
			return false;
		}

		if ( chunkheader < 128 )
		{
			short counter;
			chunkheader++;
			for ( counter = 0; counter < chunkheader; counter++ )
			{
				if ( fread( colorbuffer, 1, guess_tga.guess_bytesPerPixel, fTGA ) != guess_tga.guess_bytesPerPixel )
				{
					gEngfuncs.Con_Printf( "TGA LOAD: Could not read image data\n" );
					if ( fTGA != NULL )
						fclose( fTGA );
					if ( colorbuffer != NULL )
						free( colorbuffer );
					if ( texture->guess_imageData != NULL )
						free( texture->guess_imageData );
					return false;
				}

				texture->guess_imageData[currentbyte] = colorbuffer[2];
				texture->guess_imageData[currentbyte + 1] = colorbuffer[1];
				texture->guess_imageData[currentbyte + 2] = colorbuffer[0];

				if ( guess_tga.guess_bytesPerPixel == 4 )
					texture->guess_imageData[currentbyte + 3] = colorbuffer[3];

				currentbyte += guess_tga.guess_bytesPerPixel;
				currentpixel++;

				if ( currentpixel > pixelcount )
				{
					gEngfuncs.Con_Printf( "TGA LOAD: Too many pixels read\n" );
					if ( fTGA != NULL )
						fclose( fTGA );
					if ( colorbuffer != NULL )
						free( colorbuffer );
					if ( texture->guess_imageData != NULL )
						free( texture->guess_imageData );
					return false;
				}
			}
		}
		else
		{
			short counter;
			chunkheader -= 127;
			if ( fread( colorbuffer, 1, guess_tga.guess_bytesPerPixel, fTGA ) != guess_tga.guess_bytesPerPixel )
			{
				gEngfuncs.Con_Printf( "TGA LOAD: Could not read from file\n" );
				if ( fTGA != NULL )
					fclose( fTGA );
				if ( colorbuffer != NULL )
					free( colorbuffer );
				if ( texture->guess_imageData != NULL )
					free( texture->guess_imageData );
				return false;
			}

			for ( counter = 0; counter < chunkheader; counter++ )
			{
				texture->guess_imageData[currentbyte] = colorbuffer[2];
				texture->guess_imageData[currentbyte + 1] = colorbuffer[1];
				texture->guess_imageData[currentbyte + 2] = colorbuffer[0];

				if ( guess_tga.guess_bytesPerPixel == 4 )
					texture->guess_imageData[currentbyte + 3] = colorbuffer[3];

				currentbyte += guess_tga.guess_bytesPerPixel;
				currentpixel++;

				if ( currentpixel > pixelcount )
				{
					gEngfuncs.Con_Printf( "TGA LOAD: Too many pixels read\n" );
					if ( fTGA != NULL )
						fclose( fTGA );
					if ( colorbuffer != NULL )
						free( colorbuffer );
					if ( texture->guess_imageData != NULL )
						free( texture->guess_imageData );
					return false;
				}
			}
		}
	}
	while ( currentpixel < pixelcount );

	fclose( fTGA );
	return true;
}
