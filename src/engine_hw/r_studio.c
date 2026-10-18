// r_studio.c -- studio model rendering and studio hulls

#include "quakedef.h"
#include "mathlib.h"
#include "const.h"
#include "cvar.h"
#include "zone.h"
#include "GL_MODEL.H"
#include "progs.h"
#include "eiface.h"
#include "delta.h"
#include "server.h"
#include "client.h"
#include "sound.h"
#include "vid.h"
#include "render.h"
#include "GLQUAKE.H"
#include "gl_cvars.h"
#include "studio.h"
#include "r_studioint.h"
#include "host.h"
#include "view.h"
#include "qgl.h"
#include "mem.h"
#include "triangleapi.h"

#include <math.h>

#define STUDIO_CACHE_SIZE	16
#define STUDIO_CACHEMASK	( STUDIO_CACHE_SIZE - 1 )

#define MAXSTUDIOHULLS		128

typedef struct
{
	float		frame;
	int			sequence;
	vec3_t		angles;
	vec3_t		origin;
	vec3_t		size;
	unsigned char	controller[4];
	unsigned char	blending[2];
	model_t		*pModel;
	int			nStartHull;
	int			nStartPlane;
	int			numhulls;
} r_studiocache_t;

cvar_t			*cl_righthand;

static studiohdr_t	*pstudiohdr;

float			rotationmatrix[3][4];
float			bonetransform[MAXSTUDIOBONES][3][4];
float			lighttransform[MAXSTUDIOBONES][3][4];
int				cached_numbones;
char			cached_bonename[MAXSTUDIOBONES][32];
float			cached_bonetransform[MAXSTUDIOBONES][3][4];
float			cached_lighttransform[MAXSTUDIOBONES][3][4];

static int		cache_hull_hitgroup[MAXSTUDIOHULLS];
static hull_t	cache_hull[MAXSTUDIOHULLS];
static mplane_t	cache_planes[MAXSTUDIOHULLS * 6];
static int		nCurrentHull;
static int		nCurrentPlane;

static r_studiocache_t	rgStudioCache[STUDIO_CACHE_SIZE];
static int		r_cachecurrent;

int				r_dointerp = 1;

cvar_t			r_cachestudio = { "r_cachestudio", "1" };
cvar_t			r_cullsequencebox = { "r_cullsequencebox", "1" };

static int		studio_hull_hitgroup[MAXSTUDIOHULLS];
static hull_t	studio_hull[MAXSTUDIOHULLS];
static dclipnode_t	studio_clipnodes[6];
static mplane_t	studio_planes[MAXSTUDIOHULLS * 6];

int				g_iBackFaceCull;

static mstudiobodyparts_t	*pbodypart;
static mstudiomodel_t	*psubmodel;

static player_info_t	*r_playerinfo;
static int		r_playerindex;
static model_t	*r_model;
static float	r_gaitmovement;
static int		g_ForcedFaceFlags;
static vec3_t	g_ChromeOrigin;

static int		chrome[MAXSTUDIOVERTS][2];
static int		g_NormalIndex[MAXSTUDIOVERTS];
static int		chromeage[MAXSTUDIOBONES];
static vec3_t	r_chromeup[MAXSTUDIOBONES];
static vec3_t	r_chromeright[MAXSTUDIOBONES];

static int		numlights;
static dlight_t	*locallight[3];
static int		locallinearlight[3][3];
static float	locallightR2[3];
static float	lightpos[MAXSTUDIOVERTS][3][4];
static vec3_t	lightbonepos[MAXSTUDIOBONES][3];
static int		lightage[MAXSTUDIOBONES];

auxvert_t		*pauxverts;

int				r_ambientlight;
float			r_shadelight;

int				r_amodels_drawn;
int				r_smodels_total;

void R_FlushStudioCache( void );
void *Mod_Extradata( model_t *mod );

extern sv_blending_interface_t	*g_pSvBlendingAPI;
extern int		g_bIsTerrorStrike;
extern cvar_t	r_glowshellfreq;
extern triangleapi_t	tri;

int		SignbitsForPlane( mplane_t *out );
qboolean	R_CullBox( vec3_t mins, vec3_t maxs );
void	R_StudioTransformVector( vec_t *in, vec_t *out );
void	R_StudioTransformAuxVert( auxvert_t *av, int bone, vec_t *vert );

/*
================
SV_InitStudioHull
================
*/
void SV_InitStudioHull( void )
{
	int		i;
	int		side;

	if ( studio_hull[0].planes != NULL )
		return;

	for ( i = 0; i < 6; i++ )
	{
		studio_clipnodes[i].planenum = i;

		side = i & 1;

		studio_clipnodes[i].children[side] = CONTENTS_EMPTY;
		if ( i != 5 )
			studio_clipnodes[i].children[side ^ 1] = i + 1;
		else
			studio_clipnodes[i].children[side ^ 1] = CONTENTS_SOLID;
	}

	for ( i = 0; i < MAXSTUDIOHULLS; i++ )
	{
		studio_hull[i].clipnodes = studio_clipnodes;
		studio_hull[i].planes = &studio_planes[i * 6];
		studio_hull[i].firstclipnode = 0;
		studio_hull[i].lastclipnode = 5;
	}
}

/*
================
R_CheckStudioCache
================
*/
r_studiocache_t *R_CheckStudioCache( model_t *pModel, float frame, int sequence, const vec_t *angles, const vec_t *origin, const vec_t *size, const unsigned char *controller, const unsigned char *blending )
{
	int		i;
	r_studiocache_t	*pCached;

	for ( i = 0; i < STUDIO_CACHE_SIZE; i++ )
	{
		pCached = &rgStudioCache[( r_cachecurrent - i ) & STUDIO_CACHEMASK];

		if ( pCached->pModel != pModel )
			continue;

		if ( pCached->frame != frame )
			continue;

		if ( pCached->sequence != sequence )
			continue;

		if ( !VectorCompare( pCached->angles, angles ) )
			continue;

		if ( !VectorCompare( pCached->origin, origin ) )
			continue;

		if ( !VectorCompare( pCached->size, size ) )
			continue;

		if ( Q_memcmp( pCached->controller, (void *)controller, 4 ) != 0 )
			continue;

		if ( Q_memcmp( pCached->blending, (void *)blending, 2 ) != 0 )
			continue;

		return pCached;
	}

	return NULL;
}

/*
================
R_AddToStudioCache
================
*/
void R_AddToStudioCache( float frame, int sequence, const vec_t *angles, const vec_t *origin, const vec_t *size, const unsigned char *controller, const unsigned char *pblending, model_t *pModel, hull_t *pHulls, int numhulls )
{
	r_studiocache_t	*p;

	if ( nCurrentHull + numhulls >= MAXSTUDIOHULLS )
		R_FlushStudioCache();

	r_cachecurrent++;

	p = &rgStudioCache[r_cachecurrent & STUDIO_CACHEMASK];

	p->frame = frame;
	p->sequence = sequence;

	VectorCopy( angles, p->angles );
	VectorCopy( origin, p->origin );
	VectorCopy( size, p->size );

	Q_memcpy( p->controller, (void *)controller, sizeof( p->controller ) );
	Q_memcpy( p->blending, (void *)pblending, sizeof( p->blending ) );

	p->pModel = pModel;
	p->nStartHull = nCurrentHull;
	p->nStartPlane = nCurrentPlane;

	Q_memcpy( &cache_hull[nCurrentHull], pHulls, numhulls * sizeof( hull_t ) );
	Q_memcpy( &cache_planes[nCurrentPlane], studio_planes, numhulls * 6 * sizeof( mplane_t ) );
	Q_memcpy( &cache_hull_hitgroup[nCurrentHull], studio_hull_hitgroup, numhulls * sizeof( int ) );

	nCurrentHull += numhulls;
	nCurrentPlane += numhulls * 6;

	p->numhulls = numhulls;
}

