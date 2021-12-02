#include "Dark_Alonguy.h"

void Dark_Alonguy::Spawn(CHR_Model mdl,core::vector3df spawnpos)
{
	Character::Spawn(mdl,spawnpos);

	video::IVideoDriver * vmgr = device->getVideoDriver();

	body->getMaterial(0).setTexture(0,vmgr->getTexture("dark body.png"));
	body->getMaterial(2).setTexture(0,vmgr->getTexture("dark body.png"));
	body->getMaterial(4).setTexture(0,vmgr->getTexture("dark body.png"));
	body->getMaterial(6).setTexture(0,vmgr->getTexture("dark body.png"));

	body->getMaterial(1).setTexture(0,vmgr->getTexture("dark head.png"));
	body->getMaterial(3).setTexture(0,vmgr->getTexture("dark head.png"));
	body->getMaterial(5).setTexture(0,vmgr->getTexture("dark head.png"));
}