/**************************************************************************************************/
/*!
\file	bOptions.h
\par	Project: Coalescence
\date	12/6/2011
\authors
Primary : Lincoln Tosi

\brief
A button that opens the game options (both in pause and main menu).

\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/**************************************************************************************************/
#pragma once

#include "ILogical.h"

namespace Scuttlemeyer
{
	//class Transform;

	class bOptions : public ILogical
	{
	public:
		static void* operator new(unsigned)                                        
		{                                                                          
		   //std::cout << "Message" << "::new(" << sizeof(Message) << ")" << std::endl;            
																					
		   typedef select_allocator<sizeof(bOptions), false>::type alloc_type;  
		   return alloc_type::malloc();      
		}                                                                          
																					
		static void operator delete(void* data)                                    
		{                                                                          
		   //std::cout << "Message" << "::delete(" << sizeof(Message) << ")" << std::endl;         
																					
		   typedef select_allocator<sizeof(bOptions), false>::type alloc_type;  
		   alloc_type::free(data);                                                  
		}

		bOptions();
		virtual void Initialize();
		virtual bool isMouseOn();
		virtual void Triggered();
		virtual Mid::MessageIdType SendMessage(Message* message);
		virtual std::string ToString();
		virtual void ToBar();
		~bOptions();
	private:
		bool selected;
		int bOffset;
		unsigned int pListEnd;
		bool isLeft;
	};
}