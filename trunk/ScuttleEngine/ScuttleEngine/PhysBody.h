/**************************************************************************************************/
/*!
\file	PhysBody.h
\par	Project: Coalescence
\date	12/7/2011
\authors
Primary : Alex Poplawski

\brief
Force class for force based movement inside physics engine
Rigid body physics component for objects that require any physics inside the game

\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/**************************************************************************************************/
#pragma once

#include "Composition.h"
#include "VMath.h"
#include "PhysVars.h"
#include "Force.h"
namespace Scuttlemeyer
{
	class Force;
	//just incase of external flag names
	namespace PhysFlags
	{
		//physics flags
		const DWORD FStaticObj = 00000010;
		const DWORD FStand = 00001000;
		const DWORD FGravity = 00000001;
		const DWORD FUN = 00000100;
		const DWORD HUD = 00010000;
		const DWORD BUL = 00100000;
		const DWORD PLA = 01000000;
		const DWORD LIQ = 010000000;
		const DWORD GAS = 1 << 2;
		const DWORD HELD = 1 << 5;
		const DWORD SWITCH = 1 << 7;
	}

	//macro velocity into a 2d vector
	typedef Vec2 Velocity;
	

	

	//rotational force
	class RotForce
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
																					
		   typedef select_allocator<sizeof(RotForce), false>::type alloc_type;  
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
																					
		   typedef select_allocator<sizeof(RotForce), false>::type alloc_type;  
		   alloc_type::free(data);                                                  
		}
		///////////////////////////////////////////////////////////////////////
		//constructor
		RotForce(const float magnitude,const float direction,const float activeTime,const DWORD flags = 0);//, void* owner);
		
		int Update(const float dt, float* rotVel);
		
		//deconstructor
		~RotForce();
	private:
		float mMagnitude, mDirection;
		float mActiveTime;
		DWORD flag;
	};



	//Need to forward declare Transform to use it here without stupid includes.
	class Transform;

	//rigid body physics class
	class PhysBody : public GameComponent
	{
	public:
		//constructor
		PhysBody();

		void Unload();
		void Initialize();
		virtual void Serialize(TextLoader& stream);
		void Update(const float dt);
		void DebugDraw();
		void AddForce(const float magnitude,const float direction,const DWORD flags = 0,const float activeTime = cOneFrame);
		void AddResponse(const float magnitude,const float direction,const DWORD flags = 0,const float activeTime = cOneFrame);
		void AddForce(Force* a);		
		void AddRotForce(const float magnitude,const float direction,const DWORD flags = 0,const float activeTime = cOneFrame);
		void UpdateForces(const float dt);
		std::string ToString() const;
		void ToBar() const;
		void UpdatePoints(const float dt);
		void ReversePoints(const float dt, int numCol);
		~PhysBody();
		void Resolve(const float dt, int& numCol);
		//NEED TO FIX MOST OF THESE TO PRIVATE
	public:
		Velocity mVelocity;
		DWORD mColFlags;
		DWORD mPhysFlags;
		Transform* mTransform;
		std::vector<Vec2> mPoints;
		float mRotVel;
		float mMass;
		Vec2 mExtents;
		std::list<Force*> mActiveForces;
		std::list<Force*> mResponseForces;
		DWORD mStack;
	private:
		std::list<RotForce*> mActiveRotForces;
		std::vector<Vec2> mCPoints;
	};
}