// common.h -- general purpose engine functions (common.c)

#ifndef COMMON_H
#define COMMON_H

extern char		com_gamedir[];

extern int		(*LittleLong)( int l );

void	Q_memset( void *dest, int fill, int count );
void	Q_memcpy( void *dest, const void *src, int count );

#endif // COMMON_H
