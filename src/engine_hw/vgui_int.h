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

#endif // VGUI_INT_H
