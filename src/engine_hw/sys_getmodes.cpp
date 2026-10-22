// sys_getmodes.cpp

extern "C"
{
#include "quakedef.h"
}
#include "../launcher/iregistry.h"
#include "../public/UtlVector.h"

typedef struct vmode_s
{
	int			width;
	int			height;
	int			bpp;
} vmode_t;

//-----------------------------------------------------------------------------
// Purpose: Video mode interface
//-----------------------------------------------------------------------------
class IVideoMode
{
public:
	virtual const char	*GetName() = 0;
};

class CVideoMode_Common : public IVideoMode
{
public:
	CVideoMode_Common( void );

protected:
	struct bimage_t
	{
		byte		*buffer;
		int			x, y, width, height;
		bool		scaled;
	};

	vmode_t			m_rgModeList[ 32 ];
	vmode_t			m_safeMode;
	int				m_nNumModes;
	int				m_nGameMode;
	bool			m_bInitialized;
	CUtlVector< bimage_t > m_ImageID;
	int				m_iBaseResX;
	int				m_iBaseResY;
	bool			m_bWindowed;
};

class CVideoMode_OpenGL : public CVideoMode_Common
{
public:
	CVideoMode_OpenGL( bool windowed );

	virtual const char	*GetName();
};

IVideoMode *videomode;

extern "C" void VideoMode_Create( void )
{
	bool windowed = false;

	windowed = registry->ReadInt( "ScreenWindowed", 0 ) ? true : false;

	if ( COM_CheckParm( "-sw" ) || COM_CheckParm( "-startwindowed" ) || COM_CheckParm( "-windowed" ) || COM_CheckParm( "-window" ) )
	{
		windowed = true;
	}
	else if ( COM_CheckParm( "-full" ) || COM_CheckParm( "-fullscreen" ) )
	{
		windowed = false;
	}

	registry->WriteInt( "ScreenWindowed", windowed ? 1 : 0 );

	videomode = new CVideoMode_OpenGL( windowed );
}
