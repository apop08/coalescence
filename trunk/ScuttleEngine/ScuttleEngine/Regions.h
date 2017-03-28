/**************************************************************************************************/
/*!
\file	Regions.h
\par	Project: Coalescence
\date	12/7/2011
\authors
Primary : Alex Poplawski

\brief
Region components for the physics engine

\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/**************************************************************************************************/

#pragma once

#include "Composition.h"
#include "VMath.h"

namespace Scuttlemeyer
{	
	//Need to forward declare Transform and PhysBody to use it here without stupid includes.
	class Transform;
	class PhysBody;

	


	class Region : public GameComponent
	{
	public:
		static void* operator new(unsigned)                                        
		{                                                                          
		   //std::cout << "Message" << "::new(" << sizeof(Message) << ")" << std::endl;            
																					
		   typedef select_allocator<sizeof(Region), false>::type alloc_type;  
		   return alloc_type::malloc();      
		}                                                                          
																					
		static void operator delete(void* data)                                    
		{                                                                          
		   //std::cout << "Message" << "::delete(" << sizeof(Message) << ")" << std::endl;         
																					
		   typedef select_allocator<sizeof(Region), false>::type alloc_type;  
		   alloc_type::free(data);                                                  
		}

		Region();
		void Unload();
		void Initialize();
		virtual void Serialize(TextLoader& stream);
		//void DebugDraw();
		void AddForce(PhysBody *b1) const;
		void UpdatePoints();
		std::string ToString() const;
		void DebugDraw();
		~Region();
		DWORD mFlags;
		int GetMaxDistance() { return mMaxDist; }
		float GetPower() { return mPower; }
		void SetPower(float power) { mPower = power; }
	public:
		Transform* mTransform;
		std::vector<Vec2> mPoints;
	private:
		int mMaxDist;
		float mPower, mRot;
		
		Vec2 mNormal;
		Vec2 mExtents;
		std::vector<Vec2> mCPoints;
		
	};
}