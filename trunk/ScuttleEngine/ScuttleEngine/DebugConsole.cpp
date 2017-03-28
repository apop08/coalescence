/******************************************************************************/
/*!
\file   DebugConsole.cpp
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

#include "Precompiled.h"
#include "DebugConsole.h"
#include <time.h>
#include "Core.h"
#include "VMath.h"

#include "Graphics.h"
#include "GameLogic.h"
#include "Particle.h"

namespace Scuttlemeyer
{
	DebugConsole* DBGCNSL = NULL;

	DWORD WINAPI DebugThread(LPVOID p)
	{
		new DebugConsole();
		int done = 0;
		struct DebugTPar* params = (struct DebugTPar *)p;

		while(!done)
		{
			DWORD result = WaitForSingleObject(params->_mutex, 0);
			switch (result)
			{
				/* Acquired the mutex */
			case WAIT_OBJECT_0:
				DBGCNSL->Update(0.0f);
				ReleaseMutex(params->_mutex);

				break;

				/* The mutex wasn't available yet */
			case WAIT_TIMEOUT:
				break;
			}
		}

		return TRUE;
	}

	DebugConsole::DebugConsole()
	{
#ifdef BLARG
		AllocConsole();
#endif

		DBGCNSL = this;
	
#ifdef BLARG
		//Redirect stdout to the console so we can get debug output.
		HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
		int hCrt = _open_osfhandle((long) handle_out, _O_TEXT);
		FILE* hf_out = _fdopen(hCrt, "w");
		setvbuf(hf_out, NULL, _IONBF, 1);
		*stdout = *hf_out;

		//Set the console title and position.
		SetConsoleTitle("Scuttle Debug");
		//ShowWindow(GetConsoleWindow() , SW_MAXIMIZE);
		
		//Don't need to take user input at the console, but if we did this would be necessary.
		//HANDLE handle_in = GetStdHandle(STD_INPUT_HANDLE);
		//hCrt = _open_osfhandle((long) handle_in, _O_TEXT);
		//FILE* hf_in = _fdopen(hCrt, "r");
		//setvbuf(hf_in, NULL, _IONBF, 128);
		//*stdin = *hf_in;

		mInput = new std::string();
		mFeedback = new std::string();
#endif
	}

	void DebugConsole::Unload()
	{
#ifdef BLARG
		//Send a message to let us know this finished successfully.
		Message* q = new Message(Mid::DebugCNSL, 1, new std::string("Debug Console Unload."));
		CORE->EnqueueMessage(q);

		mInput->clear();
#endif
	}

	void DebugConsole::Load()
	{
#ifdef BLARG
		mInput->push_back(':');
		mInput->push_back(' ');
#endif

		//Message* q = new Message(Mid::DebugCNSL, 1, new std::string("Debug Console Load."));
		//CORE->EnqueueMessage(q);
	}
	void DebugConsole::Free()
	{
#ifdef BLARG
		Message* q = new Message(Mid::DebugCNSL, 1, new std::string("Debug Console Free."));
		CORE->EnqueueMessage(q);

		mFeedback->clear();
#endif
	}

	void DebugConsole::Initialize()
	{
#ifdef BLARG
		//Send a message to let us know this finished successfully.
		Message* q = new Message(Mid::DebugCNSL, 1, new std::string("Debug Console Init."));
		CORE->EnqueueMessage(q);
#endif

		DBConsole = 0;

		print2 = 0;
		lines = 0;
		mLastInputLoc = 0;
		mSteps = 0;
	}

	void DebugConsole::Update(float dt)
	{
#ifdef BLARG
		//GOC* mPlayer = FACTORY->GetObjectWithId(1);
		//Transform* pTransform = ((Transform*)mPlayer->GetComponent(CT_Transform));

		//float tempx = CORE->MouseInput.MousePosition.x - GRAPHICS->mScreenWidth / 2;
		//float tempy = CORE->MouseInput.MousePosition.y - CameraOffset - GRAPHICS->mScreenHeight / 2;

		//if(CORE->mMode >= 2)
		//{
		//	//The call for this probably needs to move into another system.
		//	char tempString[400];
		//	
		//	sprintf_s(tempString, 400, "Current FPS: %f\nNumber of Game Objects: %d\nNumber of Messages: %i\nCurrent Position: %d, %d\nMouse Position: %d, %d\n#Jumps | Last Force: %d, %f", 
		//		GRAPHICS->GetFPS(), GameObjectComposition::GetNumberOfGameObjects(), Message::GetNumberOfMessages(), (int)pTransform->mPosition.x, (int)pTransform->mPosition.y, (int)tempx, (int)tempy, LOGIC->JumpCount, LOGIC->JumpForce);
		//	std::string writeme(tempString);
		//	GRAPHICS->QueString(&writeme, 0, 0, 0);
		//}
		if(CORE->mMode >= 3)
		{
			while(FACTORY->GetObjectWithId(CORE->GetCurObj()) == NULL || FACTORY->GetObjectWithId(CORE->GetCurObj())->has(Particle))
			{
				CORE->IncCurObj();
			}

			//GOC* tempO = FACTORY->GetObjectWithId(CORE->GetCurObj());

			//char tStr[400];
			//sprintf_s(tStr, 400, "Current Object: %i\n", CORE->GetCurObj());
			//std::string tempString(tStr);

			//for(unsigned int i = 0; i < tempO->GetNumberOfComponents(); ++i)
			//{
			//	tempString.append(tempO->GetComponentByIndex(i)->ToString());
			//	tempString.append("\n");
			//}

			//char stateChar [5];
			//_itoa_s((int)tempO->state, stateChar, 5, 10);
			//tempString.append("State: ");
			//tempString.append(stateChar);
			//tempString.append("\n");

			//GRAPHICS->QueString(&tempString, 0, 100, 0);
		}

		if (DBConsole == 1)
		{
			GRAPHICS->QueString(mFeedback, 0, GRAPHICS->mScreenHeight - 20 - (12 * lines), 0);
			GRAPHICS->QueString(mInput, 0, GRAPHICS->mScreenHeight - 20, 0);
			if(mSteps > 0)
				--mSteps;
		}
#endif
	}

	void DebugConsole::Scroll(bool direction)
	{	
		std::list<std::string*>::iterator it = mLastInput.begin();
		if(direction == 1)// up
		{
			if(mLastInputLoc != 0)
				--mLastInputLoc;
			if(mLastInputLoc <= 0)
				mLastInputLoc =  mLastInput.size();
		}
		else
		{
			if(++mLastInputLoc >=  mLastInput.size() + 1)//down
				mLastInputLoc = 0;
		}
		if(mLastInputLoc != 0)
		{
			for(int i = 0; i != mLastInputLoc - 1; i++)
			{
				++(it);
			}
		}
		if(mLastInput.size() != 0 && *it != NULL)
		{
			mInput->clear();
			mInput->append(*(*(it)));
		}

	}

	void DebugConsole::DBHandleInput()
	{
		++(lines);
		mFeedback->append(*(mInput));
		mFeedback->append("\n");
		//HandleDBString();
		mLastInput.push_back(new std::string(*mInput));
		ParseString();
		mLastInputLoc = 0;
		mInput->clear();
		mInput->push_back(':');
		mInput->push_back(' ');
	}

	void DebugConsole::ParseString()
	{
		std::string StringBuffer;
		size_t loc = 1, loc2 = 1;// = mInput->find_first_of(";");
		do{
			loc = loc2 + 1;
			loc2 = mInput->find(";", loc);
			StringBuffer.clear();
			StringBuffer = mInput->substr(loc, (loc2 - loc));
			if(StringBuffer[0] == ' ')
			{
				StringBuffer = StringBuffer.substr(1);
			}
			ParseCommand(StringBuffer);
		}while(loc2 != std::string::npos);
	}

	void DebugConsole::ParseCommand(std::string cmd)
	{
		std::vector<std::string*> StringBuffer;
		size_t loc = 0, loc2 = -1;// = mInput->find_first_of(";");
		int i = 0;
		do{
			loc = loc2 + 1;
			loc2 = cmd.find(" ", loc);
			StringBuffer.push_back(new std::string(cmd.substr(loc, (loc2 - loc))));
			//StringBuffer = 
			++i;
		}while(loc2 != std::string::npos);
		HandleDBString(StringBuffer, i);
	}

	void DebugConsole::Write(UINT key)
	{
		mInput->push_back(key);
	}

	void DebugConsole::HandleDBString(std::vector<std::string*> exec, int args)
	{
		if(exec[0]->compare("SPAWN") == 0)
		{
			if(args >= 8)
			{
				std::string temp = *(exec[1]);
				double x = atof((exec[2])->c_str());
				double y = atof((exec[3])->c_str());
				double rotation = atof((exec[4])->c_str());
				double scalex = atof((exec[5])->c_str());
				double scaley = atof((exec[6])->c_str());
				int state = atoi((exec[7])->c_str());

				LOGIC->SpawnObj(temp, x, y, rotation, scalex, scaley, EState(state));

				/*Vec2 position;
				position.x = (float)atof((exec[2])->c_str());
				position.y = (float)atof((exec[3])->c_str());
				Vec2 scale;
				scale.x = (float)atof((exec[5])->c_str());
				scale.y = (float)atof((exec[6])->c_str());

				std::string str("..//Content//Test//");
				str.append(*(exec[1]));
				str.append(".txt");


				LOGIC->CreateObjectAt(
					position, 
					scale, 
					(float)atof((exec[4])->c_str()) * cDelicious / 180, 
					State(atoi((exec[7])->c_str())),
					str
				);*/
			}
			else
			{
				mFeedback->append("Error: Invalid Parameter Number for Call ");
				for(int i = 0; i < args; ++i)
				{
					mFeedback->append(*(exec[i]));
					mFeedback->append(" ");
				}
				mFeedback->append("Expected: SPAWN SPRITEFILE X Y ROTATION SCALEX SCALEY");
				mFeedback->append("\n");
				++lines;
			}
		}
		else if(exec[0]->compare("STEP") == 0)
		{
			if(args >= 2)
			{
				mSteps = atoi((exec[1])->c_str());
			}
			else
			{
				mFeedback->append("Error: Invalid Parameter Number for Call ");
				for(int i = 0; i < args; ++i)
				{
					mFeedback->append(*(exec[i]));
					mFeedback->append(" ");
				}
				mFeedback->append("Expected: STEP #FRAMES");
				mFeedback->append("\n");
				++lines;
			}
		}
		else if(exec[0]->compare("MODE") == 0)
		{
			if(args >= 2)
			{
				CORE->mMode = atoi((exec[1])->c_str());
			}
			else
			{
				mFeedback->append("Error: Invalid Parameter Number for Call ");
				for(int i = 0; i < args; ++i)
				{
					mFeedback->append(*(exec[i]));
					mFeedback->append(" ");
				}
				mFeedback->append("Expected: MODE #LEVEL");
				mFeedback->append("\n");
				++lines;
			}
		}
		else
		{
			mFeedback->append("Error: Function Not Found ");
			mFeedback->append(*(exec[0]));
			mFeedback->append("\n");
			++lines;
		}
		for(int i = 0; i < args; ++i)
		{
			delete exec[i];
		}
	}


	Mid::MessageIdType DebugConsole::SendMessage(Message *message)
	{
#ifdef BLARG
		//If this is a debug message, we want to service it.
		if(message->mMessageId == Mid::DebugCNSL)
		{
			char date[22];
			struct tm newtime;
			time_t t = time(0);

			//Format a date/time stamp and output that and the message's data.
			localtime_s(&newtime, &t);
			strftime(date, sizeof(date), "%m/%d/%Y - %I:%M:%S", &newtime);
			std::cout << date << std::endl;

			for(int i = 0; i < message->mParamCount; i++)
			{
				std::cout << ((std::string*)(message->mMessageData[i]))->c_str() << std::endl;
			}
			std::cout << std::endl;
		}

		// Debug console input
		if (message->mMessageId == Mid::KeyDown)
		{
			int key = atoi(((std::string*)(message->mMessageData[0]))->c_str());

			if (DBConsole == 1)
			{
				// (( Number keys 0-9      ) || ( Alphabet A-Z         ) || Space           || Semicolon      )
				if ((key >= 48 && key <= 57) || (key >= 65 && key <= 90) || key == VK_SPACE || key == VK_OEM_1)
					Write(MapVirtualKey(key, MAPVK_VK_TO_CHAR));

				if(key == VK_UP)	// Up and down arrow keys (scrolling)
					Scroll(1);
				if(key == VK_DOWN)
					Scroll(0);

				if(key == VK_RETURN)
					DBHandleInput();

				if(key == VK_BACK && mInput->length() > 2)	// Backspace
					mInput->erase(--(mInput->end()));

				if(key == VK_OEM_3)	// ~ key (toggle the debug console off)
				{
					mInput->clear();
					mInput->push_back(':');
					mInput->push_back(' ');
					DBConsole = 0;
				}
			}
			else
			{
				if(key == VK_OEM_3)  // ~ key (toggle the debug console on)
					DBGCNSL->DBConsole = 1;
			}
		}
#endif

		return Mid::None;
	}

	DebugConsole::~DebugConsole()
	{
#ifdef BLARG
		delete mInput;
		delete mFeedback;
		for(std::list<std::string*>::iterator it = mLastInput.begin(); it != mLastInput.end();)
		{
			delete *it;
			it++;
		}

		FreeConsole();
#endif
	}
}