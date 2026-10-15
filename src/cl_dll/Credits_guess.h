// ======================================
// Cry of Fear credits, statistics and unlocks panel header file
// (class names from the RTTI of client.dll, file name guessed)
// ======================================

#ifndef _CREDITS_H
#define _CREDITS_H
using namespace vgui;

#define guess_MAX_CREDIT_LINES	255

class CCredits : public Panel
{
public:
	CCredits();
	void Initialize();

	int guess_CountLines();
	void guess_SetLine( int iUnused, int iLine );
	void guess_ShowUnlock( int iUnlock, const char *pszText, int iDistance, char *pszModel, int iBody );

protected:
	virtual void solve();

public:
	bool	guess_m_bShowStats;				// 0xbc, the statistics are shown after the credits
	bool	guess_m_bTextOnly;				// 0xbd, a text file without statistics (Credits message byte 1)
	bool	guess_m_bContinue;				// 0xbe, set by a key press (input.cpp)
	int		guess_m_iUnknownC0;				// 0xc0, never used in 1.6a
	Panel	*guess_m_pLetterboxTop;			// 0xc4
	Panel	*guess_m_pLetterboxBottom;		// 0xc8
	char	guess_m_szUnlockText[256];		// 0xcc, "Unlocked: %s" shown by guess_ShowUnlock
	bool	guess_m_bUnlocks;				// 0x1cc, showing the unlocks after the statistics
	int		guess_m_iUnlocks;				// 0x1d0, bit field of the unlocks still to show
	int		guess_m_iScrollSpeed;			// 0x1d4
	int		guess_m_iUnknown1D8[4];			// 0x1d8, never used in 1.6a
	int		guess_m_iShotsFired;			// 0x1e8
	int		guess_m_iShotsHit;				// 0x1ec
	int		guess_m_iSyringesUsed;			// 0x1f0
	int		guess_m_iShotsMissed;			// 0x1f4
	float	guess_m_flAccuracy;				// 0x1f8
	int		guess_m_iTimesSaved;			// 0x1fc
	int		guess_m_iWeaponsFound;			// 0x200
	int		guess_m_iDamageTaken;			// 0x204
	float	guess_m_flDistanceDecimal;		// 0x208
	float	guess_m_flDistance;				// 0x20c, km
	int		guess_m_iSeconds;				// 0x210
	int		guess_m_iMinutes;				// 0x214
	int		guess_m_iHours;					// 0x218
	int		guess_m_iMeleeKills;			// 0x21c
	int		guess_m_iFirearmKills;			// 0x220
	int		guess_m_iDifficulty;			// 0x224, 1 easy to 4 nightmare
	Label	*guess_m_pStatsTitleLabel;		// 0x228, "Statistics and grade"
	Label	*guess_m_pGradeLabel;			// 0x22c
	Label	*guess_m_pGradeShadowLabel;		// 0x230, red copy of the grade
	Label	*guess_m_pStatLabel[15];		// 0x234, [1-14]
	int		guess_m_iUnknown270[17];		// 0x270, never used in 1.6a
	Label	*guess_m_pUnlockLabel;			// 0x2b4, text of the unlock shown
	bool	guess_m_bStatsTimer;			// 0x2b8
	float	guess_m_flStatsTime;			// 0x2bc, the statistics are shown after this time
	char	guess_m_szFile[256];			// 0x2c0, text file of the credits
	Label	*guess_m_pCreditLabel[guess_MAX_CREDIT_LINES + 1];	// 0x3c0, [1-255], one per line of the file
	int		guess_m_iNumLines;				// 0x7c0
	bool	guess_m_bStarted;				// 0x7c4, set by the Credits message, never read in 1.6a
	bool	guess_m_bScrollUp;				// 0x7c5, false when the lines come from the bottom
	int		guess_m_iBrightness;			// 0x7c8, 0-255
	bool	guess_m_bFadeIn;				// 0x7cc
	bool	guess_m_bFadeDone;				// 0x7cd
	Font	*guess_m_pCreditsFont;			// 0x7d0, scheme "credits"
	Font	*guess_m_pCreditsTitleFont;		// 0x7d4, scheme "creditstitle", lines starting with #
	float	guess_m_flStartTime;			// 0x7d8
};

extern bool guess_g_bUnlockShown;

char *guess_va( const char *format, ... );

#endif // _CREDITS_H
