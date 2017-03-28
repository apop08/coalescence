/**************************************************************************************************/
/*!
\file	Player.cpp
\par	Project: Coalescence
\date	10/1/2011
\authors
Primary  : Lincoln Tosi

\brief
Logic for everything the player does (jumping, shooting, etc.).

\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/**************************************************************************************************/
#include "Precompiled.h"
#include "Player.h"
#include "GameLogic.h"
#include "GameStateMgr.h"

#include "Core.h"

#include "SoundNode.h"
#include "DebugConsole.h"
#include "DebugBars.h"
#include "ParticleEmitter.h"

namespace Scuttlemeyer
{
	const float JUMP_FORCE = 50.0f;
	const float BULLET_OFFSET_X = 90.0f;
	const float BULLET_OFFSET_Y = 100.0f;
	const float ARM_OFFSET_X = 6.0f;
	const float ARM_OFFSET_Y = 20.0f;
	const float HOLDING_ROTATION_LIMIT = 100.0f * cDelicious / 180.0f;
	const float BULLET_SPEED = 700.0f;
	const DWORD FIRE_DELAY = 500;
	const DWORD HEALTH_REGEN_TIMER = 5000;

	/*****************************************************************************/
	/*!
		\brief
			Constructor - Initializes necessary variables.
	*/
	/*****************************************************************************/
	Player::Player()
	{
		mName = "Player";
	}

	/*****************************************************************************/
	/*!
		\brief
			Initializes variables.
			Adds this object to the logic objects list.
	*/
	/*****************************************************************************/
	void Player::Initialize()
	{
		LOGIC->mLogicList.push_back(reinterpret_cast<ILogical*>(this));

		pPhysics = GetOwner()->has(PhysBody);
		pTransform = GetOwner()->has(Transform);
		pDrawable = GetOwner()->has(CompSprite);
		pPhysics->mColFlags |= 1;

		mFireState = Cold;
		mHealthTimer = LOGIC->fireTimer = timeGetTime();
		mOldHealth = mHealth = PLAYER_MAX_HEALTH;
		mTutFlag = GetOwner()->state;
		mFaceDir = 1;

		CompSprite* pDrawable = ((CompSprite*)GetOwner()->GetComponent(CT_CompSprite));
		pDrawable->mComponents[1]->SetAnimation(STOP);
		pDrawable->mComponents[1]->mAnimActive = false;
		pDrawable->mComponents[1]->GoToFrame(1.f);
		pDrawable->mComponents[0]->SetAnimation(STOP);
		pDrawable->mComponents[0]->mAnimActive = false;
		pDrawable->mComponents[0]->GoToFrame(0.f);

		mInvincible = false;
	}

	/*****************************************************************************/
	/*!
		\brief
			Change the player's state.

		\param nState
			The state to change to.

		\param veloReset
			Whether or not to reset the objects velocity (only used in Block.cpp).
	*/
	/*****************************************************************************/
	int Player::ChangeState(EState nState, bool veloReset)
	{
		if(mFireState != nState)
		{
			LOGIC->mHUD->has(SoundNode)->Start();
		}

		LOGIC->mHUD->has(CompSprite)->mComponents[mFireState]->SetFadeMode(-1, 1.f);
		mFireState = nState;
		LOGIC->mHUD->has(CompSprite)->mComponents[mFireState]->SetFadeMode(1, 1.f);
		return 1;
	}

	/*****************************************************************************/
	/*!
		\brief
			Handles the player's health regen after a certain time.
	*/
	/*****************************************************************************/
	void Player::HealthRegen(void)
	{
		if (mHealth < PLAYER_MAX_HEALTH)
		{
			mOldHealth = (mHealth = (mHealth + 0.5f <= PLAYER_MAX_HEALTH ? mHealth + 0.5f : PLAYER_MAX_HEALTH));
			LOGIC->UpdateHealthBar();
		}
		else
		{
			LOGIC->mHUD->has(CompSprite)->mComponents[1]->SetFadeMode(-1, .03f);
			LOGIC->mHUD->has(CompSprite)->mComponents[2]->SetFadeMode(-1, .03f);
			LOGIC->mHUD->has(CompSprite)->mComponents[6]->SetFadeMode(-1, .03f);
		}
	}

