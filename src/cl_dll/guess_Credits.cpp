// ======================================
// Cry of Fear credits, statistics and unlocks panel
// (class names from the RTTI of client.dll, file name guessed)
// ======================================

#include <fstream>
#include <string>
#include <vector>

#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "vgui_TeamFortressViewport.h"
#include "guess_Credits.h"
#include "guess_CoopStats.h"
#include <stdarg.h>

extern int guess_iMenuMode;
extern int guess_g_iHeadShieldType;
extern cl_entity_t g_HeadShieldEnt;
extern int g_HeadShieldState;

#define SHIELD_ON	0
extern cvar_t *cl_fastcredits;
extern int guess_iUnknown101902FC;

// the custom costume textures are loaded again when these are cleared (studio renderer)
extern bool guess_g_bHoodieLoaded;
extern bool guess_g_bJeansLoaded;
extern bool guess_g_bFaceLoaded;

void guess_IrrKlangPlay( const char *pszFile );
void guess_IrrKlangSetLoop( bool bLoop );
void guess_IrrKlangStop( float flFadeTime );

bool guess_g_bUnlockShown;			// one unlock is on the screen, cleared by a key press (input.cpp)
int guess_g_iUnlockClothes;			// clothes drawn on the unfolded clothes model of the unlock
int guess_g_iCreditsLetterboxHeight;
int guess_g_iCoopCredits;			// Credits message, the co-op statistics follow
int guess_iUnknown101B27C8;			// pitch of the unlock model

CCredits::CCredits() : Panel( 0, 0, ScreenWidth, ScreenHeight )
{
	int i;
	int y;

	setBgColor( 255, 255, 255, 0 );

	CSchemeManager *pSchemes = gViewPort->GetSchemeManager();
	SchemeHandle_t hCreditsScheme = pSchemes->getSchemeHandle( "credits" );
	SchemeHandle_t hCreditsTitleScheme = pSchemes->getSchemeHandle( "creditstitle" );
	guess_m_pCreditsFont = pSchemes->getFont( hCreditsScheme );
	guess_m_pCreditsTitleFont = pSchemes->getFont( hCreditsTitleScheme );

	guess_m_bScrollUp = true;

	for ( i = 1; i <= guess_MAX_CREDIT_LINES; i++ )
	{
		y = ScreenHeight;
		guess_m_pCreditLabel[i] = new Label( " ", 0, y + 64 + i * 32, ScreenWidth, 32 );
		guess_m_pCreditLabel[i]->setParent( this );
		guess_m_pCreditLabel[i]->setFont( guess_m_pCreditsFont );
		guess_m_pCreditLabel[i]->setFgColor( 0, 0, 0, 255 );
		guess_m_pCreditLabel[i]->setBgColor( 0, 0, 0, 255 );
		guess_m_pCreditLabel[i]->setText( " " );
		guess_m_pCreditLabel[i]->setContentAlignment( Label::a_center );
	}

	y = ScreenHeight / 2;

	for ( i = 1; i <= 14; i++ )
	{
		guess_m_pStatLabel[i] = new Label( " ", 0, y - 256 + i * 32, ScreenWidth, 32 );
		guess_m_pStatLabel[i]->setParent( this );
		guess_m_pStatLabel[i]->setFont( guess_m_pCreditsFont );
		guess_m_pStatLabel[i]->setFgColor( 0, 0, 0, 255 );
		guess_m_pStatLabel[i]->setBgColor( 0, 0, 0, 255 );
		guess_m_pStatLabel[i]->setText( " " );
		guess_m_pStatLabel[i]->setContentAlignment( Label::a_center );
		guess_m_pStatLabel[i]->setVisible( false );
	}

	guess_m_pGradeLabel = new Label( " ", 0, y + 256, ScreenWidth, 42 );
	guess_m_pGradeLabel->setParent( this );
	guess_m_pGradeLabel->setFont( pSchemes->getFont( pSchemes->getSchemeHandle( "chaptertitle" ) ) );
	guess_m_pGradeLabel->setFgColor( 0, 0, 0, 255 );
	guess_m_pGradeLabel->setBgColor( 0, 0, 0, 255 );
	guess_m_pGradeLabel->setText( " " );
	guess_m_pGradeLabel->setContentAlignment( Label::a_center );
	guess_m_pGradeLabel->setVisible( false );

	guess_m_pGradeShadowLabel = new Label( " ", 0, y + 256, ScreenWidth, 42 );
	guess_m_pGradeShadowLabel->setParent( this );
	guess_m_pGradeShadowLabel->setFont( pSchemes->getFont( pSchemes->getSchemeHandle( "chaptertitle" ) ) );
	guess_m_pGradeShadowLabel->setFgColor( 0, 0, 0, 255 );
	guess_m_pGradeShadowLabel->setBgColor( 0, 0, 0, 255 );
	guess_m_pGradeShadowLabel->setText( " " );
	guess_m_pGradeShadowLabel->setContentAlignment( Label::a_center );
	guess_m_pGradeShadowLabel->setVisible( false );

	guess_m_pStatsTitleLabel = new Label( "Statistics and grade", 0, y - 266, ScreenWidth, 42 );
	guess_m_pStatsTitleLabel->setParent( this );
	guess_m_pStatsTitleLabel->setFont( guess_m_pCreditsTitleFont );
	guess_m_pStatsTitleLabel->setFgColor( 0, 0, 0, 255 );
	guess_m_pStatsTitleLabel->setBgColor( 0, 0, 0, 255 );
	guess_m_pStatsTitleLabel->setText( "Statistics and grade" );
	guess_m_pStatsTitleLabel->setContentAlignment( Label::a_center );
	guess_m_pStatsTitleLabel->setVisible( false );

	guess_m_pUnlockLabel = new Label( "Unlocks", 0, y - 266, ScreenWidth, 42 );
	guess_m_pUnlockLabel->setParent( this );
	guess_m_pUnlockLabel->setFont( guess_m_pCreditsTitleFont );
	guess_m_pUnlockLabel->setFgColor( 0, 0, 0, 255 );
	guess_m_pUnlockLabel->setBgColor( 0, 0, 0, 255 );
	guess_m_pUnlockLabel->setText( "Unlocks" );
	guess_m_pUnlockLabel->setContentAlignment( Label::a_center );
	guess_m_pUnlockLabel->setVisible( false );

	guess_g_iCreditsLetterboxHeight = ScreenHeight / 480.0f * 25.0f + 0.5f;

	guess_m_pLetterboxTop = new Panel( 0, 0, ScreenWidth, guess_g_iCreditsLetterboxHeight );
	guess_m_pLetterboxTop->setParent( this );
	guess_m_pLetterboxTop->setSize( ScreenWidth, guess_g_iCreditsLetterboxHeight );
	guess_m_pLetterboxTop->setBgColor( 0, 0, 0, 0 );
	guess_m_pLetterboxTop->setVisible( true );

	guess_m_pLetterboxBottom = new Panel( 0, ScreenHeight - guess_g_iCreditsLetterboxHeight, ScreenWidth, guess_g_iCreditsLetterboxHeight );
	guess_m_pLetterboxBottom->setParent( this );
	guess_m_pLetterboxBottom->setSize( ScreenWidth, guess_g_iCreditsLetterboxHeight );
	guess_m_pLetterboxBottom->setBgColor( 0, 0, 0, 0 );
	guess_m_pLetterboxBottom->setVisible( true );

	Initialize();
}

int CCredits::guess_CountLines()
{
	std::ifstream file( guess_m_szFile );
	std::string line;
	std::vector<std::string> lines;

	while ( std::getline( file, line ) )
		lines.push_back( line );

	return lines.size();
}

void CCredits::Initialize()
{
	guess_g_iUnlockClothes = 0;
	setVisible( false );
	guess_m_bShowStats = false;
	guess_m_bUnlocks = false;
	guess_m_iUnlocks = 0;
}

