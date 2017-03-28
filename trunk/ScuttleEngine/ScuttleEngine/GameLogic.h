/**************************************************************************************************/
/*!
\file	GameLogic.h
\par	Project: Coalescence
\date	9/15/2011
\authors
Primary  : Lincoln Tosi

\brief
The major implementation of the Game Logic System.

\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/**************************************************************************************************/
#pragma once

#include "Graphics.h"
#include "Engine.h"

#include "Player.h"
#include "Turret.h"
#include "Bullet.h"
#include "Rocket.h"
#include "Block.h"
#include "Switch.h"
#include "BrokenPipe.h"
#include "KillZone.h"
#include "Fan.h"
#include "BiDirectionalFan.h"
#include "BlockWin.h"
#include "EntranceDoor.h"
#include "Static.h"

namespace Scuttlemeyer
{	
	class GameLogic : public ISystem
	{
	public:
		GameLogic(HCURSOR* cursors);
		~GameLogic();
		void Unload();
		void Load();
		void Free();
		void Initialize();
		void Update(float dt);
		virtual std::string GetName(){return "GameLogic";}
		virtual Mid::MessageIdType SendMessage(Message *);
		//------------------------------------------------
		void DamagePlayer(float damageAmount);
		void UpdateHealthBar();
		//------------------------------------------------
		void Splash();
		void Save();
		void LoadFile();
		void LoadFileStuff();
		void LoadLevelFile(const std::string& file);
		void LoadPauseFile(const std::string& file);
		void LoadConfirmQuitFile(const std::string& file);
		GOC* CreateObjectAt(Vec2& position, Vec2& scale, float rotation, EState state, const std::string& file, const std::string& objName = "DefaultObjectName");
		GOC* SpawnObj(std::string temp,double x, double y, double rotation, double scalex, double scaley, int state);
		GOC* SpawnObj(std::string temp,Transform* tempt, int state);
		//------------------------------------------------
		int ProcessCollision(GOC* obj1, GOC* obj2, const float* dt);
	private:
		void SwapState();
		void Menu();
		void Play();
		void Play(const std::string &filename);
		void Pause();
		void Transition();
		//------------------------------------------------
		void LoadSplashFile(const std::string &filename);
		void UnloadSplashFile();
		void LoadMenuFile(const std::string& file);
		void UnLoadPauseFile();
		//------------------------------------------------
		void CreateSplashObjectAt(Vec2& position, Vec2& scale, float rotation, const std::string& file);
		GOC* CreateMenuObjectAt(Vec2& position, Vec2& scale, float rotation, const std::string& file, bool pause = 0);
		//------------------------------------------------
		// Helper functions
		std::string AppendNumberToFile(std::string path, int num);
	public:
		ObjectLinkedList<ILogical> mLogicList;
		ObjectLinkedList<ILogical> mButtonList;
		std::vector<GOC *> PauseObjList;
		Vec2 WorldMousePosition;
		GOC* mPlayer;
		GOC* mHUD;
		GOC* mSplash;
		bool isHolding;
		bool inBlock;
		DWORD fireTimer;
		int curScreen;
		int curMenu;
		int curLevel;
		int splashScreen;
		//-------------------------------
		float mOriginalHudSizeX;
		float mOriginalHudXOffset;
		float mOriginalXScale;
		int nextState;
		DWORD transTime;
		int curTransition;
		std::string mCurInstName;
		bool mBCheckpointActive;
		static const int mMaxLevel = 11;
		bool mBGameEnd;
		bool mBDied;
		int mCurCursor;
		int VOLUME;
		int RESOLUTION;
		bool FULLSCREEN;
		unsigned int pListEnd;
		bool mKevinISAJEWPACABRAMEXICAN;
	private:
		short mScrollValue;
		HCURSOR mCursors[5];
	};

	
	extern GameLogic* LOGIC;
}
