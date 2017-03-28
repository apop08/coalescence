#include "Precompiled.h"
#include "Button.h"
#include "GameLogic.h"

#include "Core.h"

#include "Transform.h"
#include "PhysBody.h"
#include "CompSprite.h"
#include "DebugConsole.h"
#include "Camera.h"
#include "DebugBars.h"
#include "Graphics.h"
#include "GameStateMgr.h"
namespace Scuttlemeyer
{
	Button::Button()
	{
		mName = "Button";
		trigger = false;
	}

	void Button::Initialize()
	{
		LOGIC->mButtonList.push_back(reinterpret_cast<ILogical*>(this));
	}

	bool Button::isMouseOn()
	{
			Transform* pTransform = ((Transform*)GetOwner()->GetComponent(CT_Transform));
			Sprite * bSprite = (Sprite*)GetOwner()->GetComponent(CT_Sprite);
			float mx = CORE->MouseInput.MousePosition.x - GRAPHICS->mScreenWidth / 2;
			float my = CORE->MouseInput.MousePosition.y - 150 - GRAPHICS->mScreenHeight / 2;
	float dx,dy;
		if(gGameStateCurr == GS_PAUSE)
		{
			Transform* temp = ((Transform*) LOGIC->mPlayer->GetComponent(CT_Transform));	

			dx = abs(mx - pTransform->mPosition.x + temp->mPosition.x);	// CHANGE
			dy = abs(my + pTransform->mPosition.y - temp->mPosition.y);	// CHANGE
		}
		else
		{
			dx = abs(mx - pTransform->mPosition.x);	// CHANGE
			dy = abs(my + pTransform->mPosition.y);	// CHANGE
		}
			if (dx < bSprite->mSize.x / 2 && dy < bSprite->mSize.y / 2)
				return true;
			else
				return false;
	}

	bool Button::isTriggered()
	{
		return trigger;
	}

	void Button::SendMessage(Message* message)
	{
		if (message->mMessageId == Mid::MOUSEMOVE)
		{
			Sprite * bSprite = (Sprite*)GetOwner()->GetComponent(CT_Sprite);
			if (isMouseOn())
				bSprite->mTexture = GRAPHICS->GetTexture("..//Content//Test//new_selected_small.png");
			else
				bSprite->mTexture = GRAPHICS->GetTexture("..//Content//Test//new_small.png");
		}
		else if (message->mMessageId == Mid::LBUTTONDOWN && isMouseOn())
		{
			Sprite * bSprite = (Sprite*)GetOwner()->GetComponent(CT_Sprite);
			bSprite->mTexture = GRAPHICS->GetTexture("..//Content//Test//new_small.png");
		}
		else if (message->mMessageId == Mid::LBUTTONUP && isMouseOn())
		{
			Sprite * bSprite = (Sprite*)GetOwner()->GetComponent(CT_Sprite);
			bSprite->mTexture = GRAPHICS->GetTexture("..//Content//Test//new_selected_small.png");
			// activate button logic
			trigger = true;
		}
	}

	std::string Button::ToString()
	{
		return "Logic Component: Button";
	}

	void Button::ToBar()
	{
		TwBar* bar = DEBUGBARS->GetBarByName("ObjectProperties");

		TwAddVarRO(bar, "Logic Component", TW_TYPE_STDSTRING, &mName, " help='Type of logical component.' ");
	}

	Button::~Button()
	{
		LOGIC->mButtonList.erase(reinterpret_cast<ILogical*>(this));
	}
}