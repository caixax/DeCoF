// draw.h -- these are the only functions outside the refresh allowed
// to touch the vid buffer (gl_draw.c)

#ifndef DRAW_H
#define DRAW_H

extern	struct qpic_s	*draw_disc;	// also used on sbar

typedef struct
{
	char			name[64];
	cache_user_t	cache;
} cacheentry_t;

typedef struct cachewad_s cachewad_t;

typedef void (*PFNCACHE)( cachewad_t *, byte * );

struct cachewad_s
{
	char			*name;
	cacheentry_t	*cache;
	int				cacheCount;
	int				cacheMax;
	struct lumpinfo_s	*lumps;
	int				lumpCount;
	int				cacheExtra;
	PFNCACHE		pfnCacheBuild;
	int				numpaths;
	char			**basedirs;
	int				*lumppathindices;
	int				tempWad;
};

int		Draw_Character( int x, int y, int num, unsigned int font );
int		Draw_String( int x, int y, char *str );
int		Draw_StringLen( const char *psz, unsigned int font );
void	Draw_SetTextColor( float r, float g, float b );
void	Draw_ResetTextColor( void );

// decals.c
void	Draw_DecalSetName( int decal, char *name );

// cl_draw.c
void	SPR_Shutdown_NoModelFree( void );

// gl_draw.c
void	Draw_FillRGBA( int x, int y, int w, int h, int r, int g, int b, int a );

#endif // DRAW_H
