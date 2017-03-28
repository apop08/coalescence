/**************************************************************************************************/
/*!
\file	GameLogic.cpp
\par	Project: Coalescence
\date	9/15/2011
\authors
Primary  : Lincoln Tosi
Secondary: Kevin Verholtz - Reload routines
\brief
The major implementation of the Game Logic System.

\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/**************************************************************************************************/
#include "Precompiled.h"

#include "Physics.h"
#include "GameLogic.h"
#include "Core.h"
#include "Camera.h"
#include "ComponentCreator.h"
#include "TextSerialization.h"
#include "DebugConsole.h"
#include "ParticleEmitter.h"
#include "GameStateMgr.h"
#include <ctime>
#include "Shlobj.h"
#include "bNewGame.h"
#include "bHowToPlay.h"
#include "bOptions.h"
#include "bCredits.h"
#include "bQuit.h"
#include "bYes.h"
#include "bNo.h"
#include "bResume.h"
#include "bRestart.h"
#include "bMainMenu.h"
#include "bVolume.h"
#include "bResolution.h"
#include "bFullscreen.h"
#include "bArrowLeft.h"
#include "bArrowRight.h"
#include "bLevel.h"
#include "bFullscreen.h"
#include "bBack.h"
#include "CreditsText.h"

#include "TutBox.h"
#include "EditCam.h"
#include "LevelPanCam.h"
#include "Checkpoint.h"
#include "SoundNode.h"

#include "Audio.h"
namespace Scuttlemeyer
{
	GameLogic* LOGIC = NULL;

	/*****************************************************************************/
	/*!
	\brief
	Register's game components and assigns the default level.
	*/
	/*****************************************************************************/
	GameLogic::GameLogic(HCURSOR* cursors)
	{
		RegisterComponent(Transform);
		RegisterComponent(ILogical);
		RegisterComponent(Player);
		RegisterComponent(Turret);
		RegisterComponent(Bullet);
		RegisterComponent(Rocket);
		RegisterComponent(Block);
		RegisterComponent(Switch);
		RegisterComponent(BrokenPipe);
		RegisterComponent(KillZone);
		RegisterComponent(BlockWin);
		RegisterComponent(EntranceDoor);
		RegisterComponent(Static);
		RegisterComponent(bNewGame);
		RegisterComponent(bHowToPlay);
		RegisterComponent(bOptions);
		RegisterComponent(bCredits);
		RegisterComponent(bQuit);
		RegisterComponent(bYes);
		RegisterComponent(bNo);
		RegisterComponent(bResume);
		RegisterComponent(bRestart);
		RegisterComponent(bMainMenu);
		RegisterComponent(bVolume);
		RegisterComponent(bResolution);
		RegisterComponent(bFullscreen);
		RegisterComponent(bArrowLeft);
		RegisterComponent(bArrowRight);
		RegisterComponent(bLevel);
		RegisterComponent(bFullscreen);
		RegisterComponent(bBack);
		RegisterComponent(TutBox);
		RegisterComponent(EditCam);
		RegisterComponent(LevelPanCam);
		RegisterComponent(Fan);
		RegisterComponent(BiDirectionalFan);
		RegisterComponent(Checkpoint);
		RegisterComponent(CreditsText);


		isHolding = false;
		inBlock = false;
		curMenu = 0;
		curScreen = 1;
		curLevel = 1;
		mSplash = 0;
		nextState = 0;
		transTime = timeGetTime();
		curTransition = 1;
		mBCheckpointActive = false;
		mBGameEnd = false;
		mBDied = false;
		mScrollValue = 0;
		mKevinISAJEWPACABRAMEXICAN = false;
		mCurCursor = 0;

		memcpy(mCursors, cursors, 5 * sizeof(HCURSOR));

		LOGIC = this;
	}

	/*****************************************************************************/
	/*!
	\brief
	Deconstructor.
	*/
	/*****************************************************************************/
	GameLogic::~GameLogic()
	{
		//mLevelName.clear();
	}

	/*****************************************************************************/
	/*!
	\brief
	Uload the Game Logic System.
	*/
	/*****************************************************************************/
	void GameLogic::Unload()
	{
#ifdef BLARG
		Message* q = new Message(Mid::DebugCNSL, 1, new std::string("GameLogic Unload."));
		CORE->EnqueueMessage(q);
#endif
		mPlayer = 0;
		mHUD = 0;
	}

	/*****************************************************************************/
	/*!
	\brief
	Load the Game Logic System.
	*/
	/*****************************************************************************/
	void GameLogic::Load()
	{
#ifdef BLARG
		Message* q = new Message(Mid::DebugCNSL, 1, new std::string("GameLogic Load."));
		CORE->EnqueueMessage(q);
#endif

		//LoadFile();

		
		//mLevelName[level];
	}

