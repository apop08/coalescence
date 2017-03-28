//Co Auther
#pragma once

#include "Engine.h"
#include "TextSerialization.h"


namespace Scuttlemeyer
{
	
	
	//because i can, got a problem?
	//also used for constant forces like gravity
	const float cTInfinity = -2000;

	class PhysBody;
	class Region;
	class Force;

	class Physics : public ISystem
	{
	public:
		Physics();
		void Unload();
		void Load();
		void Free();
		void Initialize();
		void Update(float dt);
		virtual std::string GetName() { return "Physics"; }
		Mid::MessageIdType SendMessage(Message *message);
		void CheckCol(PhysBody* B1, PhysBody* B2,const float dt);
		Vec2* CheckColHeld(PhysBody* B1, PhysBody* B2,const float dt);
		Vec2* CheckBlock(PhysBody* B1, const float dt);
		void CheckRegion(PhysBody* B1, Region* B2,const float dt);
		Vec2* SAT(PhysBody* B1, PhysBody* B2,const float dt);
		Vec2* SATR(PhysBody* B1, Region* B2,const float dt);
		void ApplyCol(PhysBody* B1, PhysBody* B2,const float dt);
		void ApplyCol(PhysBody* B1, PhysBody* B2,const float dt,const Vec2* temp);
		bool RecCol(Vec2 &Rect0, float Width0, float Height0, Vec2 &Rect1, float Width1, float Height1);
		int Side(Vec2 &Rect0, float Width0, float Height0, Vec2 &Rect1, float Width1, float Height1);

		void DebugDraw();
		void DebugDrawR();
		~Physics();
	public:
		int mForces;
		int mRotForces;
		std::list<Region*> mRegionsList;
		std::list<PhysBody*> mPhysBodyList;
	
		std::list<PhysBody*> mCollidableList;
	
		std::list<PhysBody*> mStaticList;
		std::list<PhysBody*> mNonStaticList;
	};

	extern Physics* PHYSICS;
}