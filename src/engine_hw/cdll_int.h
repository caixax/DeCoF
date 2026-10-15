// cdll_int.h -- the engine's calls into the client dll (cdll_int.c)

#ifndef CDLL_INT_H
#define CDLL_INT_H

void	ClientDLL_ChatInputPosition( int *x, int *y );
int		ClientDLL_Key_Event( int down, int keynum, const char *pszCurrentBinding );

#endif // CDLL_INT_H