	/*****************************************************************************/
	/*!
	\brief
	Free the Game Logic System.
	*/
	/*****************************************************************************/
	void GameLogic::Free()
	{
#ifdef BLARG
		Message* q = new Message(Mid::DebugCNSL, 1, new std::string("GameLogic Free."));
		CORE->EnqueueMessage(q);
#endif
		if(gGameStateCurr == GS_PAUSE)
		{
			UnLoadPauseFile();
		}
		else
		{
			mPlayer = NULL;
			mHUD = NULL;
		}
	}

	/*****************************************************************************/
	/*!
	\brief
	Loads the level, initializes variables, and sets the player's
	initial position.
	*/
	/*****************************************************************************/
	void GameLogic::Initialize()
	{
		mBDied = false;
		mKevinISAJEWPACABRAMEXICAN = false;
#ifdef BLARG
		Message* q = new Message(Mid::DebugCNSL, 1, new std::string("GameLogic Init."));
		CORE->EnqueueMessage(q);
#endif
		SwapState();
	}

	/*****************************************************************************/
	/*!
	\brief
	Update each of the logical components.

	\param dt
	The frame time.
	*/
	/*****************************************************************************/
	void GameLogic::Update(float dt)
	{
		mCurCursor = gGameStateCurr == GS_PLAY ? 0 : 2;

		if (DBGCNSL->DBConsole != 1 && !CORE->mBEditorActive)
		{
			if (gGameStateCurr == GS_PLAY)	// CHANGE
			{
				for (ObjectLinkedList<ILogical>::iterator it = mLogicList.begin(); it != mLogicList.end(); ++it)
					it->Update(dt);

				if(mBDied && mPlayer->has(CompSprite)->mComponents[0]->mColor.y <= 0.f)
				{
					//Instead of Mid::Restart
					//You lose screen or something like that
					Message* q = new Message(Mid::Restart, 0);
					CORE->EnqueueMessage(q);
				}
			}
			else if (gGameStateCurr == GS_TRANSITION)
			{
				if(LOGIC->curTransition == 3)
				{
					if(timeGetTime() - transTime > 3000)
					{
						gGameStateNext = nextState;
					}
				}
				else if (timeGetTime() - transTime > 1500)
				{
					gGameStateNext = nextState;
				}
			}
			else
			{
				Message* q = new Message(Mid::MOUSEMOVE, 0);
				if(CORE->mConfirmAction == false)
					CORE->EnqueueMessage(q);
			}
		}

		SetCursor(mCursors[mCurCursor]);
	}

