//========= Copyright (c) 1996-2003, Valve LLC, All rights reserved. ============
//
// Purpose: the Internet Explorer control behind vgui2::IHTML (HtmlWindow.cpp)
//
// $NoKeywords: $
//=============================================================================

#ifndef HTMLWINDOW_H
#define HTMLWINDOW_H

#ifdef _WIN32
#pragma once
#endif

#include <windows.h>
#include <vgui/VGUI.h>
#include <vgui/IHTML.h>

using vgui2::IHTML;
using vgui2::IHTMLEvents;
using vgui2::VPANEL;

class HtmlWindow : public IHTML, public IHTMLEvents
{
public:
	HtmlWindow(IHTMLEvents *events, VPANEL c, HWND parent, bool AllowJavaScript, bool DirectToHWND);
	virtual ~HtmlWindow();

	// IHTML
	virtual void OpenURL(const char *);
	virtual bool StopLoading();
	virtual bool Refresh();
	virtual bool Show(bool shown);
	virtual char *GetOpenedPage();
	virtual void OnSize(int x, int y, int w, int h);
	virtual void GetHTMLSize(int &wide, int &tall);
	virtual void Clear();
	virtual void AddText(const char *text);
	virtual void OnMouse(vgui2::MouseCode code, MOUSE_STATE s, int x, int y);
	virtual void OnChar(wchar_t unichar);
	virtual void OnKeyDown(vgui2::KeyCode code);
	virtual vgui2::IImage *GetBitmap();
	virtual void SetVisible(bool state);

	virtual void OnPaint(HDC hDC);

	// IHTMLEvents
	virtual bool OnStartURL(const char *url, const char *target, bool first);
	virtual void OnFinishURL(const char *url);
	virtual void OnProgressURL(long current, long maximum);
	virtual void OnSetStatusText(const char *text);
	virtual void OnUpdate();
	virtual void OnLink();
	virtual void OffLink();

private:
	// NOTE: HtmlWindow.cpp is not written yet. This block stands only for the
	// size of the members (the object is 0x290 bytes: the operator new in
	// BaseUISurface::CreateHTMLWindow); no code reads it.
	char m_Members_guess[0x288];
};

#endif // HTMLWINDOW_H
