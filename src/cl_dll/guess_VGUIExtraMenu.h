// ======================================
// Cry of Fear extras menu header file
// (class names from the RTTI of client.dll, file name guessed)
// ======================================

#ifndef _VGUIEXTRAMENU_H
#define _VGUIEXTRAMENU_H
using namespace vgui;

class CVGUIExtraMenu : public Panel
{
public:
	CVGUIExtraMenu();
	~CVGUIExtraMenu();
	void Initialize();
	void ActionSignal( int cmd );

	virtual void setVisible( bool state );

protected:
	virtual void solve();

public:
	Image	*guess_m_pDonateImage;		// 0xbc, gfx/vgui/menu/donate.tga
	ImagePanel	*guess_m_pDonate;		// 0xc0
	Image	*guess_m_pSoundtrack1Image;		// 0xc4, gfx/vgui/menu/soundtrack1.tga
	ImagePanel	*guess_m_pSoundtrack1;		// 0xc8
	Image	*guess_m_pSoundtrack2Image;		// 0xcc, gfx/vgui/menu/soundtrack2.tga
	ImagePanel	*guess_m_pSoundtrack2;		// 0xd0
	Image	*guess_m_pClothesImage;		// 0xd4, gfx/vgui/menu/clothes.tga
	ImagePanel	*guess_m_pClothes;		// 0xd8
	Image	*guess_m_pAoMDCImage;		// 0xdc, gfx/vgui/menu/aomdc.tga
	ImagePanel	*guess_m_pAoMDC;		// 0xe0
	Image	*guess_m_pFacebookImage;		// 0xe4, gfx/vgui/menu/facebook.tga
	ImagePanel	*guess_m_pFacebook;		// 0xe8
	Image	*guess_m_pCancelImage;		// 0xec, gfx/vgui/menu/cancel.tga
	ImagePanel	*guess_m_pCancel;		// 0xf0
	ColorButton	*guess_m_pDonateButton;	// 0xf4
	ColorButton	*guess_m_pSoundtrack1Button;	// 0xf8
	ColorButton	*guess_m_pSoundtrack2Button;	// 0xfc
	ColorButton	*guess_m_pClothesButton;	// 0x100
	ColorButton	*guess_m_pAoMDCButton;	// 0x104
	ColorButton	*guess_m_pFacebookButton;	// 0x108
	ColorButton	*guess_m_pCancelButton;	// 0x10c
	bool	guess_m_bDonateArmed;		// 0x110, the donate_mo.tga picture is shown
	bool	guess_m_bSoundtrack1Armed;		// 0x111, the soundtrack1_mo.tga picture is shown
	bool	guess_m_bSoundtrack2Armed;		// 0x112, the soundtrack2_mo.tga picture is shown
	bool	guess_m_bClothesArmed;		// 0x113, the clothes_mo.tga picture is shown
	bool	guess_m_bAoMDCArmed;		// 0x114, the aomdc_mo.tga picture is shown
	bool	guess_m_bFacebookArmed;		// 0x115, the facebook_mo.tga picture is shown
	bool	guess_m_bCancelArmed;		// 0x116, the cancel_mo.tga picture is shown
};

class CVGUIExtraMenuHandler_Command : public ActionSignal
{
private:
	CVGUIExtraMenu	*m_pFather;
	int			m_cmd;

public:
	CVGUIExtraMenuHandler_Command( CVGUIExtraMenu *panel, int cmd )
	{
		m_pFather = panel;
		m_cmd = cmd;
	}

	virtual void actionPerformed( Panel *panel )
	{
		m_pFather->ActionSignal( m_cmd );
	}
};

#endif // _VGUIEXTRAMENU_H
