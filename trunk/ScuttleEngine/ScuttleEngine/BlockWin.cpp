/******************************************************************************/
/*!
\file   BlockWin.cpp
\author Kevin Verholtz
\par    email: kevin.verholtz\@digipen.edu
\par    DigiPen login: kevin.verholtz
\par    Course: GAM200
\date   12/8/2011
\brief  
Block that handles our win condition for levels.
Very simple right now.
\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/******************************************************************************/
#include "Precompiled.h"
#include "BlockWin.h"
#include "GameLogic.h"

#include "Core.h"

#include "Transform.h"
#include "PhysBody.h"
#include "SoundNode.h"

#include "DebugConsole.h"
#include "DebugBars.h"

#include "GameStateList.h"

namespace Scuttlemeyer
{
	/******************************************************************************/
	/*!
	  \brief
		Constructor sets mName to name of class
	*/
	/******************************************************************************/
	BlockWin::BlockWin()
	{
		mName = "BlockWin";
		mBIsWin = false;
	}
	/******************************************************************************/
	/*!
	  \brief
		initialaizes this block
	*/
	/******************************************************************************/
	void BlockWin::Initialize()
	{
		GetOwner()->has(Sprite)->SetAnimation(STOP, FORWARD);
		GetOwner()->has(Sprite)->mAnimActive = false;
		if(GetOwner()->state != Solid)
		{
			GetOwner()->has(Sprite)->GoToFrame(GetOwner()->has(Sprite)->mNumFrames - 1.f);
			mBIsActivated = true;
		}
		else
		{
			mBIsActivated = false;
		}

		LOGIC->mLogicList.push_back(reinterpret_cast<ILogical*>(this));
	}
	/******************************************************************************/
	/*!
	  \brief
		If you pass the finish line, which is this block, you win the level

	  \param dt
	    this is needed to overload the virtual function even though dt isn't used
	*/
	/******************************************************************************/
	void BlockWin::Update(float dt)
	{
		Transform* pTransform = ((Transform*)LOGIC->mPlayer->GetComponent(CT_Transform));
		Transform* bTransform = ((Transform*)GetOwner()->GetComponent(CT_Transform));
		PhysBody* bPhys = GetOwner()->has(PhysBody);

		//if you enter the door you win
		if(mBIsActivated && 
			pTransform->mPosition.x < bTransform->mPosition.x + bPhys->mExtents.x && 
			pTransform->mPosition.x > bTransform->mPosition.x - bPhys->mExtents.x && 
			pTransform->mPosition.y < bTransform->mPosition.y + bPhys->mExtents.y && 
			pTransform->mPosition.y > bTransform->mPosition.y - bPhys->mExtents.y && 
			GetOwner()->has(Sprite)->mCurFrame == GetOwner()->has(Sprite)->mNumFrames - 1)
		{
			Message* q = new Message(Mid::RBUTTONDOWN, 0);
			CORE->EnqueueMessage(q);

			LOGIC->mPlayer->has(Player)->mTutFlag = PickUp;//Cant Do Anything
			LOGIC->mPlayer->has(Player)->mInvincible = true;

			mBIsWin = true;
			LOGIC->mPlayer->has(CompSprite)->SetFadeMode(-1, 0.05f);
			Deactivate();
		}

		if(mBIsWin && GetOwner()->has(Sprite)->mCurFrame == 0)
		{
			//if you at the last level, go to the credits screen
			if(LOGIC->curLevel == LOGIC->mMaxLevel)
			{
				LOGIC->mBGameEnd = true;
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
				LOGIC->curTransition = 2;
				LOGIC->mBCheckpointActive = false;
				Message* q = new Message(Mid::Transition, 0);
				CORE->EnqueueMessage(q);
			}
			if(CORE->mLevelUnlocked <= CORE->mLevelSelected)
			{
				CORE->mLevelSelected++;
				CORE->mLevelUnlocked = CORE->mLevelSelected;
				LOGIC->Save();
			}
		}
	}

	void BlockWin::Activate()
	{
		GetOwner()->has(SoundNode)->Stop(1);
		GetOwner()->has(SoundNode)->Start(0);
		GetOwner()->has(Sprite)->mAnimActive = true;
		GetOwner()->has(Sprite)->SetAnimation(PLAYONCE, FORWARD);
		mBIsActivated = true;
	}

	void BlockWin::Deactivate()
	{
		GetOwner()->has(SoundNode)->Stop(0);
		GetOwner()->has(SoundNode)->Start(1);
		GetOwner()->has(Sprite)->mAnimActive = true;
		GetOwner()->has(Sprite)->SetAnimation(PLAYONCE, REVERSE);
		mBIsActivated = false;
	}

	/******************************************************************************/
	/*!
	  \brief
		empty for now maybe later will do something

	  \param message
	    message to process
	*/
	/******************************************************************************/
	Mid::MessageIdType BlockWin::SendMessage(Message* message)
	{
		if(message->mMessageId == Mid::Activate)
		{
			Activate();
		}
		else if(message->mMessageId == Mid::Deactivate)
		{
			Deactivate();
		}

		return Mid::None;
	}
	/******************************************************************************/
	/*!
	  \brief
		returns string of what this class is

	  \return
	    string of the class
	*/
	/******************************************************************************/
	std::string BlockWin::ToString()
	{
		return "Logic Component: BlockWin";
	}
	/******************************************************************************/
	/*!
	  \brief
		adds what this class is to the tweek bar
	*/
	/******************************************************************************/
	void BlockWin::ToBar()
	{
		TwBar* bar = DEBUGBARS->GetBarByName("ObjectProperties");

		TwAddVarRO(bar, "Logic Component", TW_TYPE_STDSTRING, &mName, " help='Type of logical component.' ");
	}
	/******************************************************************************/
	/*!
	  \brief
		destructor removes freed object from the list of logic objects
	*/
	/******************************************************************************/
	BlockWin::~BlockWin()
	{
		LOGIC->mLogicList.erase(reinterpret_cast<ILogical*>(this));
	}
}