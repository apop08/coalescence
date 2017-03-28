/******************************************************************************/
/*!
\file   Transform.cpp
\author Matt Sutherlin
\par    email: matt.sutherlin\@digipen.edu
\par    DigiPen login: matt.sutherlin
\par    Course: GAM200
\date   9/9/2011
\brief  
A component structure built to hold position, scale, and rotation.
\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/******************************************************************************/

#include "Precompiled.h"
#include "Transform.h"
#include "Graphics.h"
#include "DebugBars.h"

namespace Scuttlemeyer
{
	Transform::Transform()
	{
		mPosition = Vec2(0, 0);
		mScale = Vec2(1, 1);
		mRotation = 0.0f;
	}

	void Transform::Serialize(TextLoader &stream)
	{
		StreamRead(stream, mPosition);
		StreamRead(stream, mScale);
		StreamRead(stream, mRotation);
	}

	void Transform::SerializeOut(TextSaver &stream)
	{
		char temp = '\n';
		StreamWrite(stream, mPosition);
		StreamWrite(stream, temp);
		StreamWrite(stream, mScale);
		StreamWrite(stream, temp);
		StreamWrite(stream, mRotation);
	}

	std::string Transform::ToString()
	{
		char tempStr[400];

		sprintf_s(tempStr, 400, "Transform Component\nPosition: %.2f, %.2f\nScale: %.2f, %.2f\nRotation: %.2f", 
			mPosition.x, mPosition.y, mScale.x, mScale.y, mRotation);
		std::string tempString(tempStr);
		return tempString;
	}

	void Transform::ToBar()
	{
		TwBar* bar = DEBUGBARS->GetBarByName("ObjectProperties");

		TwAddVarRW(bar, "Position", DEBUGBARS->vec2Type, &mPosition, " group='Transform' label='Position' help='Position of the transform.' ");
		TwAddVarRW(bar, "Scale", DEBUGBARS->vec2Type, &mScale, " group='Transform' label='Scale' help='Scale of the transform.' ");
		TwAddVarRW(bar, "Rotation", TW_TYPE_FLOAT, &mRotation, " group='Transform' label='Rotation' help='Rotation of the transform.' ");

		TwDefine(" ObjProp/Transform opened=false ");
	}
}