	/*****************************************************************************/
	/*!
	\brief
	Message handling for Game Logic. Currently only handles keyboard input.

	\param message
	The message to be interpreted.
	*/
	/*****************************************************************************/
	Mid::MessageIdType GameLogic::SendMessage(Message *message)
	{
		// Uncomment when not debugging (DO NOT DELETE!)
		// CHANGE
		/*if (message->mMessageId == Mid::INACTIVE && gGameStateCurr == GS_PLAY)
		{
			Message* q = new Message(Mid::Pause, 0);
			CORE->EnqueueMessage(q);
		}*/

		if (gGameStateCurr == GS_SPLASH)
		{
			// skip the splash screens if the user inputs
			if (message->mMessageId == Mid::LBUTTONDOWN
				|| message->mMessageId == Mid::RBUTTONDOWN
				|| message->mMessageId == Mid::KeyDown)
			{
				Message* q = new Message(Mid::Menu, 0);
				CORE->EnqueueMessage(q);
			}
		}
		else if (message->mMessageId == Mid::KeyDown)
		{
			int key = atoi(((std::string*)(message->mMessageData[0]))->c_str());

			// Don't process this while the debug console is active
			if (DBGCNSL->DBConsole == 0 && !CORE->mBEditorActive)
			{
				// 1 key (Hot firing state)
				if (key == 49 && mPlayer->has(Player)->mTutFlag & ShootG)
					((GameComponent*)mPlayer->GetComponent(CT_Player))->ChangeState(Hot);

				// 2 key (Warm firing state)
				if (key == 50 && mPlayer->has(Player)->mTutFlag & ShootL)
					((GameComponent*)mPlayer->GetComponent(CT_Player))->ChangeState(Warm);

				// 3 key (Cold firing state)
				if (key == 51 && mPlayer->has(Player)->mTutFlag & ShootS)
					((GameComponent*)mPlayer->GetComponent(CT_Player))->ChangeState(Cold);

				// esc key (play/pause)
				if (key == 27)
				{
					if (gGameStateCurr == GS_MENU && gGameStateNext == GS_MENU)
					{
					}
					else
					{
						Message* q;
						if (gGameStateCurr == GS_PLAY)
							q = new Message(Mid::Pause, 0);
						else
							q = new Message(Mid::Play, 0);

						CORE->EnqueueMessage(q);
					}
				}

#ifdef BLARG
				if (gGameStateCurr == GS_PLAY)
				{
					// l key (load next level - cheat)
					if (key == 76)
					{
						if(LOGIC->curLevel == mMaxLevel)
						{
							/*LOGIC->curMenu = 3;
							Message* q = new Message(Mid::Menu, 0);
							CORE->EnqueueMessage(q);*/

							LOGIC->curMenu = 3;
							LOGIC->nextState = GS_MENU;
							LOGIC->curTransition = 3;
							Message* q = new Message(Mid::Transition, 0);
							CORE->EnqueueMessage(q);
						}
						else
						{
							LOGIC->curLevel++;
							LOGIC->nextState = GS_PLAY;
							LOGIC->curTransition = 1;
							Message* q = new Message(Mid::Transition, 0);
							CORE->EnqueueMessage(q);
						}
					}

					//	// t key (reset player's rotation - cheat)
					//if (key == 84)
					//{
					//	if (gGameStateNext == GS_PLAY)
					//	{
					//		PhysBody* pPhysics = ((PhysBody*)mPlayer->GetComponent(CT_PhysBody));
					//		Transform* pTransform = ((Transform*)mPlayer->GetComponent(CT_Transform));

					//		pPhysics->mRotVel = 0.0f;
					//		pTransform->mRotation = 0.0f;
					//		pTransform->mPosition.x = pTransform->mPosition.x + 10.0f;
					//		pTransform->mPosition.y = pTransform->mPosition.y + 10.0f;
					//	}
					//}
				}
#endif
			}

#ifdef BLARG
			if (gGameStateCurr == GS_PLAY)
			{
				// tab key (cycle through showing information for each object)
				if (key == VK_TAB)
					CORE->IncCurObj();
			}
#endif
		}
		else if(message->mMessageId == Mid::MOUSESCROLL)
		{
			if (DBGCNSL->DBConsole == 0 && !CORE->mBEditorActive && gGameStateCurr == GS_PLAY && mPlayer && mHUD)
			{
				mScrollValue += CORE->MouseInput.MouseScroll;

				if(mScrollValue >= WHEEL_DELTA)
				{
					mScrollValue = 0;

					if(mPlayer->has(Player)->mFireState == Hot && mPlayer->has(Player)->mTutFlag & ShootS)
					{
						mPlayer->has(Player)->ChangeState(Cold);
					}
					else if(mPlayer->has(Player)->mFireState == Warm && mPlayer->has(Player)->mTutFlag & ShootG)
					{
						mPlayer->has(Player)->ChangeState(Hot);
					}
					else if(mPlayer->has(Player)->mFireState == Cold && mPlayer->has(Player)->mTutFlag & ShootL)
					{
						mPlayer->has(Player)->ChangeState(Warm);
					}
				}
				else if(mScrollValue <= -WHEEL_DELTA)
				{
					mScrollValue = 0;

					if(mPlayer->has(Player)->mFireState == Hot && mPlayer->has(Player)->mTutFlag & ShootL)
					{
						mPlayer->has(Player)->ChangeState(Warm);
					}
					else if(mPlayer->has(Player)->mFireState == Warm && mPlayer->has(Player)->mTutFlag & ShootS)
					{
						mPlayer->has(Player)->ChangeState(Cold);
					}
					else if(mPlayer->has(Player)->mFireState == Cold && mPlayer->has(Player)->mTutFlag & ShootG)
					{
						mPlayer->has(Player)->ChangeState(Hot);
					}
				}
			}
		}
		else if (message->mMessageId == Mid::Collision)
		{
			GOC* B1 = (GOC*)(message->mMessageData[0]);
			GOC* B2 = (GOC*)(message->mMessageData[1]);
			const float* dt = (const float*)(message->mMessageData[2]);

			if (!(LOGIC->ProcessCollision(B1, B2, dt)))
			{
				//PHYSICS->ApplyCol(B1->has(PhysBody), B2->has(PhysBody), *dt);

				// here
				Vec2* temp = PHYSICS->SAT(B1->has(PhysBody), B2->has(PhysBody), *dt);
				if(temp != NULL )
					PHYSICS->ApplyCol(B1->has(PhysBody), B2->has(PhysBody), *dt, temp);
				if(temp != NULL)
					delete temp;
			}
		}

		return Mid::None;
	}


	//-----------------------------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------------------------


	void GameLogic::SwapState()
	{
		switch (gGameStateCurr)
		{
		case GS_SPLASH:
			Splash();
			break;
		case GS_MENU:
			Menu();
			break;
		case GS_PLAY:
			//if(!mBCheckpointActive)
			//{
			Play();
			//}
			//else
			//{
			//	Play("Checkpoint.txt");
			//}
			break;
		case GS_PAUSE:
			Pause();
			break;
		case GS_TRANSITION:
			Transition();
			break;
		}
	}

	void GameLogic::Splash()
	{
		std::string screen = AppendNumberToFile("..//Content//Levels//SplashScreen", curScreen);
		LoadSplashFile(screen);
	}

	void GameLogic::Menu()
	{
		std::string menu = AppendNumberToFile("..//Content//Levels//Menu", curMenu);
		LoadMenuFile(menu);
		curLevel = 1;
	}

