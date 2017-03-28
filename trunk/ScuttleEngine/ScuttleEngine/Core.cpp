/******************************************************************************/
/*!
\file   Core.cpp
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

#include "Precompiled.h"
#include "Core.h"
#include "ParticleManager.h"
#include "Graphics.h"
#include "GameStateMgr.h"
#include "DebugBars.h"
#include "Factory.h"
#include "GameLogic.h"

namespace Scuttlemeyer
{
	CoreEngine* CORE = NULL;

	CoreEngine::CoreEngine()
	{
		mBGameActive = true;
		mBEditorActive = false;
		mConfirmAction = false;
		mLastTime = 0;
		frames = 0;
		mInit = true;
		mLevelUnlocked = 1;
		mLevelSelected = 1;
		gGameStateNext = gGameStateCurr = GS_MENU;
		MouseInput.MouseScroll = 0;

		CORE = this;
	}

	void CoreEngine::Free()
	{
		//Call Unload for all engine subsystems.
		for(unsigned int i = 0; i < mSystems.size(); ++i)
		{
			mSystems[i]->Free();
		}
	}

	void CoreEngine::Unload()
	{
		//Call Unload for all engine subsystems.
		for(unsigned int i = 0; i < mSystems.size(); ++i)
		{
			mSystems[i]->Unload();
		}

#ifdef BLARG
		//Send a message to the debug console to let us know it was successful.
		Message* q = new Message(Mid::DebugCNSL, 1, new std::string("All systems unloaded."));
		EnqueueMessage(q);
#endif

		BroadcastMessages();
	}

	void CoreEngine::Load()
	{
		StartCounter();/*either here or initialize doesn't matter
						*just this one comes first */

		//Call load for all engine subsystems.
		for(unsigned int i = 0; i < mSystems.size(); ++i)
		{
			mSystems[i]->Load();
		}

#ifdef BLARG
		//Send a message to the debug console to let us know it was successful.
		Message* q = new Message(Mid::DebugCNSL, 1, new std::string("All systems started."));
		EnqueueMessage(q);
