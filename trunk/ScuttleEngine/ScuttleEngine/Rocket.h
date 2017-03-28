/**************************************************************************************************/
/*!
\file	Rocket.h
\par	Project: Coalescence
\date	4/19/2012
\authors
Primary : Kevin Verholtz

\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/**************************************************************************************************/
#pragma once

#include "ILogical.h"

namespace Scuttlemeyer
{
	class Rocket : public ILogical
	{
	public:
		static void* operator new(unsigned)                                        
		{                                                                          
		   //std::cout << "Message" << "::new(" << sizeof(Message) << ")" << std::endl;            
																					
		   typedef select_allocator<sizeof(Rocket), false>::type alloc_type;  
		   return alloc_type::malloc();      
		}                                                                          
																					
		static void operator delete(void* data)                                    
		{                                                                          
		   //std::cout << "Message" << "::delete(" << sizeof(Message) << ")" << std::endl;         
																					
		   typedef select_allocator<sizeof(Rocket), false>::type alloc_type;  
		   alloc_type::free(data);                                                  
		}
		Rocket();
		virtual void Initialize();
		virtual void Update(float dt);
		virtual Mid::MessageIdType SendMessage(Message* message);
		virtual std::string ToString();
		virtual void ToBar();
		~Rocket();
		int SetCheckCollided();
	private:
		void ClearCollided();
	public:
	private:
		bool rocketset;
		int mCollided;
	};
}