#include "object.h"
#include "asq.h"

#ifndef ENEMY_H
#define ENEMY_H

class MiniShoop : public Object
{
public:

	MiniShoop(irr::IrrlichtDevice * dev,irrklang::ISoundEngine * aud) : Object(dev,aud){};

	void Spawn(core::vector3df Spawn_Pos,s32 level = 1);
	void Damage(s32 amount,core::vector3df source);

	irrklang::ISound * lazer_snd;

	void update();
	void Move();
	void Animate();

	void Wander(f32 radius);
	void Chase();

	s32 direction_change;
	s32 target_time;

	void  LoadSeq(core::stringc path,char * name);
	void  PickSeq(char * name);

	void Attack();

	void setWorld(scene::IMetaTriangleSelector * world);

	core::vector3df direction;
	core::vector3df origin;

	bool target_lock;
	scene::ISceneNode * target;

	scene::IAnimatedMeshSceneNode * lazor;
	core::line3df damage;

	s32 Attack_Phase;

	bool isMoving;
	bool isAttacking;
	bool hitStun;
	bool isAlive;

	bool dying;

	bool give_EXP;

	// Animations
	sequence Anims[32];
	s32 anim_num;
	s32 numAnims;

	s32 HP;
	s32 max_HP;

	u32 attack;
	u32 defense;
	u32 speed;
	u32 award_exp;

	f32 vision;

	f32 max_velocity;
	f32 velocity;



};

class BOSS_ShoopDaWhoop : public MiniShoop
{
public:
	BOSS_ShoopDaWhoop(irr::IrrlichtDevice * dev,irrklang::ISoundEngine * aud) : MiniShoop(dev,aud){};

	void Spawn(core::vector3df spawn_pos);
	void Damage(s32 amount,core::vector3df source);
	void Attack();

	bool defeated;
};

#endif