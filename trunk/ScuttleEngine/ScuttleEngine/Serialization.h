/******************************************************************************/
/*!
\file   Serialization.h
\author Matt Sutherlin
\par    email: matt.sutherlin\@digipen.edu
\par    DigiPen login: matt.sutherlin
\par    Course: GAM200
\date   7/14/2011
\brief  
Base serialization interface.
\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/******************************************************************************/

#pragma once

#include "Precompiled.h"

namespace Scuttlemeyer
{
	//Abstract serialization interface.  DO NOT CREATE AN ISERIALIZER OBJECT!
	class ISerializer
	{
	public:
		virtual bool Open(const std::string& filename)=0;
		virtual bool IsGood()=0;
	};
}