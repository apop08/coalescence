/**************************************************************************************************/
/*!
\file	BrokenPipe.cpp
\par	Project: Coalescence
\date	2/24/2012
\authors
Primary  : Lincoln Tosi

\brief
Logic for "broken" pipes that spew a special state changing gas.

\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/**************************************************************************************************/
#include "Precompiled.h"
#include "BrokenPipe.h"

#include "GameLogic.h"
#include "DebugBars.h"
#include "ParticleEmitter.h"
#include "AttenuatedSoundNode.h"

namespace Scuttlemeyer
{
	/*****************************************************************************/
	/*!
		\brief
			Constructor - Initializes necessary variables.
	*/
	/*****************************************************************************/
	BrokenPipe::BrokenPipe()
	{
		mName = "BrokenPipe";
		mBStarted = false;
	}

	/*****************************************************************************/
	/*!
		\brief
			Initializes variables.
			Adds this object to the logic objects list.
	*/
	/*****************************************************************************/
	void BrokenPipe::Initialize()
	{
		CalculateExtents();
		pipe_state = GetOwner()->state;	// (3)Hot, (4)Warm, (5)Cold

		LOGIC->mLogicList.push_back(reinterpret_cast<ILogical*>(this));
	}

	/*****************************************************************************/
	/*!
		\brief
			Reads in configuration data from a file.

		\param stream
			The stream to read in from.
	*/
	/*****************************************************************************/
	void BrokenPipe::Serialize(TextLoader &stream)
	{
		StreamRead(stream, mExtents);
		StreamRead(stream, open_side);	// (0) top, (1) left, (2) bottom, (3) right

		TextLoader stream2;

		std::string name = "..//Content//Objects//";
		char tempChar[20];
		sprintf_s(tempChar, 20, "%i", LOGIC->curLevel);
		name.append(tempChar);
		name.append("//");
		name.append(LOGIC->mCurInstName);
		name.append(".bkp.data");

		stream2.Open(name);

		if(stream2.IsGood())
		{
			int tempBool;
			StreamRead(stream2, tempBool);
			mBActivateIsActivate = tempBool == 0 ? false : true;
			StreamRead(stream2, tempBool);
			mBIsActivated = tempBool == 0 ? false : true;
		}
		else
		{
			mBActivateIsActivate = true;
			mBIsActivated = true;
		}
	}

	/*****************************************************************************/
	/*!
		\brief
			Called every frame. Checks if any block objects pass through the
			region and change their state accordingly.

		\param dt
			The frame time.
	*/
	/*****************************************************************************/
	void BrokenPipe::Update(float dt)
	{
		if(!mBStarted)
		{
			if ((mBIsActivated && mBActivateIsActivate) || (mBIsActivated && !mBActivateIsActivate))
			{
				GetOwner()->has(Emitter)->mJetPack = true;
				GetOwner()->has(AttenuatedSoundNode)->Start(0);
			}
			else
			{
				GetOwner()->has(Emitter)->mJetPack = false;
			}

			mBStarted = true;
		}

		if ((mBIsActivated && mBActivateIsActivate) || (mBIsActivated && !mBActivateIsActivate))
		{
			ObjectLinkedList<ILogical>::iterator it;
			for (it = LOGIC->mLogicList.begin(); it != LOGIC->mLogicList.end(); ++it)
			{
				GOC* parent = it->GetOwner();
				if (parent->has(Block) && CheckExtents(parent))
				{
					parent->has(Block)->ChangeState(EState(pipe_state), true);
				}
			}
		}
	}

	/*****************************************************************************/
	/*!
		\brief
			Returns the side that the gas is being emitted from.

		\return
			The side that the gas is being emitted from.
	*/
	/*****************************************************************************/
	int BrokenPipe::GetSide()
	{
		return open_side;
	}

	/*****************************************************************************/
	/*!
		\brief
			Returns the state changer that the gas is being emitted from.

		\return
			The state changer that the gas is being emitted from.
	*/
	/*****************************************************************************/
	int BrokenPipe::GetState()
	{
		return pipe_state;
	}

