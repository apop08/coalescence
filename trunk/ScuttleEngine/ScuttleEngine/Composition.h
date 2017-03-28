/******************************************************************************/
/*!
\file   Composition.h
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

#include "Message.h"
#include "TextSerialization.h"
#include "Component.h"

namespace Scuttlemeyer
{
	///Game Object Id Type
	typedef unsigned int GOCId;

	///This class is the foundational object for all dynamic objects in the game world. 
	///The GOC is a piece of logical interactive content and the primary mechanism
	///game systems (Graphics, Physics, Etc.) provide functionality and communicate.
	///A GOC can be anything from physical objects like trees, tanks, players to 
	///to logical objects like teams, triggers, or Ais. 
	///GameObject are formed by composing Component objects into the game 
	///object composition. This is usually done by the GameObjectFactory.
	///A Game Object's Lifetime States
	///- Constructed: The game object is constructed but is not initialized. Components
	///	can be added and component data can be adjusted either by serialization or
	///	code.
	///- Initialized: All components are initialized. This is the GOC's normal state.
	///- Dead: The object still exists but it is on the GameObjectFactory's destroy list 
	///and it's GOCId is no longer valid it will be destroyed at the end of the frame.

	//A vector of components pointers is used to store components.
	typedef std::vector<GameComponent*> ComponentArray;

	class GameObjectComposition
	{
	public:
		
		static void* operator new(unsigned)                                        
		{                                                                          
		   //std::cout << "GameObjectComposition" << "::new(" << sizeof(GameObjectComposition) << ")" << std::endl;            
																					
		   typedef select_allocator<sizeof(GameObjectComposition), false>::type alloc_type;  
		   return alloc_type::malloc();                                             
		}                                                                          
																					
		static void operator delete(void* data)                                    
		{                                                                          
		   //std::cout << "GameObjectComposition" << "::delete(" << sizeof(GameObjectComposition) << ")" << std::endl;         
																					
		   typedef select_allocator<sizeof(GameObjectComposition), false>::type alloc_type;  
		   alloc_type::free(data);                                                  
		}

		//Game object factory is the only class allowed to
		//create and destroy game objects.
		friend class GameObjectFactory;

		///Message interface See Message.h
		///Message will be sent to each component on this composition.
		Mid::MessageIdType SendMessage(Message* message);

		///Get a component on this composition by component's typename. 
		///This will return NULL if the component is not found.
		GameComponent* GetComponent(ComponentTypeId typeId);

		///Type safe way of accessing components.
		template<typename type>
		type* GetComponentType(ComponentTypeId typeId);

		///Initialize all the components on the composition.
		///See function for details.
		void Initialize();

		///Correctly destroys an object by adding it to the factories destroy list.
		///Object will be destroy the next time the factory updates.
		void Destroy();

		void ForceDestroy();

		void FadeDestroy(float fadeSpeed);

		///Add an component used only for dynamic composition construction.
		void AddComponent(ComponentTypeId typeId, GameComponent* component);

		///Get the game object's Id
		GOCId GetId(){ return mObjectId; }

		GameComponent* GetComponentByIndex(int index);

		unsigned int GetNumberOfComponents();

		unsigned int static& GetNumberOfGameObjects() { return smNumberOfGameObjects; }

		// Fix this!
		EState state;

		std::string mArchetype;
		std::string mObjectName;
	private:
		//Sorted array of components.
		ComponentArray mComponents;
		typedef ComponentArray::iterator ComponentIt;

		//A unique id for each object used to safely reference 
		//GOCs.
		GOCId mObjectId;

		//The constructor and destructor are private to prevent a user
		//from creating or deleting an object directly they must use the Destroy function

		//FACTORY ONLY: Use CreateEmptyComposition on factory instead
		GameObjectComposition();
		//FACTORY ONLY: Use Destroy instead, factory will call the destructor correctly
		~GameObjectComposition();

		static unsigned int smNumberOfGameObjects;
	
	};

	//A Short name for GameObjectComposition
	typedef GameObjectComposition GOC;

	//A more advanced type safe way of accessing components.
	//Interface becomes Transform* transform = object->has(Transform);
	template<typename type>
	type* GameObjectComposition::GetComponentType(ComponentTypeId typeId)
	{
		return static_cast<type*>(GetComponent(typeId));
	}

#define has(type) GetComponentType<type>(CT_##type)

}
