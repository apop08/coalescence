/**************************************************************************************************/
/*!
\file	bVolume.h
\par	Project: Coalescence
\date	12/6/2011
\authors
Primary : Lincoln Tosi

\brief
A button that cycles through the different levels of game volume.

\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/**************************************************************************************************/
#pragma once

#include "ILogical.h"

namespace Scuttlemeyer
{
	//class Transform;

	class bVolume : public ILogical
	{
	public:
		static void* operator new(unsigned)                                        
		{                                                                          
		   //std::cout << "Message" << "::new(" << sizeof(Message) << ")" << std::endl;            
																					
		   typedef select_allocator<sizeof(bVolume), false>::type alloc_type;  
		   return alloc_type::malloc();      
		}                                                                          
																					
		static void operator delete(void* data)                                    
		{                                                                          
		   //std::cout << "Message" << "::delete(" << sizeof(Message) << ")" << std::endl;         
																					
		   typedef select_allocator<sizeof(bVolume), false>::type alloc_type;  
		   alloc_type::free(data);                                                  
		}

		bVolume();
		virtual void Initialize();
		virtual bool isMouseOn();
		virtual void Triggered();
		virtual Mid::MessageIdType SendMessage(Message* message);
		virtual std::string ToString();
		virtual void ToBar();
		~bVolume();
	private:
		bool selected;
		std::string bImage;
	};
}