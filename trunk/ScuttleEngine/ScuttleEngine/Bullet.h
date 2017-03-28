/**************************************************************************************************/
/*!
\file	Bullet.h
\par	Project: Coalescence
\date	10/3/2011
\authors
Primary  : Matt Sutherlin
Secondary: Lincoln Tosi

\brief
Logic for the bullets fired by the player.

\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/**************************************************************************************************/
#pragma once

#include "ILogical.h"

namespace Scuttlemeyer
{
	class Transform;

	class Bullet : public ILogical
	{
	public:
		static void* operator new(unsigned)                                        
		{                                                                          
		   //std::cout << "Message" << "::new(" << sizeof(Message) << ")" << std::endl;            
																					
		   typedef select_allocator<sizeof(Bullet), false>::type alloc_type;  
		   return alloc_type::malloc();      
		}                                                                          
																					
		static void operator delete(void* data)                                    
		{                                                                          
		   //std::cout << "Message" << "::delete(" << sizeof(Message) << ")" << std::endl;         
																					
		   typedef select_allocator<sizeof(Bullet), false>::type alloc_type;  
		   alloc_type::free(data);                                                  
		}
		Bullet();
		virtual void Initialize();
		virtual int ChangeState(EState nState, bool veloReset = false);
		virtual void Update(float dt);
		virtual Mid::MessageIdType SendMessage(Message* message);
		virtual std::string ToString();
		virtual void ToBar();
		~Bullet();
		bool CheckCollided();
		void SetCollided(bool collided);
	private:
		bool mCollided;
	};
}