/*
====================
AngleQuaternion

====================
*/
void AngleQuaternion( vec_t *angles, vec_t *quaternion )
{
	float		angle;
	float		sr, sp, sy, cr, cp, cy;

	// FIXME: rescale the inputs to 1/2 angle
	angle = angles[2] * 0.5;
	sy = sin( angle );
	cy = cos( angle );
	angle = angles[1] * 0.5;
	sp = sin( angle );
	cp = cos( angle );
	angle = angles[0] * 0.5;
	sr = sin( angle );
	cr = cos( angle );

	quaternion[0] = sr*cp*cy-cr*sp*sy; // X
	quaternion[1] = cr*sp*cy+sr*cp*sy; // Y
	quaternion[2] = cr*cp*sy-sr*sp*cy; // Z
	quaternion[3] = cr*cp*cy+sr*sp*sy; // W
}

/*
====================
QuaternionSlerp

====================
*/
float	omega, cosom, sinom, sclp, sclq;

void QuaternionSlerp( vec_t *p, vec_t *q, float t, vec_t *qt )
{
	int		i;

	// decide if one of the quaternions is backwards
	float	a = 0;
	float	b = 0;

	for ( i = 0; i < 4; i++ )
	{
		a += ( p[i] - q[i] ) * ( p[i] - q[i] );
		b += ( p[i] + q[i] ) * ( p[i] + q[i] );
	}
	if ( a > b )
	{
		for ( i = 0; i < 4; i++ )
		{
			q[i] = -q[i];
		}
	}

	cosom = p[0]*q[0] + p[1]*q[1] + p[2]*q[2] + p[3]*q[3];

	if ( ( 1.0 + cosom ) > 0.000001 )
	{
		if ( ( 1.0 - cosom ) > 0.000001 )
		{
			omega = acos( cosom );
			sinom = sin( omega );
			sclp = sin( ( 1.0 - t ) * omega ) / sinom;
			sclq = sin( t * omega ) / sinom;
		}
		else
		{
			sclp = 1.0 - t;
			sclq = t;
		}
		for ( i = 0; i < 4; i++ )
		{
			qt[i] = sclp * p[i] + sclq * q[i];
		}
	}
	else
	{
		qt[0] = -q[1];
		qt[1] = q[0];
		qt[2] = -q[3];
		qt[3] = q[2];
		sclp = sin( ( 1.0 - t ) * ( 0.5 * M_PI ) );
		sclq = sin( t * ( 0.5 * M_PI ) );
		for ( i = 0; i < 3; i++ )
		{
			qt[i] = sclp * p[i] + sclq * qt[i];
		}
	}
}

/*
====================
QuaternionMatrix

====================
*/
void QuaternionMatrix( vec_t *quaternion, float (*matrix)[4] )
{
	matrix[0][0] = 1.0 - 2.0 * quaternion[1] * quaternion[1] - 2.0 * quaternion[2] * quaternion[2];
	matrix[1][0] = 2.0 * quaternion[0] * quaternion[1] + 2.0 * quaternion[3] * quaternion[2];
	matrix[2][0] = 2.0 * quaternion[0] * quaternion[2] - 2.0 * quaternion[3] * quaternion[1];

	matrix[0][1] = 2.0 * quaternion[0] * quaternion[1] - 2.0 * quaternion[3] * quaternion[2];
	matrix[1][1] = 1.0 - 2.0 * quaternion[0] * quaternion[0] - 2.0 * quaternion[2] * quaternion[2];
	matrix[2][1] = 2.0 * quaternion[1] * quaternion[2] + 2.0 * quaternion[3] * quaternion[0];

	matrix[0][2] = 2.0 * quaternion[0] * quaternion[2] + 2.0 * quaternion[3] * quaternion[1];
	matrix[1][2] = 2.0 * quaternion[1] * quaternion[2] - 2.0 * quaternion[3] * quaternion[0];
	matrix[2][2] = 1.0 - 2.0 * quaternion[0] * quaternion[0] - 2.0 * quaternion[1] * quaternion[1];
}

/*
====================
R_StudioCalcBoneAdj

====================
*/
void R_StudioCalcBoneAdj( float dadt, float *adj, const unsigned char *pcontroller1, const unsigned char *pcontroller2, unsigned char mouthopen )
{
	int					i, j;
	float				value;
	mstudiobonecontroller_t *pbonecontroller;

	pbonecontroller = (mstudiobonecontroller_t *)( (byte *)pstudiohdr + pstudiohdr->bonecontrollerindex );

	for ( j = 0; j < pstudiohdr->numbonecontrollers; j++ )
	{
		i = pbonecontroller[j].index;
		if ( i <= 3 )
		{
			// check for 360% wrapping
			if ( pbonecontroller[j].type & STUDIO_RLOOP )
			{
				if ( abs( pcontroller1[i] - pcontroller2[i] ) > 128 )
				{
					int a, b;
					a = ( pcontroller1[j] + 128 ) % 256;
					b = ( pcontroller2[j] + 128 ) % 256;
					value = ( ( a * dadt ) + ( b * ( 1 - dadt ) ) - 128 ) * ( 360.0 / 256.0 ) + pbonecontroller[j].start;
				}
				else
				{
					value = ( ( pcontroller1[i] * dadt + ( pcontroller2[i] ) * ( 1.0 - dadt ) ) ) * ( 360.0 / 256.0 ) + pbonecontroller[j].start;
				}
			}
			else
			{
				value = ( pcontroller1[i] * dadt + pcontroller2[i] * ( 1.0 - dadt ) ) / 255.0;
				if ( value < 0 ) value = 0;
				if ( value > 1.0 ) value = 1.0;
				value = ( 1.0 - value ) * pbonecontroller[j].start + value * pbonecontroller[j].end;
			}
		}
		else
		{
			value = mouthopen / 64.0;
			if ( value > 1.0 ) value = 1.0;
			value = ( 1.0 - value ) * pbonecontroller[j].start + value * pbonecontroller[j].end;
		}

		switch ( pbonecontroller[j].type & STUDIO_TYPES )
		{
		case STUDIO_XR:
		case STUDIO_YR:
		case STUDIO_ZR:
			adj[j] = value * ( M_PI / 180.0 );
			break;
		case STUDIO_X:
		case STUDIO_Y:
		case STUDIO_Z:
			adj[j] = value;
			break;
		}
	}
}

/*
====================
R_StudioCalcBoneQuaterion

====================
*/
void R_StudioCalcBoneQuaterion( int frame, float s, mstudiobone_t *pbone, mstudioanim_t *panim, float *adj, float *q )
{
	int					j, k;
	vec4_t				q1, q2;
	vec3_t				angle1, angle2;
	mstudioanimvalue_t	*panimvalue;

	for ( j = 0; j < 3; j++ )
	{
		if ( panim->offset[j+3] == 0 )
		{
			angle2[j] = angle1[j] = pbone->value[j+3]; // default;
		}
		else
		{
			panimvalue = (mstudioanimvalue_t *)( (byte *)panim + panim->offset[j+3] );
			k = frame;
			// DEBUG
			if ( panimvalue->num.total < panimvalue->num.valid )
				k = 0;
			while ( panimvalue->num.total <= k )
			{
				k -= panimvalue->num.total;
				panimvalue += panimvalue->num.valid + 1;
				// DEBUG
				if ( panimvalue->num.total < panimvalue->num.valid )
					k = 0;
			}
			// Bah, missing blend!
			if ( panimvalue->num.valid > k )
			{
				angle1[j] = panimvalue[k+1].value;

				if ( panimvalue->num.valid > k + 1 )
				{
					angle2[j] = panimvalue[k+2].value;
				}
				else
				{
					if ( panimvalue->num.total > k + 1 )
						angle2[j] = angle1[j];
					else
						angle2[j] = panimvalue[panimvalue->num.valid+2].value;
				}
			}
			else
			{
				angle1[j] = panimvalue[panimvalue->num.valid].value;
				if ( panimvalue->num.total > k + 1 )
				{
					angle2[j] = angle1[j];
				}
				else
				{
					angle2[j] = panimvalue[panimvalue->num.valid + 2].value;
				}
			}
			angle1[j] = pbone->value[j+3] + angle1[j] * pbone->scale[j+3];
			angle2[j] = pbone->value[j+3] + angle2[j] * pbone->scale[j+3];
		}

		if ( pbone->bonecontroller[j+3] != -1 )
		{
			angle1[j] += adj[pbone->bonecontroller[j+3]];
			angle2[j] += adj[pbone->bonecontroller[j+3]];
		}
	}

	if ( !VectorCompare( angle1, angle2 ) )
	{
		AngleQuaternion( angle1, q1 );
		AngleQuaternion( angle2, q2 );
		QuaternionSlerp( q1, q2, s, q );
	}
	else
	{
		AngleQuaternion( angle1, q );
	}
}

