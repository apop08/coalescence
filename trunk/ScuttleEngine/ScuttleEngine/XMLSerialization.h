/******************************************************************************/
/*!
\file   XMLSerialization.h
\author Matt Sutherlin
\par    email: matt.sutherlin\@digipen.edu
\par    DigiPen login: matt.sutherlin
\par    Course: GAM200
\date   7/14/2011
\brief  
Serialization system for XML files.  Started, but incomplete.
\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/******************************************************************************/

#pragma once

#include "Serialization.h"
#include <fstream>

namespace Scuttlemeyer
{
	class XMLSerialization : public ISerializer
	{
	public:
		std::ifstream mStream;
		virtual bool Open(const std::string& filename)=0;
		virtual bool IsValid()=0;
		virtual void ReadInt(int &i)=0;
		virtual void ReadFloat(float &f)=0;
		virtual void ReadString(std::string &s)=0;
	};
}