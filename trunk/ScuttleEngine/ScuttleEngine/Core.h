/******************************************************************************/
/*!
\file   Core.h
\author Matt Sutherlin
\author Kevin Verholtz (Gamestate management)
\par    email: matt.sutherlin\@digipen.edu
\par    DigiPen login: matt.sutherlin
\par    Course: GAM200
\date   7/25/2011
\brief  
	The core engine.  Handles all other systems.
\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/******************************************************************************/

#pragma once

#include "System.h"
#include "VMath.h"
//#define BLARG

namespace Scuttlemeyer
{
	typedef struct _MouseInput_
	{
		bool LeftButtonDown;
		bool RightButtonDown;
		short MouseScroll;
		Vec2 MousePosition;
	}MouseInput_;
	
	class CoreEngine
	{
	public:
		CoreEngine();
		void Unload();
		void Load();
		void Free();
		void Initialize();
		void AddSystem(ISystem* system);
		void DestroySystems();
		void GameLoop();
		void Paused();

		void InstantBroadcastMessage(Message* m);
		void EnqueueMessage(Message* m);
		void EnqueueConfirmMessage(Message* m);

		void IncCurObj();
		unsigned GetCurObj();
		void SetCurObj(unsigned obj);

		void StartCounter();
		double GetCounter();

		void ConfirmQuit(Mid::MessageIdType message);

		~CoreEngine();
	public:
		double PCFreq;
		__int64 CounterStart;
		int mMode;
		MouseInput_ MouseInput;
		std::vector<ISystem*> mSystems;
		bool mBGameActive;
		bool mBEditorActive;
		bool mConfirmAction;
		int mLevelUnlocked;
		int mLevelSelected;
	private:
		void BroadcastMessages();
		void ConfirmMessages();
	private:
		Mid::MessageIdType mConfirmGoTo;
		double mLastTime;
		std::deque<Message*> mMessageQueue;
		std::deque<Message*> mConfirmMessageQueue;
		int frames;
		unsigned int mCurObj;
		bool mInit;
	};

	extern CoreEngine* CORE;
}