	void GameLogic::LoadFileStuff()
	{
		if (RESOLUTION == 2)
		{
			//set resolution to 1600x1200
			//bImage = "1600x1200";
			if(GRAPHICS->GetD3DPP().Windowed)
			{
				GRAPHICS->ChangeResolution(GRAPHICS->mMaxResWindowed.Width, GRAPHICS->mMaxResWindowed.Height);
			}
			else
			{
				GRAPHICS->ChangeResolution(GRAPHICS->mMaxRes.Width, GRAPHICS->mMaxRes.Height);
			}
			//LOGIC->RESOLUTION = 2;
		}
		else if (RESOLUTION == 0)
		{
			//set resolution to 800x600
			//bImage = "800x600";
			GRAPHICS->ChangeResolution(800, 600);
			//LOGIC->RESOLUTION = 0;
		}
		else if (RESOLUTION == 1)
		{
			//set resolution to 1024x768
			//bImage = "1024x768";
			GRAPHICS->ChangeResolution(1024, 768);
			//LOGIC->RESOLUTION = 1;
		}

		if (VOLUME == 3)
		{
			//set volume to high
			//bImage = "High";
			AUDIO->SetVolume(1.0f);
			AUDIO->DecrementVolume();
			AUDIO->IncrementVolume();
			//LOGIC->VOLUME = 3;
		}
		else if (VOLUME == 0)
		{
			//set volume to off
			//bImage = "Off";
			AUDIO->SetVolume(0.0f);
			AUDIO->IncrementVolume();
			AUDIO->DecrementVolume();
			//LOGIC->VOLUME = 0;
		}
		else if (VOLUME == 1)
		{
			//set volume to low
			//bImage = "Low";
			AUDIO->SetVolume(0.3f);
			AUDIO->DecrementVolume();
			AUDIO->IncrementVolume();
			//LOGIC->VOLUME = 1;
		}
		else if (VOLUME == 2)
		{
			//set volume to medium
			//bImage = "Medium";
			AUDIO->SetVolume(0.6f);
			AUDIO->DecrementVolume();
			AUDIO->IncrementVolume();
			//LOGIC->VOLUME = 2;
		}
		if(FULLSCREEN)
		{
			GRAPHICS->ToggleWindowed();
		}
	}

	void GameLogic::Play()
	{
		std::string lvl = AppendNumberToFile("..//Content//Levels//Level", curLevel);
		LoadLevelFile(lvl);

		if (gGameStatePrev != GS_PAUSE)
			isHolding = false;

		Transform* pTHud = ((Transform*)mHUD->GetComponent(CT_Transform));
		pTHud->mPosition.x = 0.0f;
		pTHud->mPosition.y = GRAPHICS->mCameraOffset;
		pTHud->mRotation = 0.0f;

		mOriginalHudSizeX = mHUD->has(CompSprite)->mComponents[2]->mSize.x;
		mOriginalHudXOffset = mHUD->has(CompSprite)->mComponents[2]->mLocalTransform->mPosition.x;
		mOriginalXScale = mHUD->has(CompSprite)->mComponents[2]->mLocalTransform->mScale.x;

		CompSprite* sHud = mHUD->has(CompSprite);//->mComponents[0]->;

		sHud->mComponents[1]->SetFadeMode(-1, 1.0f);
		sHud->mComponents[2]->SetFadeMode(-1, 1.0f);
		//sHud->mComponents[3]->SetFadeMode(-1, 1.0f);
		//sHud->mComponents[4]->SetFadeMode(-1, 1.0f);
		//sHud->mComponents[5]->SetFadeMode(-1, 1.0f);
		sHud->mComponents[6]->SetFadeMode(-1, 1.0f);
		//_CrtMemState s1;
		//_CrtMemCheckpoint( &s1 );
		//_CrtMemDumpStatistics(&s1);
	}

	void GameLogic::Play(const std::string &filename)
	{
		std::string lvl = "..//Content//Levels//";
		lvl.append(filename);

		LoadLevelFile(lvl);

		if (gGameStatePrev != GS_PAUSE)
			isHolding = false;

		Transform* pTHud = ((Transform*)mHUD->GetComponent(CT_Transform));
		pTHud->mPosition.x = 0.0f;
		pTHud->mPosition.y = GRAPHICS->mCameraOffset;
		pTHud->mRotation = 0.0f;

		mOriginalHudSizeX = mHUD->has(CompSprite)->mComponents[2]->mSize.x;
		mOriginalHudXOffset = mHUD->has(CompSprite)->mComponents[2]->mLocalTransform->mPosition.x;
		mOriginalXScale = mHUD->has(CompSprite)->mComponents[2]->mLocalTransform->mScale.x;

		CompSprite* sHud = mHUD->has(CompSprite);//->mComponents[0]->;

		sHud->mComponents[1]->SetFadeMode(-1, 1.0f);
		sHud->mComponents[2]->SetFadeMode(-1, 1.0f);
		//sHud->mComponents[3]->SetFadeMode(-1, 1.0f);
		//sHud->mComponents[4]->SetFadeMode(-1, 1.0f);
		//sHud->mComponents[5]->SetFadeMode(-1, 1.0f);
		sHud->mComponents[6]->SetFadeMode(-1, 1.0f);
		//_CrtMemState s1;
		//_CrtMemCheckpoint( &s1 );
		//_CrtMemDumpStatistics(&s1);
	}

