/******************************************************************************/
/*!
\file   Core.cpp
\author Matt Sutherlin
\author Kevin Verholtz (Gameloop)
\par    email: matt.sutherlin\@digipen.edu
\par    DigiPen login: matt.sutherlin
\par    Course: GAM200
\date   7/25/2011
\brief  
Entrypoint for the program.
\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/******************************************************************************/

#include "Precompiled.h"
#include "Engine.h"
#include "Core.h"
#include "DebugConsole.h"
#include "Graphics.h"
#include "Physics.h"
#include "Audio.h"
#include "GameLogic.h"
#include "Windowsx.h"
#include "GameStateMgr.h"
#include "ParticleManager.h"
#include "TextSerialization.h"
#include "resource.h"
#include "DebugBars.h"

#pragma comment(lib, "AntTweakBar.lib")

using namespace Scuttlemeyer;

//just for fun should be moved
struct Info
{
	int width;
	int height;
};

HCURSOR cursors[5];
int gameState = 2;

LRESULT CALLBACK WinProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
int DebugInit(struct DebugTPar* params, HANDLE* Thread);
int DebugClean(struct DebugTPar* params, HANDLE* Debug);
void LoadFromIni(struct Info*);
//
//  FUNCTION: WinMain()
//
//  PURPOSE: Main used by the Windows API to run Win32 programs.
//
//  COMMENTS:
//
//    This is the replacement for main seen in traditional C/C++ programs.
//    The Windows API will always be looking for a WinMain function, so it
//    can be treated as such.  The function params are a bit different, so if
//    your program requires their usage, the information can be found on MSDN.
//



void EnableMemoryLeakChecking(int breakAlloc=-1)
{
	//Set the leak checking flag
	int tmpDbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
	tmpDbgFlag |= _CRTDBG_LEAK_CHECK_DF;
	_CrtSetDbgFlag(tmpDbgFlag);

	//If a valid break alloc provided set the breakAlloc
	if(breakAlloc!=-1) _CrtSetBreakAlloc( breakAlloc );
}


int WINAPI WinMain(HINSTANCE instance, HINSTANCE hPreviousInstance, LPSTR command_line, int show)
{
	//_CrtMemState s1, s2, s3;
	//_CrtMemCheckpoint( &s1 );
	//_CrtMemDumpStatistics(&s1);
// memory allocations take place here

	timeBeginPeriod(1);
	struct Info iniInfo;
	LoadFromIni(&iniInfo);
	RECT fullWinRect = {0, 0, iniInfo.width, iniInfo.height};
	AdjustWindowRect(&fullWinRect,			//The rectangle for the full size of the window
		WS_OVERLAPPEDWINDOW ^ WS_MINIMIZEBOX ^ WS_MAXIMIZEBOX ^ WS_THICKFRAME,	//The style of the window, which must match what is passed in to CreateWindow below
		FALSE);					//Does this window have a menu?

	WNDCLASS wc;
	HWND hWnd;

	cursors[0] = LoadCursor(instance, MAKEINTRESOURCE(IDC_CURSOR1));
	cursors[1] = LoadCursor(instance, MAKEINTRESOURCE(IDC_CURSOR2));
	cursors[2] = LoadCursor(instance, MAKEINTRESOURCE(IDC_CURSOR3));
	cursors[3] = LoadCursor(instance, MAKEINTRESOURCE(IDC_CURSOR4));
	cursors[4] = LoadCursor(instance, MAKEINTRESOURCE(IDC_CURSOR5));

	wc.style = CS_HREDRAW | CS_VREDRAW;							/*Class style*/
	wc.lpfnWndProc = WinProc;									/*A function pointer which should point to the procedure function. Remember that the procedure function handles the window messages*/
	wc.cbClsExtra = 0;											/*The number of extra bytes you want to allocate for this window class structure. The default value is 0*/
	wc.cbWndExtra = 0;											/*The number of extra bytes you want to allocate for the window instance*/
	wc.hInstance = instance;									/*Instance of the module associated with the window. This is the 1st paramter passed to the WinMain function*/
	wc.hIcon = LoadIcon(instance,  MAKEINTRESOURCE(IDI_ICON1));/*Handle to the icon class which will be displayed on the top left part of the window*/
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);					/*Handle to the cursor class which will be used in this window class*/
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);		/*Handle to the class background brush. It can be a color value*/
	wc.lpszMenuName = NULL;										/*Pointer to a null terminated string which specifies the name of the menu resources used by this window class (if any)*/
	wc.lpszClassName = "Coalescence";						/*String that specifies the window class name. This is needed to create any window based on this window class*/

	RegisterClass(&wc);

	hWnd = CreateWindow(wc.lpszClassName,							/*The class name we chose for the window class*/
		"Coalescence",												/*The window caption*/
		(WS_OVERLAPPEDWINDOW ^ WS_MINIMIZEBOX 
		^ WS_MAXIMIZEBOX ^ WS_THICKFRAME),							/*The style of the window, which determines if the window will have a minimize/maximize buttons, if its frame is thick or not.. */
		(GetSystemMetrics(SM_CXSCREEN) - fullWinRect.right) / 2,	/*The X position of the top left corner of the window. Remember that (0,0) is the top left of the monitor*/
		(GetSystemMetrics(SM_CYSCREEN) - fullWinRect.bottom) / 2,	/*The Y position of the top left corner of the window. Remember that (0,0) is the top left of the monitor*/
		fullWinRect.right - fullWinRect.left,						/*The width of the window*/
		fullWinRect.bottom - fullWinRect.top,						/*The heiht of the window*/
		NULL,														/*Handle to the parent window (in case this was a child window)*/
		NULL,														/*Handle to a menu (In case there is a menu for this window)*/
		instance,													/*Instance of the module associated with the window. This is the 1st paramter passed to the WinMain function*/
		NULL);														/*Pointer to a value sent to the window in the WM_CREATE message*/

	EnableMemoryLeakChecking();

	bool bIsFirstRun = true;
