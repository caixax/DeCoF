// ======================================
// Cry of Fear note document panel
// (class names from the RTTI of client.dll, file name guessed)
// ======================================

#include <fstream>
#include <string>
#include <vector>

#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "vgui_TeamFortressViewport.h"
#include "guess_NoteDocument.h"
#include "..\game_shared\vgui_loadtga.h"

CNoteDocument::CNoteDocument( int iTrans, int iRemoveMe, int x, int y, int wide, int tall ) : CMenuPanel( iTrans, iRemoveMe, x, y, wide, tall )
{
	int iXPos, iYPos;
	int iWide, iTall;
	int iX, iY;
	Panel *pPanel;
	CommandButton *pButton;

	guess_m_pLineLabel = NULL;
	Initialize();

	iXPos = ScreenWidth / 2 - 320;
	iYPos = ScreenHeight / 2 - 240;

	setBgColor( Color( 0, 0, 0, 0 ) );

	guess_m_pBackgroundImage = vgui_LoadTGA( "gfx/vgui/backgroundtoall.tga" );
	if ( guess_m_pBackgroundImage )
		guess_m_pBackgroundImage->setColor( Color( 255, 255, 255, 1 ) );
	else
		gEngfuncs.Con_Printf( "Cannot load gfx/vgui/backgroundtoall.tga!\n" );

	guess_m_pBackgroundImage->getSize( iWide, iTall );
	iX = ScreenWidth / 2 - iWide / 2;
	iY = ScreenHeight / 2 - iTall / 2;

	guess_m_pBackground = new ImagePanel( guess_m_pBackgroundImage );
	guess_m_pBackground->setParent( this );
	guess_m_pBackground->setPos( iX, iY );
	guess_m_pBackground->setVisible( true );

	pPanel = new Panel( 0, 0, 0, 0 );
	pPanel->setParent( this );
	pPanel->setSize( ScreenWidth, ScreenHeight );
	pPanel->setBgColor( 0, 0, 0, 128 );
	pPanel->setVisible( true );

	guess_m_pPanel = new CTransparentPanel( 0, iXPos, iYPos, 640, 480 );
	guess_m_pPanel->setParent( this );
	guess_m_pPanel->setPaintBorderEnabled( false );
	guess_m_pPanel->setBgColor( Color( 0, 0, 0, 255 ) );

	guess_m_pImageLabel = new CImageLabel( guess_m_szImage1, -5, -5 );
	guess_m_pImageLabel->setVisible( true );
	guess_m_pImageLabel->setParent( guess_m_pPanel );

	guess_m_pCancelLabel = new CImageLabel( "notecancel", -5, -5 );
	guess_m_pCancelLabel->setVisible( true );
	guess_m_pCancelLabel->setParent( guess_m_pPanel );

	guess_m_pCloseButton = new CommandButton( "", 0, 0, 20, 20, true, true );
	guess_m_pCloseButton->setParent( guess_m_pPanel );
	guess_m_pCloseButton->addActionSignal( new CNoteDocumentHandler_Command( this, 3 ) );

	pButton = new CommandButton( "", 38, 411, 33, 33, true, true );
	pButton->setParent( guess_m_pPanel );
	pButton->addActionSignal( new CNoteDocumentHandler_Command( this, 1 ) );

	pButton = new CommandButton( "", 38, 447, 33, 33, true, true );
	pButton->setParent( guess_m_pPanel );
	pButton->addActionSignal( new CNoteDocumentHandler_Command( this, 2 ) );

	CSchemeManager *pSchemes = gViewPort->GetSchemeManager();
	SchemeHandle_t hCreditsScheme = pSchemes->getSchemeHandle( "credits" );
	guess_m_pFont = pSchemes->getFont( hCreditsScheme );

	guess_m_pLineLabel = new Label( "", 0, 0, ScreenWidth, 64 );
	guess_m_pLineLabel->setParent( this );
	guess_m_pLineLabel->setFont( guess_m_pFont );
	guess_m_pLineLabel->setFgColor( 255, 255, 255, 0 );
	guess_m_pLineLabel->setBgColor( 0, 0, 0, 255 );
	guess_m_pLineLabel->setText( "" );
	guess_m_pLineLabel->setContentAlignment( Label::a_center );
}

