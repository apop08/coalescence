/******************************************************************************/
/*!
\file   GameStateMgr.h
\author Kevin Verholtz
\par    email: kevin.verholtz\@digipen.edu
\par    DigiPen login: kevin.verholtz
\par    Course: GAM200
\date   12/8/2011
\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/******************************************************************************/
#pragma once
// ---------------------------------------------------------------------------
// include the list of game states

#include "GameStateList.h"
namespace Scuttlemeyer
{
	// ---------------------------------------------------------------------------
	// externs

	extern unsigned int gGameStateInit;
	extern unsigned int gGameStateCurr;
	extern unsigned int gGameStatePrev;
	extern unsigned int gGameStateNext;

	// ---------------------------------------------------------------------------
	// ---------------------------------------------------------------------------
	// Function prototypes

	// call this at the beginning and AFTER all game states are added to the manager
	void GameStateMgrInit(unsigned int gameStateInit);

}