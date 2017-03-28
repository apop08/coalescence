/******************************************************************************/
/*!
\file   Graphics.cpp
\author Matt Sutherlin
\author Kevin Verholtz (Reload routines)
\par    email: matt.sutherlin\@digipen.edu
\par    DigiPen login: matt.sutherlin
\par    Course: GAM200
\date   7/20/2011
\brief  
DirectX based graphics system.
\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/******************************************************************************/
#include "Precompiled.h"
#include "Graphics.h"
#include "QuadBatcher.h"
#include "VertexTypes.h"
#include "GameLogic.h"
#include "ComponentCreator.h"
#include "Camera.h"
#include "Core.h"
#include "GameStateMgr.h"

#include "DebugConsole.h"
#include "DebugDraw.h"
#include "DebugBars.h"
#include "EditorBar.h"

#define SAFE_RELEASE(x) if (x) { x->Release(); x = NULL; }

namespace Scuttlemeyer
{
	Graphics* GRAPHICS = NULL;

	/*****************************************************************************/
	/*!
	\brief
		Graphics constructor.
	*/
	/*****************************************************************************/
	Graphics::Graphics()
	{
		mD3D = NULL;
		mDevice = NULL;
		dwElapsed = 0;
		mFps = 0;
		mInit = true;
		mCurrentShader = 0;
		mCameraOffset = 0.f;
		mZoom = 1.f;

		mBatcher = new QuadBatcher();

		RegisterComponent(Camera);
		RegisterComponent(Sprite);
		RegisterComponent(CompSprite);

		GRAPHICS = this;
	}

	/*****************************************************************************/
	/*!
	\brief
		Unload function for the GSM.
	*/
	/*****************************************************************************/
	void Graphics::Unload()
	{
#ifdef BLARG
		Message* q = new Message(Mid::DebugCNSL, 1, new std::string("Graphics Unload."));
		CORE->EnqueueMessage(q);
#endif

		//GRAPHICS->mShaders[1]->Release();
		//GRAPHICS->mShaders[0]->Release();
		//GRAPHICS->mTextureMap.clear();
		//GRAPHICS->mDefTexture->Release();
	}

	/*****************************************************************************/
	/*!
	\brief
		Load function for the GSM.  Has a run once routine for creating the device.
	*/
	/*****************************************************************************/
	void Graphics::Load()
	{
		//First time only need to be done once
		if(mInit == true)
		{
			mInit = false;

			mD3D = Direct3DCreate9(D3D_SDK_VERSION);
			if(mD3D == NULL)
			{
#ifdef BLARG
				Message* q = new Message(Mid::DebugCNSL, 1, new std::string("Graphics failed to create interface!"));
				CORE->EnqueueMessage(q);
#endif
			}

			D3DDISPLAYMODE d3ddm;
			if(FAILED(mD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm)))
			{
#ifdef BLARG
				Message* q = new Message(Mid::DebugCNSL, 1, new std::string("Graphics failed to get display mode!"));
				CORE->EnqueueMessage(q);
#endif
			}

			HRESULT hr;
			if(FAILED(hr = mD3D->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, 
				d3ddm.Format, D3DUSAGE_DEPTHSTENCIL,
				D3DRTYPE_SURFACE, D3DFMT_D16)))
			{
				if(hr == D3DERR_NOTAVAILABLE)
				{
#ifdef BLARG
					Message* q = new Message(Mid::DebugCNSL, 1, new std::string("Graphics failed to get display format!"));
					CORE->EnqueueMessage(q);
#endif
				}
			}

			D3DCAPS9 d3dCaps;
			if(FAILED(mD3D->GetDeviceCaps(D3DADAPTER_DEFAULT, 
				D3DDEVTYPE_HAL, &d3dCaps)))
			{
#ifdef BLARG
				Message* q = new Message(Mid::DebugCNSL, 1, new std::string("Graphics failed to get graphics card capabilities"));
				CORE->EnqueueMessage(q);
#endif
			}