/*
====================
R_StudioCalcBonePosition

====================
*/
void R_StudioCalcBonePosition( int frame, float s, mstudiobone_t *pbone, mstudioanim_t *panim, float *adj, float *pos )
{
	int					j, k;
	mstudioanimvalue_t	*panimvalue;

	for ( j = 0; j < 3; j++ )
	{
		pos[j] = pbone->value[j]; // default;
		if ( panim->offset[j] != 0 )
		{
			panimvalue = (mstudioanimvalue_t *)( (byte *)panim + panim->offset[j] );

			k = frame;
			// DEBUG
			if ( panimvalue->num.total < panimvalue->num.valid )
				k = 0;
			// find span of values that includes the frame we want
			while ( panimvalue->num.total <= k )
			{
				k -= panimvalue->num.total;
				panimvalue += panimvalue->num.valid + 1;
				// DEBUG
				if ( panimvalue->num.total < panimvalue->num.valid )
					k = 0;
			}
			// if we're inside the span
			if ( panimvalue->num.valid > k )
			{
				// and there's more data in the span
				if ( panimvalue->num.valid > k + 1 )
				{
					pos[j] += ( panimvalue[k+1].value * ( 1.0 - s ) + s * panimvalue[k+2].value ) * pbone->scale[j];
				}
				else
				{
					pos[j] += panimvalue[k+1].value * pbone->scale[j];
				}
			}
			else
			{
				// are we at the end of the repeating values section and there's another section with data?
				if ( panimvalue->num.total <= k + 1 )
				{
					pos[j] += ( panimvalue[panimvalue->num.valid].value * ( 1.0 - s ) + s * panimvalue[panimvalue->num.valid + 2].value ) * pbone->scale[j];
				}
				else
				{
					pos[j] += panimvalue[panimvalue->num.valid].value * pbone->scale[j];
				}
			}
		}
		if ( pbone->bonecontroller[j] != -1 && adj )
		{
			pos[j] += adj[pbone->bonecontroller[j]];
		}
	}
}

/*
====================
R_StudioSlerpBones

====================
*/
void R_StudioSlerpBones( vec4_t q1[], float pos1[][3], vec4_t q2[], float pos2[][3], float s )
{
	int			i;
	vec4_t		q3;
	float		s1;

	if ( s < 0 ) s = 0;
	else if ( s > 1.0 ) s = 1.0;

	s1 = 1.0 - s;

	for ( i = 0; i < pstudiohdr->numbones; i++ )
	{
		QuaternionSlerp( q1[i], q2[i], s, q3 );
		q1[i][0] = q3[0];
		q1[i][1] = q3[1];
		q1[i][2] = q3[2];
		q1[i][3] = q3[3];
		pos1[i][0] = pos1[i][0] * s1 + pos2[i][0] * s;
		pos1[i][1] = pos1[i][1] * s1 + pos2[i][1] * s;
		pos1[i][2] = pos1[i][2] * s1 + pos2[i][2] * s;
	}
}

/*
====================
R_GetAnim

====================
*/
mstudioanim_t *R_GetAnim( model_t *psubmodel, mstudioseqdesc_t *pseqdesc )
{
	mstudioseqgroup_t	*pseqgroup;
	cache_user_t		*paSequences;

	pseqgroup = (mstudioseqgroup_t *)( (byte *)pstudiohdr + pstudiohdr->seqgroupindex ) + pseqdesc->seqgroup;

	if ( pseqdesc->seqgroup == 0 )
	{
		return (mstudioanim_t *)( (byte *)pstudiohdr + pseqdesc->animindex );
	}

	paSequences = (cache_user_t *)psubmodel->submodels;

	if ( paSequences == NULL )
	{
		paSequences = (cache_user_t *)Mem_Calloc( 16, sizeof( cache_user_t ) ); // UNDONE: leak!
		psubmodel->submodels = (dmodel_t *)paSequences;
	}

	if ( !Cache_Check( (struct cache_user_s *)&( paSequences[pseqdesc->seqgroup] ) ) )
	{
		Con_DPrintf( "loading %s\n", pseqgroup->name );
		COM_LoadCacheFile( pseqgroup->name, (struct cache_user_s *)&paSequences[pseqdesc->seqgroup] );
	}
	return (mstudioanim_t *)( (byte *)paSequences[pseqdesc->seqgroup].data + pseqdesc->animindex );
}

/*
====================
SV_StudioSetupBones

====================
*/
void SV_StudioSetupBones( model_t *pModel, float frame, int sequence, const vec_t *angles, const vec_t *origin, const unsigned char *pcontroller, const unsigned char *pblending, int iBone, const edict_t *edict )
{
	int					i, j;
	float				f, s;
	float				adj[MAXSTUDIOCONTROLLERS];
	mstudiobone_t		*pbones;
	mstudioseqdesc_t	*pseqdesc;
	mstudioanim_t		*panim;

	static vec3_t		pos[MAXSTUDIOBONES];
	float				bonematrix[3][4];
	static vec4_t		q[MAXSTUDIOBONES];

	int					chain[MAXSTUDIOBONES];
	int					chainlength;

	chainlength = 0;

	if ( sequence < 0 || sequence >= pstudiohdr->numseq )
	{
		Con_DPrintf( "SV_StudioSetupBones:  sequence %i/%i out of range for model %s\n", sequence, pstudiohdr->numseq, pstudiohdr->name );
		sequence = 0;
	}

	pseqdesc = (mstudioseqdesc_t *)( (byte *)pstudiohdr + pstudiohdr->seqindex ) + sequence;
	pbones = (mstudiobone_t *)( (byte *)pstudiohdr + pstudiohdr->boneindex );
	panim = R_GetAnim( pModel, pseqdesc );

	if ( iBone < -1 || iBone >= pstudiohdr->numbones )
		iBone = 0;

	if ( iBone == -1 )
	{
		chainlength = pstudiohdr->numbones;
		for ( i = 0; i < pstudiohdr->numbones; i++ )
		{
			chain[chainlength - i - 1] = i;
		}
	}
	else
	{
		// only the parent bones
		i = iBone;
		while ( i != -1 )
		{
			chain[chainlength++] = i;
			i = pbones[i].parent;
		}
	}

	if ( pseqdesc->numframes <= 1 )
		f = 0;
	else
		f = ( pseqdesc->numframes - 1 ) * frame / 256.0;

	s = f - (int)f;

	R_StudioCalcBoneAdj( 0, adj, pcontroller, pcontroller, 0 );

	for ( j = chainlength - 1; j >= 0; j-- )
	{
		i = chain[j];
		R_StudioCalcBoneQuaterion( (int)f, s, &pbones[i], &panim[i], adj, q[i] );
		R_StudioCalcBonePosition( (int)f, s, &pbones[i], &panim[i], adj, pos[i] );
	}

	if ( pseqdesc->numblends > 1 )
	{
		static vec3_t	pos2[MAXSTUDIOBONES];
		static vec4_t	q2[MAXSTUDIOBONES];
		float			b;

		pseqdesc = (mstudioseqdesc_t *)( (byte *)pstudiohdr + pstudiohdr->seqindex ) + sequence;
		panim = R_GetAnim( pModel, pseqdesc );
		panim += pstudiohdr->numbones;

		for ( j = chainlength - 1; j >= 0; j-- )
		{
			i = chain[j];
			R_StudioCalcBoneQuaterion( (int)f, s, &pbones[i], &panim[i], adj, q2[i] );
			R_StudioCalcBonePosition( (int)f, s, &pbones[i], &panim[i], adj, pos2[i] );
		}

		b = *pblending / 255.0;

		R_StudioSlerpBones( q, pos, q2, pos2, b );
	}

	AngleMatrix( angles, rotationmatrix );

	rotationmatrix[0][3] = origin[0];
	rotationmatrix[1][3] = origin[1];
	rotationmatrix[2][3] = origin[2];

	for ( j = chainlength - 1; j >= 0; j-- )
	{
		i = chain[j];

		QuaternionMatrix( q[i], bonematrix );

		bonematrix[0][3] = pos[i][0];
		bonematrix[1][3] = pos[i][1];
		bonematrix[2][3] = pos[i][2];

		if ( pbones[i].parent == -1 )
			R_ConcatTransforms( rotationmatrix, bonematrix, bonetransform[i] );
		else
			R_ConcatTransforms( bonetransform[pbones[i].parent], bonematrix, bonetransform[i] );
	}
}

