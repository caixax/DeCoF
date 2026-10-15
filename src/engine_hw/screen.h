// screen.h -- the screen refresh (gl_screen.c)

#ifndef SCREEN_H
#define SCREEN_H

extern	float	scr_con_current;
extern	int		clearnotify;	// set to 0 whenever notify text is drawn
extern	int		scr_copytop;

extern	float	scr_downloading;	// percent of the current download, -1 when none

#endif // SCREEN_H
