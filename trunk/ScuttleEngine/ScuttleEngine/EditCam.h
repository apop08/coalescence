/******************************************************************************/
/*!
\file   EditCam.h
\author Matt Sutherlin
\par    email: matt.sutherlin\@digipen.edu
\par    DigiPen login: matt.sutherlin
\par    Course: GAM250
\date   12/05/2011
\brief  
	Logic control unit for the level editor camera.
\copyright 
“All content © 2012 DigiPen (USA) Corporation, all rights reserved.”
*/
/******************************************************************************/

#pragma once

#include "ILogical.h"
#include "Camera.h"

namespace Scuttlemeyer
{
	class EditCam : public ILogical
	{
	public:
		EditCam();
		virtual void Initialize();
		virtual void Update(float dt);
		virtual Mid::MessageIdType SendMessage(Message* message);
		virtual std::string ToString();
		virtual void ToBar();
		~EditCam();
	private:
		Camera mCamera;
	};
}