/*
====================
SV_SetStudioHullPlane

====================
*/
void SV_SetStudioHullPlane( mplane_t *pplane, int iBone, int k, float dist )
{
	pplane->type = 5;

	pplane->normal[0] = bonetransform[iBone][0][k];
	pplane->normal[1] = bonetransform[iBone][1][k];
	pplane->normal[2] = bonetransform[iBone][2][k];

	pplane->dist = bonetransform[iBone][0][3] * pplane->normal[0] + bonetransform[iBone][1][3] * pplane->normal[1] + bonetransform[iBone][2][3] * pplane->normal[2] + dist;
}

/*
====================
R_StudioHull

====================
*/
hull_t *R_StudioHull( model_t *pModel, float frame, int sequence, const vec_t *angles, const vec_t *origin, const vec_t *size, const unsigned char *pcontroller, const unsigned char *pblending, int *pNumHulls, const edict_t *pEdict, int bSkipShield )
{
	int					i, j;
	mstudiobbox_t		*pbbox;
	vec3_t				angles2;
	r_studiocache_t		*pCached = NULL;
	int					numHitBoxes;

	SV_InitStudioHull();

	if ( r_cachestudio.value )
	{
		pCached = R_CheckStudioCache( pModel, frame, sequence, angles, origin, size, pcontroller, pblending );

		if ( pCached )
		{
			Q_memcpy( studio_planes, &cache_planes[pCached->nStartPlane], pCached->numhulls * 6 * sizeof( mplane_t ) );
			Q_memcpy( studio_hull, &cache_hull[pCached->nStartHull], pCached->numhulls * sizeof( hull_t ) );
			Q_memcpy( studio_hull_hitgroup, &cache_hull_hitgroup[pCached->nStartHull], pCached->numhulls * sizeof( int ) );

			*pNumHulls = pCached->numhulls;
			return studio_hull;
		}
	}

	pstudiohdr = (studiohdr_t *)Mod_Extradata( pModel );

	angles2[ROLL] = angles[ROLL];
	angles2[PITCH] = -angles[PITCH];
	angles2[YAW] = angles[YAW];

	g_pSvBlendingAPI->SV_StudioSetupBones( pModel, frame, sequence, angles2, origin, pcontroller, pblending, -1, pEdict );

	pbbox = (mstudiobbox_t *)( (byte *)pstudiohdr + pstudiohdr->hitboxindex );

	for ( i = 0, j = 0; i < pstudiohdr->numhitboxes; i++ )
	{
		if ( bSkipShield == 1 && i == 21 )
		{
			j += 6;
			continue;
		}

		studio_hull_hitgroup[i] = pbbox[i].group;

		SV_SetStudioHullPlane( &studio_planes[j + 0], pbbox[i].bone, 0, pbbox[i].bbmax[0] );
		SV_SetStudioHullPlane( &studio_planes[j + 1], pbbox[i].bone, 0, pbbox[i].bbmin[0] );
		SV_SetStudioHullPlane( &studio_planes[j + 2], pbbox[i].bone, 1, pbbox[i].bbmax[1] );
		SV_SetStudioHullPlane( &studio_planes[j + 3], pbbox[i].bone, 1, pbbox[i].bbmin[1] );
		SV_SetStudioHullPlane( &studio_planes[j + 4], pbbox[i].bone, 2, pbbox[i].bbmax[2] );
		SV_SetStudioHullPlane( &studio_planes[j + 5], pbbox[i].bone, 2, pbbox[i].bbmin[2] );

		studio_planes[j + 0].dist += fabs( (double)studio_planes[j + 0].normal[0] * size[0] ) + fabs( (double)studio_planes[j + 0].normal[1] * size[1] ) + fabs( (double)studio_planes[j + 0].normal[2] * size[2] );
		studio_planes[j + 1].dist -= fabs( (double)studio_planes[j + 1].normal[0] * size[0] ) + fabs( (double)studio_planes[j + 1].normal[1] * size[1] ) + fabs( (double)studio_planes[j + 1].normal[2] * size[2] );
		studio_planes[j + 2].dist += fabs( (double)studio_planes[j + 2].normal[0] * size[0] ) + fabs( (double)studio_planes[j + 2].normal[1] * size[1] ) + fabs( (double)studio_planes[j + 2].normal[2] * size[2] );
		studio_planes[j + 3].dist -= fabs( (double)studio_planes[j + 3].normal[0] * size[0] ) + fabs( (double)studio_planes[j + 3].normal[1] * size[1] ) + fabs( (double)studio_planes[j + 3].normal[2] * size[2] );
		studio_planes[j + 4].dist += fabs( (double)studio_planes[j + 4].normal[0] * size[0] ) + fabs( (double)studio_planes[j + 4].normal[1] * size[1] ) + fabs( (double)studio_planes[j + 4].normal[2] * size[2] );
		studio_planes[j + 5].dist -= fabs( (double)studio_planes[j + 5].normal[0] * size[0] ) + fabs( (double)studio_planes[j + 5].normal[1] * size[1] ) + fabs( (double)studio_planes[j + 5].normal[2] * size[2] );

		j += 6;
	}

	if ( bSkipShield == 1 )
	{
		numHitBoxes = pstudiohdr->numhitboxes - 1;
		*pNumHulls = numHitBoxes;
	}
	else
	{
		numHitBoxes = pstudiohdr->numhitboxes;
		*pNumHulls = numHitBoxes;
	}

	if ( r_cachestudio.value )
		R_AddToStudioCache( frame, sequence, angles, origin, size, pcontroller, pblending, pModel, studio_hull, numHitBoxes );

	return studio_hull;
}

/*
====================
SV_HitgroupForStudioHull

====================
*/
int SV_HitgroupForStudioHull( int index )
{
	return studio_hull_hitgroup[index];
}

/*
================
R_InitStudioCache
================
*/
void R_InitStudioCache( void )
{
	Q_memset( rgStudioCache, 0, sizeof( rgStudioCache ) );

	r_cachecurrent = 0;
	nCurrentHull = 0;
	nCurrentPlane = 0;
}

/*
================
R_FlushStudioCache
================
*/
void R_FlushStudioCache( void )
{
	R_InitStudioCache();
}

/*
====================
R_StudioBodyVariations

====================
*/
int R_StudioBodyVariations( model_t *model )
{
	studiohdr_t			*pstudiohdr;
	mstudiobodyparts_t	*pbodypart;
	int					i, count;

	if ( model->type != mod_studio )
		return 0;

	pstudiohdr = (studiohdr_t *)Mod_Extradata( model );
	if ( !pstudiohdr )
		return 0;

	count = 1;
	pbodypart = (mstudiobodyparts_t *)( (byte *)pstudiohdr + pstudiohdr->bodypartindex );
	for ( i = 0; i < pstudiohdr->numbodyparts; i++ )
	{
		count *= pbodypart[i].nummodels;
	}

	return count;
}

