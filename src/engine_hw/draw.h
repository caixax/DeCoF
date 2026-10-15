// draw.h -- these are the only functions outside the refresh allowed
// to touch the vid buffer (gl_draw.c)

#ifndef DRAW_H
#define DRAW_H

int		Draw_Character( int x, int y, int num, unsigned int font );
int		Draw_String( int x, int y, char *str );
int		Draw_StringLen( const char *psz, unsigned int font );
void	Draw_SetTextColor( float r, float g, float b );
void	Draw_ResetTextColor( void );

#endif // DRAW_H
