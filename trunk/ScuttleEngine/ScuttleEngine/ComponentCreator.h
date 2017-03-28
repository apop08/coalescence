/******************************************************************************/
/*!
\file   ComponentCreator.h
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

namespace Scuttlemeyer
{
	//Forward declaration of component
	class GameComponent;

	///Game Component Creator Interface.
	///Used to create components in data driven method in the GameObjectFactory.
	class ComponentCreator
	{
	public:
		ComponentCreator(ComponentTypeId typeId)
			:mTypeId(typeId)
		{
		}

		ComponentTypeId mTypeId;
		///Create the component
		virtual GameComponent* Create()=0;
	};

	///Templated helper class used to make concrete component creators.
	template<typename type>
	class ComponentCreatorType : public ComponentCreator
	{
	public:
		ComponentCreatorType(ComponentTypeId typeId)
			:ComponentCreator(typeId)
		{
		}

		virtual GameComponent* Create()
		{
			return new type();
		}
	};

};

//Register component macro
#define RegisterComponent(type) FACTORY->AddComponentCreator(#type, new ComponentCreatorType<type>(CT_##type));