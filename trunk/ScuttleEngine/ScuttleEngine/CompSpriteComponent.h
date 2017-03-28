/******************************************************************************/
/*!
\file   CompSpriteComponent.h
\author Matt Sutherlin
\par    email: matt.sutherlin\@digipen.edu
\par    DigiPen login: matt.sutherlin
\par    Course: GAM200
\date   9/22/2011
\brief  
Building block component for the CompSprite.
\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/******************************************************************************/

#pragma once

#include "IDrawable.h"
#include "VMath.h"

namespace Scuttlemeyer
{
	//Need to forward declare Transform to use it here without stupid includes.
	class Transform;

	class CompSpriteComp : public IDrawable
	{
	public:
		static void* operator new(unsigned)                                        
		{                                                                          
		   //std::cout << "Message" << "::new(" << sizeof(Message) << ")" << std::endl;            
																					
		   typedef select_allocator<sizeof(CompSpriteComp), false>::type alloc_type;  
		   return alloc_type::malloc();      
		}                                                                          
																					
		static void operator delete(void* data)                                    
		{                                                                          
		   //std::cout << "Message" << "::delete(" << sizeof(Message) << ")" << std::endl;         
																					
		   typedef select_allocator<sizeof(CompSpriteComp), false>::type alloc_type;  
		   alloc_type::free(data);                                                  
		}

		CompSpriteComp() {};
		//virtual void Initialize();
		//virtual void Serialize(TextLoader& stream);
		//virtual void Draw(IDirect3DDevice9* pDevice, ID3DXEffect* shader);
		~CompSpriteComp() {};

	public:
		IDirect3DTexture9* mTexture;
		Vec4 mColor;
		Vec2 mSize;
		Transform* mTransform;
		Transform* mLocalTransform;
	};
}