int guess_MsgCredits( const char *pszName, int iSize, void *pbuf )
{
	char szPath[256];
	char szFile[256];
	char szEnding[256];

	BEGIN_READ( pbuf, iSize );

	if ( !gViewPort->guess_m_pCredits )
		return 1;

	gViewPort->guess_m_pCredits->guess_m_szUnlockText[0] = '\0';
	guess_g_iUnlockClothes = 0;
	gViewPort->guess_m_pCredits->guess_m_bUnlocks = false;
	gViewPort->guess_m_pCredits->guess_m_iUnlocks = 0;
	gViewPort->guess_m_pCredits->guess_m_pStatsTitleLabel->setVisible( false );
	gViewPort->guess_m_pCredits->guess_m_pGradeLabel->setVisible( false );
	gViewPort->guess_m_pCredits->guess_m_pGradeShadowLabel->setVisible( false );
	gViewPort->guess_m_pCredits->setVisible( true );

	int iTextOnly = READ_BYTE();
	bool bConclusion = false;
	int iLanguage;

	if ( iTextOnly )
	{
		strcpy( szFile, READ_STRING() );

		if ( strstr( szFile, "coconc.txt" ) || strstr( szFile, "conclusion1.txt" ) || strstr( szFile, "conclusion2.txt" ) ||
			strstr( szFile, "conclusion3.txt" ) || strstr( szFile, "conclusion4.txt" ) || strstr( szFile, "docconc.txt" ) )
		{
			bConclusion = true;
			strcpy( gViewPort->guess_m_pCredits->guess_m_szFile, szFile );
		}
		else if ( !strstr( szFile, gEngfuncs.pfnGetGameDirectory() ) )
		{
			strcpy( szPath, gEngfuncs.pfnGetGameDirectory() );
			strcat( szPath, szFile );
			strcpy( gViewPort->guess_m_pCredits->guess_m_szFile, szPath );
		}
		else
		{
			strcpy( gViewPort->guess_m_pCredits->guess_m_szFile, szFile );
		}
	}
	else
	{
		strcpy( gViewPort->guess_m_pCredits->guess_m_szFile, READ_STRING() );
	}

	iLanguage = gEngfuncs.pfnGetCvarFloat( "cof_subtitlelanguage" );

	if ( !iTextOnly || bConclusion == true )
	{
		// the conclusion texts are translated (coconc.txt gets the docconc.txt of the language)
		switch ( iLanguage )
		{
		case 2:
			if ( strstr( gViewPort->guess_m_pCredits->guess_m_szFile, "coconc.txt" ) )
			{
				strcpy( gViewPort->guess_m_pCredits->guess_m_szFile, gEngfuncs.pfnGetGameDirectory() );
				strcat( gViewPort->guess_m_pCredits->guess_m_szFile, "/txtfiles/languages/dutch/docconc.txt" );
			}
			else if ( strstr( gViewPort->guess_m_pCredits->guess_m_szFile, "conclusion1.txt" ) )
			{
				strcpy( gViewPort->guess_m_pCredits->guess_m_szFile, gEngfuncs.pfnGetGameDirectory() );
				strcat( gViewPort->guess_m_pCredits->guess_m_szFile, "/txtfiles/languages/dutch/conclusion1.txt" );
			}
			else if ( strstr( gViewPort->guess_m_pCredits->guess_m_szFile, "conclusion2.txt" ) )
			{
				strcpy( gViewPort->guess_m_pCredits->guess_m_szFile, gEngfuncs.pfnGetGameDirectory() );
				strcat( gViewPort->guess_m_pCredits->guess_m_szFile, "/txtfiles/languages/dutch/conclusion2.txt" );
			}
			else if ( strstr( gViewPort->guess_m_pCredits->guess_m_szFile, "conclusion3.txt" ) )
			{
				strcpy( gViewPort->guess_m_pCredits->guess_m_szFile, gEngfuncs.pfnGetGameDirectory() );
				strcat( gViewPort->guess_m_pCredits->guess_m_szFile, "/txtfiles/languages/dutch/conclusion3.txt" );
			}
			else if ( strstr( gViewPort->guess_m_pCredits->guess_m_szFile, "conclusion4.txt" ) )
			{
				strcpy( gViewPort->guess_m_pCredits->guess_m_szFile, gEngfuncs.pfnGetGameDirectory() );
				strcat( gViewPort->guess_m_pCredits->guess_m_szFile, "/txtfiles/languages/dutch/conclusion4.txt" );
			}
			else if ( strstr( gViewPort->guess_m_pCredits->guess_m_szFile, "docconc.txt" ) )
			{
				strcpy( gViewPort->guess_m_pCredits->guess_m_szFile, gEngfuncs.pfnGetGameDirectory() );
				strcat( gViewPort->guess_m_pCredits->guess_m_szFile, "/txtfiles/languages/dutch/docconc.txt" );
			}
			break;
		case 3:
			if ( strstr( gViewPort->guess_m_pCredits->guess_m_szFile, "coconc.txt" ) )
			{
				strcpy( gViewPort->guess_m_pCredits->guess_m_szFile, gEngfuncs.pfnGetGameDirectory() );
				strcat( gViewPort->guess_m_pCredits->guess_m_szFile, "/txtfiles/languages/french/docconc.txt" );
			}
			else if ( strstr( gViewPort->guess_m_pCredits->guess_m_szFile, "conclusion1.txt" ) )
			{
				strcpy( gViewPort->guess_m_pCredits->guess_m_szFile, gEngfuncs.pfnGetGameDirectory() );
				strcat( gViewPort->guess_m_pCredits->guess_m_szFile, "/txtfiles/languages/french/conclusion1.txt" );
			}
			else if ( strstr( gViewPort->guess_m_pCredits->guess_m_szFile, "conclusion2.txt" ) )
			{
				strcpy( gViewPort->guess_m_pCredits->guess_m_szFile, gEngfuncs.pfnGetGameDirectory() );
				strcat( gViewPort->guess_m_pCredits->guess_m_szFile, "/txtfiles/languages/french/conclusion2.txt" );
			}
			else if ( strstr( gViewPort->guess_m_pCredits->guess_m_szFile, "conclusion3.txt" ) )
			{
				strcpy( gViewPort->guess_m_pCredits->guess_m_szFile, gEngfuncs.pfnGetGameDirectory() );
				strcat( gViewPort->guess_m_pCredits->guess_m_szFile, "/txtfiles/languages/french/conclusion3.txt" );
			}
			else if ( strstr( gViewPort->guess_m_pCredits->guess_m_szFile, "conclusion4.txt" ) )
			{
				strcpy( gViewPort->guess_m_pCredits->guess_m_szFile, gEngfuncs.pfnGetGameDirectory() );
				strcat( gViewPort->guess_m_pCredits->guess_m_szFile, "/txtfiles/languages/french/conclusion4.txt" );
			}
			else if ( strstr( gViewPort->guess_m_pCredits->guess_m_szFile, "docconc.txt" ) )
			{
				strcpy( gViewPort->guess_m_pCredits->guess_m_szFile, gEngfuncs.pfnGetGameDirectory() );
				strcat( gViewPort->guess_m_pCredits->guess_m_szFile, "/txtfiles/languages/french/docconc.txt" );
			}
			break;
		case 4:
			if ( strstr( gViewPort->guess_m_pCredits->guess_m_szFile, "coconc.txt" ) )
			{
				strcpy( gViewPort->guess_m_pCredits->guess_m_szFile, gEngfuncs.pfnGetGameDirectory() );
				strcat( gViewPort->guess_m_pCredits->guess_m_szFile, "/txtfiles/languages/german/docconc.txt" );
			}
			else if ( strstr( gViewPort->guess_m_pCredits->guess_m_szFile, "conclusion1.txt" ) )
			{
				strcpy( gViewPort->guess_m_pCredits->guess_m_szFile, gEngfuncs.pfnGetGameDirectory() );
				strcat( gViewPort->guess_m_pCredits->guess_m_szFile, "/txtfiles/languages/german/conclusion1.txt" );
			}
			else if ( strstr( gViewPort->guess_m_pCredits->guess_m_szFile, "conclusion2.txt" ) )
			{
				strcpy( gViewPort->guess_m_pCredits->guess_m_szFile, gEngfuncs.pfnGetGameDirectory() );
				strcat( gViewPort->guess_m_pCredits->guess_m_szFile, "/txtfiles/languages/german/conclusion2.txt" );
			}
			else if ( strstr( gViewPort->guess_m_pCredits->guess_m_szFile, "conclusion3.txt" ) )
			{
				strcpy( gViewPort->guess_m_pCredits->guess_m_szFile, gEngfuncs.pfnGetGameDirectory() );
				strcat( gViewPort->guess_m_pCredits->guess_m_szFile, "/txtfiles/languages/german/conclusion3.txt" );
			}
			else if ( strstr( gViewPort->guess_m_pCredits->guess_m_szFile, "conclusion4.txt" ) )
			{
				strcpy( gViewPort->guess_m_pCredits->guess_m_szFile, gEngfuncs.pfnGetGameDirectory() );
				strcat( gViewPort->guess_m_pCredits->guess_m_szFile, "/txtfiles/languages/german/conclusion4.txt" );
			}
			else if ( strstr( gViewPort->guess_m_pCredits->guess_m_szFile, "docconc.txt" ) )
			{
				strcpy( gViewPort->guess_m_pCredits->guess_m_szFile, gEngfuncs.pfnGetGameDirectory() );
				strcat( gViewPort->guess_m_pCredits->guess_m_szFile, "/txtfiles/languages/german/docconc.txt" );
			}
			break;
		case 5:
			if ( strstr( gViewPort->guess_m_pCredits->guess_m_szFile, "coconc.txt" ) )
			{
				strcpy( gViewPort->guess_m_pCredits->guess_m_szFile, gEngfuncs.pfnGetGameDirectory() );
				strcat( gViewPort->guess_m_pCredits->guess_m_szFile, "/txtfiles/languages/norwegian/docconc.txt" );
			}
			else if ( strstr( gViewPort->guess_m_pCredits->guess_m_szFile, "conclusion1.txt" ) )
			{
				strcpy( gViewPort->guess_m_pCredits->guess_m_szFile, gEngfuncs.pfnGetGameDirectory() );
				strcat( gViewPort->guess_m_pCredits->guess_m_szFile, "/txtfiles/languages/norwegian/conclusion1.txt" );
			}
			else if ( strstr( gViewPort->guess_m_pCredits->guess_m_szFile, "conclusion2.txt" ) )
			{
				strcpy( gViewPort->guess_m_pCredits->guess_m_szFile, gEngfuncs.pfnGetGameDirectory() );
				strcat( gViewPort->guess_m_pCredits->guess_m_szFile, "/txtfiles/languages/norwegian/conclusion2.txt" );
			}
			else if ( strstr( gViewPort->guess_m_pCredits->guess_m_szFile, "conclusion3.txt" ) )
			{
				strcpy( gViewPort->guess_m_pCredits->guess_m_szFile, gEngfuncs.pfnGetGameDirectory() );
				strcat( gViewPort->guess_m_pCredits->guess_m_szFile, "/txtfiles/languages/norwegian/conclusion3.txt" );
			}
			else if ( strstr( gViewPort->guess_m_pCredits->guess_m_szFile, "conclusion4.txt" ) )
			{
				strcpy( gViewPort->guess_m_pCredits->guess_m_szFile, gEngfuncs.pfnGetGameDirectory() );
				strcat( gViewPort->guess_m_pCredits->guess_m_szFile, "/txtfiles/languages/norwegian/conclusion4.txt" );
			}
			else if ( strstr( gViewPort->guess_m_pCredits->guess_m_szFile, "docconc.txt" ) )
			{
				strcpy( gViewPort->guess_m_pCredits->guess_m_szFile, gEngfuncs.pfnGetGameDirectory() );
				strcat( gViewPort->guess_m_pCredits->guess_m_szFile, "/txtfiles/languages/norwegian/docconc.txt" );
			}
			break;
		case 6:
			if ( strstr( gViewPort->guess_m_pCredits->guess_m_szFile, "coconc.txt" ) )
			{
				strcpy( gViewPort->guess_m_pCredits->guess_m_szFile, gEngfuncs.pfnGetGameDirectory() );
				strcat( gViewPort->guess_m_pCredits->guess_m_szFile, "/txtfiles/languages/spanish/docconc.txt" );
			}
			else if ( strstr( gViewPort->guess_m_pCredits->guess_m_szFile, "conclusion1.txt" ) )
			{
				strcpy( gViewPort->guess_m_pCredits->guess_m_szFile, gEngfuncs.pfnGetGameDirectory() );
				strcat( gViewPort->guess_m_pCredits->guess_m_szFile, "/txtfiles/languages/spanish/conclusion1.txt" );
			}
			else if ( strstr( gViewPort->guess_m_pCredits->guess_m_szFile, "conclusion2.txt" ) )
			{
				strcpy( gViewPort->guess_m_pCredits->guess_m_szFile, gEngfuncs.pfnGetGameDirectory() );
				strcat( gViewPort->guess_m_pCredits->guess_m_szFile, "/txtfiles/languages/spanish/conclusion2.txt" );
			}
			else if ( strstr( gViewPort->guess_m_pCredits->guess_m_szFile, "conclusion3.txt" ) )
			{
				strcpy( gViewPort->guess_m_pCredits->guess_m_szFile, gEngfuncs.pfnGetGameDirectory() );
				strcat( gViewPort->guess_m_pCredits->guess_m_szFile, "/txtfiles/languages/spanish/conclusion3.txt" );
			}
			else if ( strstr( gViewPort->guess_m_pCredits->guess_m_szFile, "conclusion4.txt" ) )
			{
				strcpy( gViewPort->guess_m_pCredits->guess_m_szFile, gEngfuncs.pfnGetGameDirectory() );
				strcat( gViewPort->guess_m_pCredits->guess_m_szFile, "/txtfiles/languages/spanish/conclusion4.txt" );
			}
			else if ( strstr( gViewPort->guess_m_pCredits->guess_m_szFile, "docconc.txt" ) )
			{
				strcpy( gViewPort->guess_m_pCredits->guess_m_szFile, gEngfuncs.pfnGetGameDirectory() );
				strcat( gViewPort->guess_m_pCredits->guess_m_szFile, "/txtfiles/languages/spanish/docconc.txt" );
			}
			break;
		case 7:
			if ( strstr( gViewPort->guess_m_pCredits->guess_m_szFile, "coconc.txt" ) )
			{
				strcpy( gViewPort->guess_m_pCredits->guess_m_szFile, gEngfuncs.pfnGetGameDirectory() );
				strcat( gViewPort->guess_m_pCredits->guess_m_szFile, "/txtfiles/languages/swedish/docconc.txt" );
			}
			else if ( strstr( gViewPort->guess_m_pCredits->guess_m_szFile, "conclusion1.txt" ) )
			{
				strcpy( gViewPort->guess_m_pCredits->guess_m_szFile, gEngfuncs.pfnGetGameDirectory() );
				strcat( gViewPort->guess_m_pCredits->guess_m_szFile, "/txtfiles/languages/swedish/conclusion1.txt" );
			}
			else if ( strstr( gViewPort->guess_m_pCredits->guess_m_szFile, "conclusion2.txt" ) )
			{
				strcpy( gViewPort->guess_m_pCredits->guess_m_szFile, gEngfuncs.pfnGetGameDirectory() );
				strcat( gViewPort->guess_m_pCredits->guess_m_szFile, "/txtfiles/languages/swedish/conclusion2.txt" );
			}
			else if ( strstr( gViewPort->guess_m_pCredits->guess_m_szFile, "conclusion3.txt" ) )
			{
				strcpy( gViewPort->guess_m_pCredits->guess_m_szFile, gEngfuncs.pfnGetGameDirectory() );
				strcat( gViewPort->guess_m_pCredits->guess_m_szFile, "/txtfiles/languages/swedish/conclusion3.txt" );
			}
			else if ( strstr( gViewPort->guess_m_pCredits->guess_m_szFile, "conclusion4.txt" ) )
			{
				strcpy( gViewPort->guess_m_pCredits->guess_m_szFile, gEngfuncs.pfnGetGameDirectory() );
				strcat( gViewPort->guess_m_pCredits->guess_m_szFile, "/txtfiles/languages/swedish/conclusion4.txt" );
			}
			else if ( strstr( gViewPort->guess_m_pCredits->guess_m_szFile, "docconc.txt" ) )
			{
				strcpy( gViewPort->guess_m_pCredits->guess_m_szFile, gEngfuncs.pfnGetGameDirectory() );
				strcat( gViewPort->guess_m_pCredits->guess_m_szFile, "/txtfiles/languages/swedish/docconc.txt" );
			}
			break;
		}
	}

	gViewPort->guess_m_pCredits->guess_m_iNumLines = gViewPort->guess_m_pCredits->guess_CountLines();
	gViewPort->guess_m_pCredits->guess_m_bStarted = true;

	int iStartY = 0;
	if ( !gViewPort->guess_m_pCredits->guess_m_bScrollUp )
		iStartY = ScreenHeight;

	gViewPort->guess_m_pCredits->guess_m_iBrightness = 0;
	gViewPort->guess_m_pCredits->guess_m_bFadeIn = false;
	gViewPort->guess_m_pCredits->guess_m_bFadeDone = false;
	gViewPort->guess_m_pCredits->guess_m_bStatsTimer = false;
	gViewPort->guess_m_pCredits->guess_m_flStatsTime = 0;

	int i;

	for ( i = 1; i <= gViewPort->guess_m_pCredits->guess_m_iNumLines; i++ )
	{
		gViewPort->guess_m_pCredits->guess_SetLine( i, i );

		int iOffset = i * 32;
		if ( gViewPort->guess_m_pCredits->guess_m_bScrollUp )
			iOffset = -iOffset;

		gViewPort->guess_m_pCredits->guess_m_pCreditLabel[i]->setPos( 0, iStartY + iOffset );
		gViewPort->guess_m_pCredits->guess_m_pCreditLabel[i]->setVisible( true );
	}

	gViewPort->guess_m_pCredits->guess_m_iShotsFired = READ_SHORT();
	gViewPort->guess_m_pCredits->guess_m_iShotsHit = READ_SHORT();
	gViewPort->guess_m_pCredits->guess_m_iSyringesUsed = READ_SHORT();
	gViewPort->guess_m_pCredits->guess_m_iShotsMissed = READ_SHORT();
	gViewPort->guess_m_pCredits->guess_m_flAccuracy = READ_COORD();
	gViewPort->guess_m_pCredits->guess_m_iTimesSaved = READ_SHORT();
	gViewPort->guess_m_pCredits->guess_m_iWeaponsFound = READ_SHORT();
	gViewPort->guess_m_pCredits->guess_m_iDamageTaken = READ_SHORT();
	gViewPort->guess_m_pCredits->guess_m_flDistanceDecimal = READ_SHORT();
	gViewPort->guess_m_pCredits->guess_m_flDistance = READ_SHORT();
	gViewPort->guess_m_pCredits->guess_m_iSeconds = READ_SHORT();
	gViewPort->guess_m_pCredits->guess_m_iMinutes = READ_SHORT();
	gViewPort->guess_m_pCredits->guess_m_iHours = READ_SHORT();
	gViewPort->guess_m_pCredits->guess_m_iMeleeKills = READ_SHORT();
	gViewPort->guess_m_pCredits->guess_m_iFirearmKills = READ_SHORT();
	READ_BYTE();
	gViewPort->guess_m_pCredits->guess_m_iScrollSpeed = READ_BYTE();
	int iHide = READ_BYTE();
	gViewPort->guess_m_pCredits->guess_m_iDifficulty = READ_BYTE();
	gViewPort->guess_m_pCredits->guess_m_iUnlocks = READ_LONG();
	int iEnding = READ_BYTE();
	int iGrade = READ_BYTE();
	int iCoop = READ_BYTE();
	int iStatsTimer = READ_BYTE();
	guess_g_iCoopCredits = READ_BYTE();
	int iEnding6 = READ_BYTE();
	int iEnding7 = READ_BYTE();
	READ_BYTE();
	READ_BYTE();
	int iShowGrade = READ_BYTE();

	if ( gViewPort->guess_m_pCredits->guess_m_iDifficulty < 1 || gViewPort->guess_m_pCredits->guess_m_iDifficulty > 4 )
		gViewPort->guess_m_pCredits->guess_m_iDifficulty = 1;

	if ( iHide )
		gViewPort->guess_m_pCredits->setVisible( false );

	if ( iTextOnly )
	{
		gViewPort->guess_m_pCredits->guess_m_bTextOnly = true;
		gViewPort->guess_m_pCredits->guess_m_bScrollUp = false;

		for ( i = 1; i <= guess_MAX_CREDIT_LINES; i++ )
			gViewPort->guess_m_pCredits->guess_m_pCreditLabel[i]->setPos( 0, ScreenHeight + i * 32 );
	}
	else
	{
		gViewPort->guess_m_pCredits->guess_m_bScrollUp = true;
		gViewPort->guess_m_pCredits->guess_m_bTextOnly = false;
	}

	gViewPort->guess_m_pCredits->guess_m_bShowStats = false;
	gViewPort->guess_m_pCredits->guess_m_flStartTime = gEngfuncs.GetClientTime();

	gViewPort->guess_m_pCredits->guess_m_pStatLabel[1]->setText( guess_va( "Shots fired: %i", gViewPort->guess_m_pCredits->guess_m_iShotsFired ) );
	gViewPort->guess_m_pCredits->guess_m_pStatLabel[2]->setText( guess_va( "Shots hit: %i", gViewPort->guess_m_pCredits->guess_m_iShotsHit ) );
	gViewPort->guess_m_pCredits->guess_m_pStatLabel[3]->setText( guess_va( "Shots missed: %i", gViewPort->guess_m_pCredits->guess_m_iShotsMissed ) );
	gViewPort->guess_m_pCredits->guess_m_pStatLabel[4]->setText( guess_va( "Overall accuracy: %i percent", (int)gViewPort->guess_m_pCredits->guess_m_flAccuracy ) );

	if ( iCoop )
	{
		gViewPort->guess_m_pCredits->guess_m_pStatLabel[5]->setText( guess_va( "Overall damage taken: %i", gViewPort->guess_m_pCredits->guess_m_iDamageTaken ) );
		gViewPort->guess_m_pCredits->guess_m_pStatLabel[6]->setText( guess_va( "Distance travelled on foot: %i.%i km", (int)gViewPort->guess_m_pCredits->guess_m_flDistance, (int)gViewPort->guess_m_pCredits->guess_m_flDistanceDecimal ) );
		gViewPort->guess_m_pCredits->guess_m_pStatLabel[7]->setText( guess_va( "Time taken: %i:%i:%i", gViewPort->guess_m_pCredits->guess_m_iHours, gViewPort->guess_m_pCredits->guess_m_iMinutes, gViewPort->guess_m_pCredits->guess_m_iSeconds ) );
		gViewPort->guess_m_pCredits->guess_m_pStatLabel[8]->setText( guess_va( "Enemies killed by melee: %i", gViewPort->guess_m_pCredits->guess_m_iMeleeKills ) );
		gViewPort->guess_m_pCredits->guess_m_pStatLabel[9]->setText( guess_va( "Enemies killed by firearms: %i", gViewPort->guess_m_pCredits->guess_m_iFirearmKills ) );
		gViewPort->guess_m_pCredits->guess_m_pStatLabel[10]->setText( "" );
		gViewPort->guess_m_pCredits->guess_m_pStatLabel[11]->setText( "" );
		gViewPort->guess_m_pCredits->guess_m_pStatLabel[12]->setText( "" );
	}
	else
	{
		gViewPort->guess_m_pCredits->guess_m_pStatLabel[5]->setText( guess_va( "Syringes used: %i", gViewPort->guess_m_pCredits->guess_m_iSyringesUsed ) );
		gViewPort->guess_m_pCredits->guess_m_pStatLabel[6]->setText( guess_va( "Times saved: %i", gViewPort->guess_m_pCredits->guess_m_iTimesSaved ) );
		gViewPort->guess_m_pCredits->guess_m_pStatLabel[7]->setText( guess_va( "Weapons found: %i", gViewPort->guess_m_pCredits->guess_m_iWeaponsFound ) );
		gViewPort->guess_m_pCredits->guess_m_pStatLabel[8]->setText( guess_va( "Overall damage taken: %i", gViewPort->guess_m_pCredits->guess_m_iDamageTaken ) );
		gViewPort->guess_m_pCredits->guess_m_pStatLabel[9]->setText( guess_va( "Distance travelled on foot: %i.%i km", (int)gViewPort->guess_m_pCredits->guess_m_flDistance, (int)gViewPort->guess_m_pCredits->guess_m_flDistanceDecimal ) );
		gViewPort->guess_m_pCredits->guess_m_pStatLabel[10]->setText( guess_va( "Time taken: %i:%i:%i", gViewPort->guess_m_pCredits->guess_m_iHours, gViewPort->guess_m_pCredits->guess_m_iMinutes, gViewPort->guess_m_pCredits->guess_m_iSeconds ) );
		gViewPort->guess_m_pCredits->guess_m_pStatLabel[11]->setText( guess_va( "Enemies killed by melee: %i", gViewPort->guess_m_pCredits->guess_m_iMeleeKills ) );
		gViewPort->guess_m_pCredits->guess_m_pStatLabel[12]->setText( guess_va( "Enemies killed by firearms: %i", gViewPort->guess_m_pCredits->guess_m_iFirearmKills ) );
	}

	switch ( gViewPort->guess_m_pCredits->guess_m_iDifficulty )
	{
	case 1:
		gViewPort->guess_m_pCredits->guess_m_pStatLabel[13]->setText( guess_va( "Difficulty chosen: Easy" ) );
		break;
	case 2:
		gViewPort->guess_m_pCredits->guess_m_pStatLabel[13]->setText( guess_va( "Difficulty chosen: Normal" ) );
		break;
	case 3:
		gViewPort->guess_m_pCredits->guess_m_pStatLabel[13]->setText( guess_va( "Difficulty chosen: Hard" ) );
		break;
	case 4:
		gViewPort->guess_m_pCredits->guess_m_pStatLabel[13]->setText( guess_va( "Difficulty chosen: Nightmare" ) );
		break;
	default:
		gViewPort->guess_m_pCredits->guess_m_pStatLabel[13]->setText( guess_va( "Difficulty chosen: Easy" ) );
		break;
	}

	if ( iEnding6 && iCoop )
		iEnding = 6;

	if ( iEnding7 && guess_g_iCoopCredits )
	{
		strcpy( szEnding, "Ending seen: " );
		strcat( szEnding, "7" );
	}
	else
	{
		strcpy( szEnding, "Ending seen: " );

		switch ( iEnding )
		{
		case 1:
			strcat( szEnding, "1" );
			break;
		case 2:
			strcat( szEnding, "2" );
			break;
		case 3:
			strcat( szEnding, "3" );
			break;
		case 4:
			strcat( szEnding, "4" );
			break;
		case 5:
			strcat( szEnding, "5" );
			break;
		case 6:
			strcat( szEnding, "6" );
			break;
		case 7:
			strcat( szEnding, "7" );
			break;
		}
	}

	gViewPort->guess_m_pCredits->guess_m_pStatLabel[14]->setText( szEnding );

	for ( i = 1; i <= 14; i++ )
	{
		gViewPort->guess_m_pCredits->guess_m_pStatLabel[i]->setContentAlignment( Label::a_center );
		gViewPort->guess_m_pCredits->guess_m_pStatLabel[i]->setVisible( false );
	}

	switch ( iGrade )
	{
	case 1:
		gViewPort->guess_m_pCredits->guess_m_pGradeLabel->setText( "S" );
		gViewPort->guess_m_pCredits->guess_m_pGradeShadowLabel->setText( "S" );
		break;
	case 2:
		gViewPort->guess_m_pCredits->guess_m_pGradeLabel->setText( "A" );
		gViewPort->guess_m_pCredits->guess_m_pGradeShadowLabel->setText( "A" );
		break;
	case 3:
		gViewPort->guess_m_pCredits->guess_m_pGradeLabel->setText( "B" );
		gViewPort->guess_m_pCredits->guess_m_pGradeShadowLabel->setText( "B" );
		break;
	case 4:
		gViewPort->guess_m_pCredits->guess_m_pGradeLabel->setText( "C" );
		gViewPort->guess_m_pCredits->guess_m_pGradeShadowLabel->setText( "C" );
		break;
	case 5:
		gViewPort->guess_m_pCredits->guess_m_pGradeLabel->setText( "D" );
		gViewPort->guess_m_pCredits->guess_m_pGradeShadowLabel->setText( "D" );
		break;
	case 6:
		gViewPort->guess_m_pCredits->guess_m_pGradeLabel->setText( "E" );
		gViewPort->guess_m_pCredits->guess_m_pGradeShadowLabel->setText( "E" );
		break;
	default:
		gViewPort->guess_m_pCredits->guess_m_pGradeLabel->setText( "F" );
		gViewPort->guess_m_pCredits->guess_m_pGradeShadowLabel->setText( "F" );
		break;
	}

	if ( !iShowGrade )
	{
		gViewPort->guess_m_pCredits->guess_m_pGradeLabel->setText( " " );
		gViewPort->guess_m_pCredits->guess_m_pGradeShadowLabel->setText( " " );
		gViewPort->guess_m_pCredits->guess_m_pStatLabel[14]->setText( " " );
	}

	if ( iStatsTimer )
	{
		gViewPort->guess_m_pCredits->guess_m_bStatsTimer = true;
		gViewPort->guess_m_pCredits->guess_m_flStatsTime = gEngfuncs.GetClientTime() + 0.2;
	}

	return 1;
}

