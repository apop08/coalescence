/******************************************************************************/
/*!
\file   Factory.h
\author Kevin Verholtz
\par    email: kevin.verholtz\@digipen.edu
\par    DigiPen login: kevin.verholtz
\par    Course: GAM200
\date   12/8/2011
\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/******************************************************************************/
#pragma once
#include "System.h"
#include "Composition.h"
//#include "VMath.h"

namespace Scuttlemeyer
{
	///Forward Declaration of Component Creator
	class ComponentCreator;

	///Game Object Factory The game object factory creates composition objects 
	///from data streams and manages their lifetimes. As part of controlling the 
	///lifetimes of the GOC it also provides an integer based Id system for safe 
	///referencing of game objects through integer Id Handles.
	class GameObjectFactory : public ISystem
	{
	public:
		GameObjectFactory();
		~GameObjectFactory();

		///Create initialize and Id a GOC from the data file.
		GOC* Create(const std::string& filename);

		///Add a GOC to the destroy list for delayed destruction.
		void Destroy(GOC* gameObject);

		void ForceDestroy(GOC* gameObject);

		void FadeDestroy(GOC* gameObject, float fadeSpeed);

		///Update the factory, destroying dead objects.
		virtual void Update(float dt);

		virtual void Load();
		virtual void Initialize();
		virtual void Free();
		virtual void Unload();

		///Name of the system is factory.
		virtual std::string GetName(){return "Factory";}

		///Message Interface see Message.h
		virtual Mid::MessageIdType SendMessage(Message* message);

		///Destroy all the GOCs in the world. Used for final shutdown.
		void DestroyAllObjects();

		///Create and Id a GOC at runtime. Used to dynamically build GOC.
		///After components have been added call GOC->Initialize().
		GOC* CreateEmptyComposition();

		///Build a composition and serialize from the data file but do not initialize the GOC.
		///Used to create a composition and then adjust its data before initialization
		///see GameObjectComposition::Initialize for details.
		GOC* BuildAndSerialize(const std::string& filename);

		///Id object and store it in the object map.
		void IdGameObject(GOC* gameObject);

		///Add a component creator enabling data driven composition
		void AddComponentCreator(const std::string& name, ComponentCreator* creator);

		///Get the game object with given id. This function will return NULL if
		///the object has been destroyed.
		GOC* GetObjectWithId(GOCId id);

		unsigned GetLastObjectId();

		void SaveLevel(TextSaver &stream);

		void RegisterForMessages(GOC* gameObject);
		void UnregisterForMessages(GOC* gameObject);
	private:
		void DeleteDeferredObjects();
		void DeleteFadedObjects();
	private:
		///Used to incrementally generate unique id's.
		unsigned mLastGameObjectId;

		/////Map of component creator used for data driven composition
		typedef stdext::hash_map<std::string,ComponentCreator*> ComponentMapType;
		ComponentMapType mComponentMap;

		/////Map of GOC to their Ids used for safe referencing of game objects
		typedef std::map<unsigned,GOC*> GameObjectIdMapType;
		GameObjectIdMapType mGameObjectIdMap;

		std::vector<GOC*> mMessageList;

		/////Objects to be deleted
		/////this is a set to prevent problems when a game object
		/////is deleted in multiple locations
		std::set<GOC*> mObjectsToBeDeleted;
		std::set<GOC*> mFadedObjectsToBeDeleted;
	};

	extern GameObjectFactory* FACTORY;
}
