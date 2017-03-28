/******************************************************************************/
/*!
\file   EditorBar.h
\author Matt Sutherlin
\par    email: matt.sutherlin\@digipen.edu
\par    DigiPen login: matt.sutherlin
\par    Course: GAM250
\date   2/2/2012
\brief  
System to handle AntTweakBar for the editor.
\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/******************************************************************************/

#pragma once

namespace Scuttlemeyer
{
	class Camera;

	class EditorBar
	{
	public:
		EditorBar();
		void Initialize();
		TwBar* GetBar();
		~EditorBar();
	public:
		TwType vec2Type;
		TwType eArchetypes;
		std::string mLevelName;
		std::vector<std::string> mArchetypeNames;
		int mCurArchetype;
	private:
		void CreateTypes();
		void LoadArchetypes();
	private:
		TwBar* mEditorBar;
		Camera* mEditCam;
	};

	extern EditorBar* EDITORBAR;
}