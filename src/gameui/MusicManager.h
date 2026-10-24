//========= Copyright (c) 1996-2003, Valve LLC, All rights reserved. ============
//
// Purpose: The menu music player
//
// $NoKeywords: $
//=============================================================================

#ifndef MUSICMANAGER_H
#define MUSICMANAGER_H

#ifdef _WIN32
#pragma once
#endif

#include "IMusicManager.h"

//-----------------------------------------------------------------------------
// Purpose: Plays the menu and round end music tracks
// (the data members are declared with MusicManager.cpp)
//-----------------------------------------------------------------------------
class MusicManager : public IMusicManager
{
public:
	MusicManager();
	virtual ~MusicManager();

	virtual void StartTrack( TrackType track, bool looping );
	virtual void StopTrack();
	virtual void FadeOutTrack();
	virtual bool IsPlayingTrack();
	virtual bool IsPlayingTrack( TrackType track );

	void Init();
	void Reset();
};

extern MusicManager *TheMusicManager;

#endif // MUSICMANAGER_H