#ifdef BLARG
	DebugBars* barSystem;
#endif

	CoreEngine* engine = new CoreEngine();

	//HANDLE Debug;
	//struct DebugTPar params;

	//assert(DebugInit(&params, &Debug));
	engine->AddSystem(new DebugConsole());

	engine->AddSystem(new GameObjectFactory());

	engine->AddSystem(new Audio());

	engine->AddSystem(new Physics());

	engine->AddSystem(new ParticleManager());

	engine->AddSystem(new GameLogic(cursors));

	Graphics* graphics = new Graphics();
	graphics->SetWindowProperties(hWnd, iniInfo.width, iniInfo.height);
	engine->AddSystem(graphics);

	ShowWindow(hWnd, show);
	UpdateWindow(hWnd);

	GameStateMgrInit(GS_SPLASH);

	while(gGameStateCurr != GS_QUIT)
	{
		// If not restarting, load the gamestate
		if(gGameStateCurr != GS_RESTART){
			//Load function
			engine->Load();

			if (gGameStateCurr == GS_RELOAD)
			{
				if(gGameStatePrev == GS_PLAY)
					gGameStateCurr = gGameStateNext = GS_PLAY;
				if(gGameStatePrev == GS_MENU)
					gGameStateCurr = gGameStateNext = GS_MENU;
			}

			if(bIsFirstRun)
			{
#ifdef BLARG
				barSystem = new DebugBars();
				barSystem->Initialize();
#endif

				bIsFirstRun = false;

				LOGIC->LoadFile();
				LOGIC->LoadFileStuff();
			}
		}
		else
		{
			if(gGameStatePrev == GS_PAUSE)
			{
				gGameStateCurr = gGameStateNext = GS_PLAY;
			}
			else
			{
				gGameStateNext = gGameStateCurr = gGameStatePrev;
			}
		}

		// Initialize the gamestate
		engine->Initialize();//This does both loading and initializing

		while(gGameStateCurr == gGameStateNext)
		{
			engine->GameLoop();

			//------------------------------------------
			//   When going to the pause screen don't
			// unload the level, simply halt it until
			// either the play state is activated
			// (resume game) or the menu state (unload
			// the level and load the main menu)
			//------------------------------------------
			if (gGameStateCurr == GS_PLAY)
			{
				while (gGameStateNext == GS_PAUSE)
				{
					gGameStateCurr = GS_PAUSE;
					engine->Paused();
					if(gGameStateNext == GS_PLAY)
						gGameStateCurr = gGameStateNext;
				}
			}

			// check if forcing the application to quit
			if ((CORE->mBGameActive == false))
				gGameStateNext = GS_QUIT;
		}

		engine->Free();

		//Free here not unload
		//then right here unload
		if(gGameStateNext != GS_RESTART){
			//Unload function
			engine->Unload();
		}

		gGameStatePrev = gGameStateCurr;
		gGameStateCurr = gGameStateNext;
	}

#ifdef BLARG
	delete barSystem;
#endif

	engine->DestroySystems();

	delete engine;

//_CrtMemCheckpoint( &s2 );
//
//if ( _CrtMemDifference( &s3, &s1, &s2) )
//   _CrtMemDumpStatistics( &s3 );
	//assert(DebugClean(&params, &Debug));

	return 0;

	//---------------------------------------------------------------------------
	// Cleanup block.  In particular, the reason I added keep track of hWnd in
	//   the project was to call DestroyWindow here.  This safely deletes the
	//   window in the event that it has not been deleted elsewhere.
	//---------------------------------------------------------------------------
	if (hWnd)
		DestroyWindow(hWnd);

	//return (int) msg.wParam;

	UnregisterClass(wc.lpszClassName, instance);
	timeEndPeriod(1);
	//return (int)msg.wParam;
}