/*
====================
R_StudioPlayerBlend

====================
*/
void R_StudioPlayerBlend( mstudioseqdesc_t *pseqdesc, int *pBlend, float *pPitch )
{
	// calc up/down pointing
	*pBlend = ( *pPitch * 3 );
	if ( *pBlend < pseqdesc->blendstart[0] )
	{
		*pPitch -= pseqdesc->blendstart[0] / 3.0;
		*pBlend = 0;
	}
	else if ( *pBlend > pseqdesc->blendend[0] )
	{
		*pPitch -= pseqdesc->blendend[0] / 3.0;
		*pBlend = 255;
	}
	else
	{
		if ( pseqdesc->blendend[0] - pseqdesc->blendstart[0] < 0.1 ) // catch qc error
			*pBlend = 127;
		else
			*pBlend = 255 * ( *pBlend - pseqdesc->blendstart[0] ) / ( pseqdesc->blendend[0] - pseqdesc->blendstart[0] );
		*pPitch = 0;
	}
}

/*
====================
SV_HullForStudioModel

====================
*/
hull_t *SV_HullForStudioModel( const edict_t *pEdict, const vec_t *mins, const vec_t *maxs, vec_t *offset, int *pNumHulls )
{
	qboolean	useComplexHull;
	vec3_t		size;
	float		factor;
	int			bSkipShield;

	useComplexHull = FALSE;
	bSkipShield = 0;
	factor = 0.5;

	VectorSubtract( maxs, mins, size );

	if ( VectorCompare( vec3_origin, size ) && !( gGlobalVariables.trace_flags & FTRACE_SIMPLEBOX ) )
	{
		useComplexHull = TRUE;

		if ( pEdict->v.flags & FL_CLIENT )
		{
			if ( sv_clienttrace.value != 0.0f )
			{
				factor = sv_clienttrace.value * 0.5;
				size[0] = size[1] = size[2] = 1.0;
			}
			else
			{
				useComplexHull = FALSE;
			}
		}
	}

	if ( pEdict->v.gamestate == 1 && ( g_bIsTerrorStrike == 1 || g_bIsCStrike == 1 || g_bIsCZero == 1 ) )
		bSkipShield = 1;

	if ( ( sv.models[pEdict->v.modelindex]->flags & STUDIO_TRACE_HITBOX ) || useComplexHull )
	{
		VectorScale( size, factor, size );

		offset[0] = 0;
		offset[1] = 0;
		offset[2] = 0;

		if ( pEdict->v.flags & FL_CLIENT )
		{
			vec3_t				angles;
			int					iBlend;
			unsigned char		blending[2];
			unsigned char		controller[4];
			mstudioseqdesc_t	*pseqdesc;

			pstudiohdr = (studiohdr_t *)Mod_Extradata( sv.models[pEdict->v.modelindex] );
			pseqdesc = (mstudioseqdesc_t *)( (byte *)pstudiohdr + pstudiohdr->seqindex ) + pEdict->v.sequence;

			VectorCopy( pEdict->v.angles, angles );
			R_StudioPlayerBlend( pseqdesc, &iBlend, &angles[PITCH] );

			blending[0] = iBlend;
			blending[1] = 0;

			controller[0] = 0x7F;
			controller[1] = 0x7F;
			controller[2] = 0x7F;
			controller[3] = 0x7F;

			return R_StudioHull( sv.models[pEdict->v.modelindex], pEdict->v.frame, pEdict->v.sequence, angles, pEdict->v.origin, size, controller, blending, pNumHulls, pEdict, bSkipShield );
		}
		else
		{
			return R_StudioHull( sv.models[pEdict->v.modelindex], pEdict->v.frame, pEdict->v.sequence, pEdict->v.angles, pEdict->v.origin, size, pEdict->v.controller, pEdict->v.blending, pNumHulls, pEdict, bSkipShield );
		}
	}
	else
	{
		*pNumHulls = 1;
		return SV_HullForEntity( (edict_t *)pEdict, mins, maxs, offset );
	}
}

/*
====================
DoesSphereIntersect

====================
*/
qboolean DoesSphereIntersect( float *vSphereCenter, float fSphereRadiusSquared, float *vLinePt, float *vLineDir )
{
	vec3_t		P;
	float		a, b, c;
	float		insideSqr;

	VectorSubtract( vLinePt, vSphereCenter, P );

	a = DotProduct( vLineDir, vLineDir );
	b = 2.0 * DotProduct( P, vLineDir );
	c = DotProduct( P, P ) - fSphereRadiusSquared;

	insideSqr = b * b - 4.0f * a * c;

	if ( insideSqr <= 0.000001f )
		return FALSE;

	return TRUE;
}

/*
====================
SV_CheckSphereIntersection

====================
*/
qboolean SV_CheckSphereIntersection( edict_t *ent, const vec_t *start, const vec_t *end )
{
	int					i;
	float				radiusSquared;
	vec3_t				maxDim;
	vec3_t				traceOrg, traceDir;
	studiohdr_t			*pstudiohdr;
	mstudioseqdesc_t	*pseqdesc;

	radiusSquared = 0;

	if ( !( ent->v.flags & FL_CLIENT ) )
		return TRUE;

	VectorCopy( start, traceOrg );
	VectorSubtract( end, start, traceDir );

	pstudiohdr = (studiohdr_t *)Mod_Extradata( sv.models[ent->v.modelindex] );
	pseqdesc = (mstudioseqdesc_t *)( (byte *)pstudiohdr + pstudiohdr->seqindex ) + ent->v.sequence;

	for ( i = 0; i < 3; i++ )
	{
		maxDim[i] = max( fabs( pseqdesc->bbmin[i] ), fabs( pseqdesc->bbmax[i] ) );
		radiusSquared += maxDim[i] * maxDim[i];
	}

	if ( DoesSphereIntersect( ent->v.origin, radiusSquared, traceOrg, traceDir ) )
		return TRUE;

	return FALSE;
}

/*
====================
AnimationAutomove

====================
*/
void AnimationAutomove( const edict_t *pEdict, float flTime )
{
}

/*
====================
GetBonePosition

====================
*/
void GetBonePosition( const edict_t *pEdict, int iBone, float *rgflOrigin, float *rgflAngles )
{
	pstudiohdr = (studiohdr_t *)Mod_Extradata( sv.models[pEdict->v.modelindex] );

	g_pSvBlendingAPI->SV_StudioSetupBones( sv.models[pEdict->v.modelindex], pEdict->v.frame, pEdict->v.sequence, pEdict->v.angles, pEdict->v.origin, pEdict->v.controller, pEdict->v.blending, iBone, pEdict );

	if ( rgflOrigin )
	{
		rgflOrigin[0] = bonetransform[iBone][0][3];
		rgflOrigin[1] = bonetransform[iBone][1][3];
		rgflOrigin[2] = bonetransform[iBone][2][3];
	}
}

/*
====================
GetAttachment

====================
*/
void GetAttachment( const edict_t *pEdict, int iAttachment, float *rgflOrigin, float *rgflAngles )
{
	mstudioattachment_t	*pattachment;
	vec3_t				angles;

	pstudiohdr = (studiohdr_t *)Mod_Extradata( sv.models[pEdict->v.modelindex] );
	pattachment = (mstudioattachment_t *)( (byte *)pstudiohdr + pstudiohdr->attachmentindex ) + iAttachment;

	angles[ROLL] = pEdict->v.angles[ROLL];
	angles[PITCH] = -pEdict->v.angles[PITCH];
	angles[YAW] = pEdict->v.angles[YAW];

	g_pSvBlendingAPI->SV_StudioSetupBones( sv.models[pEdict->v.modelindex], pEdict->v.frame, pEdict->v.sequence, angles, pEdict->v.origin, pEdict->v.controller, pEdict->v.blending, pattachment->bone, pEdict );

	if ( rgflOrigin )
		VectorTransform( pattachment->org, bonetransform[pattachment->bone], rgflOrigin );
}

/*
====================
ModelFrameCount

====================
*/
int ModelFrameCount( model_t *model )
{
	int		count;

	count = 1;

	if ( !model )
		return count;

	if ( model->type == mod_sprite )
	{
		msprite_t	*psprite;

		psprite = (msprite_t *)model->cache.data;
		count = psprite->numframes;
	}
	else if ( model->type == mod_studio )
	{
		count = R_StudioBodyVariations( model );
	}

	if ( count < 1 )
		count = 1;

	return count;
}

