/******************************************************************************/
/*!
\file   Camera.h
\author Matt Sutherlin
\par    email: matt.sutherlin\@digipen.edu
\par    DigiPen login: matt.sutherlin
\par    Course: GAM200
\date   9/9/2011
\brief  
Camera for the graphics system.
\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/******************************************************************************/

#pragma once

#include "Engine.h"

namespace Scuttlemeyer
{
	class Camera : public GameComponent
	{
	public:
		Camera();
		virtual void Unload();
		virtual void Initialize();
		virtual std::string ToString();
		virtual void ToBar();
		~Camera();
	public:
		Transform* mTransform;
		std::string mName;
	};
}