	void GameLogic::Pause()
	{
		LoadPauseFile("..//Content//Levels//Pause.txt");
	}

	void GameLogic::Transition()
	{
		std::string trans = AppendNumberToFile("..//Content//Levels//TransitionScreen", curTransition);
		LoadSplashFile(trans);
		transTime = timeGetTime();
	}


	//-----------------------------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------------------------

	void GameLogic::Save()
	{
		char save[100];
		SHGetFolderPathA(NULL, 
			CSIDL_PERSONAL|CSIDL_FLAG_CREATE, 
			NULL, 
			0, 
			save);
		strcat_s(save, 100, "//Games//Coalescence//Saves//config.txt");
		TextSaver stream;

		stream.Open(save);
		stream.WriteInt(RESOLUTION);
		stream.WriteInt(VOLUME);
		int temp;
		
		if(FULLSCREEN)
		{
			temp = 1;
			stream.WriteInt(temp);
		}
		else
		{
			temp = 0;
			stream.WriteInt(temp);
		}
		stream.WriteInt(CORE->mLevelUnlocked);


	}

	void GameLogic::LoadFile()
	{
		char save[100];
		SHGetFolderPathA(NULL, 
			CSIDL_PERSONAL|CSIDL_FLAG_CREATE, 
			NULL, 
			0, 
			save);
		strcat_s(save, 100, "//Games//Coalescence//Saves//config.txt");
		TextLoader stream;

		stream.Open(save);
		if(stream.IsGood())
		{
			stream.ReadInt(RESOLUTION);
			stream.ReadInt(VOLUME);
			int temp;
			stream.ReadInt(temp);
			if(temp)
				FULLSCREEN = true;
			else
				FULLSCREEN = false;
			stream.ReadInt(CORE->mLevelUnlocked);
		}
		else
		{
			RESOLUTION = 1;
			VOLUME = 2;
			FULLSCREEN = true;
			CORE->mLevelUnlocked = 1;
		}

	}


	/*****************************************************************************/
	/*!
	\brief
	When two objects in the game world collide, this function is called
	from physics to determine the outcome of the collision and whether or
	not it should be processed.

	\param obj1
	The first colliding object.

	\param obj2
	The second colliding object.

	\return
	1 if collision shouldn't be processed, 0 if it should.
	*/
	/*****************************************************************************/
	int GameLogic::ProcessCollision(GOC* obj1, GOC* obj2, const float* dt)
	{
		GameComponent* logic1;
		GameComponent* logic2;

		if (obj1->has(Bullet) || obj1->has(Rocket))
		{
			GOC* temp = obj1;
			obj1 = obj2;
			obj2 = temp;
		}

		// Bullet collision
		if (logic2 = (GameComponent*)obj2->GetComponent(CT_Bullet))
		{
			if (!((Bullet*)logic2)->CheckCollided())
			{
				// Bullet to Block collision
				if (logic1 = (GameComponent*)obj1->GetComponent(CT_Block))
				{
					EState temp = obj1->state;

					// If 1, don't delete the bullet
					if (logic1->ChangeState(obj2->state) || logic2->ChangeState(temp))
						return 1;
				}

				// Deletion detection
				if (!(obj1->has(Player)) && !(obj1->has(Rocket)) && !(obj1->has(BlockWin)) && obj1->state != Ghost)
				{
					((Bullet*)logic2)->SetCollided(true);
					obj2->Destroy();
				}
			}
			return 1;
		}

		// Rocket collision
		if (logic2 = (GameComponent*)obj2->GetComponent(CT_Rocket))
		{
			// Deletion detection
			if (!(obj1->has(Turret)) && !(obj1->has(Bullet)) && !(obj1->has(Rocket)) && !(obj1->has(BlockWin)) && obj1->state != Gas && obj1->state != Ghost)
			{
				obj2->Destroy();
			}

			//Damaging Player
			if(obj1->has(Player) && obj2->has(Sprite)->mColor.w != 0 && ((Rocket*)logic2)->SetCheckCollided())
			{
				//10%
				//DamagePlayer(PLAYER_MAX_HEALTH/10);
				//could just do this
				if(!obj1->has(Player)->mInvincible)
					DamagePlayer(2.5f);
			}

			return 1;
		}

		// Block collision
		if (obj2->has(Block) && (obj2->state == Gas || obj2->state == Liquid))
		{
			GOC* temp = obj1;
			obj1 = obj2;
			obj2 = temp;
		}
		if (logic1 = (GameComponent*)obj1->GetComponent(CT_Block))
		{
			Block* aBlock = obj1->has(Block);
			if (obj1->state == Gas)
			{
				if (obj2->has(Player))
				{
					inBlock = true;
					DamagePlayer(.05f);
					return 1;
				}
				Block* bBlock = obj2->has(Block);
				if (obj2->has(Static) || (bBlock && !(bBlock->mHolding)))
					return 0;
				return 1;
			}
			else if (obj1->state == Liquid)
			{
				Block* bBlock = obj2->has(Block);
				// Liquid doesn't collide with these
				if (aBlock->mHolding || obj2->has(BlockWin) || obj2->has(Turret) || (bBlock && bBlock->mHolding))
					return 1;
				// Liquid collides with gas but doesn't stick
				if (obj2->has(Block) && obj2->state == Gas)
					return 0;
				// Liquid to player "collision"
				if (obj2->has(Player))
				{
					inBlock = true;
					return 1;
				}
				// Liquid collides with and sticks to these
				if (!(aBlock->isFixed()))
					PHYSICS->ApplyCol(obj1->has(PhysBody), obj2->has(PhysBody), *dt);
				aBlock->Stick(obj2);
				return 1;
			}
			else
			{
				if(aBlock->mDropSound)
				{
					if(!aBlock->mHolding && obj2->has(Static))
					{
						aBlock->mDropSound = false;
						obj1->has(SoundNode)->Start(2);
					}
				}
				else
				{
					if(aBlock->mDropSoundFlag)
					{
						if(obj2->has(Static))
						{
							aBlock->mDropSoundFlag = false;
						}
					}
				}
			}
		}

		return 1;
	}

