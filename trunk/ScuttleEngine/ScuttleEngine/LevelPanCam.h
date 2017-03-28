/******************************************************************************/
/*!
\file   LevelPanCam.h
\author Matt Sutherlin
\par    email: matt.sutherlin\@digipen.edu
\par    DigiPen login: matt.sutherlin
\par    Course: GAM200
\date   12/15/2011
\brief  
	Logic control unit for scripted camera.
\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/******************************************************************************/

#pragma once

#include "ILogical.h"
#include "Camera.h"
#include "Engine.h"

namespace Scuttlemeyer
{
	class LevelPanCam : public ILogical
	{
	public:
		LevelPanCam();
		virtual void Initialize();
		virtual void Serialize(TextLoader& stream);
		virtual void Update(float dt);
		virtual Mid::MessageIdType SendMessage(Message* message);
		virtual std::string ToString();
		virtual void ToBar();
		~LevelPanCam();
	private:
		Camera mCamera;
		unsigned mNumPoints;
		unsigned mCurPoint;
		std::vector<Vec2> mPoints;
	};
}
