/******************************************************************************/
/*!
\file   GameStateList.h
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
// game state list
namespace Scuttlemeyer
{
	enum
	{
		// list of all game states
		GS_SPLASH = 0,
		GS_MENU,
		GS_PLAY,
		GS_PAUSE,
		GS_TRANSITION,
		GS_RESTART,
		GS_RELOAD,
		GS_QUIT,
		GS_NUM
	};
}