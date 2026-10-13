// ======================================
// Cry of Fear inventory panel header file
// (class names from the RTTI of client.dll, file name guessed)
// ======================================

#ifndef _RECBINVENTORY_H
#define _RECBINVENTORY_H
using namespace vgui;

class CRECBInventory : public CMenuPanel
{
public:
	CRECBInventory( int iTrans, int iRemoveMe, int x, int y, int wide, int tall );
	void Initialize();
	void ActionSignal( int cmd );
	int KeyInput( int down, int keynum, const char *pszCurrentBinding );

	void guess_Reset();
	void guess_DoAction( int slot );
	bool guess_IsCursorOver( int x, int y, int wide, int tall );
	void guess_SelectItem( int slot );
	void guess_SetAction( int action );
	void guess_Update();

	virtual void setVisible( bool state );

protected:
	virtual void solve();

public:
	Image	*guess_m_pBackgroundImage;		// 0xd4, gfx/vgui/backgroundtoall.tga
	ImagePanel	*guess_m_pBackground;		// 0xd8
	CImageLabel	*guess_m_pMenuImage;		// 0xdc, inventory_weapons.tga or inventory_items.tga behind the item menu
	bool	guess_m_bUnknownE0;				// 0xe0, cleared by setVisible
	int		guess_m_iUnknownE4;				// 0xe4, cleared by setVisible
	int		guess_m_iDualWield;				// 0xe8, slot picked for inventorydualwield (menu command 53)
	int		guess_m_iCombine;				// 0xec, slot picked for inventorycombine (menu command 51)
	int		guess_m_iSelected;				// 0xf0, slot of the item menu (1 to 6)
	int		guess_m_iSelectionAlpha;		// 0xf4, alpha of the red border of guess_m_pSelection
	Panel	*guess_m_pSelection;			// 0xf8, red border around the slot picked for combine or dual wield
	int		guess_m_iUnusedFC;				// 0xfc, not referenced by any code
	ColorButton	*guess_m_pMenuButtons[7];	// 0x100, item menu, commands 50 to 56
	int		guess_m_iUnused11C;				// 0x11c, not referenced by any code
	CImageLabel	*guess_m_pItemImages[10];	// 0x120, pictures of the 6 items, the 3 quick slots ([6], [7], [9]) and the hovered item ([8])
	Panel	*guess_m_pBackgroundPanel;		// 0x148
	Image	*guess_m_pInventoryImage;		// 0x14c, gfx/vgui/640_inventory.tga
	ImagePanel	*guess_m_pInventory;		// 0x150, parent of the buttons and pictures
	CImageLabel	*guess_m_pPopupImage;		// 0x154
	int		guess_m_iUnused158;				// 0x158, not referenced by any code
	ColorButton	*guess_m_pItemButtons[6];	// 0x15c, item slots, commands 1 to 6
	int		guess_m_iUnused174[3];			// 0x174, not referenced by any code
	ColorButton	*guess_m_pOptionButtons[5];	// 0x180, commands 7, 8, 10, 15 and 16
	ColorButton	*guess_m_pQuickSlot1Button;	// 0x194, command 12
	ColorButton	*guess_m_pQuickSlot2Button;	// 0x198, command 13
	ColorButton	*guess_m_pQuickSlot3Button;	// 0x19c, command 14
	Label	*guess_m_pObjectiveLabel;		// 0x1a0, gHUD.guess_m_szObjectiveText
	int		guess_m_iThree;					// 0x1a4, picks gfx/vgui/640_inventory_three.tga
	int		guess_m_iUnused1A8[2];			// 0x1a8, not referenced by any code
	int		guess_m_iAction;				// 0x1b0, set by guess_SetAction, run by guess_DoAction
	int		guess_m_iUnused1B4;				// 0x1b4, not referenced by any code
	int		guess_m_iActionItem1;			// 0x1b8, first slot of the combine and dual wield actions
	int		guess_m_iActionItem2;			// 0x1bc, second slot
	int		guess_m_iHoverItem;				// 0x1c0, slot shown in guess_m_pItemImages[8] (1 to 6, 7 to 9 quick slots)
};

class CRECBInventoryHandler_Command : public ActionSignal
{
private:
	CRECBInventory	*m_pFather;
	int				m_cmd;

public:
	CRECBInventoryHandler_Command( CRECBInventory *panel, int cmd )
	{
		m_pFather = panel;
		m_cmd = cmd;
	}

	virtual void actionPerformed( Panel *panel )
	{
		m_pFather->ActionSignal( m_cmd );
	}
};

void guess_QSlotInit();

#endif // _RECBINVENTORY_H
