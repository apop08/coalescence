/**************************************************************************************************/
/*!
\file	CreditsText.cpp
\par	Project: Coalescence
\date	4/11/2012
\authors
Primary : Kevin Verholtz

\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/**************************************************************************************************/
#include "Precompiled.h"
#include "CreditsText.h"
#include "GameLogic.h"
#include "Graphics.h"
#include "Camera.h"
#include "GameStateMgr.h"

#include "Core.h"

#include "DebugBars.h"

#define MoveSpeed .5f

namespace Scuttlemeyer
{
	/******************************************************************************/
	/*!
	  \brief
		Constructor sets mName to name of class
	*/
	/******************************************************************************/
	CreditsText::CreditsText()
	{
		mName = "CreditsText";
	}

	void CreditsText::Initialize()
	{
		FACTORY->RegisterForMessages(GetOwner());
		mTransform = GetOwner()->has(Transform);
		mSprite = GetOwner()->has(Sprite);
		mSprite->SetFadeMode(-1, 1);
	}

	Mid::MessageIdType CreditsText::SendMessage(Message* message)
	{
		if (message->mMessageId == Mid::MOUSEMOVE)
		{
			mTransform->mPosition.y += MoveSpeed;

			if(GetOwner()->mArchetype.find("CreditsText14.txt") != -1 && mTransform->mPosition.y >= 370 + floor(GRAPHICS->mCurrentCamera->mTransform->mPosition.y + GRAPHICS->mCameraOffset))
			{
				if (gGameStatePrev == GS_MENU || LOGIC->mBGameEnd)
				{
					LOGIC->curMenu = 0;
					Message * q = new Message(Mid::Reload, 0);
					CORE->EnqueueMessage(q);
				}
				else
				{
					LOGIC->mKevinISAJEWPACABRAMEXICAN = false;
					// delete the pause buttons
					while (LOGIC->PauseObjList.size() > LOGIC->pListEnd)
					{
						std::vector<GOC*>::iterator it = LOGIC->PauseObjList.begin() + LOGIC->pListEnd;
						FACTORY->Destroy(FACTORY->GetObjectWithId(LOGIC->PauseObjList[LOGIC->pListEnd]->GetId()));
						LOGIC->PauseObjList.erase(it);
					}
				}
			}

			if(mTransform->mPosition.y >= 280 + floor(GRAPHICS->mCurrentCamera->mTransform->mPosition.y + GRAPHICS->mCameraOffset))
			{
				mSprite->SetFadeMode(-1, 0.006f);
			}
			else if(mTransform->mPosition.y >= -250 + floor(GRAPHICS->mCurrentCamera->mTransform->mPosition.y + GRAPHICS->mCameraOffset))
			{
				mSprite->SetFadeMode(1, 0.01f);
			}
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
	std::string CreditsText::ToString()
	{
		return "Logic Component: CreditsText";
	}
	/******************************************************************************/
	/*!
	  \brief
		adds what this class is to the tweek bar
	*/
	/******************************************************************************/
	void CreditsText::ToBar()
	{
		TwBar* bar = DEBUGBARS->GetBarByName("ObjectProperties");

		TwAddVarRO(bar, "Logic Component", TW_TYPE_STDSTRING, &mName, " help='Type of logical component.' ");
	}
	/******************************************************************************/
	/*!
	  \brief
		destructor removes freed button from the list of button objects
	*/
	/******************************************************************************/
	CreditsText::~CreditsText()
	{
		FACTORY->UnregisterForMessages(GetOwner());
	}
}