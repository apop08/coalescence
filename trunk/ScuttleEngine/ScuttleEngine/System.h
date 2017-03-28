/******************************************************************************/
/*!
\file   Component.h
\author Matt Sutherlin
\par    email: matt.sutherlin\@digipen.edu
\par    DigiPen login: matt.sutherlin
\par    Course: GAM200
\date   7/21/2011
\brief
Interface for engine systems.
\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/******************************************************************************/
#pragma once

#include "Message.h"

namespace Scuttlemeyer
{
	//Abstract engine component interface.  DO NOT CREATE AN ISYSTEM OBJECT!
	class ISystem
	{
	public:
		virtual void Unload() {};
		virtual void Load() {};
		virtual void Free() {};
		virtual void Initialize() {};
		virtual void Update(float dt) = 0;
		virtual std::string GetName() = 0;

		virtual Mid::MessageIdType SendMessage(Message* message) { return Mid::None; }
		virtual void SetFrameTime(double fTime) { mFrameTime = fTime; }
		virtual double& GetFrameTime() { return mFrameTime; }

		virtual ~ISystem() {};
	private:
		double mFrameTime;
	};
}