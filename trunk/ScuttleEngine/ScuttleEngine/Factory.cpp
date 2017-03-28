/******************************************************************************/
/*!
\file   Factory.cpp
\author Kevin Verholtz
\par    email: kevin.verholtz\@digipen.edu
\par    DigiPen login: kevin.verholtz
\par    Course: GAM200
\date   12/8/2011
\brief  
Factory handles the building of Components.
\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/******************************************************************************/

#include "Precompiled.h"
#include "Factory.h"
#include "Composition.h"
#include "ComponentCreator.h"
#include "TextSerialization.h"
#include "DebugBars.h"

#include "Sprite.h"
#include "Particle.h"
#include "Turret.h"
#include "EditCam.h"
#include "Bullet.h"
#include "Rocket.h"

namespace Scuttlemeyer
{
	GameObjectFactory* FACTORY = NULL;

	/******************************************************************************/
	/*!
	  \brief
		Constructor for the Factory
	*/
	/******************************************************************************/
	GameObjectFactory::GameObjectFactory()
	{		
		if(FACTORY!=NULL)
		{
			printf("Factory already created\n");
		}
		FACTORY = this;
		mLastGameObjectId = 0;
	}
	/******************************************************************************/
	/*!
	  \brief
		Destructor for the Factory
	*/
	/******************************************************************************/
	GameObjectFactory::~GameObjectFactory()
	{
		//Delete all component creators
		ComponentMapType::iterator it = mComponentMap.begin();
		for(; it != mComponentMap.end(); ++it)
			delete it->second;
	}
	/******************************************************************************/
	/*!
	  \brief
		Game State Load
	*/
	/******************************************************************************/
	void GameObjectFactory::Load()
	{
		mLastGameObjectId = 0;
	}
	/******************************************************************************/
	/*!
	  \brief
		Reitialize the TwBar
	*/
	/******************************************************************************/
	void GameObjectFactory::Initialize()
	{
#ifdef BLARG
		TwBar* bar = DEBUGBARS->GetBarByName("General");

		TwAddVarRO(bar, "NumGameObj", TW_TYPE_UINT32, &GameObjectComposition::GetNumberOfGameObjects(), " label='Number of Game Objects' help='Current number of game objects.' ");
#endif
	}
	/******************************************************************************/
	/*!
	  \brief
		correctly destroys all objects
	*/
	/******************************************************************************/
	void GameObjectFactory::Free()
	{
		DestroyAllObjects();
	}
	/******************************************************************************/
	/*!
	  \brief
		Nothing is loaded so nothing needs to be unloaded
	*/
	/******************************************************************************/
	void GameObjectFactory::Unload()
	{
		//Nothing is loaded so nothing needs to be unloaded
		//as of now
	}
	
