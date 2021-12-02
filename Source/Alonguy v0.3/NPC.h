#include "object.h"
#include "ASQ.h"

#ifndef NPC_H
#define NPC_H

class NPC : public Object
{
public:

	NPC(irr::IrrlichtDevice * dev,irrklang::ISoundEngine * aud) : Object(dev,aud){};

	void Spawn(core::vector3df spawnpos);
	void update();

	void lookAt(core::vector3df lookpos);

	video::SMaterial skin;
	video::SMaterial top;
	video::SMaterial bottom;
	video::SMaterial eye;
	video::SMaterial hair;

	f32 breast_size;
	f32 hair_length;
	f32 butt_size;

	scene::IBoneSceneNode * br;
	scene::IBoneSceneNode * bt_l;
	scene::IBoneSceneNode * bt_r;
	scene::IBoneSceneNode * hr;


};





#endif