// ======================================
// Cry of Fear ammo inventory panel header file
// (class names from the RTTI of client.dll, file name guessed)
// ======================================

#ifndef _AMMOINVENTORY_H
#define _AMMOINVENTORY_H
using namespace vgui;

#define guess_MAX_AMMO_SLOTS	6

// the slot arrays are indexed by the slot number, 1 to guess_MAX_AMMO_SLOTS
// (the viewport never creates this panel in 1.6a, so its size is unknown)
class CAmmoInventory : public Panel
{
public:
	CAmmoInventory();
	void Initialize();
	void ActionSignal( int cmd );
	int KeyInput( int down, int keynum, const char *pszCurrentBinding );

	virtual void setVisible( bool state );

protected:
	virtual void solve();

public:
	Image	*guess_m_pBackgroundImage;					// 0xbc, gfx/vgui/backgroundtoall.tga
	ImagePanel	*guess_m_pBackground;					// 0xc0
	int		guess_m_iDropMode;							// 0xc4, set by button 7, the slot buttons then drop their ammo
	int		guess_m_iSelected;							// 0xc8, slot under the cursor
	Label	*guess_m_pCountLabel[guess_MAX_AMMO_SLOTS + 1];	// 0xcc, ammo count of the AmmoInv message
	int		guess_m_iAmmoType[guess_MAX_AMMO_SLOTS + 1];		// 0xe8, 1 glock, 2 m16, 3 p345, 4 shotgun, 5 tmp
	CImageLabel	*guess_m_pSlotImage[guess_MAX_AMMO_SLOTS + 1];	// 0x104, gfx/vgui/ammo/*_slot.tga
	CImageLabel	*guess_m_pSelectedImage;				// 0x120, gfx/vgui/ammo/*_check.tga of the selected slot
	ColorButton	*guess_m_pButton[guess_MAX_AMMO_SLOTS + 1];	// 0x124
	ColorButton	*guess_m_pDropButton;					// 0x140
	Image	*guess_m_pInventoryImage;					// 0x144, gfx/vgui/ammo/ammo_inventory.tga
	ImagePanel	*guess_m_pInventory;					// 0x148, parent of the slots
};

class CAmmoInventoryHandler_Command : public ActionSignal
{
private:
	CAmmoInventory	*m_pFather;
	int				m_cmd;

public:
	CAmmoInventoryHandler_Command( CAmmoInventory *panel, int cmd )
	{
		m_pFather = panel;
		m_cmd = cmd;
	}

	virtual void actionPerformed( Panel *panel )
	{
		m_pFather->ActionSignal( m_cmd );
	}
};

void guess_AmmoInvInit();

#endif // _AMMOINVENTORY_H
