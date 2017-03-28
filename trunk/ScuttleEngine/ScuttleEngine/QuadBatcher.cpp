/******************************************************************************/
/*!
\file   QuadBatcher.cpp
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

#include "Precompiled.h"
#include "QuadBatcher.h"
#include "Core.h"

#define SAFE_RELEASE(x) if (x) { x->Release(); x = NULL; }

namespace Scuttlemeyer
{
	/*****************************************************************************/
	/*!
	\brief
		Batcher constructor.  Sets maximum number of verts per batch to 990.
	*/
	/*****************************************************************************/
	QuadBatcher::QuadBatcher() : cMaxVerts(6 * 165)
	{
		mWorldProjMat = new Mat4();
	}

	/*****************************************************************************/
	/*!
	\brief
		Initializes the batcher with the graphics system's vertex buffer.
	*/
	/*****************************************************************************/
	void QuadBatcher::Initialize(IDirect3DDevice9* pDevice)
	{
		mVertexBuffer = NULL;
		if(FAILED(pDevice->CreateVertexBuffer(cMaxVerts * sizeof(Vertex2D), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY | D3DCREATE_HARDWARE_VERTEXPROCESSING, VERTEX2D_FVF,
			D3DPOOL_DEFAULT, &mVertexBuffer, NULL)))
		{
#ifdef BLARG
			Message* q = new Message(Mid::DebugCNSL, 1, new std::string("Graphics failed to create vertex buffer."));
			CORE->EnqueueMessage(q);
#endif

			////Since Initialize is a void function, we can't return a value anymore.  So, we just return.
			//return;
		}

		mDevice = pDevice;
		SetDeviceData();

		mVertexBufferPointer = NULL;

		Start();
	}

	/*****************************************************************************/
	/*!
	\brief
		Starts a new batch.
	*/
	/*****************************************************************************/
	HRESULT QuadBatcher::Start()
	{
		HRESULT hr;
		
		mNumVerts = 0;
		mDepth = 0;

		mVertexBufferPointer = NULL;
		if(FAILED(hr = mVertexBuffer->Lock(0, 0, reinterpret_cast<void**>(&mVertexBufferPointer), D3DLOCK_DISCARD)))
		{
			return hr;
		}

		return S_OK;
	}

	/*****************************************************************************/
	/*!
	\brief
		Adds verts to the batch.  Flushes the batch to the renderer if the batch is broken.
	*/
	/*****************************************************************************/
	HRESULT QuadBatcher::Draw(IDirect3DTexture9* pTexture, ID3DXEffect* pShader, int pDepth, unsigned int offset, Mat4* pTransMat, 
		float indexX, float sizeX, float indexY, float sizeY, Mat4* pWorldProjMat, Vec4 pColor)
	{
		HRESULT hr;

		if((pTexture != mTexture) || (pShader != mShader) || (pDepth != mDepth) || (pColor != mColor) || (mNumVerts + offset > cMaxVerts)) 
		{
			Flush();

			mTexture = pTexture;
			mShader = pShader;
			mDepth = pDepth;
			mColor = pColor;
			StoreMatrixByValue(pWorldProjMat);
		}

		if(!mVertexBufferPointer)
		{
			if(FAILED(hr = mVertexBuffer->Lock(0, 0, reinterpret_cast<void**>(&mVertexBufferPointer), D3DLOCK_DISCARD)))
			{
				return hr;
			}
		}

		Vec4 transVec;

		mVertexBufferPointer[0 + mNumVerts]._Pos		= Vec3(-0.5f, -0.5f, 0.0f);
		mVertexBufferPointer[0 + mNumVerts]._tu			= indexX / sizeX;
		mVertexBufferPointer[0 + mNumVerts]._tv			= (indexY + 1) / sizeY;
		D3DXVec3Transform(&transVec, &mVertexBufferPointer[0 + mNumVerts]._Pos, pTransMat);
		mVertexBufferPointer[0 + mNumVerts]._Pos.x		= transVec.x;
		mVertexBufferPointer[0 + mNumVerts]._Pos.y		= transVec.y;

		mVertexBufferPointer[1 + mNumVerts]._Pos		= Vec3(-0.5f, 0.5f, 0.0f);
		mVertexBufferPointer[1 + mNumVerts]._tu			= indexX / sizeX;
		mVertexBufferPointer[1 + mNumVerts]._tv			= indexY / sizeY;
		D3DXVec3Transform(&transVec, &mVertexBufferPointer[1 + mNumVerts]._Pos, pTransMat);
		mVertexBufferPointer[1 + mNumVerts]._Pos.x		= transVec.x;
		mVertexBufferPointer[1 + mNumVerts]._Pos.y		= transVec.y;

		mVertexBufferPointer[2 + mNumVerts]._Pos		= Vec3( 0.5f, -0.5f, 0.0f);
		mVertexBufferPointer[2 + mNumVerts]._tu			= (indexX + 1) / sizeX;
		mVertexBufferPointer[2 + mNumVerts]._tv			= (indexY + 1) / sizeY;
		D3DXVec3Transform(&transVec, &mVertexBufferPointer[2 + mNumVerts]._Pos, pTransMat);
		mVertexBufferPointer[2 + mNumVerts]._Pos.x		= transVec.x;
		mVertexBufferPointer[2 + mNumVerts]._Pos.y		= transVec.y;

		mVertexBufferPointer[3 + mNumVerts]._Pos		= Vec3( 0.5f, -0.5f, 0.0f);
		mVertexBufferPointer[3 + mNumVerts]._tu			= (indexX + 1) / sizeX;
		mVertexBufferPointer[3 + mNumVerts]._tv			= (indexY + 1) / sizeY;
		D3DXVec3Transform(&transVec, &mVertexBufferPointer[3 + mNumVerts]._Pos, pTransMat);
		mVertexBufferPointer[3 + mNumVerts]._Pos.x		= transVec.x;
		mVertexBufferPointer[3 + mNumVerts]._Pos.y		= transVec.y;

		mVertexBufferPointer[4 + mNumVerts]._Pos		= Vec3(-0.5f, 0.5f, 0.0f);
		mVertexBufferPointer[4 + mNumVerts]._tu			= indexX / sizeX;
		mVertexBufferPointer[4 + mNumVerts]._tv			= indexY / sizeY;
		D3DXVec3Transform(&transVec, &mVertexBufferPointer[4 + mNumVerts]._Pos, pTransMat);
		mVertexBufferPointer[4 + mNumVerts]._Pos.x		= transVec.x;
		mVertexBufferPointer[4 + mNumVerts]._Pos.y		= transVec.y;

		mVertexBufferPointer[5 + mNumVerts]._Pos		= Vec3( 0.5f, 0.5f, 0.0f);
		mVertexBufferPointer[5 + mNumVerts]._tu			= (indexX + 1) / sizeX;
		mVertexBufferPointer[5 + mNumVerts]._tv			= indexY / sizeY;
		D3DXVec3Transform(&transVec, &mVertexBufferPointer[5 + mNumVerts]._Pos, pTransMat);
		mVertexBufferPointer[5 + mNumVerts]._Pos.x		= transVec.x;
		mVertexBufferPointer[5 + mNumVerts]._Pos.y		= transVec.y;

		mNumVerts += offset;

		return S_OK;
	}

	/*****************************************************************************/
	/*!
	\brief
		Draws the current batch when it's been broken.
	*/
	/*****************************************************************************/
	HRESULT QuadBatcher::Flush()
	{
		if(mVertexBufferPointer)
		{
			mVertexBuffer->Unlock();
			mVertexBufferPointer = NULL;
		}

		if(mNumVerts) 
		{
			mShader->SetTechnique("Technique0");
			mShader->SetMatrix("WorldViewProj", mWorldProjMat);
			mShader->SetTexture("texture0", mTexture);
			mShader->SetVector("color", &mColor);

			UINT numberOfPasses = 0;
			
			mShader->Begin(&numberOfPasses, 0);
			
			for(UINT pass = 0; pass < numberOfPasses; ++pass)
			{
				mShader->BeginPass(pass);
				mDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, mNumVerts / 3);
				mShader->EndPass();
			}
			mShader->End();
		}

		mNumVerts = 0;

		return S_OK;
	}

	/*****************************************************************************/
	/*!
	\brief
		Function to update device data.  Doesn't need to happen every update.
	*/
	/*****************************************************************************/
	void QuadBatcher::SetDeviceData()
	{
		mDevice->SetStreamSource(0, mVertexBuffer, 0, sizeof(Vertex2D));
		mDevice->SetFVF(VERTEX2D_FVF);
	}

	/*****************************************************************************/
	/*!
	\brief
		Lost device handling specific to the batcher.
	*/
	/*****************************************************************************/
	void QuadBatcher::OnLostDevice()
	{
		SAFE_RELEASE(mVertexBuffer);
		mVertexBufferPointer = NULL;
		mTexture = NULL;
		mShader = NULL;
		mDevice = NULL;
	}

	/*****************************************************************************/
	/*!
	\brief
		Stores a copy of the world projection matrix to the batcher.
	*/
	/*****************************************************************************/
	void QuadBatcher::StoreMatrixByValue(Mat4* pWorldProjMat)
	{
		memcpy(mWorldProjMat, pWorldProjMat, sizeof(Mat4));
	}

	/*****************************************************************************/
	/*!
	\brief
		Batcher Destructor.
	*/
	/*****************************************************************************/
	QuadBatcher::~QuadBatcher()
	{
		delete mWorldProjMat;
		SAFE_RELEASE(mVertexBuffer);
	}
}