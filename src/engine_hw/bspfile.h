// bspfile.h -- the on-disk format of a compiled map

#ifndef BSPFILE_H
#define BSPFILE_H

#define Q1BSP_VERSION		29		// quake1 regular version (beta is 28)
#define HLBSP_VERSION		30		// half-life regular version

#define MAX_MAP_LEAFS		8192

#define LUMP_ENTITIES		0
#define LUMP_PLANES			1
#define LUMP_TEXTURES		2
#define LUMP_VERTEXES		3
#define LUMP_VISIBILITY		4
#define LUMP_NODES			5
#define LUMP_TEXINFO		6
#define LUMP_FACES			7
#define LUMP_LIGHTING		8
#define LUMP_CLIPNODES		9
#define LUMP_LEAFS			10
#define LUMP_MARKSURFACES	11
#define LUMP_EDGES			12
#define LUMP_SURFEDGES		13
#define LUMP_MODELS			14

#define HEADER_LUMPS		15

typedef struct lump_s
{
	int		fileofs;
	int		filelen;
} lump_t;

typedef struct dheader_s
{
	int		version;
	lump_t	lumps[HEADER_LUMPS];
} dheader_t;

#define	MAX_MAP_HULLS		4

#define	MAXLIGHTMAPS		4

#define	NUM_AMBIENTS		4		// automatic ambient sounds

// the contents that const.h leaves to this file
#define	CONTENTS_ORIGIN		-7		// removed at csg time
#define	CONTENTS_CLIP		-8		// changed to contents_solid
#define	CONTENTS_CURRENT_0		-9
#define	CONTENTS_CURRENT_90		-10
#define	CONTENTS_CURRENT_180	-11
#define	CONTENTS_CURRENT_270	-12
#define	CONTENTS_CURRENT_UP		-13
#define	CONTENTS_CURRENT_DOWN	-14

#define CONTENTS_TRANSLUCENT	-15

typedef struct
{
	float		mins[3], maxs[3];
	float		origin[3];
	int			headnode[MAX_MAP_HULLS];
	int			visleafs;		// not including the solid leaf 0
	int			firstface, numfaces;
} dmodel_t;

typedef struct
{
	int			planenum;
	short		children[2];	// negative numbers are contents
} dclipnode_t;

#define	MIPLEVELS	4
typedef struct miptex_s
{
	char		name[16];
	unsigned	width, height;
	unsigned	offsets[MIPLEVELS];		// four mip maps stored
} miptex_t;

#endif // BSPFILE_H
