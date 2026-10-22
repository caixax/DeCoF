// wad.h -- WAD file lumps

#ifndef WAD_H
#define WAD_H

//===============
//   TYPES
//===============

#define	CMP_NONE		0
#define	CMP_LZSS		1

#define	TYP_NONE		0
#define	TYP_LABEL		1

#define	TYP_LUMPY		64				// 64 + grab command number

typedef struct qpic_s
{
	int			width, height;
	byte		data[4];			// variably sized
} qpic_t;



typedef struct
{
	char		identification[4];		// should be WAD2 or 2DAW
	int			numlumps;
	int			infotableofs;
} wadinfo_t;

typedef struct lumpinfo_s
{
	int			filepos;
	int			disksize;
	int			size;					// uncompressed
	char		type;
	char		compression;
	char		pad1, pad2;
	char		name[16];				// must be null terminated
} lumpinfo_t;

void	*W_GetLumpName( int wad, char *name );
void	W_CleanupName( char *in, char *out );

#endif // WAD_H