void CNoteDocument::ActionSignal( int cmd )
{
	switch ( cmd )
	{
	case 1:
		guess_PrevLine();
		break;
	case 2:
		guess_NextLine();
		break;
	case 3:
		gViewPort->HideTopMenu();
		gEngfuncs.pfnServerCmd( "closedocument" );
		break;
	}
}

void CNoteDocument::guess_ClearString( char *str )
{
	for ( int i = 0; i < strlen( str ); i++ )
		str[i] = 0;
}

int CNoteDocument::guess_CountLines()
{
	char szPath[256];

	strcpy( szPath, gEngfuncs.pfnGetGameDirectory() );
	strcat( szPath, "/notes/" );

	switch ( guess_m_iPage )
	{
	case 1:
		strcat( szPath, guess_m_szText1 );
		break;
	case 2:
		strcat( szPath, guess_m_szText2 );
		break;
	case 3:
		strcat( szPath, guess_m_szText3 );
		break;
	case 4:
		strcat( szPath, guess_m_szText4 );
		break;
	}

	std::ifstream file( szPath );
	std::string line;
	std::vector<std::string> lines;

	while ( std::getline( file, line ) )
		lines.push_back( line );

	return lines.size();
}

void CNoteDocument::Initialize()
{
	if ( guess_m_pLineLabel )
		guess_m_pLineLabel->setText( "" );

	guess_m_iLine = 0;
	guess_m_iPage = 0;

	strncpy( guess_m_szImage1, "", 16 );
	strncpy( guess_m_szImage2, "", 16 );
	strncpy( guess_m_szImage3, "", 16 );
	strncpy( guess_m_szImage4, "", 16 );
	strncpy( guess_m_szSound, "", 48 );
	strncpy( guess_m_szText1, "", 16 );
	strncpy( guess_m_szText2, "", 16 );
	strncpy( guess_m_szText3, "", 16 );
	strncpy( guess_m_szText4, "", 16 );
	strncpy( guess_m_szLine, "", 128 );

	guess_m_bTitle = false;
}

int CNoteDocument::KeyInput( int down, int keynum, const char *pszCurrentBinding )
{
	if ( pszCurrentBinding && !strcmp( pszCurrentBinding, "toggleconsole" ) )
		return 1;

	return 0;
}

