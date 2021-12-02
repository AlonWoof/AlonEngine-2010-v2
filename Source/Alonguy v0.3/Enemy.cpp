#include "enemy.h"

void MiniShoop::Spawn(core::vector3df spawn_pos,s32 level)
{
	scene::ISceneManager * smgr = device->getSceneManager();
	body = smgr->addAnimatedMeshSceneNode(smgr->getMesh("mini_shoop.b3d"),0,ENEMY_MINISHOOP,spawn_pos);
    lazor = smgr->addAnimatedMeshSceneNode(smgr->getMesh("lazor.b3d"),body,-1);
	lazor->setMaterialFlag(video::EMF_LIGHTING,false);


	anim_num = -1;
	numAnims = 0;
	direction = direction = core::vector3df(0,0,1);
	origin = spawn_pos;
	direction_change = 0;

	lazer_snd = 0;

	hitStun = false;
	give_EXP = false;



	HP = 20;
	max_HP = 20;

	attack = Random(4 + level,10 + level);
	defense = Random(0 + level,5 + level);
	speed = Random(1 + level,3 + level);

	award_exp = Random(5 + level,10 + level);

	target_lock = false;
	target = 0;

	target_time = 0;

	isMoving = false;
	isAttacking = false;
	isAlive = true;
	dying = false;

	vision = (f32)Random(50,75);

	max_velocity = 1.0f;

	core::stringc path;
	path +=	"mini_shoop_anim.asq";

	LoadSeq(path,"Idle");
	LoadSeq(path,"Fire");
	LoadSeq(path,"Fire_Loop");
	LoadSeq(path,"Fire_End");
	LoadSeq(path,"Hit_A");
	LoadSeq(path,"Hit_B");
	LoadSeq(path,"Die");


}

void MiniShoop::update()
{
   	if (!isAlive)
	{
		body->setVisible(false);
		isAttacking = false;
		hitStun = false;
		return;
	}


	pos = body->getAbsolutePosition();
	rot = body->getRotation();


	if (HP < 0)
		HP = 0;

	Move();
	Animate();

	damage.start = pos;
	damage.end = pos + (direction * 1000);

	if (target_lock && !isAttacking && !hitStun && !dying)
		Chase();
	else if (!isAttacking && !hitStun && !dying)
		Wander(vision);


	if (target && !isAttacking && !hitStun)
	{
		core::vector3df mid = ((target->getAbsolutePosition() + core::vector3df(0,5,0)) - pos);
		direction = mid.getHorizontalAngle().rotationToDirection();
	}

	body->setRotation(rot);
	body->setPosition(pos);
}

void MiniShoop::Move()
{
	if (velocity > max_velocity)
		velocity = max_velocity;

	if (velocity < 0)
		velocity = 0;

	if (velocity > 0)
		pos += (direction * velocity);

	if (velocity > 0)
		isMoving = true;
	else
		isMoving = false;

	rot = direction.getHorizontalAngle();
}

void MiniShoop::Animate()
{

	if (dying)
	{
		if (body->getFrameNr() + 0.5f > body->getEndFrame() && dying)
			isAlive = false;

		return;
	}

	if (hitStun)
	{
		Attack_Phase = 0;
		isAttacking = false;
	}
		

	if (!isAttacking && !hitStun)
	{
		PickSeq("Idle");
		body->setAnimationSpeed(5);
	}
	else if (body->getFrameNr() + 0.5f > body->getEndFrame() && Attack_Phase == 1)
	{
		lazer_snd = audio->play3D("./data/audio/sfx/enemy/mini_shoop/lazer.ogg",pos,false,false,true);
		PickSeq("Fire_Loop");
		body->setAnimationSpeed(10);
		Attack_Phase = 2;
	}
	else if (lazer_snd && Attack_Phase == 2)
	{
		if (lazer_snd->isFinished())
		{
			lazer_snd = 0;
			Attack_Phase = 3;
			body->setAnimationSpeed(5);
			PickSeq("Fire_End");
		}
	}
	else if (body->getFrameNr() + 0.5f > body->getEndFrame() && Attack_Phase == 3)
	{
		isAttacking = false;
	}
	else if (body->getFrameNr() + 0.5f > body->getEndFrame() && hitStun)
		hitStun  = false;


	if (Attack_Phase == 2)
	{
		lazor->setVisible(true);
	}
	else
		lazor->setVisible(false);

	
}

