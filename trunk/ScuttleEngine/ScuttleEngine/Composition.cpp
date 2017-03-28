/******************************************************************************/
/*!
\file   Composition.cpp
\author Kevin Verholtz
\par    email: kevin.verholtz\@digipen.edu
\par    DigiPen login: kevin.verholtz
\par    Course: GAM200
\date   12/8/2011
\brief  
Composition of Game Objects.
\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/******************************************************************************/
#include "Precompiled.h"
#include "Composition.h"
#include "Factory.h"
#include <algorithm>

namespace Scuttlemeyer
{
	unsigned int GameObjectComposition::smNumberOfGameObjects = 0;
	
	//Used to sort components using their type Id.
	struct ComponentSorter
	{
		bool operator()(GameComponent* left, GameComponent* right) const
		{
			return left->mTypeId < right->mTypeId;
		}
	};

	/******************************************************************************/
	/*!
	  \brief
		Binary search a sorted array of components.

	  \param components
		components to search

	  \param name
	    name we are looking for

	  \return
	    pointer to Component with given name
	*/
	/******************************************************************************/
	GameComponent* BinaryComponentSearch(ComponentArray& components, ComponentTypeId name)
	{
		size_t begin = 0;
		size_t end = components.size();

		while(begin < end)
		{
			size_t mid = (begin+end) / 2;
			if(components[mid]->mTypeId < name)
				begin = mid + 1;
			else
				end = mid;
		}

		if((begin < components.size()) && (components[begin]->mTypeId == name))
			return components[begin];
		else
			return NULL;
	}
	/******************************************************************************/
	/*!
	  \brief
		Initializes all the Compositions.

	*/
	/******************************************************************************/
	void GameObjectComposition::Initialize()
	{
		//Initialize every component on this composition
		//and set their composition owner. This allows each
		//component to initialize itself separate from its constructor.
		//Which is need for serialization and to allow components to get
		//pointers to each other during initialization.
		for(ComponentIt it = mComponents.begin(); it != mComponents.end(); ++it)
		{	
			(*it)->mBase = this;
			(*it)->Initialize();
		}
	}
	/******************************************************************************/
	/*!
	  \brief
		Constructor for each individual Composition
	*/
	/******************************************************************************/
	GameObjectComposition::GameObjectComposition()
	{
		mObjectId = 0;
		++smNumberOfGameObjects;
	}
	/******************************************************************************/
	/*!
	  \brief
		Destructor for each individual Composition
	*/
	/******************************************************************************/
	GameObjectComposition::~GameObjectComposition()
	{
		//Delete each component using the component's virtual destructor
		//takes care of all resources and memory.
		for(ComponentIt it = mComponents.begin(); it != mComponents.end(); ++it)
			delete *it;

		--smNumberOfGameObjects;
	}
	/******************************************************************************/
	/*!
	  \brief
		Adds a Component with given type

	  \param typeId
	    type of component

	  \param component
	    adds this component to mComponents
	*/
	/******************************************************************************/
	void GameObjectComposition::AddComponent(ComponentTypeId typeId, GameComponent* component)
	{
		//Store the components type Id
		component->mTypeId = typeId;
		mComponents.push_back(component);

		//Sort the component array so binary search can be used to find components quickly.
		std::sort(mComponents.begin(), mComponents.end(), ComponentSorter() );
	}
	/******************************************************************************/
	/*!
	  \brief
		sends all the components message

	  \param message
	    message to send
	*/
	/******************************************************************************/
	Mid::MessageIdType GameObjectComposition::SendMessage(Message* message)
	{
		//When a message is sent to a composition it sends to
		//all of its components
		//for( MapIt it = Map.begin();it!=Map.end();++it)
		for(ComponentIt it = mComponents.begin(); it != mComponents.end(); ++it)
		{
			Mid::MessageIdType m = (*it)->SendMessage(message);

			if(m != Mid::None)
			{
				return m;
			}
		}

		return Mid::None;
	}
	/******************************************************************************/
	/*!
	  \brief
		finds component with given ID

	  \param typeId
	    ID of component we are looking for

	  \return
		pointer to the found GameComponent
	*/
	/******************************************************************************/
	GameComponent* GameObjectComposition::GetComponent(ComponentTypeId typeId)
	{
		return BinaryComponentSearch(mComponents, typeId);
	}
	/******************************************************************************/
	/*!
	  \brief
		gets the component with given index

	  \param index
	    index in array of components location

	  \return
		pointer to the found GameComponent
	*/
	/******************************************************************************/
	GameComponent* GameObjectComposition::GetComponentByIndex(int index)
	{
		return mComponents[index];
	}
	/******************************************************************************/
	/*!
	  \brief
		Gets the number of Components

	  \return
		the number of Components
	*/
	/******************************************************************************/
	unsigned int GameObjectComposition::GetNumberOfComponents()
	{
		return mComponents.size();
	}
	/******************************************************************************/
	/*!
	  \brief
		Destroys this composition
	*/
	/******************************************************************************/
	void GameObjectComposition::Destroy()
	{ 
		//Signal the factory that is object needs to be destroyed
		//this will happen at the end of the frame.
		FACTORY->Destroy(this); 
	}

	void GameObjectComposition::ForceDestroy()
	{
		FACTORY->ForceDestroy(this);
	}

	void GameObjectComposition::FadeDestroy(float fadeSpeed)
	{
		FACTORY->FadeDestroy(this, fadeSpeed);
	}
}