int guess_MsgNoteDoc( const char *pszName, int iSize, void *pbuf )
{
	char szText1[256];
	char szText2[256];
	char szText3[256];
	char szText4[256];

	BEGIN_READ( pbuf, iSize );

	if ( gViewPort->guess_m_pNoteDocument )
	{
		gViewPort->guess_m_pNoteDocument->guess_ClearString( gViewPort->guess_m_pNoteDocument->guess_m_szImage1 );
		gViewPort->guess_m_pNoteDocument->guess_ClearString( gViewPort->guess_m_pNoteDocument->guess_m_szImage2 );
		gViewPort->guess_m_pNoteDocument->guess_ClearString( gViewPort->guess_m_pNoteDocument->guess_m_szImage3 );
		gViewPort->guess_m_pNoteDocument->guess_ClearString( gViewPort->guess_m_pNoteDocument->guess_m_szImage4 );
		gViewPort->guess_m_pNoteDocument->guess_ClearString( gViewPort->guess_m_pNoteDocument->guess_m_szText1 );
		gViewPort->guess_m_pNoteDocument->guess_ClearString( gViewPort->guess_m_pNoteDocument->guess_m_szSound );

		strcpy( gViewPort->guess_m_pNoteDocument->guess_m_szImage1, READ_STRING() );
		strcpy( gViewPort->guess_m_pNoteDocument->guess_m_szImage2, READ_STRING() );
		strcpy( gViewPort->guess_m_pNoteDocument->guess_m_szImage3, READ_STRING() );
		strcpy( gViewPort->guess_m_pNoteDocument->guess_m_szImage4, READ_STRING() );
		strcpy( szText1, READ_STRING() );
		strcpy( szText2, READ_STRING() );
		strcpy( szText3, READ_STRING() );
		strcpy( szText4, READ_STRING() );
		strcpy( gViewPort->guess_m_pNoteDocument->guess_m_szSound, READ_STRING() );

		int iLanguage = CVAR_GET_FLOAT( "cof_subtitlelanguage" );

		if ( strstr( szText1, "apnote" ) || strstr( szText1, "magazine" ) || strstr( szText1, "purnellnote" ) || strstr( szText1, "sewernote" ) || strstr( szText1, "subnote" ) )
		{
			switch ( iLanguage )
			{
			case 0:
			case 1:
				strcpy( gViewPort->guess_m_pNoteDocument->guess_m_szText1, szText1 );
				break;
			case 2:
				strcpy( gViewPort->guess_m_pNoteDocument->guess_m_szText1, "languages/dutch/" );
				strcat( gViewPort->guess_m_pNoteDocument->guess_m_szText1, szText1 );
				break;
			case 3:
				strcpy( gViewPort->guess_m_pNoteDocument->guess_m_szText1, "languages/french/" );
				strcat( gViewPort->guess_m_pNoteDocument->guess_m_szText1, szText1 );
				break;
			case 4:
				strcpy( gViewPort->guess_m_pNoteDocument->guess_m_szText1, "languages/german/" );
				strcat( gViewPort->guess_m_pNoteDocument->guess_m_szText1, szText1 );
				break;
			case 5:
				strcpy( gViewPort->guess_m_pNoteDocument->guess_m_szText1, "languages/norwegian/" );
				strcat( gViewPort->guess_m_pNoteDocument->guess_m_szText1, szText1 );
				break;
			case 6:
				strcpy( gViewPort->guess_m_pNoteDocument->guess_m_szText1, "languages/spanish/" );
				strcat( gViewPort->guess_m_pNoteDocument->guess_m_szText1, szText1 );
				break;
			case 7:
				strcpy( gViewPort->guess_m_pNoteDocument->guess_m_szText1, "languages/swedish/" );
				strcat( gViewPort->guess_m_pNoteDocument->guess_m_szText1, szText1 );
				break;
			}
		}

		if ( strstr( szText2, "apnote" ) || strstr( szText2, "magazine" ) || strstr( szText2, "purnellnote" ) || strstr( szText2, "sewernote" ) || strstr( szText2, "subnote" ) )
		{
			switch ( iLanguage )
			{
			case 0:
			case 1:
				strcpy( gViewPort->guess_m_pNoteDocument->guess_m_szText2, szText2 );
				break;
			case 2:
				strcpy( gViewPort->guess_m_pNoteDocument->guess_m_szText2, "languages/dutch/" );
				strcat( gViewPort->guess_m_pNoteDocument->guess_m_szText2, szText2 );
				break;
			case 3:
				strcpy( gViewPort->guess_m_pNoteDocument->guess_m_szText2, "languages/french/" );
				strcat( gViewPort->guess_m_pNoteDocument->guess_m_szText2, szText2 );
				break;
			case 4:
				strcpy( gViewPort->guess_m_pNoteDocument->guess_m_szText2, "languages/german/" );
				strcat( gViewPort->guess_m_pNoteDocument->guess_m_szText2, szText2 );
				break;
			case 5:
				strcpy( gViewPort->guess_m_pNoteDocument->guess_m_szText2, "languages/norwegian/" );
				strcat( gViewPort->guess_m_pNoteDocument->guess_m_szText2, szText2 );
				break;
			case 6:
				strcpy( gViewPort->guess_m_pNoteDocument->guess_m_szText2, "languages/spanish/" );
				strcat( gViewPort->guess_m_pNoteDocument->guess_m_szText2, szText2 );
				break;
			case 7:
				strcpy( gViewPort->guess_m_pNoteDocument->guess_m_szText2, "languages/swedish/" );
				strcat( gViewPort->guess_m_pNoteDocument->guess_m_szText2, szText2 );
				break;
			}
		}

		if ( strstr( szText3, "apnote" ) || strstr( szText3, "magazine" ) || strstr( szText3, "purnellnote" ) || strstr( szText3, "sewernote" ) || strstr( szText3, "subnote" ) )
		{
			switch ( iLanguage )
			{
			case 0:
			case 1:
				strcpy( gViewPort->guess_m_pNoteDocument->guess_m_szText3, szText3 );
				break;
			case 2:
				strcpy( gViewPort->guess_m_pNoteDocument->guess_m_szText3, "languages/dutch/" );
				strcat( gViewPort->guess_m_pNoteDocument->guess_m_szText3, szText3 );
				break;
			case 3:
				strcpy( gViewPort->guess_m_pNoteDocument->guess_m_szText3, "languages/french/" );
				strcat( gViewPort->guess_m_pNoteDocument->guess_m_szText3, szText3 );
				break;
			case 4:
				strcpy( gViewPort->guess_m_pNoteDocument->guess_m_szText3, "languages/german/" );
				strcat( gViewPort->guess_m_pNoteDocument->guess_m_szText3, szText3 );
				break;
			case 5:
				strcpy( gViewPort->guess_m_pNoteDocument->guess_m_szText3, "languages/norwegian/" );
				strcat( gViewPort->guess_m_pNoteDocument->guess_m_szText3, szText3 );
				break;
			case 6:
				strcpy( gViewPort->guess_m_pNoteDocument->guess_m_szText3, "languages/spanish/" );
				strcat( gViewPort->guess_m_pNoteDocument->guess_m_szText3, szText3 );
				break;
			case 7:
				strcpy( gViewPort->guess_m_pNoteDocument->guess_m_szText3, "languages/swedish/" );
				strcat( gViewPort->guess_m_pNoteDocument->guess_m_szText3, szText3 );
				break;
			}
		}

		if ( strstr( szText4, "apnote" ) || strstr( szText4, "magazine" ) || strstr( szText4, "purnellnote" ) || strstr( szText4, "sewernote" ) || strstr( szText4, "subnote" ) )
		{
			switch ( iLanguage )
			{
			case 0:
			case 1:
				strcpy( gViewPort->guess_m_pNoteDocument->guess_m_szText4, szText4 );
				break;
			case 2:
				strcpy( gViewPort->guess_m_pNoteDocument->guess_m_szText4, "languages/dutch/" );
				strcat( gViewPort->guess_m_pNoteDocument->guess_m_szText4, szText4 );
				break;
			case 3:
				strcpy( gViewPort->guess_m_pNoteDocument->guess_m_szText4, "languages/french/" );
				strcat( gViewPort->guess_m_pNoteDocument->guess_m_szText4, szText4 );
				break;
			case 4:
				strcpy( gViewPort->guess_m_pNoteDocument->guess_m_szText4, "languages/german/" );
				strcat( gViewPort->guess_m_pNoteDocument->guess_m_szText4, szText4 );
				break;
			case 5:
				strcpy( gViewPort->guess_m_pNoteDocument->guess_m_szText4, "languages/norwegian/" );
				strcat( gViewPort->guess_m_pNoteDocument->guess_m_szText4, szText4 );
				break;
			case 6:
				strcpy( gViewPort->guess_m_pNoteDocument->guess_m_szText4, "languages/spanish/" );
				strcat( gViewPort->guess_m_pNoteDocument->guess_m_szText4, szText4 );
				break;
			case 7:
				strcpy( gViewPort->guess_m_pNoteDocument->guess_m_szText4, "languages/swedish/" );
				strcat( gViewPort->guess_m_pNoteDocument->guess_m_szText4, szText4 );
				break;
			}
		}

		gViewPort->guess_m_pNoteDocument->guess_m_iPage = 1;
		gViewPort->guess_m_pNoteDocument->guess_SetPage( 1 );
	}

	gViewPort->ShowVGUIMenu( 31 );
	return 1;
}

