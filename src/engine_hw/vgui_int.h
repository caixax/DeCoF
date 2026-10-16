// vgui_int.h -- the engine's side of the VGUI2 game UI (vgui_intwrap2.cpp, vgui2/text_draw.cpp)

#ifndef VGUI_INT_H
#define VGUI_INT_H

int		VGuiWrap2_IsConsoleVisible( void );
void	VGuiWrap2_ShowConsole( void );
void	VGuiWrap2_HideConsole( void );
void	VGuiWrap2_ClearConsole( void );
void	VGuiWrap2_ConPrintf( const char *msg );
void	VGuiWrap2_ConDPrintf( const char *msg );
int		VGuiWrap2_IsInCareerMatch( void );
void	VGuiWrap2_HideGameUI( void );

int		VGui_GameUIKeyPressed( void );

unsigned int	VGUI2_GetConsoleFont( void );
int		VGUI2_GetFontTall( unsigned int font );

void	VGuiWrap2_NotifyOfServerConnect( const char *game, int IP, int port );
void	VGuiWrap2_NotifyOfServerDisconnect( void );
void	VGuiWrap2_LoadingFinished( const char *resourceType, const char *resourceName );
void	StartLoadingProgressBar( const char *loadingType, int numProgressPoints );
void	ContinueLoadingProgressBar( const char *loadingType, int progressPoint, float progressFraction );
void	SetLoadingProgressBarStatusText( const char *statusText );
void	StopLoadingProgressBar( void );
void	SetSecondaryProgressBar( float progress );
void	SetSecondaryProgressBarText( const char *statusText );

// C wrappers around the object at 0x01EDF808 (names unproven)
void	LevelLoaded_guess( struct model_s *worldmodel );	// 0x01D10DBF, CL_SignonReply
void	ResetLevelState_guess( void );						// 0x01D10E09, CL_Disconnect and CL_ParseServerInfo

#endif // VGUI_INT_H
