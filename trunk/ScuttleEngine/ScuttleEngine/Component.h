/******************************************************************************/
/*!
\file   Component.h
\author Kevin Verholtz
\par    email: kevin.verholtz\@digipen.edu
\par    DigiPen login: kevin.verholtz
\par    Course: GAM200
\date   12/8/2011
\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/******************************************************************************/
#pragma once

#include "Message.h"
#include "TextSerialization.h"
#include "ComponentTypeIds.h"

enum EState
{
	Solid = 0,
	Liquid,
	Gas,
	Hot,
	Warm,
	Cold,
	Ghost,
	Attack,
	Done,
	None
};

enum TutFlag
{
	Move = 1,
	Jump = 2,
	ShootS = 4,
	ShootL = 8,
	ShootG = 16,
	PickUp = 32,
	Throwable = 64,
};

namespace Scuttlemeyer
{
	//Forward declaration of GOC class
	class GameObjectComposition;
	typedef GameObjectComposition GOC;

	class GameComponent
	{
	public:
		friend class GameObjectComposition;
		virtual void Unload() {};
		virtual void Initialize() {};
		virtual int ChangeState(EState nState, bool veloReset = false) { return 0; }
		virtual Mid::MessageIdType SendMessage(Message *) { return Mid::None; }
		virtual void Serialize(TextLoader& str) {};
		GOC* GetOwner() { return mBase; }
		ComponentTypeId mTypeId;
		virtual std::string ToString() { return "Component Currently Undefined"; }
		virtual void ToBar() {};
	protected:
		virtual ~GameComponent() {};
	private:
		///Each component has a pointer back to the base owning composition.
		GOC* mBase;
	};
}