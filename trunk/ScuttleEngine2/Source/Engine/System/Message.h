/**************************************************************************************************/
/*!
\file	Message.h
\par	Project: ScuttleEngine 2
\date	5/16/12
\authors
Primary  : Matt Sutherlin
\brief
	Message class used for intersystem communications.

\copyright 
	“All content © 2012 DigiPen (USA) Corporation, all rights reserved.”
*/
/**************************************************************************************************/

#pragma once

#include "../../Precompiled.h"

namespace Scuttlemeyer
{
	namespace Mid
	{
		enum MessageIdType
		{
			None,
			KeyDown,
			INACTIVE,
			LBUTTONDOWN,
			RBUTTONDOWN,
			LBUTTONUP,
			RBUTTONUP,
			MOUSEMOVE,
			MOUSESCROLL,
			UpKey,
			Menu,
			Play,
			Pause,
			Splash,
			Restart,
			Reload,
			Quit,
			Activate,
			Deactivate
		};
	}

	class Message
	{
	};
}