LRESULT CALLBACK WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) 
{
	HDC dc;           /* device context */
	PAINTSTRUCT ps;   /* the paint struct */

#ifdef BLARG
    if(TwEventWin(hWnd, msg, wParam, lParam))
        return 0;   // Event has been handled by AntTweakBar
#endif

	switch (msg)
	{
	//case WM_CHAR:
	//	break;

	case WM_KEYDOWN:
		{
			char tempString[100];
			sprintf_s(tempString, 100, "%i", wParam);
			Message* q = new Message(Mid::KeyDown, 1, new std::string(tempString));
			if(CORE->mConfirmAction == false)
				CORE->EnqueueMessage(q);
		}
		break;

		/* when the window is created */
	case WM_CREATE:
		break;

		/* when the rectangle is drawn */
	case WM_LBUTTONDOWN:
		{
			CORE->MouseInput.LeftButtonDown = true;
			Message* q = new Message(Mid::LBUTTONDOWN, 0);
			if(CORE->mConfirmAction == false)
				CORE->EnqueueMessage(q);
			else
				CORE->EnqueueConfirmMessage(q);
		}
		break;

	case WM_RBUTTONDOWN:
		{
			CORE->MouseInput.RightButtonDown = true;
			Message* q = new Message(Mid::RBUTTONDOWN, 0);
			if(CORE->mConfirmAction == false)
				CORE->EnqueueMessage(q);
		}
		break;

	case WM_LBUTTONUP:
		{
			CORE->MouseInput.LeftButtonDown = false;
			Message* q = new Message(Mid::LBUTTONUP, 0);
			if(CORE->mConfirmAction == false)
				CORE->EnqueueMessage(q);
			else
				CORE->EnqueueConfirmMessage(q);
		}
		break;

	case WM_RBUTTONUP:
		{
			CORE->MouseInput.RightButtonDown = false;
			Message* q = new Message(Mid::RBUTTONUP, 0);
			if(CORE->mConfirmAction == false)
				CORE->EnqueueMessage(q);
		}
		break;

	case WM_MOUSEMOVE:
		{
			CORE->MouseInput.MousePosition = Vec2((float)GET_X_LPARAM(lParam), (float)GET_Y_LPARAM(lParam));
			Message* q = new Message(Mid::MOUSEMOVE, 0);
			if(CORE->mConfirmAction == false)
				CORE->EnqueueMessage(q);
			else
				CORE->EnqueueConfirmMessage(q);
		}
		break;

	case WM_MOUSEWHEEL:
		{
			CORE->MouseInput.MouseScroll = GET_WHEEL_DELTA_WPARAM(wParam);
			Message* q = new Message(Mid::MOUSESCROLL, 0);
			if(CORE->mConfirmAction == false)
				CORE->EnqueueMessage(q);
			else
				CORE->EnqueueConfirmMessage(q);
		}
		break;

	//case WM_SETCURSOR:
	//	{
	//		if(LOWORD(lParam) == HTCLIENT)
	//		{
	//			SetCursor(cursors[LOGIC->mCurCursor]);
	//			return true;
	//		}
	//		else
	//		{
	//			SetCursor(cursors[1]);
	//			return false;
	//		}
	//	}
	//	break;

	case WM_PAINT:
		dc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

		/* When it's time for the window to be closed and removed */
	case WM_DESTROY:
		gameState = 0;
		PostQuitMessage(0);
		break;

	/*case WM_ACTIVATE:
	    if (wParam == WA_INACTIVE)
	    {
		  Message* q = new Message(Mid::INACTIVE, 0);
		  if(CORE->mConfirmAction == false)
			  CORE->EnqueueMessage(q);
	    }
		break;*/

		/* called any time the window is moved */
	case WM_MOVE:
		/* Invalidate the rect to force a redraw */
		InvalidateRect(hWnd, NULL, FALSE);
		break;

	case WM_SYSKEYDOWN:
		{
			//Eat the WM_SYSKEYDOWN message to prevent freezing the game when the alt key is pressed
			switch( wParam )
			{
			case VK_LMENU:
			case VK_RMENU:
				return 0;  // Stop Alt key from stopping the winProc
			case VK_F4:    // Check for Alt F4
				DWORD dwAltKeyMask = ( 1 << 29 );
				if( ( lParam & dwAltKeyMask ) != 0 )
					PostQuitMessage(0);
				return 0;
			}
			return 0;
		}

	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

int DebugInit(struct DebugTPar* params, HANDLE* Thread)
{
	HANDLE mutex;
	int debugPrint = 0;
	DWORD ThreadID;
	mutex = CreateMutex(NULL, FALSE, NULL);
	if(mutex == NULL)
		return FALSE;
	params->_mutex = mutex;
	params->_debugmes = debugPrint;

	*Thread = CreateThread(NULL, 0, DebugThread, params, 0, &ThreadID);
	return TRUE;
}

int DebugClean(struct DebugTPar* params, HANDLE* Debug)
{
	WaitForSingleObject(*Debug, INFINITE);
	CloseHandle(params->_mutex);
	return 1;
}

void LoadFromIni(struct Info* iniInfo)
{
	TextLoader stream;
	if(stream.Open("..//Content//Test//Scuttle.ini"))
	{
		stream.ReadInt(iniInfo->width);
		stream.ReadInt(iniInfo->height);
	}
	else
	{
		iniInfo->width = 1024;
		iniInfo->height = 728;
	}

}