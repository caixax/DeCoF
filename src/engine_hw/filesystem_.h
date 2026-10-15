// filesystem_.h -- the engine's wrappers around the filesystem interface (filesystem.cpp)

#ifndef FILESYSTEM__H
#define FILESYSTEM__H

#define FILESYSTEM_SEEK_HEAD	0

FileHandle_t	FS_Open( const char *pFileName, const char *pOptions );
void			FS_Close( FileHandle_t file );
void			FS_Seek( FileHandle_t file, int pos, int seekType );
unsigned int	FS_Tell( FileHandle_t file );
unsigned int	FS_Size( FileHandle_t file );
int				FS_IsOk( FileHandle_t file );
int				FS_EndOfFile( FileHandle_t file );
int				FS_Read( void *pOutput, int size, int count, FileHandle_t file );

#endif // FILESYSTEM__H