/*
================
R_StudioCheckBBox
================
*/
qboolean R_StudioCheckBBox( void )
{
	static vec3_t		defaultmins_guess = { -16, -16, -16 };
	static vec3_t		defaultmaxs_guess = { 16, 16, 16 };

	mplane_t			plane;
	vec3_t				mins, maxs;
	int					i;
	mstudioseqdesc_t	*pseqdesc;

	// compute the bounding box
	if ( !VectorCompare( vec3_origin, pstudiohdr->bbmin ) )
	{
		VectorAdd( currententity->origin, pstudiohdr->bbmin, mins );
		VectorAdd( currententity->origin, pstudiohdr->bbmax, maxs );
	}
	else if ( !VectorCompare( vec3_origin, pstudiohdr->min ) )
	{
		VectorAdd( currententity->origin, pstudiohdr->min, mins );
		VectorAdd( currententity->origin, pstudiohdr->max, maxs );
	}
	else
	{
		VectorAdd( defaultmins_guess, currententity->origin, mins );
		VectorAdd( defaultmaxs_guess, currententity->origin, maxs );
	}

	if ( currententity->curstate.sequence >= pstudiohdr->numseq )
		currententity->curstate.sequence = 0;

	pseqdesc = (mstudioseqdesc_t *)( (byte *)pstudiohdr + pstudiohdr->seqindex ) + currententity->curstate.sequence;

	for ( i = 0; i < 8; i++ )
	{
		vec3_t	p1, p2;

		p1[0] = ( i & 1 ) ? pseqdesc->bbmin[0] : pseqdesc->bbmax[0];
		p1[1] = ( i & 2 ) ? pseqdesc->bbmin[1] : pseqdesc->bbmax[1];
		p1[2] = ( i & 4 ) ? pseqdesc->bbmin[2] : pseqdesc->bbmax[2];

		R_StudioTransformVector( p1, p2 );

		if ( p2[0] < mins[0] ) mins[0] = p2[0];
		if ( p2[0] > maxs[0] ) maxs[0] = p2[0];
		if ( p2[1] < mins[1] ) mins[1] = p2[1];
		if ( p2[1] > maxs[1] ) maxs[1] = p2[1];
		if ( p2[2] < mins[2] ) mins[2] = p2[2];
		if ( p2[2] > maxs[2] ) maxs[2] = p2[2];
	}

	if ( !Host_IsSinglePlayerGame() && r_cullsequencebox.value )
	{
		if ( R_CullBox( mins, maxs ) )
			return FALSE;
	}
	else
	{
		VectorCopy( vpn, plane.normal );
		plane.dist = DotProduct( r_origin, plane.normal );
		plane.type = 5;
		plane.signbits = SignbitsForPlane( &plane );

		if ( BoxOnPlaneSide( mins, maxs, &plane ) == 2 )
			return FALSE;
	}

	return TRUE;
}

/*
================
R_StudioTransformVector
================
*/
void R_StudioTransformVector( vec_t *in, vec_t *out )
{
	out[0] = DotProduct( in, rotationmatrix[0] ) + rotationmatrix[0][3];
	out[1] = DotProduct( in, rotationmatrix[1] ) + rotationmatrix[1][3];
	out[2] = DotProduct( in, rotationmatrix[2] ) + rotationmatrix[2][3];
}

/*
================
BuildNormalIndexTable
================
*/
void BuildNormalIndexTable( void )
{
	int				j;
	int				i;
	mstudiomesh_t	*pmesh;

	// use the first normal of each vertex
	for ( i = 0; i < psubmodel->numverts; i++ )
		g_NormalIndex[i] = -1;

	for ( j = 0; j < psubmodel->nummesh; j++ )
	{
		short	*ptricmds;

		pmesh = (mstudiomesh_t *)( (byte *)pstudiohdr + psubmodel->meshindex ) + j;
		ptricmds = (short *)( (byte *)pstudiohdr + pmesh->triindex );

		while ( i = *( ptricmds++ ) )
		{
			if ( i < 0 )
				i = -i;

			for ( ; i > 0; i--, ptricmds += 4 )
			{
				if ( g_NormalIndex[ptricmds[0]] < 0 )
					g_NormalIndex[ptricmds[0]] = ptricmds[1];
			}
		}
	}
}

/*
================
BuildGlowShellVerts
================
*/
void BuildGlowShellVerts( vec3_t *pstudioverts, auxvert_t *pauxverts )
{
	int			i;
	byte		*pvertbone;
	vec3_t		*pstudionorms;
	auxvert_t	*av;
	float		vscale;

	vscale = currententity->curstate.renderamt * 0.05;

	pvertbone = ( (byte *)pstudiohdr + psubmodel->vertinfoindex );
	pstudionorms = (vec3_t *)( (byte *)pstudiohdr + psubmodel->normindex );

	for ( i = 0; i < pstudiohdr->numbones; i++ )
		chromeage[i] = 0;

	for ( av = pauxverts, i = 0; i < psubmodel->numverts; i++, av++, pstudioverts++ )
	{
		vec3_t	vert;

		VectorMA( pstudioverts[0], vscale, pstudionorms[g_NormalIndex[i]], vert );
		R_StudioTransformAuxVert( av, pvertbone[i], vert );
	}

	g_ChromeOrigin[0] = cos( r_glowshellfreq.value * cl.time ) * 4000.0;
	g_ChromeOrigin[1] = sin( r_glowshellfreq.value * cl.time ) * 4000.0;
	g_ChromeOrigin[2] = cos( r_glowshellfreq.value * cl.time * 0.33 ) * 4000.0;

	tri.Color4ub( currententity->curstate.rendercolor.r, currententity->curstate.rendercolor.g, currententity->curstate.rendercolor.b, 255 );
}

/*
================
IsFlippedViewModel
================
*/
qboolean IsFlippedViewModel( void )
{
	if ( cl_righthand && cl_righthand->value > 0 && &cl.viewent && cl.viewent.index == currententity->index )
		return TRUE;

	return FALSE;
}

/*
================
R_StudioSetUpTransform
================
*/
void R_StudioSetUpTransform( int trivial_accept )
{
	int				i;
	vec3_t			angles;
	vec3_t			modelpos;

	VectorCopy( currententity->origin, modelpos );

	angles[ROLL] = currententity->curstate.angles[ROLL];
	angles[PITCH] = currententity->curstate.angles[PITCH];
	angles[YAW] = currententity->curstate.angles[YAW];

	if ( currententity->curstate.movetype == MOVETYPE_STEP )
	{
		float			f = 0;
		float			d;

		// don't do it if the goalstarttime hasn't updated in a while.

		// NOTE:  Because we need to interpolate multiplayer characters, the interpolation time limit
		//  was increased to 1.0 s., which is 2x the max lag we are accounting for.

		if ( ( cl.time < currententity->curstate.animtime + 1.0f ) &&
			( currententity->curstate.animtime != currententity->latched.prevanimtime ) )
		{
			f = ( cl.time - currententity->curstate.animtime ) / ( currententity->curstate.animtime - currententity->latched.prevanimtime );
		}

		if ( r_dointerp )
		{
			// ugly hack to interpolate angle, position. current is reached 0.1 seconds after being set
			f = f - 1.0;
		}
		else
		{
			f = 0;
		}

		for ( i = 0; i < 3; i++ )
		{
			modelpos[i] += ( currententity->origin[i] - currententity->latched.prevorigin[i] ) * f;
		}

		for ( i = 0; i < 3; i++ )
		{
			float ang1, ang2;

			ang1 = currententity->angles[i];
			ang2 = currententity->latched.prevangles[i];

			d = ang1 - ang2;
			if ( d > 180 )
			{
				d -= 360;
			}
			else if ( d < -180 )
			{
				d += 360;
			}

			angles[i] += d * f;
		}
	}
	else if ( currententity->curstate.movetype != MOVETYPE_NONE )
	{
		VectorCopy( currententity->angles, angles );
	}

	angles[PITCH] = -angles[PITCH];
	AngleMatrix( angles, rotationmatrix );

	rotationmatrix[0][3] = modelpos[0];
	rotationmatrix[1][3] = modelpos[1];
	rotationmatrix[2][3] = modelpos[2];
}

