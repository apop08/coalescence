/******************************************************************************/
/*!
\file   Transform.h
\author Matt Sutherlin
\par    email: matt.sutherlin\@digipen.edu
\par    DigiPen login: matt.sutherlin
\par    Course: GAM200
\date   9/9/2011
\brief  
A component structure built to hold position, scale, and rotation.
\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/******************************************************************************/

#pragma once

#include "Composition.h"
#include "VMath.h"

namespace Scuttlemeyer
{
	class Transform : public GameComponent
	{
	public:
		static void* operator new(unsigned)                                        
		{                                                                          
		   //std::cout << "Message" << "::new(" << sizeof(Message) << ")" << std::endl;            
																					
		   typedef select_allocator<sizeof(Transform), false>::type alloc_type;  
		   return alloc_type::malloc();      
		}                                                                          
																					
		static void operator delete(void* data)                                    
		{                                                                          
		   //std::cout << "Message" << "::delete(" << sizeof(Message) << ")" << std::endl;         
																					
		   typedef select_allocator<sizeof(Transform), false>::type alloc_type;  
		   alloc_type::free(data);                                                  
		}

		Transform();
		void Serialize(TextLoader& str);
		void SerializeOut(TextSaver& str);
		std::string ToString();
		void ToBar();
	public:
		Vec2 mPosition;
		Vec2 mScale;
		float mRotation;
	};
}