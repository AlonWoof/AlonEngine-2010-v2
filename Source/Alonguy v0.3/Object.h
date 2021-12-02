
#include "common.h"

#ifndef OBJECT_H
#define OBJECT_H

class Object
{
	public:
		//Constructor and Destructor 
		Object(irr::IrrlichtDevice * dev,irrklang::ISoundEngine * aud);
		~Object()
		{
			// (Empty for now!)
		};

		void setPosition(core::vector3df newpos);
		void setRotation(core::vector3df newrot);
		void setScale(core::vector3df newscale);

		void update();

		core::vector3df getPosition();
		core::vector3df getRotation();
		core::vector3df getScale();

		// The 3 basic properties of a 3d object...
		core::vector3df pos,rot,scale;

		// The scene node itself!
		scene::IAnimatedMeshSceneNode * body;

		scene::ISceneNodeAnimatorCollisionResponse * col;


	protected:

		bool moved;


		//... and a pointer to the main device!
		irr::IrrlichtDevice * device;
		irrklang::ISoundEngine * audio;
};

#endif