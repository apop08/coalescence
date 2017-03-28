/******************************************************************************/
/*!
\file   DebugDraw.h
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

#pragma once
#include "VMath.h"

namespace Scuttlemeyer
{
	class Drawer
	{
	public:
		void SetColor(Vec4 color);
		void MoveTo(Vec2 newPos);
		void LineTo(Vec2 newPos);
		void DrawBox(Vec2 center, float size);
		void DrawSegment(Vec2 start, Vec2 end);
		static Drawer Instance;
	private:
		friend class Graphics;
		void Flush();
		struct LineSet
		{
			Vec4 Color;
			unsigned Segments;
		};

		struct LineSegment
		{
			Vec3 A;
			Vec3 B;
		};

		Vec2 WritePosition;
		Vec4 Color;
		unsigned SegmentsDrawn;
		std::vector<LineSet> Sets;
		std::vector<LineSegment> LineSegments;
		void Clear();
	};
}