			DWORD dwBehaviorFlags = 0;
			if(d3dCaps.VertexProcessingCaps != 0)
			{
				dwBehaviorFlags |= D3DCREATE_HARDWARE_VERTEXPROCESSING;

#ifdef BLARG
				Message* q = new Message(Mid::DebugCNSL, 1, new std::string("Graphics set to hardware vertex processing mode."));
				CORE->EnqueueMessage(q);
#endif
			}
			else
			{
				dwBehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;

#ifdef BLARG
				Message* q = new Message(Mid::DebugCNSL, 1, new std::string("Graphics set to software vertex processing mode."));
				CORE->EnqueueMessage(q);
#endif
			}

			memset(&mD3DPP, 0, sizeof(mD3DPP));

			mD3DPP.BackBufferWidth			= mScreenWidth; 
			mD3DPP.BackBufferHeight			= mScreenHeight;
			mD3DPP.hDeviceWindow			= mHWnd;
			mD3DPP.SwapEffect				= D3DSWAPEFFECT_DISCARD;  // draw full screen every time but automatically does back buffer
			mD3DPP.Windowed					= TRUE;
			mD3DPP.BackBufferFormat			= D3DFMT_UNKNOWN;  //may need to be changed if we use full screen
			mD3DPP.PresentationInterval		= D3DPRESENT_INTERVAL_DEFAULT;

			mWidthRatio = (mScreenWidth * mZoom) / mDefResX;
			mHeightRatio = (mScreenHeight * mZoom) / mDefResY;
			D3DXMatrixScaling(&mResolutionScaleMatrix, mWidthRatio, mHeightRatio, 1.0f);
			D3DXMatrixOrthoLH(&mMatProj, (float)mScreenWidth , (float)mScreenHeight , 1.0f, 100.0f);

			if(FAILED(mD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, mHWnd,
				dwBehaviorFlags, &mD3DPP, &mDevice)))
			{
#ifdef BLARG
				Message* q = new Message(Mid::DebugCNSL, 1, new std::string("Graphics failed to create a graphics device!"));
				CORE->EnqueueMessage(q);
#endif
			}

			//Turning off lights
			mDevice->SetRenderState(D3DRS_LIGHTING, FALSE); 
			//Disregard culling
			mDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
			//Alpha Blending
			mDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			//Get Alpha from textures
			mDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			//Get the inverse alpha from textures
			mDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

			mBatcher->Initialize(mDevice);

#ifdef BLARG
			//I need to move this shit soon.
			TwInit(TW_DIRECT3D9, mDevice);
#endif

			LoadFonts();
			LoadAssets(); // CHANGE   needs to not hardcode level and move to comment bellow


			///HERE
			EnumerateDisplayModes();
			if(!ChooseDisplayMode())
				exit(-1);///YOU DONT HAVE ANY 4/3 resolution
		}

		//LoadAssets(); //HERE
#ifdef BLARG
		Message* q = new Message(Mid::DebugCNSL, 1, new std::string("Graphics Load."));
		CORE->EnqueueMessage(q);