#endif
	}

	void CoreEngine::Initialize()
	{
		mMode = 0;
		mCurObj = 0;
		mConfirmAction = false;

#ifdef BLARG
		if(mInit)
		{
			TwBar* bar = DEBUGBARS->GetBarByName("FrameTimes");
			//Call initialize for all engine subsystems.
			for(unsigned int i = 0; i < mSystems.size(); ++i)
			{
				mSystems[i]->Initialize();
				TwAddVarRO(bar, mSystems[i]->GetName().c_str(), TW_TYPE_DOUBLE, &(mSystems[i]->GetFrameTime()), " precision=7 help='Current system frame time.' ");
			}

			mInit = false;
		}
		else
		{
#endif
			//Call initialize for all engine subsystems.
			for(unsigned int i = 0; i < mSystems.size(); ++i)
			{
				mSystems[i]->Initialize();
			}
#ifdef BLARG
		}

		IncCurObj();
#endif
		//LOGIC->LoadFileStuff();
	}

	void CoreEngine::AddSystem(ISystem* system)
	{
		//Add the new engine subsystem to the core.
		mSystems.push_back(system);
	}

	void CoreEngine::EnqueueConfirmMessage(Message *m)
	{
		//Push new messages to the back of the queue.
		//These will be serviced at the end of the current frame.
		mConfirmMessageQueue.push_back(m);
	}

	void CoreEngine::EnqueueMessage(Message *m)
	{
		//Push new messages to the back of the queue.
		//These will be serviced at the end of the current frame.
		mMessageQueue.push_back(m);
	}

	void CoreEngine::InstantBroadcastMessage(Message *m)
	{
		for(unsigned i = 0; i < mSystems.size(); ++i)
		{
			mSystems[i]->SendMessage(m);
		}
	}

	void CoreEngine::ConfirmMessages()
	{
		//We want to service every message generated this frame, so loop until they're all gone.
		while(!mConfirmMessageQueue.empty())
		{
			//Grab a message from the front of the queue and get started.
			Message *tempMsg = mConfirmMessageQueue.front();

			//If we have a quit message, end the gameloop.
			if(tempMsg->mMessageId == Mid::Quit)
			{
					//If we have a quit message, end the gameloop.
				if(mConfirmGoTo == Mid::Quit)
				{
					gGameStateNext = GS_QUIT;
					mBGameActive = false;
				}
				else if(mConfirmGoTo == Mid::Menu)
					gGameStateNext = GS_MENU;
				else if(mConfirmGoTo == Mid::Restart)
				{
					LOGIC->mBCheckpointActive = true;
					gGameStateNext = GS_RESTART;
				}

				mConfirmAction = false;
			}
			else if(tempMsg->mMessageId == Mid::Play)
				mConfirmAction = false;

			//sends only to factory
			Mid::MessageIdType m = mSystems[1]->SendMessage(tempMsg);

			//Once the message is sent, remove it from the queue and clean up memory.
			mConfirmMessageQueue.pop_front();
			delete tempMsg;
		}
	}

	void CoreEngine::BroadcastMessages()
	{
		//We want to service every message generated this frame, so loop until they're all gone.
		while(!mMessageQueue.empty())
		{
			//Grab a message from the front of the queue and get started.
			Message *tempMsg = mMessageQueue.front();

			//If we have a quit message, end the gameloop.
			if(tempMsg->mMessageId == Mid::Quit)
			{
				gGameStateNext = GS_QUIT;
				mBGameActive = false;
			}
			else if(tempMsg->mMessageId == Mid::Menu)
				gGameStateNext = GS_MENU;
			else if(tempMsg->mMessageId == Mid::Play)
				gGameStateNext = GS_PLAY;
			else if(tempMsg->mMessageId == Mid::Pause)
				gGameStateNext = GS_PAUSE;
			else if(tempMsg->mMessageId == Mid::Splash)
				gGameStateNext = GS_SPLASH;
			else if(tempMsg->mMessageId == Mid::Transition)
			{
				gGameStateNext = GS_TRANSITION;
			}
			else if(tempMsg->mMessageId == Mid::Restart)
			{
				LOGIC->mBCheckpointActive = true;
				gGameStateNext = GS_RESTART;
			}
			else if(tempMsg->mMessageId == Mid::Reload)
				gGameStateNext = GS_RELOAD;

			//Otherwise, broadcast the message to all engine subsystems.
			for(unsigned i = 0; i < mSystems.size(); ++i)
			{
				Mid::MessageIdType m = mSystems[i]->SendMessage(tempMsg);
			}

			//Once the message is sent, remove it from the queue and clean up memory.
			mMessageQueue.pop_front();
			delete tempMsg;
		}
	}

	double CoreEngine::GetCounter()
	{
		LARGE_INTEGER li;
		QueryPerformanceCounter(&li);
		return double(li.QuadPart-CounterStart)/PCFreq;
	}

	void CoreEngine::StartCounter()
	{
		LARGE_INTEGER li;
		QueryPerformanceFrequency(&li);
		//cout << "QueryPerformanceFrequency failed!\n";

		PCFreq = double(li.QuadPart);

		QueryPerformanceCounter(&li);
		CounterStart = li.QuadPart;
	}


	void CoreEngine::DestroySystems()
	{
		//Loop through all engine subsystems and delete them.
		for(unsigned int i = 0; i < mSystems.size(); ++i)
		{
			delete mSystems[mSystems.size() - i - 1];
		}
	}

	void TW_CALL SetObjName(const void *value, void * clientData)
	{
		const std::string *srcPtr = static_cast<const std::string *>(value);
		((GOC*)clientData)->mObjectName = *srcPtr;
	}

	void TW_CALL GetObjName(void *value, void * clientData)
	{
		std::string *destPtr = static_cast<std::string *>(value);
		TwCopyStdStringToLibrary(*destPtr, ((GOC*)clientData)->mObjectName);
	}

	void CoreEngine::IncCurObj()
	{
		if(mCurObj < FACTORY->GetLastObjectId())
		{
			mCurObj++;
		}
		else
		{
			mCurObj = 1;
		}

		TwBar* bar = DEBUGBARS->GetBarByName("ObjectProperties");
		TwRemoveAllVars(bar);

		GOC* tempO = FACTORY->GetObjectWithId(mCurObj);

		if(tempO)
		{
			TwAddVarRW(bar, "Game Object", TW_TYPE_UINT32, &mCurObj, " label='Game Object' help='The game object currently being inspected.' ");
			TwAddVarCB(bar, "Object Name", TW_TYPE_STDSTRING, SetObjName, GetObjName, tempO, NULL);

			for(unsigned int i = 0; i < tempO->GetNumberOfComponents(); ++i)
			{
				tempO->GetComponentByIndex(i)->ToBar();
			}
		}
	}

	unsigned CoreEngine::GetCurObj()
	{
		return mCurObj;
	}

	void CoreEngine::SetCurObj(unsigned obj)
	{
		while(mCurObj < obj)
		{
			IncCurObj();
		}
	}

	void CoreEngine::GameLoop()
	{
		mLastTime = GetCounter();

		//Loop until the game wants to shut down.
		while(gGameStateCurr == gGameStateNext)
		{
			//Calculate deltatime.
			double currentTime = GetCounter();
			float dt = (float) (currentTime - mLastTime);
			mLastTime = currentTime;

			//Update each engine subsystem.
			double tempTime = mLastTime;
			for(unsigned int i = 0; i < mSystems.size(); ++i)
			{
				if (gGameStateCurr != GS_PLAY && i == 3)
					continue;
				else
				{
					mSystems[i]->Update(dt);
					mSystems[i]->SetFrameTime(GetCounter() - tempTime);
					tempTime = GetCounter();
				}
			}
			static DWORD time;
			static bool flag = true;

			if(gGameStateCurr == GS_SPLASH)
			{
				if(flag ==  true)
				{
					LOGIC->Splash();
					time = timeGetTime();
					
					flag = false;
				}
				else
				{
					if(LOGIC->curScreen == 1)
					{
						if(timeGetTime() - time > 4000)
						{
							if(LOGIC->FULLSCREEN)
							{
								LOGIC->curScreen = 4;
							}
							else
							{
								LOGIC->curScreen = 2;
							}
							flag = true;
						}
					}
					else if(LOGIC->curScreen == 2)
					{
						if(timeGetTime() - time > 1000)
						{
							LOGIC->curScreen = 3;
							flag = true;
						}
					}
					else if(LOGIC->curScreen == 3)
					{
						if(timeGetTime() - time > 1000)
						{
							LOGIC->curScreen = 4;
							flag = true;
						}
					}
					else
					{
						if(timeGetTime() - time > 4000)
						{
							Message* q = new Message(Mid::Menu, 0);
							CORE->EnqueueMessage(q);
						}
					}
				}
			}
			//Dequeue and broadcast all messages generated this frame.
			BroadcastMessages();

			//This shit needs to be moved into a separate window component later.
			MSG msg;

			while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{ 
				if(msg.message == WM_QUIT)
				{
					Message* q = new Message(Mid::Quit, 0);
					EnqueueMessage(q);
				}
				else if(!TranslateAccelerator(msg.hwnd, NULL, &msg)) 
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}
		}
	}

	void CoreEngine::Paused()
	{
		mSystems[5]->Initialize();
		//mSystems.back()->Initialize();

		mLastTime = GetCounter();

		//Loop until the game wants to shut down.
		while(gGameStateCurr == gGameStateNext)
		{
			//Calculate deltatime.
			double currentTime = GetCounter();
			float dt = (float) (currentTime - mLastTime);
			mLastTime = currentTime;

			//Update each engine subsystem.
			double tempTime = mLastTime;
			for(unsigned int i = 0; i < mSystems.size(); ++i)
			{
				if (gGameStateCurr != GS_PLAY && i == 3)
				{
					continue;
				}
				else
				{
					mSystems[i]->Update(dt);
					mSystems[i]->SetFrameTime(GetCounter() - tempTime);
					tempTime = GetCounter();
				}
			}

			//Dequeue and broadcast all messages generated this frame.
			BroadcastMessages();

			//This shit needs to be moved into a separate window component later.
			MSG msg;

			while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{ 
				if(msg.message == WM_QUIT)
				{
					Message* q = new Message(Mid::Quit, 0);
					EnqueueMessage(q);
				}
				else if(!TranslateAccelerator(msg.hwnd, NULL, &msg)) 
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}
		}

		mSystems[5]->Free();
		//mSystems.back()->Free();
	}

	void CoreEngine::ConfirmQuit(Mid::MessageIdType message)
	{
		unsigned OldEnd = LOGIC->PauseObjList.size();
		mConfirmGoTo = message;
		mConfirmAction = true;
		mConfirmMessageQueue.clear();

		LOGIC->LoadConfirmQuitFile("..//Content//Levels//Confirm.txt");

		mLastTime = GetCounter();

		//Loop until the game wants to shut down.
		while(mConfirmAction == true)
		{
			//Calculate deltatime.
			double currentTime = GetCounter();
			float dt = (float) (currentTime - mLastTime);
			mLastTime = currentTime;

			//Update each engine subsystem.
			double tempTime = mLastTime;
			for(unsigned int i = 0; i < mSystems.size(); ++i)
			{
				if (gGameStateCurr != GS_PLAY && i == 3)
				{
					continue;
				}
				else
				{
					mSystems[i]->Update(dt);
					mSystems[i]->SetFrameTime(GetCounter() - tempTime);
					tempTime = GetCounter();
				}
			}

			//Dequeue and broadcast all messages generated this frame.
			ConfirmMessages();

			//This shit needs to be moved into a separate window component later.
			MSG msg;

			while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{ 
				if(msg.message == WM_QUIT)
				{
					Message* q = new Message(Mid::Quit, 0);
					EnqueueConfirmMessage(q);
				}
				else if(!TranslateAccelerator(msg.hwnd, NULL, &msg)) 
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}
		}

			// delete the options buttons
		while (LOGIC->PauseObjList.size() > OldEnd)
		{
			std::vector<GOC*>::iterator it = LOGIC->PauseObjList.begin() + OldEnd;
			FACTORY->Destroy(FACTORY->GetObjectWithId(LOGIC->PauseObjList[OldEnd]->GetId()));
			LOGIC->PauseObjList.erase(it);
		}
	}

	CoreEngine::~CoreEngine()
	{
	}
}