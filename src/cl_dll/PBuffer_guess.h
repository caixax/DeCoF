// Cry of Fear pixel buffer (WGL_ARB_pbuffer) that renders into a texture.
// Only the constructor and the destructor are called.

#ifndef GUESS_PBUFFER_H
#define GUESS_PBUFFER_H

DECLARE_HANDLE( HPBUFFERARB );

class guess_CPBuffer
{
public:
	guess_CPBuffer();
	~guess_CPBuffer();

	void guess_BindTexture( void );
	void guess_Create( void );
	void guess_MakeCurrent( void );
	void guess_Prepare( void );
	void guess_ReleaseTexture( void );
	void guess_Shutdown( void );
	void guess_SwitchToWindow( void );

	bool	guess_m_bValid;
	int		guess_m_iHeight;
	int		guess_m_iWidth;
	GLuint	guess_m_iTexture;
	HDC		guess_m_hWindowDC;
	HDC		guess_m_hDC;
	HGLRC	guess_m_hRC;
	HPBUFFERARB guess_m_hPBuffer;
	HGLRC	guess_m_hWindowRC;
};

extern guess_CPBuffer guess_g_PBuffer;

#endif // GUESS_PBUFFER_H
