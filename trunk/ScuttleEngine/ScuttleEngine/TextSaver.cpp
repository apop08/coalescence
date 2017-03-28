/******************************************************************************/
/*!
\file   TextSaver.cpp
\author Matt Sutherlin
\par    email: matt.sutherlin\@digipen.edu
\par    DigiPen login: matt.sutherlin
\par    Course: GAM200
\date   9/20/2011
\brief  
Text file saving system built on the core serializer.
\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/******************************************************************************/

#include "Precompiled.h"
#include "TextSerialization.h"
#include <sstream>

namespace Scuttlemeyer
{
	bool TextSaver::Open(const std::string &filename)
	{
		mStream.open(filename.c_str());
		return mStream.good();
	}

	bool TextSaver::IsGood(void)
	{
		return mStream.good();
	}

	void TextSaver::WriteInt(int &i)
	{
		mStream << i;
		mStream << std::endl;
	}

	void TextSaver::WriteFloat(float &f)
	{
		mStream << f;
		mStream << std::endl;
	}

	void TextSaver::WriteString(std::string &s)
	{
		mStream << s;
		mStream << std::endl;
	}

	void TextSaver::WriteDWORD(DWORD &d)
	{
		mStream << d;
		mStream << std::endl;
	}

	void TextSaver::WriteChar(char &c)
	{
		mStream << c;
		mStream << std::endl;
	}

	std::vector<std::string>& TextSaver::split(const std::string &s, char delim, std::vector<std::string> &elems)
	{
		std::stringstream ss(s);
		std::string item;

		while(std::getline(ss, item, delim))
		{
			elems.push_back(item);
		}

		return elems;
	}


	std::vector<std::string> TextSaver::split(const std::string &s, char delim)
	{
		std::vector<std::string> elems;

		return split(s, delim, elems);
	}
}