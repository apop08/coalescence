/******************************************************************************/
/*!
\file   VertexTypes.h
\author Matt Sutherlin
\par    email: matt.sutherlin\@digipen.edu
\par    DigiPen login: matt.sutherlin
\par    Course: GAM200
\date   11/2/2011
\brief  
Definitions for vertex and line types.
\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/******************************************************************************/

#pragma once

#define VERTEX2D_FVF (D3DFVF_XYZ | D3DFVF_TEX1)
#define LINE_FVF (D3DFVF_XYZ)

namespace Scuttlemeyer
{
	struct Vertex2D
	{
		Vec3 _Pos; // Position of vertex in 3D space even though we need only 2
		float _tu, _tv;
	};
}