/******************************************************************************/
/*!
\file   VMath.h
\author Matt Sutherlin
\par    email: matt.sutherlin\@digipen.edu
\par    DigiPen login: matt.sutherlin
\par    Course: GAM200
\date   9/9/2011
\brief  
Vector math function library.
\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/******************************************************************************/

#pragma once

#include "d3dx9.h"
#include <cmath>
#include "TextSerialization.h"

typedef D3DXVECTOR2 Vec2;
typedef D3DXVECTOR3 Vec3;
typedef D3DXVECTOR4 Vec4;
typedef D3DXMATRIXA16 Mat4;

//Because pie is delicious
const float cDelicious = atan(1.0f) * 4; 

const float cDirRight = 0.0f;
const float cDirUp = cDelicious / 2;
const float cDirLeft = cDelicious;
const float cDirDown = 3 * cDelicious / 2;

namespace Scuttlemeyer
{
	inline float Dot(const Vec2& a, const Vec2& b)
	{
		return a.x * b.x + a.y * b.y;
	}

	inline float LengthSquared(const Vec2& a)
	{
		return a.x * a.x + a.y * a.y;
	}

	inline float Normalize(Vec2& a)
	{
		float len = sqrt(LengthSquared(a));	
		a.x /= len;
		a.y /= len;
		return len;
	}

	inline float DistanceSquared(const Vec2& a, const Vec2& b)
	{
		Vec2 c = b - a;

		return LengthSquared(c);
	}

	inline void StreamRead(TextLoader& stream, Vec2& v)
	{
		StreamRead(stream, v.x);
		StreamRead(stream, v.y);
	}

	inline void StreamRead(TextLoader& stream, Vec3& v)
	{
		StreamRead(stream, v.x);
		StreamRead(stream, v.y);
		StreamRead(stream, v.z);
	}

	inline void StreamRead(TextLoader& stream, Vec4& v)
	{
		StreamRead(stream, v.x);
		StreamRead(stream, v.y);
		StreamRead(stream, v.z);
		StreamRead(stream, v.w);
	}

	inline void StreamWrite(TextSaver& stream, Vec2& v)
	{
		char temp = ' ';
		StreamWrite(stream, v.x);
		StreamWrite(stream, temp);
		StreamWrite(stream, v.y);
	}

	inline void StreamWrite(TextSaver& stream, Vec3& v)
	{
		char temp = ' ';
		StreamWrite(stream, v.x);
		StreamWrite(stream, temp);
		StreamWrite(stream, v.y);
		StreamWrite(stream, temp);
		StreamWrite(stream, v.z);
	}

	inline void StreamWrite(TextSaver& stream, Vec4& v)
	{
		char temp = ' ';
		StreamWrite(stream, v.x);
		StreamWrite(stream, temp);
		StreamWrite(stream, v.y);
		StreamWrite(stream, temp);
		StreamWrite(stream, v.z);
		StreamWrite(stream, temp);
		StreamWrite(stream, v.w);
	}
}