/******************************************************************************/
/*!
\file   TextSerialization.h
\author Matt Sutherlin
\par    email: matt.sutherlin\@digipen.edu
\par    DigiPen login: matt.sutherlin
\par    Course: GAM200
\date   9/20/2011
\brief  
Serialization interface for text files.
\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/******************************************************************************/

#pragma once

#include "Serialization.h"
#include <fstream>

namespace Scuttlemeyer
{
	class TextLoader : public ISerializer
	{
	public:
		virtual bool Open(const std::string& filename);
		virtual bool IsGood();
		void ReadInt(int &i);
		void ReadUInt(unsigned int &ui);
		void ReadFloat(float &f);
		void ReadString(std::string &s);
		void ReadDWORD(DWORD &d);
	public:
		std::ifstream mStream;
	};

	template<typename T>
	inline void StreamRead(TextLoader &stream, T &type)
	{
		type.Serialize(stream);
	}

	inline void StreamRead(TextLoader &stream, int &i)
	{
		stream.ReadInt(i);
	}

	inline void StreamRead(TextLoader &stream, unsigned int &ui)
	{
		stream.ReadUInt(ui);
	}

	inline void StreamRead(TextLoader &stream, float &f)
	{
		stream.ReadFloat(f);
	}

	inline void StreamRead(TextLoader &stream, std::string &s)
	{
		stream.ReadString(s);
	}

	inline void StreamRead(TextLoader &stream, DWORD &d)
	{
		stream.ReadDWORD(d);
	}

	class TextSaver : public ISerializer
	{
	public:
		virtual bool Open(const std::string& filename);
		virtual bool IsGood();
		void WriteInt(int &i);
		void WriteFloat(float &f);
		void WriteString(std::string &s);
		void WriteDWORD(DWORD &d);
		void WriteChar(char &c);
		std::vector<std::string>& split(const std::string &s, char delim, std::vector<std::string> &elems);
		std::vector<std::string> split(const std::string &s, char delim);
	public:
		std::ofstream mStream;
	};

	template<typename T>
	inline void StreamWrite(TextSaver &stream, T &type)
	{
		type.Serialize(stream);
	}

	inline void StreamWrite(TextSaver &stream, int &i)
	{
		stream.WriteInt(i);
	}

	inline void StreamWrite(TextSaver &stream, float &f)
	{
		stream.WriteFloat(f);
	}

	inline void StreamWrite(TextSaver &stream, std::string &s)
	{
		stream.WriteString(s);
	}

	inline void StreamWrite(TextSaver &stream, DWORD &d)
	{
		stream.WriteDWORD(d);
	}

	inline void StreamWrite(TextSaver &stream, char &c)
	{
		stream.WriteChar(c);
	}
}