	void GameLogic::DamagePlayer(float damageAmount)
	{
		if(!mBDied)
		{
			mPlayer->has(Player)->mHealth -= damageAmount;

			UpdateHealthBar();

			if(mPlayer->has(Player)->mHealth <= 0)
			{
				Message* q = new Message(Mid::RBUTTONDOWN, 0);
				CORE->EnqueueMessage(q);

				mPlayer->has(Player)->mTutFlag = PickUp;//Cant Do Anything
				GRAPHICS->RedAll();

				mPlayer->has(CompSprite)->mComponents[0]->GoToFrame(0.f, 3.f);
				//1 is PLAYONCE
				mPlayer->has(CompSprite)->mComponents[0]->SetAnimation(1);
				mPlayer->has(CompSprite)->mComponents[0]->mAnimActive = true;
				mPlayer->has(SoundNode)->Start(2);

				mBDied = true;
			}
		}
	}

	void GameLogic::UpdateHealthBar()
	{
		//If updating the health bar set it visible
		mHUD->has(CompSprite)->mComponents[1]->SetFadeMode(1, 1.f);
		mHUD->has(CompSprite)->mComponents[2]->SetFadeMode(1, 1.f);
		mHUD->has(CompSprite)->mComponents[6]->SetFadeMode(1, 1.f);

		Transform * healthBar = mHUD->has(CompSprite)->mComponents[2]->mLocalTransform;

		healthBar->mScale.x = ((mPlayer->has(Player)->mHealth)/100.f) * mOriginalXScale;

		//1226 -full length
		//245.2  -real length

		//mScale.x * 1226 = 245.2
		//real / 2 = 122.6
		float HalfOfSizeX = (mOriginalXScale * mOriginalHudSizeX)/2.f;

		float offset = HalfOfSizeX - ((healthBar->mScale.x * mOriginalHudSizeX)/2.f);

		healthBar->mPosition.x = -offset + mOriginalHudXOffset;

		if(healthBar->mScale.x < 0)
		{
			healthBar->mScale.x = 0;
		}
	}


	//-----------------------------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------------------------


	void GameLogic::LoadSplashFile(const std::string &filename)
	{
		TextLoader stream;
		bool fileOpened = stream.Open(filename);

		std::string objectArchetype;
		Vec2 objectPosition;
		Vec2 objectScale;
		float objectRotation;

		// Cycles through the objects in the level file loading each one.
		StreamRead(stream, objectArchetype);
		StreamRead(stream, objectPosition);
		StreamRead(stream, objectScale);
		StreamRead(stream, objectRotation);

		CreateSplashObjectAt(objectPosition, objectScale, objectRotation, "..//Content//Objects//" + objectArchetype);

		GRAPHICS->mCameraOffset = 0.f;
		GRAPHICS->mZoom = 1.f;
		GRAPHICS->HandleLostDevice();
	}

	void GameLogic::UnloadSplashFile()
	{
		FACTORY->Destroy(mSplash);
	}

	/*****************************************************************************/
	/*!
	\brief
	Loads an entire menu from information found in a txt file.

	\param filename
	The menu file.
	*/
	/*****************************************************************************/
	void GameLogic::LoadMenuFile(const std::string& filename)
	{
		TextLoader stream;
		bool fileOpened = stream.Open(filename);

		std::string objectArchetype;
		Vec2 objectPosition;
		Vec2 objectScale;
		float objectRotation;

		// Cycles through the objects in the level file loading each one.
		while(stream.IsGood())
		{
			StreamRead(stream, objectArchetype);
			StreamRead(stream, objectPosition);
			StreamRead(stream, objectScale);
			StreamRead(stream, objectRotation);
			CreateMenuObjectAt(objectPosition, objectScale, objectRotation, "..//Content//Objects//" + objectArchetype);
		}

		GRAPHICS->mCameraOffset = 0.f;
		GRAPHICS->mZoom = 1.f;
		GRAPHICS->HandleLostDevice();
	}