/*
================
VectorIRotate
================
*/
void VectorIRotate( vec_t *in1, float in2[3][4], vec_t *out )
{
	out[0] = in1[0]*in2[0][0] + in1[1]*in2[1][0] + in1[2]*in2[2][0];
	out[1] = in1[0]*in2[0][1] + in1[1]*in2[1][1] + in1[2]*in2[2][1];
	out[2] = in1[0]*in2[0][2] + in1[1]*in2[1][2] + in1[2]*in2[2][2];
}

/*
====================
CL_StudioEstimateInterpolant

====================
*/
float CL_StudioEstimateInterpolant( void )
{
	float dadt;

	if ( r_dointerp && ( currententity->curstate.animtime >= currententity->latched.prevanimtime + 0.01 ) )
	{
		dadt = ( cl.time - currententity->curstate.animtime ) / 0.1;
		if ( dadt > 2.0 )
		{
			dadt = 2.0;
		}
	}
	else
	{
		dadt = 1.0;
	}
	return dadt;
}

/*
====================
R_StudioCalcRotations

====================
*/
void R_StudioCalcRotations( float pos[][3], vec4_t *q, mstudioseqdesc_t *pseqdesc, mstudioanim_t *panim, float f )
{
	int					i;
	int					frame;
	mstudiobone_t		*pbone;

	float				s;
	float				adj[MAXSTUDIOCONTROLLERS];
	float				dadt;

	if ( f > pseqdesc->numframes - 1 )
	{
		f = 0;	// bah, fix this bug with changing sequences too fast
	}
	// BUG ( somewhere else ) but this code should validate this data.
	// This could cause a crash if the frame # is negative, so we'll go ahead
	//  and clamp it here
	else if ( f < -0.01 )
	{
		f = -0.01;
	}

	frame = (int)f;

	dadt = CL_StudioEstimateInterpolant();
	s = ( f - frame );

	// add in programtic controllers
	pbone = (mstudiobone_t *)( (byte *)pstudiohdr + pstudiohdr->boneindex );

	R_StudioCalcBoneAdj( dadt, adj, currententity->curstate.controller, currententity->latched.prevcontroller, currententity->mouth.mouthopen );

	for ( i = 0; i < pstudiohdr->numbones; i++, pbone++, panim++ )
	{
		R_StudioCalcBoneQuaterion( frame, s, pbone, panim, adj, q[i] );

		R_StudioCalcBonePosition( frame, s, pbone, panim, adj, pos[i] );
	}

	if ( pseqdesc->motiontype & STUDIO_X )
	{
		pos[pseqdesc->motionbone][0] = 0.0;
	}
	if ( pseqdesc->motiontype & STUDIO_Y )
	{
		pos[pseqdesc->motionbone][1] = 0.0;
	}
	if ( pseqdesc->motiontype & STUDIO_Z )
	{
		pos[pseqdesc->motionbone][2] = 0.0;
	}

	s = 0 * ( ( 1.0 - ( f - (int)( f ) ) ) / ( pseqdesc->numframes ) ) * currententity->curstate.framerate;

	if ( pseqdesc->motiontype & STUDIO_LX )
	{
		pos[pseqdesc->motionbone][0] += s * pseqdesc->linearmovement[0];
	}
	if ( pseqdesc->motiontype & STUDIO_LY )
	{
		pos[pseqdesc->motionbone][1] += s * pseqdesc->linearmovement[1];
	}
	if ( pseqdesc->motiontype & STUDIO_LZ )
	{
		pos[pseqdesc->motionbone][2] += s * pseqdesc->linearmovement[2];
	}
}

/*
====================
CL_StudioEstimateFrame

====================
*/
float CL_StudioEstimateFrame( mstudioseqdesc_t *pseqdesc )
{
	double				dfdt, f;

	if ( r_dointerp )
	{
		if ( cl.time < currententity->curstate.animtime )
		{
			dfdt = 0;
		}
		else
		{
			dfdt = ( cl.time - currententity->curstate.animtime ) * currententity->curstate.framerate * pseqdesc->fps;
		}
	}
	else
	{
		dfdt = 0;
	}

	if ( pseqdesc->numframes <= 1 )
	{
		f = 0;
	}
	else
	{
		float	baseframe;

		baseframe = currententity->curstate.frame;
		f = ( baseframe * ( pseqdesc->numframes - 1 ) ) / 256.0;
	}

	f += dfdt;

	if ( pseqdesc->flags & STUDIO_LOOPING )
	{
		if ( pseqdesc->numframes > 1 )
		{
			f -= (int)( f / ( pseqdesc->numframes - 1 ) ) * ( pseqdesc->numframes - 1 );
		}
		if ( f < 0 )
		{
			f += ( pseqdesc->numframes - 1 );
		}
	}
	else
	{
		if ( f >= pseqdesc->numframes - 1.001 )
		{
			f = pseqdesc->numframes - 1.001;
		}
		if ( f < 0.0 )
		{
			f = 0.0;
		}
	}
	return f;
}

/*
====================
CL_GetEstimatedFrame

====================
*/
float CL_GetEstimatedFrame( cl_entity_t *ent )
{
	float				f;
	studiohdr_t			*pstudiohdr;
	mstudioseqdesc_t	*pseqdesc;
	cl_entity_t			*oldce;

	oldce = currententity;

	if ( !ent->model )
		return 0;

	if ( ent->model->type != mod_studio )
		return 0;

	pstudiohdr = (studiohdr_t *)Mod_Extradata( ent->model );
	if ( !pstudiohdr )
		return 0;

	pseqdesc = (mstudioseqdesc_t *)( (byte *)pstudiohdr + pstudiohdr->seqindex ) + ent->curstate.sequence;

	currententity = ent;
	f = CL_StudioEstimateFrame( pseqdesc );
	currententity = oldce;

	return f;
}