	/******************************************************************************/
	/*!
	  \brief
		Create a Empty Composition
	*/
	/******************************************************************************/
	GOC* GameObjectFactory::CreateEmptyComposition()
	{
		//Create the composition and give it an id
		//this function is used to create compositions
		//programmatically
		GOC* gameObject = new GOC();
		IdGameObject(gameObject);
		return gameObject;	
	}
	/******************************************************************************/
	/*!
	  \brief
		Builds And Serializes a Game Object

	  \param filename
	    file to build and serialize from

	  \return
	    pointer to the new Game Object
	*/
	/******************************************************************************/
	GOC* GameObjectFactory::BuildAndSerialize(const std::string& filename)
	{
		//Open the text file stream serializer
		TextLoader stream;
		bool fileOpened = stream.Open(filename);
		if(!fileOpened)
		{
			printf("Could not open file %s. File does not exist or is protected.\n", filename.c_str());
		}
		std::string componentName;

		//Make sure the stream is valid
		if(stream.IsGood())
		{
			//Create a new game object to hold the components
			GOC* gameObject = new GOC();

			while(stream.IsGood())
			{
				//Read the component's name
				StreamRead(stream, componentName);

				//Find the component's creator
				ComponentMapType::iterator it = mComponentMap.find(componentName);
				if(it == mComponentMap.end())
				{
					printf("Could not find component creator with name %s", componentName.c_str());
				}
				if(it != mComponentMap.end())
				{
				//	//ComponentCreator is an object that creates the component
					ComponentCreator* creator = it->second;

				//	//Create the component by using the interface
					GameComponent* component = creator->Create();

				//	//Serialize the component with current stream (this will move
				//	//the stream to the next component

				//	//Note: this invokes the base case of serialization in which
				//	//the object serializes itself. component->Serialize( stream );
					StreamRead(stream, *component);

				//	//Add the new component to the composition
					gameObject->AddComponent(creator->mTypeId, component);
				}
			}

			//Id and initialize the game object composition
			IdGameObject(gameObject);

			return gameObject;
		}

		return NULL;
	}
	/******************************************************************************/
	/*!
	  \brief
		Gives the Object a specific ID

	  \param gameObject
	    Object that needs a ID
	*/
	/******************************************************************************/
	void GameObjectFactory::IdGameObject(GOC* gameObject)
	{
		//Just increment the last id used. Does not handle 
		//overflow but it would take over 4 billion objects
		//to break
		++mLastGameObjectId;
		gameObject->mObjectId = mLastGameObjectId;

		//Store the game object in the global object id map
		mGameObjectIdMap[mLastGameObjectId] = gameObject;
	}
	/******************************************************************************/
	/*!
	  \brief
		Gets a Object With id

	  \param id
	    Object to search for with given id

	  \return
	    pointer to the Game Object with id
	*/
	/******************************************************************************/
	GOC* GameObjectFactory::GetObjectWithId(GOCId id)
	{
		GameObjectIdMapType::iterator it = mGameObjectIdMap.find(id);
		if(it != mGameObjectIdMap.end())
			return it->second;
		else
			return NULL;
	}

	unsigned GameObjectFactory::GetLastObjectId()
	{
		return mLastGameObjectId;
	}

	void GameObjectFactory::SaveLevel(TextSaver &stream)
	{
		for(GameObjectIdMapType::iterator it = mGameObjectIdMap.begin(); it != mGameObjectIdMap.end(); ++it)
		{
			if(!(it->second->has(Particle)) && !(it->second->has(EditCam)) && !(it->second->has(Bullet)) && !(it->second->has(Rocket)))
			{
				if(it->second->has(Transform))
				{
					char temp = '\n';

					if(it != mGameObjectIdMap.begin())
					{
						StreamWrite(stream, temp);
					}

					StreamWrite(stream, it->second->mObjectName);
					StreamWrite(stream, temp);

					std::vector<std::string> tempVec = stream.split(it->second->mArchetype, '/');

					StreamWrite(stream, tempVec[tempVec.size() - 1]);
					StreamWrite(stream, temp);
					it->second->has(Transform)->SerializeOut(stream);
					StreamWrite(stream, temp);
					int blah = it->second->state;
					StreamWrite(stream, blah);

					if(tempVec[tempVec.size() - 1] == "Turret.txt")
					{
						StreamWrite(stream, temp);
						StreamWrite(stream, it->second->has(Turret)->mFieldOfView);
					}
				}
			}
		}
	}

