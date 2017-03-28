/**************************************************************************************************/
/*!
\file	Block.h
\par	Project: Coalescence
\date	10/26/2011
\authors
Primary  : Lincoln Tosi

\brief
The logic for the block states (sollid, gel, and gas).

\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/**************************************************************************************************/
#pragma once

#include "ILogical.h"
#include "VMath.h"
#include "PhysBody.h"
#include "Transform.h"
#include "CompSprite.h"
#include "Sprite.h"

namespace Scuttlemeyer
{
	typedef struct Sticker
	{
		GOC* anchor;
		Vec2 offset;
	} Sticker;

	class Block : public ILogical
	{
	public:
		static void* operator new(unsigned)                                        
		{                                                                          
		   //std::cout << "Message" << "::new(" << sizeof(Message) << ")" << std::endl;            
																					
		   typedef select_allocator<sizeof(Block), false>::type alloc_type;  
		   return alloc_type::malloc();      
		}                                                                          
																					
		static void operator delete(void* data)                                    
		{                                                                          
		   //std::cout << "Message" << "::delete(" << sizeof(Message) << ")" << std::endl;         
																					
		   typedef select_allocator<sizeof(Block), false>::type alloc_type;  
		   alloc_type::free(data);                                                  
		}

		Block();
		virtual void Initialize();
		virtual int ChangeState(EState nState, bool veloReset = false);
		virtual void Update(float dt);
		virtual Mid::MessageIdType SendMessage(Message* message);
		virtual std::string ToString();
		virtual void ToBar();
		~Block();
		void Stick(GOC* target, bool notAgain = false);
		void ResetVelocity();
		bool isFixed();
	public:
		bool mHolding;
		bool mDropSound;
		bool mDropSoundFlag;
	private:
		bool isMouseOn();
		bool isWithinRange();
		void Grab();
		void Throw(float force);
		void Release();
		void UnStick(GOC* target);
		void UnStickAll();
		bool inWall();
		bool CheckExtents(GOC* parent);
		bool UpdateStuckList();
		void SwapAnchor(GOC* target);
	private:
		std::vector<Sticker> stuckList;

		GOC* mParent;
		CompSprite* pDrawable;
		Transform* pTransform;
		Transform* mTransform;
		PhysBody* mPhysics;
		Sprite* mSprite;
		
		Vec2 maxExtents;
		Vec2 minExtents;
		bool mSelected;
		bool mFixed;
		bool mStateChanged;
	};
}