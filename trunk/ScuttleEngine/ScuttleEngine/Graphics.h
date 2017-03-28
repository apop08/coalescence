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

#pragma once

#include "Engine.h"
#include "Sprite.h"
#include "CompSprite.h"

namespace Scuttlemeyer
{
	typedef struct printstruct_{
		std::string Print;
		Vec2 position;
		int font;
	}PrintOut;

	class Camera;
	class QuadBatcher;

	class Graphics : public ISystem
	{                        
	public:
		Graphics();
		void Unload();
		void Load();
		void Free();
		void Initialize();
		void LoadTexture(std::string filename);
		IDirect3DTexture9* GetTexture(std::string name);
		void Update(float dt);
		void DrawWorld();
		void DrawDebugInfo();
		void WriteString(PrintOut* print);
		virtual std::string GetName() { return "Graphics"; }
		void SetWindowProperties(HWND hWnd, int screenWidth, int screenHeight);
		Mid::MessageIdType SendMessage(Message *message);
		void QueString(std::string* text, int x, int y, int font);
		float GetFPS();
		void ChangeResolution(int newWidth, int newHeight);
		void ToggleWindowed();
		void HandleLostDevice();
		void RedAll();
		void FadeAll();
		D3DPRESENT_PARAMETERS & GetD3DPP();
		~Graphics();
	public:
		IDirect3DDevice9* mDevice;
		stdext::hash_map<std::string, IDirect3DTexture9*> mTextureMap;
		ID3DXEffect* mShaders[4];
		ObjectLinkedList<IDrawable> mDrawableList;
		QuadBatcher* mBatcher;
		Mat4 mMatProj;
		Mat4 mMatView;
		Mat4 mResolutionScaleMatrix;
		Mat4 mViewProjMatrix;
		Camera* mCurrentCamera;
		float mWidthRatio;
		float mHeightRatio;
		int mScreenWidth;
		int mScreenHeight;
		static const int mDefResX = 1024;
		static const int mDefResY = 768;
		float mCameraOffset;
		float mZoom;
		D3DDISPLAYMODE mMaxRes;
		D3DDISPLAYMODE mMaxResWindowed;

	private:
		HRESULT InitVertices();
		void InitVertexBuf();
		void LoadEffect(int index, std::string filename);
		void LoadDefaultText();
		void LoadAssets();
		void LoadFonts();
		void EnumerateDisplayModes();
		bool ChooseDisplayMode();

	private:
		HWND mHWnd;
		IDirect3D9* mD3D;
		D3DPRESENT_PARAMETERS mD3DPP;
		IDirect3DTexture9* mDefTexture;
		LPD3DXFONT mD3dxFont[4];
		float mFps;
		std::list<PrintOut*> mPrintStates;
		double dwElapsed;
		bool mInit;
		int mCurrentShader;
		std::vector<D3DDISPLAYMODE> mDisplayModes;
	};

	extern Graphics* GRAPHICS;
}