void CNoteDocument::guess_NextLine()
{
	char szPath[256];

	strcpy( szPath, gEngfuncs.pfnGetGameDirectory() );
	strcat( szPath, "/notes/" );

	switch ( guess_m_iPage )
	{
	case 1:
		strcat( szPath, guess_m_szText1 );
		break;
	case 2:
		strcat( szPath, guess_m_szText2 );
		break;
	case 3:
		strcat( szPath, guess_m_szText3 );
		break;
	case 4:
		strcat( szPath, guess_m_szText4 );
		break;
	}

	std::ifstream file( szPath );
	std::string line;
	std::string guess_strUnused;

	guess_m_iLine++;

	if ( guess_m_iLine > guess_CountLines() )
		guess_m_iLine = 1;

	line = guess_m_iLine;

	int i = 1;

	while ( std::getline( file, line ) )
	{
		if ( i == guess_m_iLine )
		{
			char szLine[256];
			strcpy( szLine, line.c_str() );
			szLine[line.length()] = 0;

			guess_m_bTitle = ( i == 1 );
			strcpy( guess_m_szLine, szLine );
		}

		i++;
	}
}

void CNoteDocument::guess_NextPage()
{
	switch ( guess_m_iPage )
	{
	case 1:
		if ( guess_m_szImage2[1] )
		{
			guess_m_iPage = 2;
			guess_SetPage( 2 );
		}
		break;
	case 2:
		if ( guess_m_szImage3[1] )
		{
			guess_m_iPage = 3;
			guess_SetPage( 3 );
			return;
		}

		guess_m_iPage = 1;
		guess_SetPage( 1 );
		break;
	case 3:
		if ( guess_m_szImage4[1] )
		{
			guess_m_iPage = 4;
			guess_SetPage( 4 );
			return;
		}

		guess_m_iPage = 1;
		guess_SetPage( 1 );
		break;
	case 4:
		guess_m_iPage = 1;
		guess_SetPage( 1 );
		break;
	}
}

