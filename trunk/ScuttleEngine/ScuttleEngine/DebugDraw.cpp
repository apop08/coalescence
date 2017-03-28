/******************************************************************************/
/*!
\file   DebugDraw.cpp
\author Matt Sutherlin
\par    email: matt.sutherlin\@digipen.edu
\par    DigiPen login: matt.sutherlin
\par    Course: GAM200
\date   7/25/2011
\brief  
Debug draw system.  Based on Chris' sample.
\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/******************************************************************************/

#include "Precompiled.h"
#include "DebugDraw.h"

namespace Scuttlemeyer
{
	Drawer Drawer::Instance;

	void Drawer::MoveTo(Vec2 v)
	{
		WritePosition = v;
	}

	void Drawer::SetColor(Vec4 color)
	{
		Flush();
		Color = color;
	}

	void Drawer::LineTo(Vec2 newPosition)
	{
		LineSegment seg = { WritePosition, newPosition };
		LineSegments.push_back( seg );
		++SegmentsDrawn;
		WritePosition = newPosition;
	}

	void Drawer::Flush()
	{
		if( SegmentsDrawn > 0 )
		{
			LineSet set = { Color, SegmentsDrawn };
			Sets.push_back(set);
			SegmentsDrawn = 0;
		}
	}

	void Drawer::Clear()
	{
		SegmentsDrawn = 0;
		Sets.clear();
		LineSegments.clear();
	}

	void Drawer::DrawSegment(Vec2 start, Vec2 end)
	{
		MoveTo(start);
		LineTo(end);
	}

	void  Drawer::DrawBox(Vec2 center,float size)
	{
		float halfSize = size / 2.0f;
		MoveTo(center + Vec2(halfSize, halfSize));
		LineTo(center + Vec2(halfSize, -halfSize));
		LineTo(center + Vec2(-halfSize, -halfSize));
		LineTo(center + Vec2(-halfSize, halfSize));
		LineTo(center + Vec2(halfSize, halfSize));
	}


}