void MiniShoop::LoadSeq(core::stringc path,char * name)
{
	core::stringc anim(name);
	ReadSeq(device,path,&Anims[numAnims],name);	
	numAnims++;
}

void MiniShoop::PickSeq(char * name)
{
	core::stringc anim;
	anim += name;

	for (s32 i = 0; i < numAnims; i++)
	{
		if (Anims[i].name == anim)
		{
			if (anim_num == i)
				return;

			anim_num = i;

			body->setFrameLoop(Anims[i].start,Anims[i].end);
			body->setAnimationSpeed(5);
				return;
		}
	}
}

void MiniShoop::Wander(f32 radius)
{
	if (pos.getDistanceFrom(origin) > radius)
	{
		core::vector3df mid = (origin - pos);
		direction = mid.getHorizontalAngle().rotationToDirection();

		velocity = 0.5f;
		direction_change = 0;
	}

	velocity = 0.5f;


	if (direction_change > Random(100,500))
	{

		direction_change = 0;
		core::vector3df newDir = core::vector3df(0,(f32)Random(45,180),0);

		s32 rand = Random(0,10);

		direction = newDir.rotationToDirection();
		velocity = 0.5f;

	}

	direction_change++;
}

void MiniShoop::setWorld(irr::scene::IMetaTriangleSelector *world)
{
	scene::ISceneManager * smgr = device->getSceneManager();

	scene::ISceneNodeAnimatorCollisionResponse * col = smgr->createCollisionResponseAnimator(world,body,core::vector3df(5,5,5),core::vector3df(0,0,0));
	body->addAnimator(col);
}
void MiniShoop::Attack()
{
	if (!isAttacking)
	{
		audio->play3D("./data/audio/sfx/enemy/mini_shoop/fire.ogg",pos);
		PickSeq("Fire");
		isAttacking = true;

		Attack_Phase = 1;

		velocity = 0;
	}
}

void MiniShoop::Chase()
{
	if (!target)
		return;

	if (target->getAbsolutePosition().getDistanceFrom(pos) > vision)
	{
		target_lock = false;
		target = 0;
		return;
	}

	if (target->getAbsolutePosition().Y > pos.Y + 10)
	{
		target_lock = false;
		target = 0;
		return;
	}
	
	if (target->getAbsolutePosition().getDistanceFrom(pos) < vision/4)
	{
		core::vector3df mid = (target->getAbsolutePosition() - pos);
		direction = mid.getHorizontalAngle().rotationToDirection();
		target_time = 0;
	}
	else if	(pos.Y > target->getAbsolutePosition().Y + 10)
	{
		pos.Y -= 0.5f;
	}
	else
	{
		velocity = 0;
		target_time++;
	}

	if (target_time > Random(50,150))
	{
		Attack();
		target_time = 0;
	}

}
void MiniShoop::Damage(irr::s32 amount, irr::core::vector3df source)
{
	if (hitStun || dying || !isAlive)
		return;

	s32 def = (s32)defense;
	
	amount -= Random(0,def);

	if (amount < 0)
		amount = 0;

	if (amount > 99)
		amount = 99;

	scene::ISceneManager * smgr = device->getSceneManager();
	scene::IBillboardSceneNode * num = smgr->addBillboardSceneNode(body,
		core::dimension2df(5,5),
		core::vector3df(0,5,0));

	core::stringc path;
	path += amount;
	path += ".png";

	num->setMaterialType(video::EMT_TRANSPARENT_ALPHA_CHANNEL);
	num->setMaterialTexture(0,device->getVideoDriver()->getTexture(path));
	num->addAnimator(smgr->createFlyStraightAnimator(core::vector3df(0,5,0),core::vector3df(0,10,0),1000));
	num->addAnimator(smgr->createDeleteAnimator(2000));


	hitStun = true;

	body->setAnimationSpeed(10);

	HP -= amount;

	if (Random(0,1) == 1 && HP > 0)
		PickSeq("Hit_A");
	else if (HP > 0)
		PickSeq("Hit_B");

	if (HP > 0)
	{
		core::stringc path_2;
		path_2 += "./data/audio/sfx/enemy/mini_shoop/hurt";
		path_2 += Random(0,3);
		path_2 += ".ogg";

		audio->play2D(path_2.c_str());
	}
	else
	{
		core::stringc path_2;
		path_2 += "./data/audio/sfx/enemy/mini_shoop/die";
		path_2 += Random(0,2);
		path_2 += ".ogg";

		hitStun = false;
		isAttacking = false;

		velocity = 0;

		PickSeq("Die");
		dying = true;
		

		audio->play2D(path_2.c_str());

	}


}