	/*****************************************************************************/
	/*!
	\brief
	Loads an entire level from information found in a txt file.

	\param filename
	The level file.
	*/
	/*****************************************************************************/
	void GameLogic::LoadLevelFile(const std::string& filename)
	{
		TextLoader stream;
		bool fileOpened = stream.Open(filename);

		StreamRead(stream, GRAPHICS->mZoom);

		std::string objectName;
		std::string objectArchetype;
		Vec2 objectPosition;
		Vec2 objectScale;
		float objectRotation;
		int objectState;	// (0)Solid, (1)Liquid, (2)Gas, (3)Hot, (4)Warm, (5)Cold
		float turretRotation;

		// Cycles through the objects in the level file loading each one.
		while (stream.IsGood())
		{
			StreamRead(stream, objectName);
			mCurInstName = objectName;
			StreamRead(stream, objectArchetype);
			StreamRead(stream, objectPosition);
			StreamRead(stream, objectScale);
			StreamRead(stream, objectRotation);
			StreamRead(stream, objectState);

			if (objectArchetype == "Turret.txt")
			{
				StreamRead(stream, turretRotation);
				GOC * obj = CreateObjectAt(objectPosition, objectScale, objectRotation, EState(objectState), "..//Content//Objects//" + objectArchetype, objectName);
				obj->has(Turret)->mFieldOfView = turretRotation;
			}
			else if(objectArchetype == "HUD.txt")
			{
				CreateObjectAt(objectPosition, objectScale / GRAPHICS->mZoom, objectRotation, EState(objectState), "..//Content//Objects//" + objectArchetype, objectName);
			}
			else
			{
				CreateObjectAt(objectPosition, objectScale, objectRotation, EState(objectState), "..//Content//Objects//" + objectArchetype, objectName);
			}
		}

		GRAPHICS->mCameraOffset = 150.f;
		GRAPHICS->HandleLostDevice();
	}

	/*****************************************************************************/
	/*!
	\brief
	Loads an entire menu from information found in a txt file.

	\param filename
	The menu file.
	*/
	/*****************************************************************************/
	void GameLogic::LoadPauseFile(const std::string& filename)
	{
		TextLoader stream;
		bool fileOpened = stream.Open(filename);

		std::string objectArchetype;
		Vec2 objectPosition;
		Vec2 objectScale;
		float objectRotation;

		// Cycles through the objects in the level file loading each one.
		while(stream.IsGood())
		{
			StreamRead(stream, objectArchetype);
			StreamRead(stream, objectPosition);
			StreamRead(stream, objectScale);
			StreamRead(stream, objectRotation);

			if(objectArchetype == "PauseHUD.txt")
			{
				PauseObjList.push_back(CreateMenuObjectAt(objectPosition, objectScale / GRAPHICS->mZoom, objectRotation, "..//Content//Objects//" + objectArchetype, true));
			}
			else
			{
				objectPosition.y /= GRAPHICS->mZoom;
				objectPosition.x /= GRAPHICS->mZoom;
				objectPosition.y += floor(GRAPHICS->mCurrentCamera->mTransform->mPosition.y + GRAPHICS->mCameraOffset);
				objectPosition.x += floor(GRAPHICS->mCurrentCamera->mTransform->mPosition.x);

				PauseObjList.push_back(CreateMenuObjectAt((objectPosition), objectScale / GRAPHICS->mZoom, objectRotation, "..//Content//Objects//" + objectArchetype, true));
			}
		}
	}

	void GameLogic::LoadConfirmQuitFile(const std::string& file)
	{
		TextLoader stream;
		bool fileOpened = stream.Open(file);

		std::string objectArchetype;
		Vec2 objectPosition;
		Vec2 objectScale;
		float objectRotation;

		// Cycles through the objects in the level file loading each one.
		while(stream.IsGood())
		{
			StreamRead(stream, objectArchetype);
			StreamRead(stream, objectPosition);
			StreamRead(stream, objectScale);
			StreamRead(stream, objectRotation);

			objectPosition.y /= GRAPHICS->mZoom;
			objectPosition.x /= GRAPHICS->mZoom;
			objectPosition.y += floor(GRAPHICS->mCurrentCamera->mTransform->mPosition.y + GRAPHICS->mCameraOffset);
			objectPosition.x += floor(GRAPHICS->mCurrentCamera->mTransform->mPosition.x);

			PauseObjList.push_back(CreateMenuObjectAt(objectPosition, objectScale / GRAPHICS->mZoom, objectRotation, "..//Content//Objects//" + objectArchetype, true));
		}
	}