void guess_NoteDocInit()
{
	gEngfuncs.pfnHookUserMsg( "NoteDoc", guess_MsgNoteDoc );
}

void CNoteDocument::guess_PrevLine()
{
	char szPath[256];

	strcpy( szPath, gEngfuncs.pfnGetGameDirectory() );
	strcat( szPath, "/notes/" );

	switch ( guess_m_iPage )
	{
	case 1:
		strcat( szPath, guess_m_szText1 );
		break;
	case 2:
		strcat( szPath, guess_m_szText2 );
		break;
	case 3:
		strcat( szPath, guess_m_szText3 );
		break;
	case 4:
		strcat( szPath, guess_m_szText4 );
		break;
	}

	std::ifstream file( szPath );
	std::string line;
	std::string guess_strUnused;

	guess_m_iLine--;

	if ( guess_m_iLine <= 0 )
		guess_m_iLine = guess_CountLines();

	line = guess_m_iLine;

	int i = 1;

	while ( std::getline( file, line ) )
	{
		if ( i == guess_m_iLine )
		{
			char szLine[256];
			strcpy( szLine, line.c_str() );
			szLine[line.length()] = 0;

			guess_m_bTitle = ( i == 1 );
			strcpy( guess_m_szLine, szLine );
		}

		i++;
	}
}