void BOSS_ShoopDaWhoop::Spawn(core::vector3df spawn_pos)
{
	
	scene::ISceneManager * smgr = device->getSceneManager();
	body = smgr->addAnimatedMeshSceneNode(smgr->getMesh("boss_shoop.b3d"),0,BOSS_SHOOPDAWHOOP,spawn_pos);
    lazor = smgr->addAnimatedMeshSceneNode(smgr->getMesh("lazor.b3d"),body,-1);
	lazor->setMaterialFlag(video::EMF_LIGHTING,false);


	anim_num = -1;
	numAnims = 0;
	direction = direction = core::vector3df(0,0,1);
	origin = spawn_pos;
	direction_change = 0;

	lazer_snd = 0;

	hitStun = false;
	give_EXP = false;
	defeated = false;

	HP = 50;
	max_HP = 50;

	attack = Random(8,20);
	defense = Random(5,10);
	speed = Random(10,30);

	award_exp = Random(10,30);

	target_lock = false;
	target = 0;

	target_time = 0;

	isMoving = false;
	isAttacking = false;
	isAlive = true;
	dying = false;

	vision = (f32)Random(100,200);

	max_velocity = 1.0f;

	core::stringc path;
	path +=	"mini_shoop_anim.asq";

	LoadSeq(path,"Idle");
	LoadSeq(path,"Fire");
	LoadSeq(path,"Fire_Loop");
	LoadSeq(path,"Fire_End");
	LoadSeq(path,"Hit_A");
	LoadSeq(path,"Hit_B");
	LoadSeq(path,"Die");

}

void BOSS_ShoopDaWhoop::Damage(s32 amount,core::vector3df source)
{
	if (hitStun || dying || !isAlive || defeated)
		return;

	s32 def = (s32)defense;
	
	amount -= Random(0,def);

	if (amount < 0)
		amount = 0;

	if (amount > 99)
		amount = 99;

	scene::ISceneManager * smgr = device->getSceneManager();
	scene::IBillboardSceneNode * num = smgr->addBillboardSceneNode(body,
		core::dimension2df(5,5),
		core::vector3df(0,5,0));

	core::stringc path;
	path += amount;
	path += ".png";

	num->setMaterialType(video::EMT_TRANSPARENT_ALPHA_CHANNEL);
	num->setMaterialTexture(0,device->getVideoDriver()->getTexture(path));
	num->addAnimator(smgr->createFlyStraightAnimator(core::vector3df(0,5,0),core::vector3df(0,10,0),1000));
	num->addAnimator(smgr->createDeleteAnimator(2000));


	HP -= amount;


	if (HP > 0 && amount > 2)
	{
	   	hitStun = true;
		body->setAnimationSpeed(10);

		core::stringc path_2;
		path_2 += "./data/audio/sfx/enemy/SDW/hurt";
		path_2 += Random(0,4);
		path_2 += ".ogg";

		audio->play2D(path_2.c_str());

	if (Random(0,1) == 1)
		PickSeq("Hit_A");
	else
		PickSeq("Hit_B");
	}
	else
	{
		defeated = true;
	}
}
void BOSS_ShoopDaWhoop::Attack()
{
	if (!isAttacking)
	{
		audio->play3D("./data/audio/sfx/enemy/sdw/fire.ogg",pos);
		PickSeq("Fire");
		isAttacking = true;

		Attack_Phase = 1;

		velocity = 0;
	}
}