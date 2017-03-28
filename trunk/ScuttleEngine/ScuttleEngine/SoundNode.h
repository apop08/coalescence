/******************************************************************************/
/*!
\file   SoundNode.h
\author Matt Sutherlin
\par    email: matt.sutherlin\@digipen.edu
\par    DigiPen login: matt.sutherlin
\par    Course: GAM250
\date   3/1/2012
\brief  
		Non-positional sound node.
\copyright 
		“All content © 2012 DigiPen (USA) Corporation, all rights reserved.”
*/
/******************************************************************************/

#pragma once

#include "Component.h"
#include "fmod.hpp"

namespace Scuttlemeyer
{
	class Transform;

	class SoundNode : public GameComponent
	{
	public:
		static void* operator new(unsigned)                                        
		{                                                                          
		   //std::cout << "Message" << "::new(" << sizeof(Message) << ")" << std::endl;            
																					
		   typedef select_allocator<sizeof(SoundNode), false>::type alloc_type;  
		   return alloc_type::malloc();      
		}                                                                          
																					
		static void operator delete(void* data)                                    
		{                                                                          
		   //std::cout << "Message" << "::delete(" << sizeof(Message) << ")" << std::endl;         
																					
		   typedef select_allocator<sizeof(SoundNode), false>::type alloc_type;  
		   alloc_type::free(data);                                                  
		}

		SoundNode();
		virtual void Unload();
		virtual void Initialize();
		virtual void Serialize(TextLoader& stream);
		virtual void Start(int num = 0);
		virtual void Stop(int num = 0);
		~SoundNode();
	private:
		std::vector<std::string> mSoundNames;
		std::vector<FMOD::Sound*> mSounds;
		std::vector<FMOD::Channel*> mChannels;
	};
}