#include "object.h"

Object::Object(irr::IrrlichtDevice *dev,irrklang::ISoundEngine * aud)
{
	device = dev;
	audio = aud;
	body = 0;
	moved = false;
}

void Object::setPosition(irr::core::vector3df newpos)
{
	pos = newpos;
	moved = true;
}

void Object::setRotation(irr::core::vector3df newrot)
{
	rot = newrot;
	moved = true;
}

void Object::setScale(irr::core::vector3df newscale)
{
	scale = newscale;
}

void Object::update()
{
	// We bring the 3 transformations to
	// the actual object.
	body->setPosition(pos);
	body->setRotation(rot);
	body->setScale(scale);

}