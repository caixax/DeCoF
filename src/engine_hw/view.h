// view.h -- player eye positioning and the gamma tables

#ifndef VIEW_H
#define VIEW_H

extern	cvar_t		v_gamma;
extern	cvar_t		v_lightgamma;
extern	cvar_t		v_brightness;

extern	byte		texgammatable[256];	// palette is sent through this to convert to screen gamma
extern	int			lightgammatable[1024];
extern	int			lineargammatable[1024];
extern	int			screengammatable[1024];

extern	float		v_blend[4];

#endif // VIEW_H