	/*****************************************************************************/
	/*!
		\brief
			Checks if a block is inside the region.

		\param parent
			The object to check against.

		\return
			true if inside, false if not.
	*/
	/*****************************************************************************/
	bool BrokenPipe::CheckExtents(GOC* parent)
	{
		Sprite * bSprite = (Sprite*)parent->GetComponent(CT_Sprite);
		Transform* bTransform = ((Transform*)parent->GetComponent(CT_Transform));

		if (bSprite && bTransform)
		{
				// check the midpoint first (save time)
			if (bTransform->mPosition.x < maxExtents.x
			 && bTransform->mPosition.x > minExtents.x
			 && bTransform->mPosition.y < maxExtents.y
			 && bTransform->mPosition.y > minExtents.y)
				return true;

				// if that didn't work, check bounds
			float width = (bSprite->mSize.x * bTransform->mScale.x) / 2.0f;
			float x_max = bTransform->mPosition.x + width;
			float x_min = bTransform->mPosition.x - width;

			float height = (bSprite->mSize.y * bTransform->mScale.y) / 2.0f;
			float y_max = bTransform->mPosition.y + height;
			float y_min = bTransform->mPosition.y - height;

			if ((x_max < maxExtents.x && x_max > minExtents.x)
			||	(x_min < maxExtents.x && x_min > minExtents.x)
			||  (x_max > maxExtents.x && x_min < minExtents.x))
			{
				if ((y_max < maxExtents.y && y_max > minExtents.y)
				||  (y_min < maxExtents.y && y_min > minExtents.y)
				||  (y_max > maxExtents.y && y_min < minExtents.y))
					return true;
			}
		}
		return false;
	}

	/*****************************************************************************/
	/*!
		\brief
			Calculates the original extents of the region based off of information
			read from the file.
	*/
	/*****************************************************************************/
	void BrokenPipe::CalculateExtents()
	{
		Sprite * pSprite = (Sprite*)GetOwner()->GetComponent(CT_Sprite);
		Transform* pTransform = ((Transform*)GetOwner()->GetComponent(CT_Transform));

			// complicated maths
		float width = ((pSprite->mSize.x * pTransform->mScale.x) / 2.0f) + (mExtents.x / 2.0f);
		float height = ((pSprite->mSize.y * pTransform->mScale.y) / 2.0f) + (mExtents.y / 2.0f);

		if (open_side == 0)			// top
			width = 0;
		else if (open_side == 1)	// left
		{
			width = -width;
			height = 0;
		}
		else if (open_side == 2)	// bottom
		{
			width = 0;
			height = -height;
		}
		else if (open_side == 3)	// right
			height = 0;

		maxExtents.x = pTransform->mPosition.x + (mExtents.x / 2.0f) + width;
		maxExtents.y = pTransform->mPosition.y + (mExtents.y / 2.0f) + height;

		minExtents.x = pTransform->mPosition.x - (mExtents.x / 2.0f) + width;
		minExtents.y = pTransform->mPosition.y - (mExtents.y / 2.0f) + height;
	}

	/*****************************************************************************/
	/*!
		\brief
			Turns on the gas emission.
	*/
	/*****************************************************************************/
	void BrokenPipe::Activate()
	{
		if (!mBIsActivated && mBActivateIsActivate)
		{
			// emit gasses from the pipe
			mBIsActivated = true;
			GetOwner()->has(Emitter)->mJetPack = true;
			GetOwner()->has(AttenuatedSoundNode)->Start(0);
		}
		else if(mBIsActivated && !mBActivateIsActivate)
		{
			mBIsActivated = false;
			GetOwner()->has(Emitter)->mJetPack = false;
			GetOwner()->has(AttenuatedSoundNode)->Stop(0);
		}
	}

	/*****************************************************************************/
	/*!
		\brief
			Turns off the gas emission.
	*/
	/*****************************************************************************/
	void BrokenPipe::Deactivate()
	{
		if (mBIsActivated && mBActivateIsActivate)
		{
			// stop the emitter
			mBIsActivated = false;
			GetOwner()->has(Emitter)->mJetPack = false;
			GetOwner()->has(AttenuatedSoundNode)->Stop(0);
		}
		else if(!mBIsActivated && !mBActivateIsActivate)
		{
			mBIsActivated = true;
			GetOwner()->has(Emitter)->mJetPack = true;
			GetOwner()->has(AttenuatedSoundNode)->Start(0);
		}
	}

	/*****************************************************************************/
	/*!
		\brief
			Recieves messages sent from other places in the code.

		\param message
			The message being recieved.

		\return
			The message type to be deleted.
	*/
	/*****************************************************************************/
	Mid::MessageIdType BrokenPipe::SendMessage(Message* message)
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

	/*****************************************************************************/
	/*!
		\brief
			Converts the type of this object into a string.

		\return
			The string containing this object type.
	*/
	/*****************************************************************************/
	std::string BrokenPipe::ToString()
	{
		return "Logic Component: BrokenPipe";
	}

	/*****************************************************************************/
	/*!
		\brief
			Adds this object to the debug display.
	*/
	/*****************************************************************************/
	void BrokenPipe::ToBar()
	{
		TwBar* bar = DEBUGBARS->GetBarByName("ObjectProperties");

		TwAddVarRO(bar, "Logic Component", TW_TYPE_STDSTRING, &mName, " help='Type of logical component.' ");
	}

	/*****************************************************************************/
	/*!
		\brief
			Deconstructor - removes this object from the object list.
	*/
	/*****************************************************************************/
	BrokenPipe::~BrokenPipe()
	{
		LOGIC->mLogicList.erase(reinterpret_cast<ILogical*>(this));
	}
}