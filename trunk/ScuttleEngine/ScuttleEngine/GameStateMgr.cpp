/******************************************************************************/
/*!
\file   GameStateMgr.cpp
\author Kevin Verholtz
\par    email: kevin.verholtz\@digipen.edu
\par    DigiPen login: kevin.verholtz
\par    Course: GAM200
\date   12/8/2011
\brief  
Most of the code is in Win Main where the GameLoop is.
\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/******************************************************************************/
#include "Precompiled.h"
#include "GameStateMgr.h"

// ---------------------------------------------------------------------------
// globals
namespace Scuttlemeyer
{

	// variables to keep track the current, previous and next game state
	unsigned int	gGameStateInit;
	unsigned int	gGameStateCurr;
	unsigned int	gGameStatePrev;
	unsigned int	gGameStateNext;

	// ---------------------------------------------------------------------------
	// Functions implementations
	/******************************************************************************/
	/*!
	  \brief
		Initializes the gloabal gamestates

	  \param init
		what to set the initial game state to.

	*/
	/******************************************************************************/
	void GameStateMgrInit(unsigned int init)
	{
		// set the initial game state
		gGameStateInit = init;

		// reset the current, previous and next game
		gGameStateCurr = gGameStatePrev = gGameStateNext = gGameStateInit;

	}

	// ---------------------------------------------------------------------------
	// ---------------------------------------------------------------------------
}