void CNoteDocument::guess_PrevPage()
{
	switch ( guess_m_iPage )
	{
	case 1:
		if ( guess_m_szImage4[1] )
		{
			guess_m_iPage = 4;
			guess_SetPage( 4 );
			return;
		}
		if ( guess_m_szImage3[1] )
		{
			guess_m_iPage = 3;
			guess_SetPage( 3 );
			return;
		}
		if ( guess_m_szImage2[1] )
		{
			guess_m_iPage = 2;
			guess_SetPage( 2 );
			return;
		}
	case 2:
		if ( guess_m_szImage1[1] && guess_m_iPage != 1 )
		{
			guess_m_iPage = 1;
			guess_SetPage( 1 );
			return;
		}
		if ( guess_m_szImage4[1] )
		{
			guess_m_iPage = 4;
			guess_SetPage( 4 );
			return;
		}
		if ( guess_m_szImage3[1] )
		{
			guess_m_iPage = 3;
			guess_SetPage( 3 );
			return;
		}
	case 3:
		if ( guess_m_szImage2[1] )
		{
			guess_m_iPage = 2;
			guess_SetPage( 2 );
			return;
		}
		if ( guess_m_szImage1[1] )
		{
			guess_m_iPage = 1;
			guess_SetPage( 1 );
			return;
		}
		if ( guess_m_szImage4[1] )
		{
			guess_m_iPage = 4;
			guess_SetPage( 4 );
			return;
		}
	case 4:
		if ( guess_m_szImage3[1] )
		{
			guess_m_iPage = 3;
			guess_SetPage( 3 );
			return;
		}
		if ( guess_m_szImage2[1] )
		{
			guess_m_iPage = 2;
			guess_SetPage( 2 );
			return;
		}
		if ( guess_m_szImage1[1] )
		{
			guess_m_iPage = 1;
			guess_SetPage( 1 );
			return;
		}
	}
}

void CNoteDocument::guess_SetPage( int page )
{
	BitmapTGA *pImage;

	switch ( page )
	{
	case 1:
		pImage = LoadTGAForRes( guess_m_szImage1 );
		break;
	case 2:
		pImage = LoadTGAForRes( guess_m_szImage2 );
		break;
	case 3:
		pImage = LoadTGAForRes( guess_m_szImage3 );
		break;
	case 4:
		pImage = LoadTGAForRes( guess_m_szImage4 );
		break;
	}

	if ( guess_m_szSound[1] )
		gEngfuncs.pfnPlaySoundByName( guess_m_szSound, 1 );

	guess_m_iLine = 1;
	guess_ReadLine();

	guess_m_pImageLabel->setImage( pImage );
}

void CNoteDocument::guess_ReadLine()
{
	char szPath[256];

	strcpy( szPath, gEngfuncs.pfnGetGameDirectory() );
	strcat( szPath, "/notes/" );

	switch ( guess_m_iPage )
	{
	case 1:
		strcat( szPath, guess_m_szText1 );
		break;
	case 2:
		strcat( szPath, guess_m_szText2 );
		break;
	case 3:
		strcat( szPath, guess_m_szText3 );
		break;
	case 4:
		strcat( szPath, guess_m_szText4 );
		break;
	}

	std::ifstream file( szPath );
	std::string line;
	std::string guess_strUnused;

	line = guess_m_iLine;

	int i = 1;

	while ( std::getline( file, line ) )
	{
		if ( i == guess_m_iLine )
		{
			char szLine[256];
			strcpy( szLine, line.c_str() );
			szLine[line.length()] = 0;

			guess_m_bTitle = ( i == 1 );
			strcpy( guess_m_szLine, szLine );
		}

		i++;
	}
}

void CNoteDocument::paint()
{
	if ( strlen( guess_m_szLine ) > 1 )
	{
		int wide, tall;

		if ( guess_m_szLine[0] == '*' )
			guess_m_pLineLabel->setFgColor( 255, 255, 0, 0 );
		else if ( guess_m_bTitle )
			guess_m_pLineLabel->setFgColor( 255, 255, 0, 0 );
		else
			guess_m_pLineLabel->setFgColor( 200, 200, 200, 0 );

		guess_m_pFont->getTextSize( guess_m_szLine, wide, tall );
		drawSetTextPos( ScreenWidth / 2 - wide / 2, ScreenHeight - tall * 2 );

		guess_m_pLineLabel->setSize( ScreenWidth, 64 );
		guess_m_pLineLabel->setText( guess_m_szLine );
		guess_m_pLineLabel->setPos( 0, ScreenHeight - tall * 3 );
		guess_m_pLineLabel->setContentAlignment( Label::a_center );
	}

	Panel::paint();
}