	/*****************************************************************************/
	/*!
		\brief
			Called every frame. Handles the player input and health calculation.

		\param dt
			The frame time.
	*/
	/*****************************************************************************/
	void Player::Update(float dt)
	{
		if (mOldHealth == mHealth)
		{
			if (timeGetTime() - mHealthTimer > HEALTH_REGEN_TIMER)
				HealthRegen();
		}
		else
		{
			mOldHealth = mHealth;
			mHealthTimer = timeGetTime();
		}

		HandleInput(dt);
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
	Mid::MessageIdType Player::SendMessage(Message* message)
	{
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
	std::string Player::ToString()
	{
		return "Logic Component: Player";
	}

	/*****************************************************************************/
	/*!
		\brief
			Adds this object to the debug display.
	*/
	/*****************************************************************************/
	void Player::ToBar()
	{
		TwBar* bar = DEBUGBARS->GetBarByName("ObjectProperties");
		TwAddVarRO(bar, "Logic Component", TW_TYPE_STDSTRING, &mName, " help='Type of logical component.' ");
	}

	/*****************************************************************************/
	/*!
		\brief
			Handles the input for moving the player as well as controlling the arm
			and shooting state changing bullets.

		\param dt
			The frame time.
	*/
	/*****************************************************************************/
	void Player::HandleInput(const float dt)
	{
		// Only handle input if the debug console, pause menu, and editor aren't open... also if you haven't died
		if ((DBGCNSL->DBConsole == 0 || DBGCNSL->mSteps > 0) && (gGameStateCurr != GS_PAUSE) && !(CORE->mBEditorActive))
		{
			if(LOGIC->mBDied)
			{
				pDrawable->mComponents[1]->mLocalTransform->mPosition.y -= 2.f * pDrawable->mComponents[0]->mCurFrame;
			}
			else
			{
				if (mTutFlag & Move)
				{
						// Move the player right or left (or not at all if both directions are held)
						// left arrow or a key and not right arrow or d key
					if ((GetKeyState(VK_LEFT) < 0 || GetKeyState(65) < 0) && !(GetKeyState(VK_RIGHT) < 0 || GetKeyState(68) < 0))
					{
						if (pPhysics->mColFlags & 2)
						{
							pDrawable->mComponents[0]->GoToFrameLevel(0.f);
							pDrawable->mComponents[0]->SetAnimation(PLAYONCE, mFaceDir == -1 ? FORWARD : REVERSE);
							pDrawable->mComponents[0]->mAnimActive = true;
						}

						pPhysics->AddForce(0.8f * pPhysics->mMass * (dt / cOneFrame), pTransform->mRotation + cDirLeft);
					}
						// right arrow or d key and not left arrow or a key
					else if ((GetKeyState(VK_RIGHT) < 0 || GetKeyState(68) < 0) && !(GetKeyState(VK_LEFT) < 0 || GetKeyState(65) < 0))
					{
						if (pPhysics->mColFlags & 2)
						{
							pDrawable->mComponents[0]->GoToFrameLevel(0.f);
							pDrawable->mComponents[0]->SetAnimation(PLAYONCE, mFaceDir == 1 ? FORWARD : REVERSE);
							pDrawable->mComponents[0]->mAnimActive = true;
						}

						pPhysics->AddForce(0.8f * pPhysics->mMass * (dt / cOneFrame), pTransform->mRotation + cDirRight);
					}
					else if ((pPhysics->mColFlags & 2) && (pDrawable->mComponents[0]->mAnimActive == false))
					{
						pDrawable->mComponents[0]->SetAnimation(STOP);
						//pDrawable->mComponents[0]->mAnimActive = false;
						pDrawable->mComponents[0]->GoToFrame(0.f, 1.f);
					}

						// Make the player jump if it has collided since last jump (up arrow, w key, or space)
					if ((mTutFlag & Jump) && (pPhysics->mColFlags & 2) && (GetKeyState(VK_UP) < 0 || GetKeyState(87) < 0 || GetKeyState(VK_SPACE) < 0))
					{
						pPhysics->mVelocity.y = 0.0f;
						pPhysics->AddForce(JUMP_FORCE * pPhysics->mMass, pTransform->mRotation + cDirUp);

						pDrawable->mComponents[0]->SetAnimation(STOP);
						pDrawable->mComponents[0]->mAnimActive = false;
						pDrawable->mComponents[0]->GoToFrame(0.f, 2.f);

						GetOwner()->has(Emitter)->mJetPack = true;
						GetOwner()->has(SoundNode)->Start(1);
					}

					if (pPhysics->mColFlags & 2)
						pPhysics->mColFlags -= 2;
				}

				Vec2 mousePos;
				mousePos.x = CORE->MouseInput.MousePosition.x - GRAPHICS->mScreenWidth / 2.f;
				mousePos.y = GRAPHICS->mHeightRatio * ARM_OFFSET_Y + CORE->MouseInput.MousePosition.y - GRAPHICS->mHeightRatio * GRAPHICS->mCameraOffset - GRAPHICS->mScreenHeight / 2.f;
				Normalize(mousePos);

				float rotation = atan2(-mousePos.x, -mousePos.y);

				if (rotation < 0)
				{
					mFaceDir = 1;
					if (!pDrawable->GetFacing())
						pDrawable->SetFacing(true);
					if (LOGIC->isHolding && rotation < -HOLDING_ROTATION_LIMIT)
						rotation = -HOLDING_ROTATION_LIMIT;
				}
				else
				{
					mFaceDir = -1;
					if (pDrawable->GetFacing())
						pDrawable->SetFacing(false);
					if (LOGIC->isHolding && rotation > HOLDING_ROTATION_LIMIT)
						rotation = HOLDING_ROTATION_LIMIT;
				}

				pDrawable->mComponents[1]->mLocalTransform->mRotation = ((rotation * mFaceDir * 180) / cDelicious) - pTransform->mRotation;

				if (mTutFlag & ShootS || mTutFlag & ShootL || mTutFlag & ShootG)
				{
					if ((timeGetTime() - LOGIC->fireTimer > FIRE_DELAY) && !(LOGIC->isHolding) && !(LOGIC->inBlock))
					{
							// Fire the cannon
						if (CORE->MouseInput.LeftButtonDown == true)
						{
							float cannonRotation = ((pDrawable->mComponents[1]->mLocalTransform->mRotation * mFaceDir * cDelicious) / 180) + cDirUp + pTransform->mRotation;
							float bulletX = pTransform->mPosition.x + (ARM_OFFSET_X * mFaceDir) + (BULLET_OFFSET_X * pTransform->mScale.x * cos(cannonRotation));
							float bulletY = pTransform->mPosition.y + ARM_OFFSET_Y + (BULLET_OFFSET_Y * pTransform->mScale.y * sin(cannonRotation));

							GOC* tempObj;

							if (mFireState == Hot && mTutFlag & ShootG)
							{
								float rot = pDrawable->GetFacing() ? pDrawable->mComponents[1]->mLocalTransform->mRotation : -pDrawable->mComponents[1]->mLocalTransform->mRotation;
								GetOwner()->has(SoundNode)->Start(0);
								tempObj = LOGIC->CreateObjectAt(Vec2(bulletX, bulletY), 
									Vec2(0.5f, 0.5f), 
									rot, 
									mFireState,
									"..//Content//Objects//bullet_1.txt");
								((PhysBody*)tempObj->GetComponent(CT_PhysBody))->mVelocity.x = BULLET_SPEED * -sin(((pDrawable->mComponents[1]->mLocalTransform->mRotation * mFaceDir * cDelicious) / 180) + pTransform->mRotation);
								((PhysBody*)tempObj->GetComponent(CT_PhysBody))->mVelocity.y = BULLET_SPEED * cos(((pDrawable->mComponents[1]->mLocalTransform->mRotation * mFaceDir * cDelicious) / 180) + pTransform->mRotation);
							}
							else if (mFireState == Warm && mTutFlag & ShootL)
							{
								float rot = pDrawable->GetFacing() ? pDrawable->mComponents[1]->mLocalTransform->mRotation : -pDrawable->mComponents[1]->mLocalTransform->mRotation;
								GetOwner()->has(SoundNode)->Start(0);
								tempObj = LOGIC->CreateObjectAt(Vec2(bulletX, bulletY), 
									Vec2(0.5f, 0.5f), 
									rot, 
									mFireState,
									"..//Content//Objects//bullet_2.txt");
								((PhysBody*)tempObj->GetComponent(CT_PhysBody))->mVelocity.x = BULLET_SPEED * -sin(((pDrawable->mComponents[1]->mLocalTransform->mRotation * mFaceDir * cDelicious) / 180) + pTransform->mRotation);
								((PhysBody*)tempObj->GetComponent(CT_PhysBody))->mVelocity.y = BULLET_SPEED * cos(((pDrawable->mComponents[1]->mLocalTransform->mRotation * mFaceDir * cDelicious) / 180) + pTransform->mRotation);
							}
							else if (mFireState == Cold && mTutFlag & ShootS)
							{
								float rot = pDrawable->GetFacing() ? pDrawable->mComponents[1]->mLocalTransform->mRotation : -pDrawable->mComponents[1]->mLocalTransform->mRotation;
								GetOwner()->has(SoundNode)->Start(0);
								tempObj = LOGIC->CreateObjectAt(Vec2(bulletX, bulletY), 
									Vec2(0.5f, 0.5f), 
									rot, 
									mFireState,
									"..//Content//Objects//bullet_3.txt");
								((PhysBody*)tempObj->GetComponent(CT_PhysBody))->mVelocity.x = BULLET_SPEED * -sin(((pDrawable->mComponents[1]->mLocalTransform->mRotation * mFaceDir * cDelicious) / 180) + pTransform->mRotation);
								((PhysBody*)tempObj->GetComponent(CT_PhysBody))->mVelocity.y = BULLET_SPEED * cos(((pDrawable->mComponents[1]->mLocalTransform->mRotation * mFaceDir * cDelicious) / 180) + pTransform->mRotation);
							}

							LOGIC->fireTimer = timeGetTime();
						}
					}

					if (mTutFlag & PickUp)
					{
							// Delay the fire timer when... why is this here?
						if (CORE->MouseInput.RightButtonDown == true)
							LOGIC->fireTimer = timeGetTime();
					}
				}
			}

			LOGIC->inBlock = false;
		}
	}

	/*****************************************************************************/
	/*!
		\brief
			Deconstructor - removes this object from the object list.
	*/
	/*****************************************************************************/
	Player::~Player()
	{
		LOGIC->mLogicList.erase(reinterpret_cast<ILogical*>(this));
	}
}