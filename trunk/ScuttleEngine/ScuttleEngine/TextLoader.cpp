/******************************************************************************/
/*!
\file   TextLoader.cpp
\author Matt Sutherlin
\par    email: matt.sutherlin\@digipen.edu
\par    DigiPen login: matt.sutherlin
\par    Course: GAM200
\date   9/20/2011
\brief  
Text file loading system built on the core serializer.
\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/******************************************************************************/

#include "Precompiled.h"
#include "TextSerialization.h"

namespace Scuttlemeyer
{
	bool TextLoader::Open(const std::string &filename)
	{
		mStream.open(filename.c_str());
		return mStream.good();
	}

	bool TextLoader::IsGood(void)
	{
		return mStream.good();
	}

	void TextLoader::ReadInt(int &i)
	{
		mStream >> i;
	}

	void TextLoader::ReadUInt(unsigned int &ui)
	{
		mStream >> ui;
	}


	void TextLoader::ReadFloat(float &f)
	{
		mStream >> f;
	}

	void TextLoader::ReadString(std::string &s)
	{
		mStream >> s;
	}

	void TextLoader::ReadDWORD(DWORD &d)
	{
		mStream >> d;
	}
}