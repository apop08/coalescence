/******************************************************************************/
/*!
\file   EditorBar.cpp
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

#include "Precompiled.h"
#include "EditorBar.h"
#include "VMath.h"

#include "Core.h"

#include "Graphics.h"
#include "GameLogic.h"
#include "Camera.h"
#include "EditCam.h"

namespace Scuttlemeyer
{
	EditorBar* EDITORBAR = NULL;

	EditorBar::EditorBar()
	{
		mEditorBar = TwNewBar("Editor");

		CreateTypes();

		mCurArchetype = 0;

		EDITORBAR = this;
	}

	void EditorBar::CreateTypes()
	{
		TwStructMember vec2Members[] = { 
			{ "X", TW_TYPE_FLOAT, offsetof(Vec2, x), " Step=0.01 "},
			{ "Y", TW_TYPE_FLOAT, offsetof(Vec2, y), " Step=0.01 "}
		};
		vec2Type = TwDefineStruct("VECTOR2D", vec2Members, 2, sizeof(Vec2), NULL, NULL);
	}

	void TW_CALL SetLevel(const void *value, void * clientData)
	{
		const std::string *srcPtr = static_cast<const std::string *>(value);
		EDITORBAR->mLevelName = *srcPtr;
	}

	void TW_CALL GetLevel(void *value, void * clientData)
	{
		std::string *destPtr = static_cast<std::string *>(value);
		TwCopyStdStringToLibrary(*destPtr, EDITORBAR->mLevelName);
	}

	void TW_CALL LoadLevel(void *clientData)
	{
		LOGIC->LoadLevelFile(EDITORBAR->mLevelName);
	}

	void TW_CALL SaveLevel(void *clientData)
	{
		TextSaver stream;
		stream.Open("..//Content//Levels//" + EDITORBAR->mLevelName);
		StreamWrite(stream, GRAPHICS->mZoom);
		char temp = '\n';
		StreamWrite(stream, temp);
		FACTORY->SaveLevel(stream);
	}

	void TW_CALL AddArchetype(void *clientData)
	{
		EState tempState;

		if(EDITORBAR->mArchetypeNames[EDITORBAR->mCurArchetype] == "BlockGas.txt")
		{
			tempState = Gas;
		}
		else if(EDITORBAR->mArchetypeNames[EDITORBAR->mCurArchetype] == "BlockLiquid.txt")
		{
			tempState = Liquid;
		}
		else
		{
			tempState = Solid;
		}

		LOGIC->CreateObjectAt(GRAPHICS->mCurrentCamera->mTransform->mPosition, Vec2(1.f, 1.f), 0.f, tempState, "..//Content//Objects//" + EDITORBAR->mArchetypeNames[EDITORBAR->mCurArchetype]);
		CORE->SetCurObj(FACTORY->GetLastObjectId());
	}

	void EditorBar::LoadArchetypes()
	{
		char *cPath;

		DWORD ret = GetCurrentDirectoryA(0, NULL);

		if(ret == 0)
		{
			return;
		}
		else
		{
			cPath = new char[ret];

			if(!GetCurrentDirectoryA(ret, cPath))
			{
				return;
			}
		}

		std::string path(cPath);

		path += "\\..\\Content\\Objects\\";

		HANDLE hDir;
		WIN32_FIND_DATA fileData;

		char findDir[256];
		strcpy_s(findDir, 256, path.c_str());
		strcat_s(findDir, 256, "*");

		hDir = FindFirstFile(findDir, &fileData);

		char aNames[5000] = {0};

		if(hDir == INVALID_HANDLE_VALUE)
		{
		}
		else do
		{
			std::string fullPath = path + fileData.cFileName;

			if((fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0 &&
				fullPath.rfind(".txt") == fullPath.length() - 4)
			{
				char tempnames[40] = {0};

				for(unsigned i = 0; i < strlen(fileData.cFileName) - 4; ++i)
				{
					tempnames[i] = fileData.cFileName[i];
				}

				mArchetypeNames.push_back(fileData.cFileName);
				strcat_s(aNames, 5000, tempnames);
				strcat_s(aNames, 5000, ",");
			}

		} while (FindNextFile(hDir, &fileData));

		FindClose(hDir);

		delete cPath;

		eArchetypes = TwDefineEnumFromString("Archetypes", aNames);
	}

	void EditorBar::Initialize()
	{
		LoadArchetypes();

		TwDefine(" Editor label='Editor' position='400 600' color='200 200 200' alpha=128 text=light refresh=0.5 ");
		TwDefine(" Editor valueswidth=100 ");
		TwDefine(" Editor size='300 100' ");
		TwDefine(" Editor visible=true ");
		TwDefine(" Editor iconified=false ");
		TwDefine(" Editor iconpos=bottomright ");

		TwAddVarCB(mEditorBar, "Level", TW_TYPE_STDSTRING, SetLevel, GetLevel, NULL, NULL);
		TwAddButton(mEditorBar, "Load", LoadLevel, NULL, NULL);
		TwAddButton(mEditorBar, "Save", SaveLevel, NULL, NULL);
		TwAddVarRW(mEditorBar, "Archetypes", eArchetypes, &mCurArchetype, NULL);
		TwAddButton(mEditorBar, "Add Archetype", AddArchetype, NULL, NULL);

		GOC* temp = LOGIC->CreateObjectAt(LOGIC->mPlayer->has(Transform)->mPosition, Vec2(1.f, 1.f), 0.f, None, "..//Content//Objects//EditorCam.txt", "EditCam");
		mEditCam = temp->has(Camera);
	}

	TwBar* EditorBar::GetBar()
	{
		return mEditorBar;
	}

	EditorBar::~EditorBar()
	{
		TwDeleteBar(mEditorBar);
		mEditCam->GetOwner()->Destroy();
		GRAPHICS->mCurrentCamera = LOGIC->mPlayer->has(Camera);
	}
}