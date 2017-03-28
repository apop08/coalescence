/******************************************************************************/
/*!
\file   Camera.cpp
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

#include "Precompiled.h"
#include "Camera.h"
#include "Graphics.h"
#include "DebugBars.h"

namespace Scuttlemeyer
{
	Camera::Camera()
	{
		mTransform = NULL;
		mName = "Camera";
		GRAPHICS->mCurrentCamera = this;
	}

	void Camera::Unload()
	{
		mTransform = NULL;
	}

	void Camera::Initialize()
	{
		mTransform = GetOwner()->has(Transform);
	}

	std::string Camera::ToString()
	{
		return "Graphics Component: Camera";
	}

	void Camera::ToBar()
	{
		TwBar* bar = DEBUGBARS->GetBarByName("ObjectProperties");

		TwAddVarRO(bar, "Graphics Component", TW_TYPE_STDSTRING, &mName, " help='Type of graphics component.' ");
	}

	Camera::~Camera()
	{
		if(GRAPHICS->mCurrentCamera == this)
		{
			GRAPHICS->mCurrentCamera = NULL;
		}
	}
}