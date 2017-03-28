/**************************************************************************************************/
/*!
\file	Force.h
\par	Project: Coalescence
\date	12/7/2011
\authors
Primary : Alex Poplawski

\brief
Force classes

\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/**************************************************************************************************/
#pragma once
#include "Composition.h"
#include "VMath.h"

namespace Scuttlemeyer
{
	//force class
	class Force
	{
	public:
		//use memory manager///////////////////////////////////////////////////
		/*****************************************************************************/
		/*!
		\brief
		Memory manager allocation for forces

		\return
		Pointer to the object created
		*/
		/*****************************************************************************/
		static void* operator new(unsigned)                                        
		{                                                                          
		   //std::cout << "Force" << "::new(" << sizeof(Force) << ")" << std::endl;            
																					
		   typedef select_allocator<sizeof(Force), false>::type alloc_type;  
		   return alloc_type::malloc();                                             
		}                                                                          

		/*****************************************************************************/
		/*!
		\brief
		memory manager delete function

		\param data
		pointer to the object to delete
		*/
		/*****************************************************************************/
		static void operator delete(void* data)
		{                                                                          
		   //std::cout << "Force" << "::delete(" << sizeof(Force) << ")" << std::endl;         
																					
		   typedef select_allocator<sizeof(Force), false>::type alloc_type;  
		   alloc_type::free(data);                                                  
		}
		///////////////////////////////////////////////////////////////////////


		//Force constructor
		Force(const float magnitude, const float direction,const float activeTime = cOneFrame,const DWORD flags = 0);//, void* owner);
		//Update Function
		int Update(const float dt, Vec2* velocity,const float mass);
		//Deconstructor
		~Force();
		float mMagnitude;
	private:
		float mDirection;
		float mActiveTime;
		DWORD flag;
	};
}