void CCredits::guess_SetLine( int iUnused, int iLine )
{
	std::ifstream file( guess_m_szFile );
	std::string line;
	std::string line2;
	line = iLine;
	int i = 1;
	char szLine[256];

	while ( std::getline( file, line ) )
	{
		if ( i == iLine )
		{
			strcpy( szLine, line.c_str() );
			szLine[line.length()] = 0;

			char *pszText = szLine;

			if ( szLine[0] == '#' )
			{
				pszText = &szLine[1];
				guess_m_pCreditLabel[i]->setFont( guess_m_pCreditsTitleFont );
			}
			else
			{
				guess_m_pCreditLabel[i]->setFont( guess_m_pCreditsFont );
			}

			guess_m_pCreditLabel[i]->setText( pszText );
			guess_m_pCreditLabel[i]->setContentAlignment( Label::a_center );
		}

		i++;
	}
}

void CCredits::guess_ShowUnlock( int iUnlock, const char *pszText, int iDistance, char *pszModel, int iBody )
{
	int i;

	guess_iUnknown101902FC = iDistance;
	guess_m_iUnlocks &= ~iUnlock;
	guess_m_bContinue = false;

	for ( i = 1; i <= 14; i++ )
	{
		if ( guess_m_pStatLabel[i]->isVisible() )
			guess_m_pStatLabel[i]->setVisible( false );
	}

	for ( i = 1; i <= guess_m_iNumLines; i++ )
	{
		if ( guess_m_pCreditLabel[i]->isVisible() )
			guess_m_pCreditLabel[i]->setVisible( false );
	}

	if ( guess_m_pGradeLabel->isVisible() )
		guess_m_pGradeLabel->setVisible( false );
	if ( guess_m_pGradeShadowLabel->isVisible() )
		guess_m_pGradeShadowLabel->setVisible( false );
	if ( guess_m_pStatsTitleLabel->isVisible() )
		guess_m_pStatsTitleLabel->setVisible( false );

	strcpy( guess_m_szUnlockText, pszText );

	if ( guess_g_iHeadShieldType != 6 )
		guess_g_iHeadShieldType = 6;

	guess_iUnknown101902FC = 50;

	if ( !strcmp( pszModel, "models/items/generic_note.mdl" ) || !strcmp( pszModel, "models/weapons/axe/w_axe.mdl" ) ||
		!strcmp( pszModel, "models/weapons/famas/w_famas.mdl" ) || !strcmp( pszModel, "models/items/package.mdl" ) )
	{
		guess_iUnknown101B27C8 = 90;
	}
	else if ( !strcmp( pszModel, "models/items/cassette_tape.mdl" ) )
	{
		guess_iUnknown101902FC = 20;
		guess_iUnknown101B27C8 = 90;
	}
	else
	{
		if ( !strcmp( pszModel, "models/doctor_unlock.mdl" ) )
			guess_iUnknown101902FC = 80;

		guess_iUnknown101B27C8 = 0;
	}

	gHUD.guess_m_pszHeadShieldSound = pszModel;
	gHUD.guess_m_iHeadShieldSoundFlags = iBody;
	g_HeadShieldState = SHIELD_ON;
}

