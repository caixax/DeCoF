//========= Copyright (c) 1996-2003, Valve LLC, All rights reserved. ============
//
// Purpose: The career game state: profile, roster, maps and the current match
//
// $NoKeywords: $
//=============================================================================

#ifndef CAREERGAME_H
#define CAREERGAME_H

#ifdef _WIN32
#pragma once
#endif

#include "ICareerUI.h"
#include <vector>

class CCareerProfileData;
class CCareerProfileDifficultyData;
class CCareerCharacter;
class CCareerMap;
class BufferWriter;
class CvarLatch;

typedef std::vector< CCareerCharacter * > CharacterVec;
typedef CharacterVec::iterator CharacterVecIt;

typedef std::vector< CCareerMap * > MapVec;
typedef MapVec::iterator MapVecIt;

typedef std::vector< MapVec > MapsVec;
typedef MapsVec::iterator MapsVecIt;

typedef std::vector< int > CharCostVector;

typedef std::vector< CvarLatch * > CvarLatchVector;

//-----------------------------------------------------------------------------
// Purpose: The career game
//-----------------------------------------------------------------------------
class CCareerGame : public ICareerUI
{
public:
	CCareerGame();
	virtual ~CCareerGame();

	static void Create();
	void Init();
	void Reset();
	void SetDifficulty( CareerDifficultyType difficulty );
	bool AreChecksumsOk();

	bool LoadProfile( CareerDifficultyType );
	bool SaveProfile( bool );
	void DeleteProfile( const char * );

	void Update();

	static const char *GetSaveGamePlayerName( const char * );

	CharacterVec &GetHiredCharacters();
	CharacterVec &GetAvailableCharacters();
	void ClearRoster();
	void AddCharacter( CCareerCharacter * );
	bool Hire( const char *name );
	bool Fire( const char *name );

	int GetPoints();
	int GetPointsRemaining();
	void SetPoints( int );
	int GetHighestCostAvailable( int reputationPoints );
	void MarkJumpButtonsFlashed();
	int GetPointsFlashed();

	void AddMapTriplet( MapVec );
	void PrintMaps();
	char *SaveMaps( char *, int & );
	void DefeatMap( const char *mapName );
	bool DoesCurrentMapUnlockTriplet();
	const char *GetLastMap();
	bool SetLastMap( const char *mapName );
	CCareerMap *GetLastMapPtr();
	CCareerMap *GetPlayableMap( const char *name );
	MapsVec &GetMaps();
	CCareerMap *GetMap( const char *name );
	int GetMapTripletNum( const char *name );
	int GetMapTripletOffset( const char *name );

	ITaskVec *GetTaskVecForMap( const char *mapName );
	int GetNumTasksOnCurrentMap();
	ICareerTask *GetCurrentTask( int index );

	void StartMatch();
	void StopMatch( int ctWins, int tWins );
	void StartLoading();
	void SendMatchCommand();
	void FinishMatchStart();

	// ICareerUI
	virtual bool PlayAsCT();
	virtual bool IsPlayingMatch();
	virtual ITaskVec *GetCurrentTaskVec();
	virtual int GetReputationGained();
	virtual int GetNumMapsUnlocked();
	virtual bool DoesWinUnlockAll();
	virtual int GetRoundTimeLength();
	virtual int GetWinfastLength();
	virtual CareerDifficultyType GetDifficulty() const;
	virtual int GetCurrentMapTriplet( MapInfo *maps );
	virtual void OnRoundEndMenuOpen( bool didWin );
	virtual void OnMatchEndMenuOpen( bool didWin );
	virtual void OnRoundEndMenuClose( bool stillPlaying );
	virtual void OnMatchEndMenuClose( bool stillPlaying );

	void SetBotPoolSliderPosition( int position );
	int GetBotPoolSliderPosition();
	int GetMatchNumWins() const;
	int GetMatchWinBy() const;
	CCareerMap *GetFirstUndefeatedMap();
	void SetRoundTimeLength( int val );
	void SetWinfastLength( int );
	bool IsMedalEarned( CareerDifficultyType );

	std::vector< CCareerProfileData * > BuildProfileList();
	CCareerProfileData *GetProfileForName( const char * );
	void CreateBlankProfile();

	bool CanUnpause() const { return m_canUnpause; }

protected:
	bool ParseSavedProfile( const unsigned char *, int, CCareerProfileData * );
	bool ParseMedalChunk( CCareerProfileData *, int );
	bool ParseDifficultyChunk( CCareerProfileDifficultyData *, int );
	bool ParseTutorMessageDecayChunk( CCareerProfileData *, int );
	void ApplySavedProfile( CCareerProfileData *, CareerDifficultyType );
	void ApplyDifficultyChunk( CCareerProfileDifficultyData * );
	void ApplyTutorMessageData( int * );
	int CreateProfileBuffer( CCareerProfileData *, unsigned char *, int, bool );
	void WriteMedalChunk( CCareerProfileData * );
	void WriteDifficultyChunk( CCareerProfileDifficultyData * );
	void WriteTutorMessageDecayChunk( bool );
	bool UpdateSavedProfile();
	void UpdateMedalChunk();
	void UpdateDifficultyChunk();
	void AddSavedCvar( const char *cvarName, const char *cvarVal, bool wait );

	CCareerProfileData *m_currentProfile;
	BufferWriter *m_writer;
	int m_reputationPoints;
	int m_reputationPointsFlashed;
	CharCostVector m_costAvailability;
	bool m_queuedLoadMenu;
	char m_queuedName[32];
	bool m_wasMapDefeatedAtStart;
	bool m_needAutosave;
	static bool m_checksumsOk;
	CharacterVec m_hiredCharacters;
	CharacterVec m_availableCharacters;
	MapsVec m_maps;
	bool m_isPlaying;
	char m_lastMapName[64];
	CCareerMap *m_pLastMap;
	int m_basePoints;
	int m_roundTime;
	int m_bombCountdownTime;
	int m_freezeTime;
	int m_isCT;
	bool m_initDone;
	int m_botPoolSliderPosition;
	int m_matchNumWins;
	int m_matchWinBy;
	int m_matchReputationGain;
	int m_matchMapsUnlocked;
	int m_roundTimeLength;
	int m_winfastLength;
	CvarLatchVector m_savedCvars;
	CareerDifficultyType m_difficulty;
	bool m_medals[MAX_CAREER_DIFFICULTY];
	bool m_canUnpause;
	int m_ctWins;
	int m_tWins;
};

extern CCareerGame *TheCareerGame;

#endif // CAREERGAME_H
