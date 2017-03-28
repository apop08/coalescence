/******************************************************************************/
/*!
\file   DebugBars.cpp
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

#include "Precompiled.h"
#include "DebugBars.h"
#include "VMath.h"

namespace Scuttlemeyer
{
	DebugBars* DEBUGBARS = NULL;

	DebugBars::DebugBars()
	{
		mBarMap.insert(std::pair<std::string, TwBar*>("General", TwNewBar("General")));
		mBarMap.insert(std::pair<std::string, TwBar*>("FrameTimes", TwNewBar("FrameTimes")));
		mBarMap.insert(std::pair<std::string, TwBar*>("ObjectProperties", TwNewBar("ObjProp")));

		CreateTypes();

		DEBUGBARS = this;
	}

	void DebugBars::CreateTypes()
	{
		TwStructMember vec2Members[] = { 
			{ "X", TW_TYPE_FLOAT, offsetof(Vec2, x), " Step=0.01 "},
			{ "Y", TW_TYPE_FLOAT, offsetof(Vec2, y), " Step=0.01 "}
		};
		vec2Type = TwDefineStruct("VECTOR2D", vec2Members, 2, sizeof(Vec2), NULL, NULL);

		TwStructMember vec4Members[] = { 
			{ "R", TW_TYPE_FLOAT, offsetof(Vec4, x), " Step=0.01 "},
			{ "G", TW_TYPE_FLOAT, offsetof(Vec4, y), " Step=0.01 "},
			{ "B", TW_TYPE_FLOAT, offsetof(Vec4, z), " Step=0.01 "},
			{ "A", TW_TYPE_FLOAT, offsetof(Vec4, w), " Step=0.01 "},
		};
		vec4Type = TwDefineStruct("VECTOR4D", vec4Members, 4, sizeof(Vec4), NULL, NULL);
	}

	void DebugBars::Initialize()
	{
			TwDefine(" GLOBAL help='Debug information and level editing tools for ScuttleEngine.' contained=true ");
			TwDefine(" GLOBAL iconpos=bottomright ");
			TwDefine(" GLOBAL fontsize=1 ");

			TwDefine(" General label='General Properties' position='0 0' color='200 200 200' alpha=128 text=light refresh=0.5 ");
			TwDefine(" General valueswidth=40 ");
			TwDefine(" General size='190 95' ");
			TwDefine(" General visible=true ");
			TwDefine(" General iconified=true ");
			TwDefine(" General iconpos=bottomright ");

			TwDefine(" FrameTimes label='Frame Times' position='876 0' color='200 200 200' alpha=128 text=light refresh=0.5 ");
			TwDefine(" FrameTimes valueswidth=50 ");
			TwDefine(" FrameTimes size='150 120' ");
			TwDefine(" FrameTimes visible=true ");
			TwDefine(" FrameTimes iconified=true ");
			TwDefine(" FrameTimes iconpos=bottomright ");

			TwDefine(" ObjProp label='Object Properties' position='0 250' color='200 200 200' alpha=128 text=light refresh=0.5 ");
			TwDefine(" ObjProp valueswidth=70 ");
			TwDefine(" ObjProp size='190 200' ");
			TwDefine(" ObjProp visible=true ");
			TwDefine(" ObjProp iconified=true ");
			TwDefine(" ObjProp iconpos=bottomright ");
	}

	TwBar* DebugBars::GetBarByName(std::string barName)
	{
		stdext::hash_map<std::string, TwBar*>::iterator it = mBarMap.find(barName);

		TwBar* returnBar = (it != mBarMap.end()) ? it->second : NULL;

		return returnBar;
	}

	DebugBars::~DebugBars()
	{
		TwTerminate();
	}
}