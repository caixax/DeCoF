// screen.h -- the screen refresh (gl_screen.c)

#ifndef SCREEN_H
#define SCREEN_H

extern	float	scr_con_current;
extern	float	scr_centertime_off;
extern	int		clearnotify;	// set to 0 whenever notify text is drawn
extern	int		scr_copytop;

extern	cvar_t	scr_downloading;	// percent of the current download, -1 when none

void	SCR_EndLoadingPlaque( void );
void	SCR_CenterPrint( char *str );

extern	cvar_t	scr_graphheight;

#endif // SCREEN_H
