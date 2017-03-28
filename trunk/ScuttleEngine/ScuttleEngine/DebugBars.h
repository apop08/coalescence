/******************************************************************************/
/*!
\file   DebugBars.h
\author Matt Sutherlin
\par    email: matt.sutherlin\@digipen.edu
\par    DigiPen login: matt.sutherlin
\par    Course: GAM200
\date   11/8/2011
\brief  
System to handle AntTweakBars.
\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/******************************************************************************/

#pragma once

namespace Scuttlemeyer
{
	class DebugBars
	{
	public:
		DebugBars();
		void Initialize();
		TwBar* GetBarByName(std::string barName);
		~DebugBars();
	public:
		TwType vec2Type;
		TwType vec4Type;
	private:
		void CreateTypes();
	private:
		stdext::hash_map<std::string, TwBar*> mBarMap;
	};

	extern DebugBars* DEBUGBARS;
}