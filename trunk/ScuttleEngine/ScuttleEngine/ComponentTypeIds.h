/******************************************************************************/
/*!
\file   ComponentTypeIds.h
\author Kevin Verholtz
\par    email: kevin.verholtz\@digipen.edu
\par    DigiPen login: kevin.verholtz
\par    Course: GAM200
\date   12/8/2011
\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/******************************************************************************/


#pragma once

namespace Scuttlemeyer
{
	//A simple enumeration used to identify components quickly and efficiently.
	//There is alternative methods for generating ids automatically, 
	//(string hashing, static type ids).
	//But this is a clean, easy, understandable and stable way.
	enum ComponentTypeId
	{
		//Invalid component id
		CT_None = 0,
		//Basic Components
		CT_Transform,
		//Graphics
		CT_Camera,
		CT_Sprite,
		CT_CompSprite,
		//Physics
		CT_PhysBody,
		CT_Region,
		//Game Logic
		CT_ILogical,
		CT_Controller,
		CT_Player,
		CT_Thwomp,
		CT_Turret,
		CT_Bullet,
		CT_Rocket,
		CT_Block,
		CT_Switch,
		CT_BrokenPipe,
		CT_KillZone,
		CT_BlockWin,
		CT_EntranceDoor,
		CT_Static,
		CT_Door,
		CT_bNewGame,
		CT_bHowToPlay,
		CT_bOptions,
		CT_bCredits,
		CT_bQuit,
		CT_bYes,
		CT_bNo,
		CT_bResume,
		CT_bRestart,
		CT_bMainMenu,
		CT_bVolume,
		CT_bResolution,
		CT_bFullscreen,
		CT_bArrowLeft,
		CT_bArrowRight,
		CT_bLevel,
		CT_bBack,
		CT_TutBox,
		CT_Fan,
		CT_BiDirectionalFan,
		CT_Checkpoint,
		CT_CreditsText,
		//Particles
		CT_Particle,
		CT_Emitter,
		//Camera Logics
		CT_EditCam,
		CT_LevelPanCam,
		//Audio
		CT_SoundNode,
		CT_AttenuatedSoundNode,
		//Max
		CT_MaxComponents
	};

}