/*
====================
R_StudioSetupBones

====================
*/
void R_StudioSetupBones( void )
{
	int					i;
	double				f;

	mstudiobone_t		*pbones;
	mstudioseqdesc_t	*pseqdesc;
	mstudioanim_t		*panim;

	static float		pos[MAXSTUDIOBONES][3];
	static vec4_t		q[MAXSTUDIOBONES];
	float				bonematrix[3][4];

	static float		pos2[MAXSTUDIOBONES][3];
	static vec4_t		q2[MAXSTUDIOBONES];
	static float		pos3[MAXSTUDIOBONES][3];
	static vec4_t		q3[MAXSTUDIOBONES];
	static float		pos4[MAXSTUDIOBONES][3];
	static vec4_t		q4[MAXSTUDIOBONES];

	if ( currententity->curstate.sequence >= pstudiohdr->numseq )
	{
		currententity->curstate.sequence = 0;
	}

	pseqdesc = (mstudioseqdesc_t *)( (byte *)pstudiohdr + pstudiohdr->seqindex ) + currententity->curstate.sequence;

	f = CL_StudioEstimateFrame( pseqdesc );

	if ( currententity->latched.prevframe > f )
		;	// Con_DPrintf( "%f %f\n", currententity->latched.prevframe, f );

	panim = R_GetAnim( r_model, pseqdesc );
	R_StudioCalcRotations( pos, q, pseqdesc, panim, f );

	if ( pseqdesc->numblends > 1 )
	{
		float				s;
		float				dadt;

		panim += pstudiohdr->numbones;
		R_StudioCalcRotations( pos2, q2, pseqdesc, panim, f );

		dadt = CL_StudioEstimateInterpolant();
		s = ( currententity->curstate.blending[0] * dadt + currententity->latched.prevblending[0] * ( 1.0 - dadt ) ) / 255.0;

		R_StudioSlerpBones( q, pos, q2, pos2, s );

		if ( pseqdesc->numblends == 4 )
		{
			panim += pstudiohdr->numbones;
			R_StudioCalcRotations( pos3, q3, pseqdesc, panim, f );

			panim += pstudiohdr->numbones;
			R_StudioCalcRotations( pos4, q4, pseqdesc, panim, f );

			s = ( currententity->curstate.blending[0] * dadt + currententity->latched.prevblending[0] * ( 1.0 - dadt ) ) / 255.0;
			R_StudioSlerpBones( q3, pos3, q4, pos4, s );

			s = ( currententity->curstate.blending[1] * dadt + currententity->latched.prevblending[1] * ( 1.0 - dadt ) ) / 255.0;
			R_StudioSlerpBones( q, pos, q3, pos3, s );
		}
	}

	if ( r_dointerp &&
		currententity->latched.sequencetime &&
		( currententity->latched.sequencetime + 0.2 > cl.time ) &&
		( currententity->latched.prevsequence < pstudiohdr->numseq ) )
	{
		// blend from last sequence
		static float		pos1b[MAXSTUDIOBONES][3];
		static vec4_t		q1b[MAXSTUDIOBONES];
		float				s;

		pseqdesc = (mstudioseqdesc_t *)( (byte *)pstudiohdr + pstudiohdr->seqindex ) + currententity->latched.prevsequence;
		panim = R_GetAnim( r_model, pseqdesc );
		// clip prevframe
		R_StudioCalcRotations( pos1b, q1b, pseqdesc, panim, currententity->latched.prevframe );

		if ( pseqdesc->numblends > 1 )
		{
			panim += pstudiohdr->numbones;
			R_StudioCalcRotations( pos2, q2, pseqdesc, panim, currententity->latched.prevframe );

			s = ( currententity->latched.prevseqblending[0] ) / 255.0;
			R_StudioSlerpBones( q1b, pos1b, q2, pos2, s );

			if ( pseqdesc->numblends == 4 )
			{
				panim += pstudiohdr->numbones;
				R_StudioCalcRotations( pos3, q3, pseqdesc, panim, currententity->latched.prevframe );

				panim += pstudiohdr->numbones;
				R_StudioCalcRotations( pos4, q4, pseqdesc, panim, currententity->latched.prevframe );

				s = ( currententity->latched.prevseqblending[0] ) / 255.0;
				R_StudioSlerpBones( q3, pos3, q4, pos4, s );

				s = ( currententity->latched.prevseqblending[1] ) / 255.0;
				R_StudioSlerpBones( q1b, pos1b, q3, pos3, s );
			}
		}

		s = 1.0 - ( cl.time - currententity->latched.sequencetime ) / 0.2;
		R_StudioSlerpBones( q, pos, q1b, pos1b, s );
	}
	else
	{
		currententity->latched.prevframe = f;
	}

	pbones = (mstudiobone_t *)( (byte *)pstudiohdr + pstudiohdr->boneindex );

	// calc gait animation
	if ( r_playerinfo && r_playerinfo->gaitsequence != 0 )
	{
		if ( r_playerinfo->gaitsequence >= pstudiohdr->numseq )
		{
			r_playerinfo->gaitsequence = 0;
		}

		pseqdesc = (mstudioseqdesc_t *)( (byte *)pstudiohdr + pstudiohdr->seqindex ) + r_playerinfo->gaitsequence;

		panim = R_GetAnim( r_model, pseqdesc );
		R_StudioCalcRotations( pos2, q2, pseqdesc, panim, r_playerinfo->gaitframe );

		for ( i = 0; i < pstudiohdr->numbones; i++ )
		{
			if ( !Q_strcmp( pbones[i].name, "Bip01 Spine" ) )
				break;

			Q_memcpy( pos[i], pos2[i], sizeof( pos[i] ) );
			Q_memcpy( q[i], q2[i], sizeof( q[i] ) );
		}
	}

	for ( i = 0; i < pstudiohdr->numbones; i++ )
	{
		QuaternionMatrix( q[i], bonematrix );

		bonematrix[0][3] = pos[i][0];
		bonematrix[1][3] = pos[i][1];
		bonematrix[2][3] = pos[i][2];

		if ( pbones[i].parent == -1 )
		{
			R_ConcatTransforms( rotationmatrix, bonematrix, bonetransform[i] );
			R_ConcatTransforms( rotationmatrix, bonematrix, lighttransform[i] );

			// Apply client-side effects to the transformation matrix
			CL_FxTransform( currententity, bonetransform[i] );
		}
		else
		{
			R_ConcatTransforms( bonetransform[pbones[i].parent], bonematrix, bonetransform[i] );
			R_ConcatTransforms( lighttransform[pbones[i].parent], bonematrix, lighttransform[i] );
		}
	}
}

/*
====================
MatrixCopy

====================
*/
void MatrixCopy( float in[3][4], float out[3][4] )
{
	Q_memcpy( out, in, sizeof( float ) * 3 * 4 );
}

/*
====================
R_StudioSaveBones

====================
*/
void R_StudioSaveBones( void )
{
	int					i;

	mstudiobone_t		*pbones;
	pbones = (mstudiobone_t *)( (byte *)pstudiohdr + pstudiohdr->boneindex );

	cached_numbones = pstudiohdr->numbones;

	for ( i = 0; i < pstudiohdr->numbones; i++ )
	{
		Q_strncpy( cached_bonename[i], pbones[i].name, sizeof( cached_bonename[i] ) - 1 );
		cached_bonename[i][sizeof( cached_bonename[i] ) - 1] = 0;
		MatrixCopy( bonetransform[i], cached_bonetransform[i] );
		MatrixCopy( lighttransform[i], cached_lighttransform[i] );
	}
}

/*
====================
R_StudioMergeBones

====================
*/
void R_StudioMergeBones( model_t *psubmodel )
{
	int					i, j;
	double				f;
	int					do_hunt = TRUE;

	mstudiobone_t		*pbones;
	mstudioseqdesc_t	*pseqdesc;
	mstudioanim_t		*panim;

	static float		pos[MAXSTUDIOBONES][3];
	float				bonematrix[3][4];
	static vec4_t		q[MAXSTUDIOBONES];

	if ( currententity->curstate.sequence >= pstudiohdr->numseq )
	{
		currententity->curstate.sequence = 0;
	}

	pseqdesc = (mstudioseqdesc_t *)( (byte *)pstudiohdr + pstudiohdr->seqindex ) + currententity->curstate.sequence;

	f = 0;
	f = CL_StudioEstimateFrame( pseqdesc );

	if ( currententity->latched.prevframe > f )
		;	// Con_DPrintf( "%f %f\n", currententity->latched.prevframe, f );

	panim = R_GetAnim( psubmodel, pseqdesc );
	R_StudioCalcRotations( pos, q, pseqdesc, panim, f );

	pbones = (mstudiobone_t *)( (byte *)pstudiohdr + pstudiohdr->boneindex );

	for ( i = 0; i < pstudiohdr->numbones; i++ )
	{
		for ( j = 0; j < cached_numbones; j++ )
		{
			if ( Q_stricmp( pbones[i].name, cached_bonename[j] ) == 0 )
			{
				MatrixCopy( cached_bonetransform[j], bonetransform[i] );
				MatrixCopy( cached_lighttransform[j], lighttransform[i] );
				break;
			}
		}
		if ( j >= cached_numbones )
		{
			QuaternionMatrix( q[i], bonematrix );

			bonematrix[0][3] = pos[i][0];
			bonematrix[1][3] = pos[i][1];
			bonematrix[2][3] = pos[i][2];

			if ( pbones[i].parent == -1 )
			{
				R_ConcatTransforms( rotationmatrix, bonematrix, bonetransform[i] );
				R_ConcatTransforms( rotationmatrix, bonematrix, lighttransform[i] );

				// Apply client-side effects to the transformation matrix
				CL_FxTransform( currententity, bonetransform[i] );
			}
			else
			{
				R_ConcatTransforms( bonetransform[pbones[i].parent], bonematrix, bonetransform[i] );
				R_ConcatTransforms( lighttransform[pbones[i].parent], bonematrix, lighttransform[i] );
			}
		}
	}
}
