// sys.h -- non-portable functions (sys_dll.c)

#ifndef SYS_H
#define SYS_H

void	Sys_Error( char *error, ... );
void	Sys_Printf( char *fmt, ... );

const char	*Sys_FindFirst( const char *path, char *basename );
const char	*Sys_FindNext( char *basename );
void	Sys_FindClose( void );

extern	int		giActive;
extern	int		giStateInfo;
extern	int		giSubState;

#endif // SYS_H
