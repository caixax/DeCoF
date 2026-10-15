// ======================================
// Cry of Fear co-op statistics panel header file
// (class names from the RTTI of client.dll, file name guessed)
// ======================================

#ifndef _COOPSTATS_H
#define _COOPSTATS_H
using namespace vgui;

#define guess_MAX_COOP_PLAYERS	4

class CCoopStats : public Panel
{
public:
	CCoopStats();
	void Initialize();

public:
	int		guess_m_iUnknownBC;				// 0xbc, only cleared
	Label	*guess_m_pTitleLabel;			// 0xc0, "STATS - CAMPAIGN WILL AUTORESTART SOON"
	Label	*guess_m_pGradeLabel;			// 0xc4, "Ending seen: 7" and "Grade: %c"
	Label	*guess_m_pStatLabel[guess_MAX_COOP_PLAYERS + 1][10];	// 0xc8, [player 1-4][1-9], ends at 0x190
};

void SetEndingOrUnlockables( int type, bool hasntgot, bool ending6, bool ending7 );

#endif // _COOPSTATS_H