#endif
	}

	/*****************************************************************************/
	/*!
	\brief
		Free function for the GSM.
	*/
	/*****************************************************************************/
	void Graphics::Free()
	{
#ifdef BLARG
		Message* q = new Message(Mid::DebugCNSL, 1, new std::string("Graphics Free."));
		CORE->EnqueueMessage(q);
#endif
	}

	/*****************************************************************************/
	/*!
	\brief
		Function to find all supported video modes of the current hardware.
	*/
	/*****************************************************************************/
	void Graphics::EnumerateDisplayModes()
	{
		UINT modeCount = mD3D->GetAdapterModeCount(D3DADAPTER_DEFAULT, D3DFMT_X8R8G8B8);
		mDisplayModes.reserve(modeCount);
		for(UINT i = 0; i < modeCount; ++i)
		{
			D3DDISPLAYMODE mode;
			FAILED(mD3D->EnumAdapterModes(D3DADAPTER_DEFAULT, D3DFMT_X8R8G8B8, i, &mode));
			mDisplayModes.push_back(mode);
		}
	}

	/*****************************************************************************/
	/*!
	\brief
		Function to pick resolution modes from supported modes.
	*/
	/*****************************************************************************/
	bool Graphics::ChooseDisplayMode()
	{
		unsigned bestMode = 0, minDiff = -1;
		UINT topWidth = mDisplayModes[mDisplayModes.size() - 1].Width;
		UINT topHeight = mDisplayModes[mDisplayModes.size() - 1].Height;

		for(unsigned i = mDisplayModes.size() - 1; i > 0 ; --i)
		{
			D3DDISPLAYMODE mode = mDisplayModes[i];

			if(((float)mode.Width/mode.Height) == 4.f/3.f)
			{
				if(mode.RefreshRate < 60)
				{
					if((mDisplayModes[i - 1].RefreshRate == 60)
						&& (mDisplayModes[i - 1].Height == mode.Height) 
						&& (mDisplayModes[i - 1].Width == mode.Width))
					{
						--i;
						mode = mDisplayModes[i];
					}
				}

				mMaxRes.Width = mode.Width;
				mMaxRes.Height = mode.Height;
				mMaxRes.RefreshRate = mode.RefreshRate;
				mMaxRes.Format = mode.Format;

				if((mode.Width == topWidth || mode.Height == topHeight) 
					&& (mode.Width > 1024 && mode.Height > 768))
				{
					for(unsigned j = i - 1; j > 0; --j)
					{
						mode = mDisplayModes[j];

						if(((float)mode.Width / mode.Height) == 4.f / 3.f)
						{
							if(mode.Width != mMaxRes.Width && mode.Height != mMaxRes.Height)
							{
								if(mode.RefreshRate < 60)
								{
									if((mDisplayModes[j - 1].RefreshRate == 60) 
										&& (mDisplayModes[j - 1].Height == mode.Height) 
										&& (mDisplayModes[j - 1].Width == mode.Width))
									{
										--j;
										mode = mDisplayModes[j];
									}
								}

								mMaxResWindowed.Width = mode.Width;
								mMaxResWindowed.Height = mode.Height;
								mMaxResWindowed.RefreshRate = mode.RefreshRate;
								mMaxResWindowed.Format = mode.Format;

								return true;
							}
						}
					}
				}
				else
				{
					mMaxResWindowed.Width = mode.Width;
					mMaxResWindowed.Height = mode.Height;
					mMaxResWindowed.RefreshRate = mode.RefreshRate;
					mMaxResWindowed.Format = mode.Format;

					return true;
				}
			}
		}

		return false;
	}

	/*****************************************************************************/
	/*!
	\brief
		Callback function for ATB to setup the level editor bars.
	*/
	/*****************************************************************************/
	void TW_CALL SetEditor(const void *value, void *clientData)
	{
		CORE->mBEditorActive = *(const bool *)value;

		if(CORE->mBEditorActive)
		{
			new EditorBar();
			EDITORBAR->Initialize();
		}
		else
		{
			delete EDITORBAR;
		}
	}

	/*****************************************************************************/
	/*!
	\brief
		Callback function for ATB to set the state of the editor.
	*/
	/*****************************************************************************/
	void TW_CALL GetEditor(void *value, void *clientData)
	{
		*(bool *)value = CORE->mBEditorActive;
	}

	/*****************************************************************************/
	/*!
	\brief
		This function mainly initializes values for ATB debug drawing.
	*/
	/*****************************************************************************/
	void Graphics::Initialize()
	{
#ifdef BLARG
		TwBar* bar = DEBUGBARS->GetBarByName("General");

		TwAddVarRO(bar, "FPS", TW_TYPE_FLOAT, &mFps, " label='FPS' help='Current Game FPS.' ");
		TwAddVarRO(bar, "Width", TW_TYPE_INT32, &mScreenWidth, " label='Window Width' help='Current graphics window width.' ");
		TwAddVarRO(bar, "Height", TW_TYPE_INT32, &mScreenHeight, " label='Window Height' help='Current graphics window height.' ");
		TwAddVarCB(bar, "Editor", TW_TYPE_BOOLCPP, SetEditor, GetEditor, NULL, NULL);

		Message* q = new Message(Mid::DebugCNSL, 1, new std::string("Graphics Init."));
		CORE->EnqueueMessage(q);
#endif
	}

	/*****************************************************************************/
	/*!
	\brief
		Function to load textures and shaders to memory.
	*/
	/*****************************************************************************/
	void Graphics::LoadAssets()
	{
		TextLoader stream;

		stream.Open("..//Content//Levels//GraphicsList.txt");
		int curShader = 0;

		LoadDefaultText();

		IDirect3DTexture9* tempTexture;
		std::string filename2("..//Content//Textures//initload.png");
		D3DXCreateTextureFromFile(mDevice, filename2.c_str(), &tempTexture);

		LoadEffect(curShader, "..//Content//Shaders//Basic.fx");
		++curShader;

		float curFrame = 0.f;
		float maxFrame = 4.f;
		int timer = 0;
		int maxTimer = 10;

		while(stream.IsGood())
		{
			HRESULT lostDeviceCheck = mDevice->TestCooperativeLevel();

			if(lostDeviceCheck == D3DERR_DEVICELOST)
			{
				Sleep(20);
				return;
			}
			else if(lostDeviceCheck == D3DERR_DEVICENOTRESET)
			{
				HandleLostDevice();
				return;
			}
			else if(lostDeviceCheck == D3D_OK)
			{
				mDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 0.0f, 0);

				if(SUCCEEDED(mDevice->BeginScene()))
				{
					Vec3 camPos(0.f, 0.f, -10.0f), 
						camLookAt(0.f, 0.f, 0.0f), 
						camUp(0.0f, 1.0f, 0.0f);

					D3DXMatrixLookAtLH(&mMatView, &camPos, &camLookAt, &camUp);

					mViewProjMatrix = mMatView * mMatProj * mResolutionScaleMatrix;

					//Transform the sprite.
					Mat4 matSprite, matTranslate, matRotate;

					//First, scale it to the proper width and height.
					D3DXMatrixScaling(&matSprite, 1024.f, 768.f, 1.0f);

					//Then, rotate it to the proper angle.
					D3DXMatrixRotationZ(&matRotate, 0.f);
					D3DXMatrixMultiply(&matSprite, &matSprite, &matRotate);

					//Finally, move it to the proper location (note the floor functions on the positions).
					D3DXMatrixTranslation(&matTranslate, 0.f, 0.f, 0.0f);
					D3DXMatrixMultiply(&matSprite, &matSprite, &matTranslate);

					Vec4 color(1.f, 1.f, 1.f, 1.f);

					++timer;

					if(timer >= maxTimer)
					{
						curFrame = curFrame + 1 >= maxFrame ? 0.0f : curFrame + 1;

						timer = 1;
					}

					mBatcher->Draw(tempTexture, mShaders[mCurrentShader], 0, 6, &matSprite, curFrame, maxFrame, 0.f, 1.f, &mViewProjMatrix, color);

					mBatcher->Flush();

					mDevice->EndScene();
				}

				mDevice->Present(NULL, NULL, NULL, NULL);
			}

			std::string tempType;
			std::string tempPath;

			//Get the filepath and type from the list file.
			StreamRead(stream, tempType);
			StreamRead(stream, tempPath);

			//If the type is Texture, we add it to the texture map.
			if(tempType == "Texture")
			{
				LoadTexture(tempPath);
			}
			//Otherwise, we add it to the shader vector.
			else if(tempType == "Shader")
			{
				LoadEffect(curShader, tempPath);
				++curShader;
			}
		}

		SAFE_RELEASE(tempTexture);
	}

	/*****************************************************************************/
	/*!
	\brief
		Loads fonts to memory.  Not really being used at this point.
	*/
	/*****************************************************************************/
	void Graphics::LoadFonts()
	{
			HDC hDC;
			int nHeight;
			int nPointSize = 10;

			hDC = GetDC(NULL);

			nHeight = -(MulDiv(nPointSize, GetDeviceCaps(hDC, LOGPIXELSY), GetDeviceCaps(hDC, LOGPIXELSX)));

			ReleaseDC(NULL, hDC);

			//LOADS THE FONT IN
			D3DXCreateFont(mDevice, nHeight, 0, FW_BOLD, 0, FALSE, 
				DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 
				DEFAULT_PITCH | FF_DONTCARE, TEXT("Lucida Console"), 
				&mD3dxFont[0]);

			D3DXCreateFont(mDevice, nHeight * 5, 0, FW_BOLD, 0, FALSE, 
				DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 
				DEFAULT_PITCH | FF_DONTCARE, TEXT("Lucida Console"), 
				&mD3dxFont[1]);

			D3DXCreateFont(mDevice, nHeight * 2, 0, FW_BOLD, 0, FALSE, 
				DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 
				DEFAULT_PITCH | FF_DONTCARE, TEXT("Lucida Console"), 
				&mD3dxFont[2]);

			D3DXCreateFont(mDevice, nHeight * 15, 0, FW_BOLD, 0, FALSE, 
				DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 
				DEFAULT_PITCH | FF_DONTCARE, TEXT("Lucida Console"), 
				&mD3dxFont[3]);
	}

	/*****************************************************************************/
	/*!
	\brief
		Loads the default texture to the system.
	*/
	/*****************************************************************************/
	void Graphics::LoadDefaultText()
	{
		std::string filename("..//Content//Textures//ohnoes.png");
		D3DXCreateTextureFromFile(mDevice, filename.c_str(), &mDefTexture);
	}

	/*****************************************************************************/
	/*!
	\brief
		Function to find a given texture in the map given a name.
	*/
	/*****************************************************************************/
	IDirect3DTexture9* Graphics::GetTexture(std::string name)
	{
		stdext::hash_map<std::string, IDirect3DTexture9*>::iterator it = mTextureMap.find(name);

		IDirect3DTexture9* returnTexture = (it != mTextureMap.end()) ? it->second : mDefTexture;

		return returnTexture;
	}

	/*****************************************************************************/
	/*!
	\brief
		Load an individual texture.  Helper for the LoadAssets function.
	*/
	/*****************************************************************************/
	void Graphics::LoadTexture(std::string filename)	
	{
		IDirect3DTexture9* pTexture;
		if(SUCCEEDED(D3DXCreateTextureFromFile(mDevice, filename.c_str(), &pTexture)))
		{
			mTextureMap.insert(std::pair<std::string, IDirect3DTexture9*>(filename, pTexture));
		}
	}

	/*****************************************************************************/
	/*!
	\brief
		Load an individual effect.  Helper for the LoadAssets function.
	*/
	/*****************************************************************************/
	void Graphics::LoadEffect(int index, std::string filename)
	{
		LPD3DXBUFFER pBufferErrors = NULL;
		DWORD shaderFlags  = D3DXSHADER_OPTIMIZATION_LEVEL3;

		if(FAILED(D3DXCreateEffectFromFile(mDevice, 
			filename.c_str(),//The shader file to to load
			NULL, //Optional NULL-terminated array of preprocessor macro definitions
			NULL, //Not using include interface
			shaderFlags, //Shader Flags
			NULL, //No shader pool
			&mShaders[index], 
			&pBufferErrors)))
		{
#ifdef BLARG
			Message* q = new Message(Mid::DebugCNSL, 1, new std::string("Failed to load shader"));
			CORE->EnqueueMessage(q);
#endif
		}
	}

	/*****************************************************************************/
	/*!
	\brief
		Graphics update loop.  Throws verts to the batcher.
	*/
	/*****************************************************************************/
	void Graphics::Update(float dt)
	{
		HRESULT lostDeviceCheck = mDevice->TestCooperativeLevel();

		if(lostDeviceCheck == D3DERR_DEVICELOST)
		{
			Sleep(20);
			return;
		}
		else if(lostDeviceCheck == D3DERR_DEVICENOTRESET)
		{
			HandleLostDevice();
			return;
		}
		else if(lostDeviceCheck == D3D_OK)
		{
			mDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 0.0f, 0);

			if(SUCCEEDED(mDevice->BeginScene()))
			{
				DrawWorld();

#ifdef BLARG
				if(CORE->mMode >= 3)
				{
					DrawDebugInfo();
					mBatcher->SetDeviceData();
				}

				if(CORE->mMode >= 1)
				{
					TwDraw();
				}
#endif

				mDevice->EndScene();
			}

			mDevice->Present(NULL, NULL, NULL, NULL);
		}