	/*****************************************************************************/
	/*!
	\brief
	UnLoads an entire menu from information found in a txt file.

	\param filename
	The menu file.
	*/
	/*****************************************************************************/
	void GameLogic::UnLoadPauseFile()
	{
		for(unsigned i = 0; i < PauseObjList.size(); ++i)
			FACTORY->Destroy(FACTORY->GetObjectWithId(PauseObjList[i]->GetId()));

		PauseObjList.clear();
	}


	//-----------------------------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------------------------


	void GameLogic::CreateSplashObjectAt(Vec2& position, Vec2& scale, float rotation, const std::string& file)
	{
		GOC* newObject = FACTORY->BuildAndSerialize(file);
		newObject->mArchetype = file;
		Transform* transform = newObject->has(Transform);
		transform->mPosition = position;
		transform->mScale = scale;
		transform->mRotation = rotation;

		newObject->Initialize();

		mSplash = newObject;
	}

	/*****************************************************************************/
	/*!
	\brief
	Creates an object from a file at a given location in the menu.

	\param position
	Coordinates for the new object.

	\param scale
	X and Y scale for the new object.

	\param rotation
	Rotation in radians for the new object.

	\param file
	The data file to create the new object from.

	\return newObject
	The newly created object.
	*/
	/*****************************************************************************/
	GOC* GameLogic::CreateMenuObjectAt(Vec2& position, Vec2& scale, float rotation, const std::string& file, bool pause)
	{
		GOC* newObject = FACTORY->BuildAndSerialize(file);
		newObject->mArchetype = file;
		Transform* transform = newObject->has(Transform);
		transform->mPosition = position;

		transform->mScale = scale;
		transform->mRotation = rotation;

		newObject->Initialize();

		return newObject;
	}

	/*****************************************************************************/
	/*!
	\brief
	Creates an object from a file at a given location in the game.

	\param position
	Coordinates for the new object.

	\param scale
	X and Y scale for the new object.

	\param rotation
	Rotation in radians for the new object.

	\param state
	The state (solid, liquid, gas) of the new object.

	\param file
	The data file to create the new object from.

	\return newObject
	The newly created object.
	*/
	/*****************************************************************************/
	GOC* GameLogic::CreateObjectAt(Vec2& position, Vec2& scale, float rotation, EState state, const std::string& file, const std::string& objName)
	{
		GOC* newObject = FACTORY->BuildAndSerialize(file);
		newObject->mObjectName = objName;
		newObject->mArchetype = file;
		Transform* transform = newObject->has(Transform);

		if(newObject->has(Particle))
		{
			transform->mPosition = position;
			transform->mRotation = rotation;
			newObject->has(Particle)->mTransform = transform;
		}
		else
		{
			transform->mPosition = position;
			transform->mScale = scale;
			transform->mRotation = rotation;
		}

		if (newObject->has(Player))
			mPlayer = newObject;

		if (newObject->mArchetype == "..//Content//Objects//HUD.txt")
			mHUD = newObject;

		newObject->state = state;
		newObject->Initialize();

		return newObject;
	}

	/*****************************************************************************/
	/*!
	\brief
	Creates an object from a file at a given location in the game.

	\param temp
	The file to read in from.

	\param x
	X position for the new object.

	\param y
	Y position for the new object.

	\param rotation
	Rotation in radians for the new object.

	\param scaleX
	X scale for the new object.

	\param scaleY
	Y scale for the new object.

	\param state
	The state (solid, liquid, gas) of the new object.

	\return tempObj
	The newly created object.
	*/
	/*****************************************************************************/
	GOC* GameLogic::SpawnObj(std::string temp, double x, double y, double rotation, double scalex, double scaley, int state)
	{
		Vec2 position;
		position.x = (float) x;
		position.y = (float) y;
		Vec2 scale;
		scale.x = (float) scalex;
		scale.y = (float) scaley;

		std::string str("..//Content//Objects//");
		str.append(temp);
		str.append(".txt");

		GOC* tempObj = CreateObjectAt(
			position,
			scale,
			(float)rotation * cDelicious / 180,
			EState(state),
			str
			);

		return tempObj;
	}

	/*****************************************************************************/
	/*!
	\brief
	Creates an object from a file at a given location in the game.

	\param temp
	The file to read in from.

	\param tempt
	The transformation of the new object.

	\param state
	The state (solid, liquid, gas) of the new object.

	\return tempObj
	The newly created object.
	*/
	/*****************************************************************************/
	GOC* GameLogic::SpawnObj(std::string temp, Transform* tempt, int state)
	{
		std::string str("..//Content//Objects//");
		str.append(temp);
		str.append(".txt");

		GOC* tempObj = CreateObjectAt(tempt->mPosition, 
			tempt->mScale, 
			tempt->mRotation,
			EState(state),
			str);

		return tempObj;
	}


	//-----------------------------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------------------------


	std::string GameLogic::AppendNumberToFile(std::string path, int num)
	{
		char temp[3];
		_itoa_s(num, temp, 3, 10);
		std::string str(temp);
		path.append(str);
		path.append(".txt");
		return path;
	}
}