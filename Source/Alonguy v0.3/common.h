#ifndef COMMON_H
#define COMMON_H

#define DEBUG_BUILD


//This will be the global header include file.
//It will also initiate global variables/classes.
//To be included in every file that uses Irrlicht.

#include <irrlicht.h>
#include <irrklang.h>

enum OBJECT_ID
{
	OBJ_BSP,
	OBJ_HAZARD,
	OBJ_WATER,
	OBJ_WATERFALL,
	OBJ_PORTAL_A,
	OBJ_PORTAL_B,
	OBJ_CRATE,
	OBJ_TARGET,
	OBJ_DARK1,
	OBJ_DARK2,
	OBJ_DARK3,
	ITEM_DARKORB,
	ENEMY_MINISHOOP,
	ENEMY_SHOOPDAWHOOP,
	ENEMY_ROCKETSHOOP,
	SPAWN_MINISHOOP,
	OBJ_DAMAGED_CRATE,
	OBJ_HEAVY_CRATE,
	SYS_BGMNAME,
	OBJ_TORCHFIRE,
	TRIGGER_ROOM_ENTER,
	TRIGGER_DEATHTRAP,
	OBJ_DOOR,
	TRIGGER_EXIT,
	TRIGGER_ENTER_A,
	TRIGGER_ENTER_B,
	TRIGGER_ENTER_C,
	OBJ_DOOR_BARS,
	TRIGGER_ENEMYCLEAR,
	TRIGGER_SPAWN_ENEMY,
	OBJ_VINES,
	OBJ_LADDER,
	OBJ_DESTRUCT_SWITCH,
	OBJ_PRESSED_SWITCH,
	BOSS_SHOOPDAWHOOP,
	COA_BOSS_TRIGGER,
	CAM_MARK0,
	CAM_MARK1,
	CAM_MARK2,
	CAM_MARK3,
	RANDOM_NPC,
	NPC_RANDOM,
	OBJ_PLAYER,
	MAGIC_FIREBALL,
	MAGIC_FIRE_EXPLOSION,
	MAGIC_LIGHT

};

enum PROGRESS_BITS
{
	MAIN_INTRO,
	COA_INTRO,
	COA_SWITCH1,
	COA_SWITCH2,
	COA_CLEAR,
	FIELD_INTRO,
	COT_INTRO,
	ENDING
};

enum FLOOR_ID
{
	WOOD,
	GRASS,
	METAL,
	STONE,
	WATER
};

using namespace irr;
using namespace video;
using namespace scene;

// Psuedo-Random, don't laugh at me!

class Randomizer
{
public:

	//! resets the randomizer
	static void reset();

	//! generates a pseudo random number
	static s32 rand();

private:
	static s32 seed;
};

s32 Random(s32 min, s32 max);

class InputReciever : public IEventReceiver
{
public:
	// This is the one method that we have to implement
	virtual bool OnEvent(const SEvent& event)
	{
		// Remember whether each key is down or up
		if (event.EventType == irr::EET_KEY_INPUT_EVENT)
			KeyIsDown[event.KeyInput.Key] = event.KeyInput.PressedDown;

		if (event.EventType == irr::EET_MOUSE_INPUT_EVENT)
		{
			L_Click = event.MouseInput.isLeftPressed();
			R_Click = event.MouseInput.isRightPressed();

			s32 x = event.MouseInput.X;
			s32 y = event.MouseInput.Y;
			mouse_pos =	 core::vector2d<s32>(x,y);
		}

		return false;
	}

	// This is used to check whether a key is being held down
	virtual bool IsKeyDown(EKEY_CODE keyCode) const
	{
		return KeyIsDown[keyCode];
	}
	
	InputReciever()
	{
		for (u32 i=0; i<KEY_KEY_CODES_COUNT; ++i)
			KeyIsDown[i] = false;
	}

	 
	core::vector2d<s32> mouse_pos;
	bool R_Click;
	bool L_Click;

private:
	// We use this array to store the current state of each key
	bool KeyIsDown[KEY_KEY_CODES_COUNT];

};

#endif