#ifdef BLARG
		static double dwFrames = 0;

		dwFrames++;
		double dwCurrent = CORE->GetCounter();
		dwElapsed += dt;
		if(dwElapsed >= (float) (50.0 / 1000.0))
		{
			mFps = (float) (dwFrames / dwElapsed);
			dwFrames = 0;
			dwElapsed = 0;
		}
#endif
	}

	/*****************************************************************************/
	/*!
	\brief
		Updated camera and matricies.  Helper for the Update loop.
	*/
	/*****************************************************************************/
	void Graphics::DrawWorld()
	{
		Vec3 camPos(floor(mCurrentCamera->mTransform->mPosition.x), floor(mCurrentCamera->mTransform->mPosition.y + mCameraOffset), -10.0f), 
			camLookAt(floor(mCurrentCamera->mTransform->mPosition.x), floor(mCurrentCamera->mTransform->mPosition.y + mCameraOffset), 0.0f), 
			camUp(0.0f, 1.0f, 0.0f);

		D3DXMatrixLookAtLH(&mMatView, &camPos, &camLookAt, &camUp);

		mViewProjMatrix = mMatView * mMatProj * mResolutionScaleMatrix;

		for(ObjectLinkedList<IDrawable>::iterator it = mDrawableList.begin(); it != mDrawableList.end(); ++it)
		{
			it->Draw(mDevice, mShaders[mCurrentShader]);
		}

		mBatcher->Flush();

		for(std::list<PrintOut*>::iterator it = mPrintStates.begin(); (it != mPrintStates.end()) && !mPrintStates.empty() && (*it) != NULL;)
		{
			WriteString(*it);
			std::list<PrintOut*>::iterator it2;
			it2 = it;
			++it;
			delete (*it2);
			mPrintStates.remove(*it2);
		}
		mPrintStates.clear();
	}

	/*****************************************************************************/
	/*!
	\brief
		Function to draw debug data to the screen.
	*/
	/*****************************************************************************/
	void Graphics::DrawDebugInfo()
	{
		Drawer::Instance.Flush();

		std::vector<Drawer::LineSegment>& Lines = Drawer::Instance.LineSegments;
		std::vector<Drawer::LineSet>& Sets = Drawer::Instance.Sets;
		mDevice->SetFVF(LINE_FVF);

		//Each set is collection of lines that are in the same style (color)
		//so they can be draw together
		unsigned index = 0;
		for(unsigned set=0; set < Sets.size(); ++set)
		{
			//Draw all the lines in the set
			unsigned numberOfSegments = Sets[set].Segments;
			Vec4 lineColor = Sets[set].Color;
			
			Mat4 worldViewProj = mMatView * mMatProj * mResolutionScaleMatrix;

			mShaders[1]->SetMatrix("WorldViewProj", &worldViewProj);
			mShaders[1]->SetVector("color", &lineColor);
			
			UINT numberOfPasses = 0;
			mShaders[1]->Begin(&numberOfPasses, 0);
			for(UINT pass=0; pass < numberOfPasses; ++pass)
			{
				mShaders[1]->BeginPass(pass);
				mDevice->DrawPrimitiveUP(D3DPT_LINELIST, numberOfSegments , &Lines[index] , sizeof(Drawer::LineSegment) / 2);
				index += numberOfSegments;
				mShaders[1]->EndPass();
			}
			mShaders[1]->End();
		}

		//Clear all the lines they must be submitted again each frame
		Drawer::Instance.Clear();
	}

	/*****************************************************************************/
	/*!
	\brief
		Function to queue strings to be printed to the screen.
	*/
	/*****************************************************************************/
	void Graphics::QueString(std::string* text, int x, int y, int font)
	{
		PrintOut* print = new PrintOut();
		print->position.x = x * 1.0f;
		print->position.y = y * 1.0f;
		print->Print = *text;
		print->font = font;
		mPrintStates.push_back(print);
	}

	/*****************************************************************************/
	/*!
	\brief
		Function to write strings to the screen.
	*/
	/*****************************************************************************/
	void Graphics::WriteString(PrintOut* print)
	{
		RECT rect;
		SetRect(&rect,(int) print->position.x,(int) print->position.y,(int) print->position.x,(int) print->position.y);

		mD3dxFont[print->font]->DrawText(NULL, print->Print.c_str(), -1, &rect,  DT_NOCLIP, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	}

	/*****************************************************************************/
	/*!
	\brief
		Returns the current FPS.  Used by ATB for debug output.
	*/
	/*****************************************************************************/
	float Graphics::GetFPS()
	{
		return mFps;
	}

	/*****************************************************************************/
	/*!
	\brief
		Sets window properties.  Called from WinMain.
	*/
	/*****************************************************************************/
	void Graphics::SetWindowProperties(HWND hWnd, int screenWidth, int screenHeight)
	{
		mHWnd = hWnd;
		mScreenWidth = screenWidth;
		mScreenHeight = screenHeight;
	}

	/*****************************************************************************/
	/*!
	\brief
		Message handler.
	*/
	/*****************************************************************************/
	Mid::MessageIdType Graphics::SendMessage(Message *message)
	{
#ifdef BLARG
		if(message->mMessageId == Mid::KeyDown)
		{
			int key = atoi(((std::string*)(message->mMessageData[0]))->c_str());

			if(DBGCNSL->DBConsole == 0 && gGameStateCurr != GS_PAUSE)	// Don't process input when the debug console is active
			{
				if(key == VK_OEM_4)
				{
					mCurrentShader = 0;
				}

				if(key == VK_OEM_6)
				{
					mCurrentShader = 2;
				}

				if(key == VK_OEM_5)
				{
					ToggleWindowed();
				}
			}
		}
#endif

		return Mid::None;
	}

	/*****************************************************************************/
	/*!
	\brief
		Function to handle resolution changes.
	*/
	/*****************************************************************************/
	void Graphics::ChangeResolution(int newWidth, int newHeight)
	{
		mScreenWidth = newWidth;
		mScreenHeight = newHeight;

		mD3DPP.BackBufferWidth = mScreenWidth; 
		mD3DPP.BackBufferHeight = mScreenHeight;

		RECT fullWinRect = {0, 0, mScreenWidth, mScreenHeight};

		if(mD3DPP.Windowed)
		{
			AdjustWindowRect(&fullWinRect, WS_OVERLAPPEDWINDOW ^ WS_MINIMIZEBOX ^ WS_MAXIMIZEBOX ^ WS_THICKFRAME, FALSE);
			//SetWindowLongPtr(mHWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW ^ WS_MINIMIZEBOX ^ WS_MAXIMIZEBOX ^ WS_THICKFRAME);
		}

		SetWindowPos(mHWnd, HWND_TOP, 0, 0, fullWinRect.right - fullWinRect.left, fullWinRect.bottom - fullWinRect.top, SWP_NOZORDER | SWP_SHOWWINDOW);

		HandleLostDevice();
	}

	/*****************************************************************************/
	/*!
	\brief
		Returns the presentation parameters.  Used by the user settings functions.
	*/
	/*****************************************************************************/
	D3DPRESENT_PARAMETERS & Graphics::GetD3DPP()
	{
		return mD3DPP;
	}

	/*****************************************************************************/
	/*!
	\brief
		Function to handle fullscreen/windowed switching.
	*/
	/*****************************************************************************/
	void Graphics::ToggleWindowed()
	{
		if(mD3DPP.Windowed)
		{
			if(mScreenWidth == mMaxResWindowed.Width && mMaxResWindowed.Width != mMaxRes.Width)
			{
				mScreenWidth = mMaxRes.Width;
				mScreenHeight = mMaxRes.Height;
			}

			mD3DPP.BackBufferWidth			= mScreenWidth; 
			mD3DPP.BackBufferHeight			= mScreenHeight;
			mD3DPP.Windowed					= FALSE;
			mD3DPP.BackBufferFormat			= D3DFMT_X8R8G8B8;

			RECT fullWinRect = {0, 0, mScreenWidth, mScreenHeight};
			AdjustWindowRect(&fullWinRect, WS_POPUP, FALSE);

			SetWindowLongPtr(mHWnd, GWL_STYLE, WS_POPUP);
			SetWindowPos(mHWnd, HWND_TOP, 0, 0, fullWinRect.right - fullWinRect.left, fullWinRect.bottom - fullWinRect.top, SWP_NOZORDER | SWP_SHOWWINDOW);
		}
		else
		{
			if(mScreenWidth == mMaxRes.Width && mMaxResWindowed.Width != mMaxRes.Width)
			{
				mScreenWidth = mMaxResWindowed.Width;
				mScreenHeight = mMaxResWindowed.Height;
			}

			mD3DPP.BackBufferWidth			= mScreenWidth; 
			mD3DPP.BackBufferHeight			= mScreenHeight;
			mD3DPP.Windowed					= TRUE;
			mD3DPP.BackBufferFormat			= D3DFMT_UNKNOWN;

			RECT fullWinRect = {0, 0, mScreenWidth, mScreenHeight};
			AdjustWindowRect(&fullWinRect, WS_OVERLAPPEDWINDOW ^ WS_MINIMIZEBOX ^ WS_MAXIMIZEBOX ^ WS_THICKFRAME, FALSE);

			SetWindowLongPtr(mHWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW ^ WS_MINIMIZEBOX ^ WS_MAXIMIZEBOX ^ WS_THICKFRAME);
			SetWindowPos(mHWnd, HWND_TOP, 0, 0, fullWinRect.right - fullWinRect.left, fullWinRect.bottom - fullWinRect.top, SWP_NOZORDER | SWP_SHOWWINDOW);
		}

		HandleLostDevice();
	}

	/*****************************************************************************/
	/*!
	\brief
		Function to handle lost device situations.
	*/
	/*****************************************************************************/
	void Graphics::HandleLostDevice()
	{
		for(int i = 0; i < 4; ++i)
		{
			mD3dxFont[i]->OnLostDevice();
		}

		for(int j = 0; j < 3; ++j)
		{
			mShaders[j]->OnLostDevice();
		}

#ifdef BLARG
		TwWindowSize(0, 0);
#endif

		mBatcher->OnLostDevice();

		HRESULT temp = mDevice->Reset(&mD3DPP);

		if(temp == D3D_OK)
		{
			//Turning off lights
			mDevice->SetRenderState(D3DRS_LIGHTING, FALSE); 
			//Disregard culling
			mDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
			//Alpha Blending
			mDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			//Get Alpha from textures
			mDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			//Get the inverse alpha from textures
			mDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

			for(int k = 0; k < 4; ++k)
			{
				mD3dxFont[k]->OnResetDevice();
			}

			for(int l = 0; l < 3; ++l)
			{
				mShaders[l]->OnResetDevice();
			}

			mBatcher->Initialize(mDevice);

#ifdef BLARG
			TwWindowSize(mScreenWidth, mScreenHeight);
#endif

			mWidthRatio = (mScreenWidth * mZoom) / mDefResX;
			mHeightRatio = (mScreenHeight * mZoom) / mDefResY;
			D3DXMatrixScaling(&mResolutionScaleMatrix, mWidthRatio, mHeightRatio, 1.0f);
			D3DXMatrixOrthoLH(&mMatProj, (float)mScreenWidth , (float)mScreenHeight , 1.0f, 100.0f);
		}
	}

	/*****************************************************************************/
	/*!
	\brief
		Function to turn everything on the screen red during character death.
	*/
	/*****************************************************************************/
	void Graphics::RedAll()
	{
		for(ObjectLinkedList<IDrawable>::iterator it = mDrawableList.begin(); it != mDrawableList.end(); ++it)
		{
			it->SetRed(true);
		}
	}

	/*****************************************************************************/
	/*!
	\brief
		Function to alpha out everything on the screen.  Not used anymore.
	*/
	/*****************************************************************************/
	void Graphics::FadeAll()
	{
		for(ObjectLinkedList<IDrawable>::iterator it = mDrawableList.begin(); it != mDrawableList.end(); ++it)
		{
			it->SetFadeMode(-1, 0.01f);
		}
	}

	/*****************************************************************************/
	/*!
	\brief
		Graphics destructor.
	*/
	/*****************************************************************************/
	Graphics::~Graphics()
	{
		for(std::list<PrintOut*>::iterator it = mPrintStates.begin(); (it != mPrintStates.end()) && !mPrintStates.empty() && (*it) != NULL;)
		{
			WriteString(*it);
			std::list<PrintOut*>::iterator it2;
			it2 = it;
			++it;
			delete (*it2);
			mPrintStates.remove(*it2);
		}
		mPrintStates.clear();

		for(int i = 0; i < 4; ++i)
		{
			SAFE_RELEASE(mD3dxFont[i]);
		}

		for(int j = 0; j < 3; ++j)
		{
			SAFE_RELEASE(mShaders[j]);
		}

		SAFE_RELEASE(mDefTexture);

		for(stdext::hash_map<std::string, IDirect3DTexture9*>::iterator it = mTextureMap.begin(); it != mTextureMap.end(); ++it)
		{
			SAFE_RELEASE(it->second);
		}

		mTextureMap.clear();

#ifdef BLARG
		TwWindowSize(0, 0);
#endif
		mBatcher->OnLostDevice();

		SAFE_RELEASE(mD3D);
		SAFE_RELEASE(mDevice);
		delete mBatcher;
	}
}
