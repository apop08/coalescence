/******************************************************************************/
/*!
\file   QuadBatcher.h
\author Matt Sutherlin
\par    email: matt.sutherlin\@digipen.edu
\par    DigiPen login: matt.sutherlin
\par    Course: GAM200
\date   11/3/2011
\brief  
Batch rendering system for use with texture quads.
\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/******************************************************************************/

#pragma once

#include "Graphics.h"
#include "VertexTypes.h"

namespace Scuttlemeyer
{
	class QuadBatcher
	{
	public:
		QuadBatcher();
		void Initialize(IDirect3DDevice9* pDevice);
		HRESULT Start();
		HRESULT Draw(IDirect3DTexture9* pTexture, ID3DXEffect* pShader, int pDepth, unsigned int offset, Mat4* pTransMat, 
			float indexX, float sizeX, float indexY, float sizeY, Mat4* pWorldProjMat, Vec4 pColor);
		HRESULT Flush();
		void SetDeviceData();
		void OnLostDevice();
		~QuadBatcher();
	private:
		IDirect3DDevice9* mDevice;
		IDirect3DVertexBuffer9* mVertexBuffer;
		IDirect3DTexture9* mTexture;
		ID3DXEffect* mShader;
		int mDepth;
		Vec4 mColor;
		const unsigned int cMaxVerts;
		unsigned int mNumVerts;
		Vertex2D* mVertexBufferPointer;
		Mat4* mWorldProjMat;
	private:
		void StoreMatrixByValue(Mat4* pWorldProjMat);
	};
}