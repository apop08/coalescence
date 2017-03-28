/******************************************************************************/
/*!
\file   Message.h
\author Kevin Verholtz (Base implementation)
\author Matt Sutherlin (Variadic list)
\par    email: kevin.verholtz\@digipen.edu
\par    DigiPen login: kevin.verholtz
\par    Course: GAM200
\date   12/8/2011
\brief  
Internal messaging system.
\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/******************************************************************************/

#pragma once

#include "Precompiled.h"
#include "MemoryTemplate.h"
#include <cstdarg>

namespace Scuttlemeyer
{
	namespace Mid
	{
		enum MessageIdType
		{
			None,
			KeyDown,
			INACTIVE,
			LBUTTONDOWN,
			RBUTTONDOWN,
			LBUTTONUP,
			RBUTTONUP,
			MOUSEMOVE,
			MOUSESCROLL,
			UpKey,
			Player,
			DebugCNSL,
			Menu,
			AudioMenu,
			Play,
			Pause,
			Splash,
			Transition,
			Restart,
			Reload,
			Quit,
			Collision,
			Activate,
			Deactivate
		};
	}

	//In the base Message type, id is an enum used by systems to identify worthwhile messages and data is a string pointer
	//that stores whatever data this message is transporting.
	class Message
	{
	public:
		static void* operator new(unsigned)                                        
		{                                                                          
		   //std::cout << "Message" << "::new(" << sizeof(Message) << ")" << std::endl;            
																					
		   typedef select_allocator<sizeof(Message), false>::type alloc_type;  
		   return alloc_type::malloc();      
		}                                                                          
																					
		static void operator delete(void* data)                                    
		{                                                                          
		   //std::cout << "Message" << "::delete(" << sizeof(Message) << ")" << std::endl;         
																					
		   typedef select_allocator<sizeof(Message), false>::type alloc_type;  
		   alloc_type::free(data);                                                  
		}

		Message(Mid::MessageIdType id, int count, ...);
		Mid::MessageIdType mMessageId;
		std::vector<void*> mMessageData;
		int mParamCount;
		unsigned int static GetNumberOfMessages() { return smNumberOfMessages; }
		~Message();
	private:
		static unsigned int smNumberOfMessages;
	};

}