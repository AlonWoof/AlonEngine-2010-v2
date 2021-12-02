#include "npc.h"


void NPC::Spawn(irr::core::vector3df spawnpos)
{
	scene::ISceneManager * smgr = device->getSceneManager();

	body = smgr->addAnimatedMeshSceneNode(smgr->getMesh("npc.b3d"),0,-1,spawnpos);

	core::stringc skin_name;
	core::stringc top_name;
	core::stringc bottom_name;
	core::stringc eye_name;
	core::stringc hair_name;

	skin_name += "npc";
	skin_name += Random(0,3);
	skin_name += Random(0,1);
	skin_name += ".png";

	top_name += "top";
	top_name += Random(0,3);
	top_name += ".png";

	bottom_name += "bottom";
	bottom_name += Random(0,3);
	bottom_name += ".png";

	hair_name += "hair";
	hair_name += Random(0,5);
	hair_name += ".png";

	eye_name += "eye";
	eye_name += Random(0,7);
	eye_name += ".png";


	breast_size = ((f32)Random(10,30))/10;
	hair_length = ((f32)Random(7,10))/10;

	butt_size = ((f32)Random(10,20))/10;

	video::IVideoDriver * vmgr = device->getVideoDriver();

	skin.setTexture(0,vmgr->getTexture(skin_name));
	hair.setTexture(0,vmgr->getTexture(hair_name));
	eye.setTexture(0,vmgr->getTexture(eye_name));

	top.MaterialType = video::EMT_TRANSPARENT_ALPHA_CHANNEL;
	bottom.MaterialType = video::EMT_TRANSPARENT_ALPHA_CHANNEL;
	hair.setFlag(video::EMF_BACK_FACE_CULLING,false);

	body->getMaterial(0) = skin;
	body->getMaterial(1) = hair;
	body->getMaterial(2) = eye;

	br = 0;
	bt_l = 0;
	bt_r = 0;
	hr = 0;

	br = body->getJointNode("breasts");
	bt_l = body->getJointNode("L_buttock");
	bt_r = body->getJointNode("R_buttock");
	hr = body->getJointNode("hair2");

	if (br)
		br->setScale(core::vector3df(breast_size,breast_size,breast_size));

	if (bt_l && bt_r)
	{
		bt_l->setScale(core::vector3df(butt_size,1,butt_size));
		bt_r->setScale(core::vector3df(butt_size,1,butt_size));
	}

	butt_size = 2;


}
void NPC::update()
{
	body->setJointMode(scene::EJUOR_CONTROL);
		
	body->animateJoints();

		//if (br)
		//	br->setScale(core::vector3df(breast_size,breast_size,breast_size));

		//if (bt_l && bt_r)
		//{
		//	bt_l->setScale(core::vector3df(butt_size,1,butt_size));
		//	bt_r->setScale(core::vector3df(butt_size,1,butt_size));
		//}
		//w
	
}