char *guess_va( const char *format, ... )
{
	va_list argptr;
	static char string[1024];

	va_start( argptr, format );
	vsprintf( string, format, argptr );
	va_end( argptr );

	return string;
}

void guess_CreditsInit()
{
	gEngfuncs.pfnHookUserMsg( "Credits", guess_MsgCredits );
}

void CCredits::solve()
{
	static bool s_bLetterboxFrame = false;
	static bool s_bFadeFrame = true;
	int i;

	int iSpeed = guess_g_iCreditsLetterboxHeight / 15;
	if ( iSpeed <= 0 )
		iSpeed = 1;

	if ( gHUD.guess_m_BlackBar.guess_m_iOn )
	{
		s_bLetterboxFrame = !s_bLetterboxFrame;
		if ( s_bLetterboxFrame )
		{
			int x, y, x2, y2;

			guess_m_pLetterboxTop->getPos( x, y );
			guess_m_pLetterboxBottom->getPos( x2, y2 );

			if ( y < 0 )
				y += iSpeed;
			if ( y > 0 )
				y = 0;
			if ( y2 > ScreenHeight - guess_g_iCreditsLetterboxHeight )
				y2 -= iSpeed;
			if ( y2 < ScreenHeight - guess_g_iCreditsLetterboxHeight )
				y2 = ScreenHeight - guess_g_iCreditsLetterboxHeight;

			guess_m_pLetterboxTop->setPos( x, y );
			guess_m_pLetterboxBottom->setPos( x2, y2 );
		}
	}
	else
	{
		s_bLetterboxFrame = !s_bLetterboxFrame;
		if ( s_bLetterboxFrame )
		{
			int x, y, x2, y2;

			guess_m_pLetterboxTop->getPos( x, y );
			guess_m_pLetterboxBottom->getPos( x2, y2 );

			if ( y > -guess_g_iCreditsLetterboxHeight )
				y -= iSpeed;
			if ( y < -guess_g_iCreditsLetterboxHeight )
				y = -guess_g_iCreditsLetterboxHeight;
			if ( y2 < ScreenHeight )
				y2 += iSpeed;
			if ( y2 > ScreenHeight )
				y2 = ScreenHeight;

			guess_m_pLetterboxTop->setPos( x, y );
			guess_m_pLetterboxBottom->setPos( x2, y2 );
		}
	}

	if ( guess_m_bStatsTimer == true && !guess_m_bTextOnly )
	{
		float flTime = gEngfuncs.GetClientTime();

		if ( flTime > guess_m_flStatsTime && !guess_m_bShowStats )
		{
			guess_IrrKlangSetLoop( true );
			guess_IrrKlangPlay( "stats.mp3" );
			guess_m_bShowStats = true;
			guess_m_pLetterboxBottom->setVisible( false );
			guess_m_pLetterboxTop->setVisible( false );

			if ( guess_g_iCoopCredits && gViewPort->guess_m_pCoopStats )
			{
				setVisible( false );
				gViewPort->guess_m_pCoopStats->setVisible( true );
			}
		}
	}

	s_bFadeFrame = !s_bFadeFrame;
	if ( !s_bFadeFrame )
	{
		Panel::solve();
		return;
	}

	if ( strlen( guess_m_szUnlockText ) > 2 )
	{
		int r, g, b, a;

		guess_m_pUnlockLabel->getFgColor( r, g, b, a );
		if ( a > 0 )
			a -= 15;
		if ( a < 0 )
			a = 0;

		guess_m_pUnlockLabel->setText( guess_m_szUnlockText );
		guess_m_pUnlockLabel->setVisible( true );
		guess_m_pUnlockLabel->setPos( 0, ScreenWidth / 2 );
		guess_m_pUnlockLabel->setFgColor( 255, 255, 255, a );
	}

	if ( guess_m_bUnlocks )
	{
		// show the unlocks one by one
		if ( guess_m_iUnlocks & 0x40 )
		{
			if ( !guess_g_bUnlockShown )
			{
				guess_ShowUnlock( 0x40, "Unlocked: FAMAS with infinite ammo", 50, "models/weapons/famas/w_famas.mdl", 0 );
				guess_g_bUnlockShown = true;
			}
		}
		else if ( guess_m_iUnlocks & 0x80 )
		{
			if ( !guess_g_bUnlockShown )
			{
				guess_ShowUnlock( 0x80, "Unlocked: David Leatherhoff suit", 50, "models/costumes/unfolded_clothes.mdl", 1 );
				guess_g_iUnlockClothes = 1;
				guess_g_bHoodieLoaded = false;
				guess_g_bJeansLoaded = false;
				guess_g_bFaceLoaded = false;
				guess_g_bUnlockShown = true;
			}
		}
		else if ( guess_m_iUnlocks & 0x100 )
		{
			if ( !guess_g_bUnlockShown )
			{
				guess_ShowUnlock( 0x100, "Unlocked: ModDB hoodie", 50, "models/costumes/unfolded_clothes.mdl", 0 );
				guess_g_iUnlockClothes = 2;
				guess_g_bHoodieLoaded = false;
				guess_g_bJeansLoaded = false;
				guess_g_bFaceLoaded = false;
				guess_g_bUnlockShown = true;
			}
		}
		else if ( guess_m_iUnlocks & 0x200 )
		{
			if ( !guess_g_bUnlockShown )
			{
				guess_ShowUnlock( 0x200, "Unlocked: Hello Kitty suit", 50, "models/costumes/unfolded_clothes.mdl", 1 );
				guess_g_iUnlockClothes = 3;
				guess_g_bHoodieLoaded = false;
				guess_g_bJeansLoaded = false;
				guess_g_bFaceLoaded = false;
				guess_g_bUnlockShown = true;
			}
		}
		else if ( guess_m_iUnlocks & 0x400 )
		{
			if ( !guess_g_bUnlockShown )
			{
				guess_ShowUnlock( 0x400, "Unlocked: Afraid of Monsters suit", 50, "models/costumes/unfolded_clothes.mdl", 1 );
				guess_g_iUnlockClothes = 4;
				guess_g_bHoodieLoaded = false;
				guess_g_bJeansLoaded = false;
				guess_g_bFaceLoaded = false;
				guess_g_bUnlockShown = true;
			}
		}
		else if ( guess_m_iUnlocks & 0x800 )
		{
			if ( !guess_g_bUnlockShown )
			{
				guess_ShowUnlock( 0x800, "Unlocked: Camouflage hoodie", 50, "models/costumes/unfolded_clothes.mdl", 0 );
				guess_g_iUnlockClothes = 5;
				guess_g_bHoodieLoaded = false;
				guess_g_bJeansLoaded = false;
				guess_g_bFaceLoaded = false;
				guess_g_bUnlockShown = true;
			}
		}
		else if ( guess_m_iUnlocks & 0x1000 )
		{
			if ( !guess_g_bUnlockShown )
			{
				guess_ShowUnlock( 0x1000, "Unlocked: Half Life Creations hoodie", 50, "models/costumes/unfolded_clothes.mdl", 0 );
				guess_g_iUnlockClothes = 6;
				guess_g_bHoodieLoaded = false;
				guess_g_bJeansLoaded = false;
				guess_g_bFaceLoaded = false;
				guess_g_bUnlockShown = true;
			}
		}
		else if ( guess_m_iUnlocks & 0x2000 )
		{
			if ( !guess_g_bUnlockShown )
			{
				guess_ShowUnlock( 0x2000, "Unlocked: Black Metal suit", 50, "models/costumes/unfolded_clothes.mdl", 1 );
				guess_g_iUnlockClothes = 7;
				guess_g_bHoodieLoaded = false;
				guess_g_bJeansLoaded = false;
				guess_g_bFaceLoaded = false;
				guess_g_bUnlockShown = true;
			}
		}
		else if ( guess_m_iUnlocks & 0x4000 )
		{
			if ( !guess_g_bUnlockShown )
			{
				guess_ShowUnlock( 0x4000, "Unlocked: Team Psykskallar hoodie", 50, "models/costumes/unfolded_clothes.mdl", 0 );
				guess_g_iUnlockClothes = 8;
				guess_g_bHoodieLoaded = false;
				guess_g_bJeansLoaded = false;
				guess_g_bFaceLoaded = false;
				guess_g_bUnlockShown = true;
			}
		}
		else if ( guess_m_iUnlocks & 0x8000 )
		{
			if ( !guess_g_bUnlockShown )
			{
				guess_ShowUnlock( 0x8000, "Unlocked: Fuck Anime suit", 50, "models/costumes/unfolded_clothes.mdl", 1 );
				guess_g_iUnlockClothes = 9;
				guess_g_bHoodieLoaded = false;
				guess_g_bJeansLoaded = false;
				guess_g_bFaceLoaded = false;
				guess_g_bUnlockShown = true;
			}
		}
		else if ( guess_m_iUnlocks & 0x10000 )
		{
			if ( !guess_g_bUnlockShown )
			{
				guess_ShowUnlock( 0x10000, "Unlocked: Sick Simon suit", 50, "models/costumes/unfolded_clothes.mdl", 1 );
				guess_g_iUnlockClothes = 10;
				guess_g_bHoodieLoaded = false;
				guess_g_bJeansLoaded = false;
				guess_g_bFaceLoaded = false;
				guess_g_bUnlockShown = true;
			}
		}
		else if ( guess_m_iUnlocks & 0x20000 )
		{
			if ( !guess_g_bUnlockShown )
			{
				guess_ShowUnlock( 0x20000, "Unlocked: AoM Twitcher suit", 50, "models/costumes/unfolded_clothes.mdl", 1 );
				guess_g_iUnlockClothes = 11;
				guess_g_bHoodieLoaded = false;
				guess_g_bJeansLoaded = false;
				guess_g_bFaceLoaded = false;
				guess_g_bUnlockShown = true;
			}
		}
		else if ( guess_m_iUnlocks & 0x40000 )
		{
			if ( !guess_g_bUnlockShown )
			{
				guess_ShowUnlock( 0x40000, "Unlocked: Custom hoodies", 50, "models/costumes/unfolded_clothes.mdl", 0 );
				guess_g_bUnlockShown = true;
			}
		}
		else if ( guess_m_iUnlocks & 0x80000 )
		{
			if ( !guess_g_bUnlockShown )
			{
				guess_ShowUnlock( 0x80000, "Unlocked: AoMDC axe", 50, "models/weapons/axe/w_axe.mdl", 0 );
				guess_g_bUnlockShown = true;
			}
		}
		else if ( guess_m_iUnlocks & 0x100000 )
		{
			if ( !guess_g_bUnlockShown )
			{
				guess_ShowUnlock( 0x100000, "Unlocked: Camera of Death", 50, "models/weapons/camera/w_camera.mdl", 0 );
				guess_g_bUnlockShown = true;
			}
		}
		else if ( guess_m_iUnlocks & 0x200000 )
		{
			if ( !guess_g_bUnlockShown )
			{
				guess_ShowUnlock( 0x200000, "Unlocked: Simon\'s Book", 50, "models/weapons/booklaser/w_booklaser.mdl", 0 );
				guess_g_bUnlockShown = true;
			}
		}
		else if ( guess_m_iUnlocks & 0x400000 )
		{
			if ( !guess_g_bUnlockShown )
			{
				guess_ShowUnlock( 0x400000, "Unlocked: Developer commentaries", 50, "models/developer_commentary.mdl", 0 );
				guess_g_bUnlockShown = true;
			}
		}
		else if ( guess_m_iUnlocks & 0x800000 )
		{
			if ( !guess_g_bUnlockShown )
			{
				guess_ShowUnlock( 0x800000, "Unlocked: Secret package", 50, "models/items/package.mdl", 0 );
				guess_g_bUnlockShown = true;
			}
		}
		else if ( guess_m_iUnlocks & 0x20 )
		{
			if ( !guess_g_bUnlockShown )
			{
				guess_ShowUnlock( 0x20, "Unlocked: Nightmare mode", 50, "models/items/cassette_tape.mdl", 0 );
				guess_g_bUnlockShown = true;
			}
		}
		else if ( guess_m_iUnlocks & 0x1000000 )
		{
			if ( !guess_g_bUnlockShown )
			{
				guess_ShowUnlock( 0x1000000, "Unlocked: Nightvision", 50, "models/items/w_nightvision.mdl", 0 );
				guess_g_bUnlockShown = true;
			}
		}
		else if ( guess_m_iUnlocks & 0x2000000 )
		{
			if ( !guess_g_bUnlockShown )
			{
				guess_ShowUnlock( 0x2000000, "Unlocked: Book page 1", 50, "models/items/generic_note.mdl", 0 );
				guess_g_bUnlockShown = true;
			}
		}
		else if ( guess_m_iUnlocks & 0x4000000 )
		{
			if ( !guess_g_bUnlockShown )
			{
				guess_ShowUnlock( 0x4000000, "Unlocked: Book page 2", 50, "models/items/generic_note.mdl", 0 );
				guess_g_bUnlockShown = true;
			}
		}
		else if ( guess_m_iUnlocks & 0x8000000 )
		{
			if ( !guess_g_bUnlockShown )
			{
				guess_ShowUnlock( 0x8000000, "Unlocked: Book page 3", 50, "models/items/generic_note.mdl", 0 );
				guess_g_bUnlockShown = true;
			}
		}
		else if ( guess_m_iUnlocks & 0x10000000 )
		{
			if ( !guess_g_bUnlockShown )
			{
				guess_ShowUnlock( 0x10000000, "Unlocked: Book page 4", 50, "models/items/generic_note.mdl", 0 );
				guess_g_bUnlockShown = true;
			}
		}
		else if ( guess_m_iUnlocks & 0x20000000 )
		{
			if ( !guess_g_bUnlockShown )
			{
				guess_ShowUnlock( 0x20000000, "Unlocked: Book page 5", 50, "models/items/generic_note.mdl", 0 );
				guess_g_bUnlockShown = true;
			}
		}
		else if ( guess_m_iUnlocks & 0x40000000 )
		{
			if ( !guess_g_bUnlockShown )
			{
				guess_ShowUnlock( 0x40000000, "Unlocked: Book page 6", 50, "models/items/generic_note.mdl", 0 );
				guess_g_bUnlockShown = true;
			}
		}
		else if ( guess_m_iUnlocks & 0x80000000 )
		{
			if ( !guess_g_bUnlockShown )
			{
				guess_ShowUnlock( 0x80000000, "Unlocked: Doctor mode", 50, "models/doctor_unlock.mdl", 0 );
				guess_g_bUnlockShown = true;
			}
		}
		else if ( guess_m_bContinue )
		{
			guess_m_bContinue = false;
			setVisible( false );
			gViewPort->UpdateCursorState();
			guess_g_iUnlockClothes = 0;

			if ( guess_iMenuMode )
			{
				gEngfuncs.pfnClientCmd( "map c_game_menu1" );
				gViewPort->UpdateCursorState();
			}
			else
			{
				gEngfuncs.pfnServerCmd( "closegame" );
				gViewPort->UpdateCursorState();
			}
		}
	}
	else
	{
		if ( guess_m_pUnlockLabel->isVisible() )
			guess_m_pUnlockLabel->setVisible( false );

		if ( !guess_m_bShowStats || guess_m_bTextOnly || guess_g_iCoopCredits )
		{
			// scroll the credits
			if ( guess_m_bFadeIn )
			{
				if ( guess_m_iBrightness < 255 )
					guess_m_iBrightness += 15;
			}
			else if ( guess_m_iBrightness > 0 )
			{
				guess_m_iBrightness -= 15;
			}

			if ( guess_m_iBrightness > 255 )
				guess_m_iBrightness = 255;
			else if ( guess_m_iBrightness < 0 )
				guess_m_iBrightness = 0;

			if ( guess_m_bTextOnly )
				guess_m_iBrightness = 255;

			for ( i = 1; i <= guess_m_iNumLines; i++ )
			{
				int x, y;
				int r, g, b, a;

				guess_m_pCreditLabel[i]->getPos( x, y );

				int iStep = ScreenHeight / guess_m_iScrollSpeed / 20;
				if ( cl_fastcredits->value )
					iStep = 20;
				else if ( iStep <= 0 )
					iStep = 1;

				if ( guess_m_bScrollUp )
					y += iStep;
				else
					y -= iStep;

				guess_m_pCreditLabel[i]->getFgColor( r, g, b, a );

				bool bScrollUp = guess_m_bScrollUp;

				if ( bScrollUp )
				{
					if ( y > 0 && y < ScreenHeight - ScreenHeight * 0.25 )
						a -= 5;
					if ( y > ScreenHeight - ScreenHeight * 0.25 )
						a += 5;
				}
				else
				{
					if ( y > ScreenHeight - ScreenHeight * 0.25 )
						a -= 5;
					if ( y < ScreenHeight * 0.25 )
						a += 5;
				}

				if ( i == guess_m_iNumLines / 2 && y > ScreenHeight / 2 - 16 && !guess_m_bFadeIn && !guess_m_bFadeDone && !guess_m_bTextOnly )
					guess_m_bFadeIn = true;

				if ( a < 0 )
					a = 0;
				else if ( a > 255 )
					a = 255;

				if ( y > ScreenHeight )
					a = 255;

				if ( bScrollUp )
				{
					if ( i == guess_m_iNumLines && y > ScreenHeight / 2 - 16 && !guess_m_bTextOnly )
					{
						guess_m_bFadeIn = false;
						guess_m_bFadeDone = true;
						y = ScreenHeight / 2 - 16;

						if ( !guess_m_bStatsTimer )
						{
							guess_m_bStatsTimer = true;
							guess_m_flStatsTime = gEngfuncs.GetClientTime() + 3.0;
							guess_IrrKlangStop( 2.0 );
						}
					}
				}
				else if ( i == guess_m_iNumLines && y < ScreenHeight / 2 - 16 && !guess_m_bTextOnly )
				{
					guess_m_bFadeIn = false;
					guess_m_bFadeDone = true;
					y = ScreenHeight / 2 - 16;

					if ( !guess_m_bStatsTimer )
					{
						guess_m_bStatsTimer = true;
						guess_m_flStatsTime = gEngfuncs.GetClientTime() + 3.0;
					}
				}

				guess_m_pCreditLabel[i]->setFgColor( guess_m_iBrightness, guess_m_iBrightness, guess_m_iBrightness, a );
				guess_m_pCreditLabel[i]->setPos( x, y );
			}
		}
		else
		{
			// show the statistics
			int x, y;
			int r, g, b, a;

			if ( guess_m_bContinue )
			{
				guess_m_bContinue = false;
				guess_m_bUnlocks = true;
			}

			setBgColor( 255, 255, 255, 255 );

			for ( i = 1; i <= guess_m_iNumLines; i++ )
			{
				guess_m_pCreditLabel[i]->getFgColor( r, g, b, a );
				a += 15;
				if ( a > 255 )
					a = 255;
				guess_m_pCreditLabel[i]->setFgColor( guess_m_iBrightness, guess_m_iBrightness, guess_m_iBrightness, a );
			}

			guess_m_bFadeIn = true;

			if ( guess_m_iBrightness < 255 )
				guess_m_iBrightness += 15;

			if ( guess_m_iBrightness > 255 )
				guess_m_iBrightness = 255;
			else if ( guess_m_iBrightness < 0 )
				guess_m_iBrightness = 0;

			if ( !guess_m_pGradeLabel->isVisible() )
				guess_m_pGradeLabel->setVisible( true );
			guess_m_pGradeLabel->getFgColor( r, g, b, a );
			if ( a > 0 )
				a -= 5;
			if ( a < 0 )
				a = 0;
			guess_m_pGradeLabel->setFgColor( guess_m_iBrightness, guess_m_iBrightness, guess_m_iBrightness, a );

			if ( !guess_m_pGradeShadowLabel->isVisible() )
				guess_m_pGradeShadowLabel->setVisible( true );
			guess_m_pGradeLabel->getFgColor( r, g, b, a );
			if ( a > 0 )
				a -= 1;
			if ( a < 0 )
				a = 0;
			guess_m_pGradeShadowLabel->setFgColor( 255, 0, 0, a );

			if ( !guess_m_pStatsTitleLabel->isVisible() )
				guess_m_pStatsTitleLabel->setVisible( true );
			guess_m_pStatsTitleLabel->getFgColor( r, g, b, a );
			if ( a > 0 )
				a -= 5;
			if ( a < 0 )
				a = 0;
			guess_m_pStatsTitleLabel->setFgColor( guess_m_iBrightness, guess_m_iBrightness, guess_m_iBrightness, a );

			for ( i = 1; i <= 14; i++ )
			{
				if ( !guess_m_pStatLabel[i]->isVisible() )
					guess_m_pStatLabel[i]->setVisible( true );
				guess_m_pStatLabel[i]->getFgColor( r, g, b, a );
				if ( a > 0 )
					a -= 5;
				if ( a < 0 )
					a = 0;
				guess_m_pStatLabel[i]->setVisible( true );
				guess_m_pStatLabel[i]->getPos( x, y );
				guess_m_pStatLabel[i]->setFgColor( guess_m_iBrightness, guess_m_iBrightness, guess_m_iBrightness, a );
				guess_m_pStatLabel[i]->setPos( x, y );
			}
		}

		if ( guess_m_bShowStats && !guess_m_bTextOnly && !guess_g_iCoopCredits )
		{
			int r, g, b, a;

			getBgColor( r, g, b, a );
			if ( a < 255 )
				a += 5;
			if ( a > 255 )
				a = 255;
			setBgColor( 255 - guess_m_iBrightness, 255 - guess_m_iBrightness, 255 - guess_m_iBrightness, a );
		}
		else if ( !guess_m_bTextOnly )
		{
			setBgColor( 255 - guess_m_iBrightness, 255 - guess_m_iBrightness, 255 - guess_m_iBrightness, 0 );
		}

		if ( !guess_m_bTextOnly )
		{
			Panel::solve();
			return;
		}
	}

	setBgColor( 255, 255, 255, 255 );
	Panel::solve();
}
