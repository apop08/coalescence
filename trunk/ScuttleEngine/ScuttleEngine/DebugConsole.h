/******************************************************************************/
/*!
\file   DebugConsole.h
\author Matt Sutherlin (Error Console)
\author Alex Poplawski (Debug Console)
\par    email: matt.sutherlin\@digipen.edu
\par    DigiPen login: matt.sutherlin
\par    Course: GAM200
\date   8/1/2011
\brief  
Error and interactive debug console.
\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/******************************************************************************/

#pragma once

#include "System.h"

#include <iostream>
#include <io.h>
#include <fcntl.h>

namespace Scuttlemeyer
{
	class DebugConsole : public ISystem
	{
	public:
		DebugConsole();
		void Unload();
		void Load();
		void Free();
		void Initialize();
		void Update(float dt);
		virtual std::string GetName(){ return "Debug Console"; }
		Mid::MessageIdType SendMessage(Message *message);
		void DBHandleInput();
		void HandleDBString(std::vector<std::string*> exec, int args);
		void Write(UINT key);
		void Scroll(bool direction);
		void ParseString();
		void ParseCommand(std::string cmd);

		int mSteps;
		UINT mLastInputLoc;
		int DBConsole;
		std::string* mInput;
		std::string* mFeedback;
		std::list<std::string*> mLastInput;
		//std::list<std::string*> mStringBuffer;
		int print2;
		int lines;
		~DebugConsole();
	};

	struct DebugTPar
	{
		/* mutex handle */
		HANDLE _mutex;
		int _debugmes;
	};


	DWORD WINAPI DebugThread(LPVOID p);

	extern DebugConsole* DBGCNSL;
}