	/******************************************************************************/
	/*!
	  \brief
		Creates a object from specified file

	  \param filename
	    file to create GameObject from

	  \return
	    pointer to the new Game Object
	*/
	/******************************************************************************/
	GOC* GameObjectFactory::Create(const std::string& filename)
	{
		GOC* goc = BuildAndSerialize(filename);
		if(goc) goc->Initialize();
		return goc;
	}
	/******************************************************************************/
	/*!
	  \brief
		call all objects send message to them

	  \param message
	    message to send to all the Objects
	*/
	/******************************************************************************/
	Mid::MessageIdType GameObjectFactory::SendMessage(Message* message)
	{
		for(unsigned i = 0; i < mMessageList.size(); ++i)
		{
			Mid::MessageIdType m = mMessageList[i]->SendMessage(message);

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
		puts the object on a list to be destroyed at the end of this frame

	  \param gameObject
	    object to destroy
	*/
	/******************************************************************************/
	void GameObjectFactory::Destroy(GOC* gameObject)
	{
		//Add the object to the to be deleted list they will be delete
		//when the factory is updated
		mObjectsToBeDeleted.insert(gameObject);
	}

	void GameObjectFactory::ForceDestroy(GOC* gameObject)
	{
		GameObjectIdMapType::iterator idItr =  mGameObjectIdMap.find(gameObject->mObjectId);
		if((idItr == mGameObjectIdMap.end()))
		{
			printf("Object %d was double deleted or is bad memory.", gameObject->mObjectId);
		}
		if(idItr!= mGameObjectIdMap.end())
		{
			//Delete it and remove its entry in the Id map
			delete gameObject;
			mGameObjectIdMap.erase(idItr);
		}
	}

	void GameObjectFactory::FadeDestroy(GOC* gameObject, float fadeSpeed)
	{
		if(gameObject->has(Sprite))
		{
			gameObject->has(Sprite)->SetFadeMode(-1, fadeSpeed);
			mFadedObjectsToBeDeleted.insert(gameObject);
		}
	}

	/******************************************************************************/
	/*!
	  \brief
		deletes all objects that are on the to be deleted list 
	*/
	/******************************************************************************/
	void GameObjectFactory::DeleteDeferredObjects()
	{
		std::set<GOC*>::iterator it = mObjectsToBeDeleted.begin();
		for(; it != mObjectsToBeDeleted.end(); ++it)
		{
			GOC* gameObject = *it;
			GameObjectIdMapType::iterator idItr =  mGameObjectIdMap.find(gameObject->mObjectId);
			if((idItr == mGameObjectIdMap.end()))
			{
				printf("Object %d was double deleted or is bad memory.", gameObject->mObjectId);
			}
			if(idItr!= mGameObjectIdMap.end())
			{
				//Delete it and remove its entry in the Id map
				delete gameObject;
				mGameObjectIdMap.erase(idItr);
			}
		}
		//All objects to be delete have been deleted
		mObjectsToBeDeleted.clear();
	}

	void GameObjectFactory::DeleteFadedObjects()
	{
		std::set<GOC*>::iterator it = mFadedObjectsToBeDeleted.begin();
		for(; it != mFadedObjectsToBeDeleted.end();)
		{
			if((*it)->has(Sprite)->mColor.w == 0.f)
			{
				GOC* gameObject = *it;
				GameObjectIdMapType::iterator idItr =  mGameObjectIdMap.find(gameObject->mObjectId);
				if((idItr == mGameObjectIdMap.end()))
				{
					++it;
					printf("Object %d was double deleted or is bad memory.", gameObject->mObjectId);
				}
				if(idItr!= mGameObjectIdMap.end())
				{
					//Delete it and remove its entry in the Id map
					delete gameObject;
					mGameObjectIdMap.erase(idItr);
					std::set<GOC*>::iterator it2 = it;
					++it;
					mFadedObjectsToBeDeleted.erase(it2);
				}
				else
				{
					++it;
				}
			}
			else
			{
				++it;
			}
		}
	}

	/******************************************************************************/
	/*!
	  \brief
		delete all objects in the ObjectsToBeDeleted list 
	*/
	/******************************************************************************/
	void GameObjectFactory::Update(float dt)
	{
		//Delete all objects in the ObjectsToBeDeleted list 
		DeleteDeferredObjects();
		DeleteFadedObjects();
	}
	/******************************************************************************/
	/*!
	  \brief
		Bind the creator for this component with the component name
 
	*/
	/******************************************************************************/
	//Bind the creator for this component with the component name
	void GameObjectFactory::AddComponentCreator(const std::string& name, ComponentCreator* creator)
	{
		mComponentMap[name] = creator;
	}

	void GameObjectFactory::RegisterForMessages(GOC* gameObject)
	{
		mMessageList.push_back(gameObject);
	}

	void GameObjectFactory::UnregisterForMessages(GOC* gameObject)
	{
		for(std::vector<GOC*>::iterator it = mMessageList.begin(); it != mMessageList.end(); ++it)
		{
			if(*it == gameObject)
			{
				mMessageList.erase(it);

				return;
			}
		}
	}

	/******************************************************************************/
	/*!
	  \brief
		Clean up the game world
 
	*/
	/******************************************************************************/
	void GameObjectFactory::DestroyAllObjects()
	{
		GameObjectIdMapType::iterator it =  mGameObjectIdMap.begin();
		for(; it != mGameObjectIdMap.end(); ++it)
			delete it->second;

		mGameObjectIdMap.clear();
		mFadedObjectsToBeDeleted.clear();
		mObjectsToBeDeleted.clear();
		mLastGameObjectId = 0;
	}
}