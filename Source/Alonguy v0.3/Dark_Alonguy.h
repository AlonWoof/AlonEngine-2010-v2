#include "character.h"

class Dark_Alonguy : public Character
{
	public:
		Dark_Alonguy(irr::IrrlichtDevice * dev,irrklang::ISoundEngine * aud) : Character(dev,aud){};

		void Spawn (CHR_Model mdl,core::vector3df spawnpos);
		void chase (scene::ISceneNode * target);

};