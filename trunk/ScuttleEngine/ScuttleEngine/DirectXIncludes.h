/******************************************************************************/
/*!
\file   DirectXIncludes.h
\author Matt Sutherlin
\par    email: matt.sutherlin\@digipen.edu
\par    DigiPen login: matt.sutherlin
\par    Course: GAM200
\date   9/9/2011
\brief  
Includes for DirectX 9.
\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/******************************************************************************/

#if defined(DEBUG)|defined(_DEBUG)
#ifndef D3D_DEBUG_INFO
#define D3D_DEBUG_INFO
#endif
#endif

#pragma comment(lib,"DxErr.lib")
#pragma comment(lib,"d3d9.lib")

#if defined(DEBUG)|defined(_DEBUG)
#pragma comment(lib,"d3dx9d.lib")
#endif

#if defined(RELEASE)|defined(_RELEASE)
#pragma comment(lib,"d3dx9.lib")
#endif

#include "d3d9.h"
#include "d3dx9.h"
#include "d3dx9tex.h"