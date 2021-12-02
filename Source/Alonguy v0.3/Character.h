#include "object.h"
#include "ASQ.h"

#ifndef CHARACTER_H
#define CHARACTER_H

//#define WEEGEE_MOD

enum CHR_Model
{
  Alonguy,
  Maria
};


class Character : public Object
{
public:
	//Inherit the Constructor from the "Object" class.
	Character(irr::IrrlichtDevice * dev,irrklang::ISoundEngine * aud) : Object(dev,aud){};

	// Spawn the Player, with the specified model id and position.
	void Spawn(CHR_Model mdl,core::vector3df spawnpos);

	// Give control to the player.
	void SetControl(InputReciever * cntrl);

	core::vector3df cpos;

	// Every tick, it should update.
	void update();

	// Movement Calculation
	void move();

	// Choose/Play animations
	void animate();

	// Move if you're pressing something!
	void checkInput();

	void setGravity(scene::IMetaTriangleSelector * world);

	void AwardEXP(s32 amount);

	void jump();

	char * char_name;

	void LoadSeq(core::stringc path,char * name);
	void PickSeq(char * name);
	s32 numAnims;

	// Player Camera
	scene::ICameraSceneNode * cam;
	scene::ISceneNodeAnimatorCollisionResponse * camcol;

	core::vector3df origin;

	u32 old_time;
	u32 new_time;

	// To safely load new levels
	bool dormant;
	bool active_camo;

	bool isClimbing;
	bool isSwimming;

	void deactivate();
	void reactivate(core::vector3df new_pos);
	void step_sound();

	s32 floor_ID;

	bool step1,step2,step3;

	bool topdown_view;


	// Cutscene-Related
	scene::ICameraSceneNode * cutscene_cam;
	bool cutscene;
	void moveTo(core::vector3df dest,f32 speed);

	void Damage(s32 amount,core::vector3df from,bool heavy);

	bool Damage_Timeout;
	s32 stun_time;

	// Attacking
	void Attack();

	gui::IGUIImage * CS_border;
	bool Anim_Override;

	bool inWater;

	core::line3df damage;

	scene::ISceneNodeAnimator * d_line;
	scene::ISceneNode * target;

	bool target_lock;
	bool isAttacking;

	f32 velocity;
		
	core::vector3df last_safe_pos;


	s32 HP;
	s32 max_HP;
	s32 MP;
	s32 max_MP;
	u32 attack;
	u32 defense;
	u32 speed;

	u32 EXP;
	u32 Level;
	
	// The internal model ID.
	CHR_Model model;

	// Input Manager
	InputReciever * control;

	// Special Free Camera
	scene::ICameraSceneNode * freecam;

	// Now for a hell of a lot of bools...
	bool isMoving;
	bool isJumping;
	bool isFalling;
	bool isFreeCam;

	bool Light_on;

	scene::IBillboardSceneNode * D_Light;
	scene::IBillboardSceneNode * Target_Arrow;

	s32 fall_time;

	bool deadly_fall;

	u32 combo;
	u32 combo_time;

	s32 A_End;
	s32 B_End;
	s32 C_End;

	core::vector3df prev_pos;


	scene::IBillboardSceneNode * recall;

	bool A_Pressed,A_Held;
	bool B_Pressed,B_Held;
	bool X_Pressed,X_Held;
	bool Y_Pressed,Y_Held;

	s32 A_HC,B_HC,X_HC,Y_HC;

	bool Up,Down,Left,Right;

	// Movement-Related
	f32 fallspeed;

	f32 max_velocity;


	core::vector3df direction;
	core::vector3df gravity;
	f32 camDist;
	f32 Jump;
	f32 maxJump;
	f32 incJump;
	f32 maxincJump;

	// Animations
	sequence Anims[64];
	s32 anim_num;


	// Player Stats!

	u32 mp_saved;

	// 40 64
	gui::IGUIImage * EXP_bar;

};

#endif