

#include "character.h"

void Character::Spawn(CHR_Model mdl, irr::core::vector3df spawnpos)
{
	scene::ISceneManager * smgr = device->getSceneManager();
	gui::IGUIEnvironment * gmgr = device->getGUIEnvironment();
	video::IVideoDriver * vmgr = device->getVideoDriver();

	cam = smgr->addCameraSceneNode();
	cutscene_cam = smgr->addCameraSceneNode(); 
	freecam = smgr->addCameraSceneNodeFPS();

	model = mdl;

	old_time = device->getTimer()->getTime();

	CS_border = gmgr->addImage(vmgr->getTexture("cs_border.png"),core::vector2d<s32>(0,0));
	CS_border->setVisible(false);

	Target_Arrow = smgr->addBillboardSceneNode(0,core::dimension2df(5,5));
	Target_Arrow->setMaterialTexture(0,vmgr->getTexture("Target_Arrow.png"));
	Target_Arrow->setMaterialType(video::EMT_TRANSPARENT_ALPHA_CHANNEL);
	Target_Arrow->setMaterialFlag(video::EMF_LIGHTING,false);
	Target_Arrow->setVisible(false);
	smgr->addLightSceneNode(Target_Arrow,core::vector3df(0,0,0),video::SColorf(0.5f,0.5f,0.5f),20);

	D_Light = 0;
	control = 0;

	core::array<video::ITexture*> textures_3;
	for (s32 g=5; g > -1; --g)
	{
		core::stringc tmp;
		tmp = "dark_ball";
		tmp += g;
		tmp += ".png";
		video::ITexture* t = device->getVideoDriver()->getTexture(tmp.c_str());
		textures_3.push_back(t);
	}

	scene::ISceneNodeAnimator * dark2 =	smgr->createTextureAnimator(textures_3, 100);


	isFreeCam = false;
	cutscene = false;
	Anim_Override = false;

	dormant = false;

	active_camo = true;

	Damage_Timeout = false;
	stun_time = 0;

	velocity = 0;
	max_velocity = 1;
	camDist = 16;
	Jump = 0;
	maxJump = 10;
	incJump = 0;
	maxincJump = 5.0f;
	
	/////////
	// Stats!
	/////////
	
	if (model == Alonguy)
	{
		HP = 50;
		max_HP = 50;
		attack = 8;
		defense = 5;
		speed = 6;

		MP = 50;
		max_MP = 50;

		EXP = 0;
		Level = 1;
	}
	else if (model == Maria)
	{
	  	HP = 75;
		max_HP = 75;
		attack = 3;
		defense = 7;
		speed = 6;

		MP = 100;
		max_MP = 100;

		EXP = 0;
		Level = 1;
	}

	numAnims = 0;


	switch(mdl)
	{
	case Alonguy:
		char_name = "Alonguy";
		break;

	case Maria:
		char_name = "Maria";
		break;

	default:
		char_name = "Alonguy";
		break;
	}

	core::stringc modelname;
	core::stringc ASQname;

	modelname += char_name;
	modelname += ".b3d";

	body = smgr->addAnimatedMeshSceneNode(smgr->getMesh(modelname),0,OBJ_PLAYER);
	ASQname +=	char_name;
	ASQname +=	"_anim.asq";


	if (body)
		body->setAnimationSpeed(0);


	body->setPosition(spawnpos);

	D_Light = smgr->addBillboardSceneNode(body,core::dimension2df(5,5));

	smgr->addLightSceneNode(D_Light,core::vector3df(0,0,0),video::SColorf(0.125f,0.0f,0.5f),50);

	D_Light->setVisible(false);
		
	D_Light->addAnimator(dark2);
	D_Light->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
	D_Light->setMaterialFlag(video::EMF_LIGHTING,false);
	smgr->addLightSceneNode(D_Light,core::vector3df(0,0,0),video::SColorf(0.125f,0.0f,0.5f),50);

	Up = false;
	Down = false;
	Left = false;
	Right = false;

	isAttacking = false;
	target_lock = false;
	deadly_fall = false;
	isClimbing = false;
	isSwimming = false;

	topdown_view = false;

	target = 0;

	combo_time = 0;
	combo = 0;
	fall_time = 0;


	  // -60 -60 475

	body->setRotation(core::vector3df(0,0,0));

	LoadSeq(ASQname,"Idle");
	LoadSeq(ASQname,"Idle_Combo");
	LoadSeq(ASQname,"Run_Combo");
	LoadSeq(ASQname,"Walk");
	LoadSeq(ASQname,"Run");
	LoadSeq(ASQname,"Jump");
	LoadSeq(ASQname,"Victory");
	LoadSeq(ASQname,"Attack_A");
	LoadSeq(ASQname,"Attack_B");
	LoadSeq(ASQname,"Attack_C");
	LoadSeq(ASQname,"Air_Attack_A");
	LoadSeq(ASQname,"Air_Attack_B");
	LoadSeq(ASQname,"Falling");
	LoadSeq(ASQname,"Hit_Light");
	LoadSeq(ASQname,"Hit_Heavy");
	LoadSeq(ASQname,"Hit_Recover");
	LoadSeq(ASQname,"Death");


	origin = spawnpos;

}

void Character::SetControl(InputReciever * cntrl)
{
	// Safety Check (If there's no body, there's DEFINITLY no camera...)
	if (!body)
		return;

	// Set the active camera to the Player's
	device->getSceneManager()->setActiveCamera(cam);

	// And set the correct input manager!
	control = cntrl;
}

void Character::update()
{
	// So you can load new levels.

	if (dormant)
		return;

	//  Safety check... even though I'd never trigger this
	// one on purpose!

	

	if (!body)
		return;


	if (isClimbing || isSwimming && col->getGravity().Y == -5)
	{
		col->setGravity(core::vector3df(0,-0.1f,0));
	}
	else if (col->getGravity() != core::vector3df(0,-5,0))
	   col->setGravity(core::vector3df(0,-5,0));

	CS_border->setVisible(cutscene);





	if (Damage_Timeout && !cutscene)
	{
		if (body->getFrameNr() + 0.75f > body->getEndFrame())
		{
			Anim_Override = false;
			body->setLoopMode(true);
			Damage_Timeout = false;
		}
	}

	if (!moved)
	{
		pos = body->getAbsolutePosition();
		rot = body->getRotation();
	}

	if (Y_Held && !Light_on && MP > 0)
	{
		D_Light->setVisible(true);
		mp_saved = 0;
	}

	if (D_Light->isVisible() && !Y_Held)
	{
		Light_on = true;

		if (MP == 0)
			D_Light->setVisible(false);
	}

	if (Y_Held && Light_on)
	{
		D_Light->setVisible(false);
	}

	if (!D_Light->isVisible() && !Y_Held)
	{
		Light_on = false;
	}

	if (Light_on)
	{
		mp_saved++;

		if (mp_saved > 100)
		{
			mp_saved = 0;
			MP--;

		}
	}

	if (MP < 0)
		MP = 0;
	if (MP > max_MP)
		MP = max_MP;

	if (HP < 0)
		HP = 0;
	if (HP > max_HP)
		HP = max_HP;

	if (isAttacking)
	{
		core::vector3df tmp_rot = rot;
		tmp_rot.invert();
		core::vector3df d_start = pos + core::vector3df(0,7.5f,0);
		d_start.rotateXZBy(tmp_rot.Y,pos);
		core::vector3df d_end = d_start + core::vector3df(0,0,10);
		d_end.rotateXZBy(tmp_rot.Y,pos);
		damage.start =	d_start;
		damage.end = d_end;
	}

	move();
	animate();
	step_sound();

	if (target && !target_lock)
	{
		if (target->getAbsolutePosition().getDistanceFrom(pos) > 20)
			target = 0;
	}

	if (target_lock && target)
	{
		cam->setTarget(pos.getInterpolated(target->getAbsolutePosition(),0.5f));

		if (target->getAbsolutePosition().getDistanceFrom(pos) > 50)
			target = 0;
	}

	if (X_Held && target)
	{
		if (!target_lock)
			audio->play2D("./data/audio/sfx/target_on.ogg");
		target_lock = true;
		Target_Arrow->setVisible(true);
		Target_Arrow->setPosition(target->getAbsolutePosition() + core::vector3df(0,5,0));
	}
	else
	{
		if (target_lock)
			audio->play2D("./data/audio/sfx/target_off.ogg");

		target_lock = false;
		Target_Arrow->setVisible(false);
	}

	if (!isFreeCam && !cutscene)
		checkInput();
	else
	{
		A_Pressed = false;
		A_Held = false;
		B_Pressed = false;
		B_Held = false;
		X_Pressed = false;
		X_Held = false;
		Y_Pressed = false;
		Y_Held = false;

		Up = false;
		Left = false;
		Right = false;
		Down = false;
	}

	if (isFreeCam)
	{
		device->getSceneManager()->setActiveCamera(freecam);

		if (control->IsKeyDown(KEY_KEY_F))
		{
			pos = (freecam->getAbsolutePosition() + (freecam->getRotation().rotationToDirection() * 8));
		}

		if (control->IsKeyDown(KEY_KEY_C))
		{
			isFreeCam = false;
			device->getSceneManager()->setActiveCamera(cam);
		}
	}

	if (cutscene)
	{
		device->getSceneManager()->setActiveCamera(cutscene_cam);

	}

	/*if (control->IsKeyDown(KEY_KEY_V))
	{
			isFreeCam = true;
			freecam->setPosition(pos + core::vector3df(0,10,0));
	}*/

	core::stringw Str;
	Str += "Alonguy test Engine -";
	Str += " FPS: ";
	Str += device->getVideoDriver()->getFPS();
	device->setWindowCaption(Str.c_str());

	if (prev_pos.Y - pos.Y > 0.5f)
	{
		isFalling = true;
	}
	else
		isFalling = false;

	if (prev_pos.Y - pos.Y < -0.5f)
	{
		isJumping = true;
	}
	else
		isJumping = false;

	prev_pos = pos;


	// Should've inherited from Object.h.... oh well!

	new_time = device->getTimer()->getTime();
	core::vector3df old_pos = body->getAbsolutePosition();
	core::vector3df pos_dir = old_pos - pos;
	core::vector3df old_cpos = cam->getAbsolutePosition();
	core::vector3df cpos_dir = old_cpos - cpos;

	u32 time_diff = new_time - old_time;

	body->setPosition(pos);

	

 	cam->setPosition(cpos);
	camcol->setTargetNode(cam);
	cam->setTarget(pos + core::vector3df(0,5,0));

	body->setRotation(rot);
	//body->setScale(scale);

	moved = false;

	old_time = device->getTimer()->getTime();

}

void Character::deactivate()
{
	dormant = true;
	isAttacking = false;
	isFalling = false;
	
	device->getSceneManager()->addToDeletionQueue(body);
	device->getSceneManager()->addToDeletionQueue(cam);
	device->getSceneManager()->addToDeletionQueue(freecam);
	device->getSceneManager()->addToDeletionQueue(Target_Arrow);
	device->getSceneManager()->addToDeletionQueue(D_Light); 

	body = 0;
	cam = 0;
	camcol = 0;
	freecam = 0;
	D_Light = 0;
	Target_Arrow = 0;

}

void Character::reactivate(core::vector3df new_pos = core::vector3df(0,20,0))
{
	scene::ISceneManager * smgr = device->getSceneManager();
	video::IVideoDriver * vmgr = device->getVideoDriver();

	core::stringc mesh;
	mesh += char_name;
	mesh += ".b3d";

	body = smgr->addAnimatedMeshSceneNode(smgr->getMesh(mesh));

	cam = smgr->addCameraSceneNode();
	cutscene_cam = smgr->addCameraSceneNode(); 
	freecam = smgr->addCameraSceneNodeFPS();

	Target_Arrow = smgr->addBillboardSceneNode(0,core::dimension2df(5,5));
	Target_Arrow->setMaterialTexture(0,vmgr->getTexture("Target_Arrow.png"));
	Target_Arrow->setMaterialType(video::EMT_TRANSPARENT_ALPHA_CHANNEL);
	Target_Arrow->setMaterialFlag(video::EMF_LIGHTING,false);
	Target_Arrow->setVisible(false);
	smgr->addLightSceneNode(Target_Arrow,core::vector3df(0,0,0),video::SColorf(0.5f,0.5f,0.5f),20);

	D_Light = 0;

	core::array<video::ITexture*> textures_3;
	for (s32 g=5; g > -1; --g)
	{
		core::stringc tmp;
		tmp = "dark_ball";
		tmp += g;
		tmp += ".png";
		video::ITexture* t = device->getVideoDriver()->getTexture(tmp.c_str());
		textures_3.push_back(t);
	}

	scene::ISceneNodeAnimator * dark2 =	smgr->createTextureAnimator(textures_3, 100);

	D_Light = smgr->addBillboardSceneNode(body->getJointNode("L_Hand"),core::dimension2df(5,5));

	smgr->addLightSceneNode(D_Light,core::vector3df(0,0,0),video::SColorf(0.125f,0.0f,0.5f),50);

	D_Light->setVisible(false);
		
	D_Light->addAnimator(dark2);
	D_Light->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
	D_Light->setMaterialFlag(video::EMF_LIGHTING,false);
	smgr->addLightSceneNode(D_Light,core::vector3df(0,0,0),video::SColorf(0.125f,0.0f,0.5f),50);

	dormant = false;

	origin = new_pos;
}

void Character::move()
{
	scene::ISceneManager * smgr = device->getSceneManager();

	f32 camHeight = 8;

	if (topdown_view)
		camHeight = 40;

	cpos = pos + core::vector3df(0,camHeight,20);
	cpos.rotateXZBy(cam->getRotation().Y,pos);
	cpos.rotateYZBy(cam->getRotation().X,pos);
	cam->setTarget(body->getPosition());
	cam->bindTargetAndRotation(false);
	core::vector3df crot = cam->getRotation();

	camDist = cpos.getDistanceFrom(pos);

	if (camDist > 30)
	{
		camcol->setEllipsoidRadius(core::vector3df(0,0,0));
	}
	else if (camcol->getEllipsoidRadius() == core::vector3df(0,0,0))
	{
		camcol->setEllipsoidRadius(core::vector3df(1,1,1));
	}

	if (B_Pressed && !isAttacking && !isClimbing)
	{
		Attack();
	}

	if (combo_time > 0)
		combo_time--;

	if (combo_time == 0)
		combo = 0;

	if (control)
	{

		if (control->IsKeyDown(KEY_KEY_Q))
			crot.Y += 2;
		if (control->IsKeyDown(KEY_KEY_W))
			crot.Y -= 2;
	}

	//if (B_HC > 20)
	//{
	//	pos = core::vector3df(0,20,0);
	//}

	cam->setRotation(crot);

	crot.X = 0;

	crot.invert();
	
	if (!isAttacking && !isClimbing)
	{

	if (Down)
	{
		velocity += (max_velocity / 20);

		if (velocity > max_velocity)
			velocity = max_velocity;

		core::vector3df dir = (crot - core::vector3df(0,0,0)).rotationToDirection();
		direction.interpolate(direction,dir,0.5f);
	}

	if (Up)
	{
		velocity += (max_velocity / 20);

		if (velocity > max_velocity)
			velocity = max_velocity;

		core::vector3df dir = (crot - core::vector3df(0,180,0)).rotationToDirection();
		direction.interpolate(direction,dir,0.5f);
	}

	if (Left)
	{
		velocity += (max_velocity / 20);

		if (velocity > max_velocity)
			velocity = max_velocity;

		core::vector3df dir = (crot - core::vector3df(0,-90,0)).rotationToDirection();
		direction.interpolate(direction,dir,0.5f);
	}

	if (Right)
	{
		velocity += (max_velocity / 10);

		if (velocity > max_velocity)
			velocity = max_velocity;

		core::vector3df dir = (crot - core::vector3df(0,90,0)).rotationToDirection();
		direction.interpolate(direction,dir,0.5f);
	}


	if (A_Pressed && !A_Held && !isJumping)
	{
		jump();
	}



	}
	else if (isClimbing)
	{
			if (Up)
				pos.Y += 0.5f;

			if (Down)
				pos.Y -= 0.5f;

			if (A_Pressed)
			{
				direction.invert();
				velocity = 1;
				isClimbing = false;
			}


	}

	if (!Up && !Down && !Left && !Right && velocity > 0)
	{
		velocity -= (max_velocity / 10);
	}

	if (velocity < 0)
	{
		velocity = 0;
	}

	//move_time = device->getTimer()->getRealTime();
	

	rot = direction.getHorizontalAngle();
	pos += direction * velocity;

	if (rot.X != 0)
		rot.X = 0;
	if (rot.Z != 0)
		rot.Z = 0;

	if (pos.getDistanceFrom(body->getAbsolutePosition()) > 5)
	{
		col->setAnimateTarget(false);
		body->setPosition(pos);
		col->setAnimateTarget(true);
	}

	if (pos.getDistanceFrom(body->getAbsolutePosition()) > 5)
	{
		col->setAnimateTarget(false);
		body->setPosition(pos);
		col->setAnimateTarget(true);
	}

	if (target_lock)
	{
		core::vector3df dest = target->getAbsolutePosition();
		core::vector3df mid = (dest - pos);
		core::vector3df new_rot = mid.getHorizontalAngle();
		new_rot.X = 0;
		new_rot.Z = 0;
		rot = new_rot;
	}


	if (Jump > 0)
	{
		pos.Y += incJump;
		Jump -= 1;
	}

}

void Character::animate()
{

	if (isAttacking && body->getFrameNr() + 0.75f > body->getEndFrame())
	{
		Anim_Override = false;
		body->setLoopMode(true);
		isAttacking = false;
	}

	if (Anim_Override)
		return;

	if (isFalling)
	{
		PickSeq("Falling");
	}
	else if (velocity > 0.75)
	{
		if (combo_time > 0)
		{
			PickSeq("Run_Combo");
		}
		else
			PickSeq("Run");

		if (model == Maria)
			body->setAnimationSpeed(10/velocity);
		else
			body->setAnimationSpeed(6.25f/velocity);
	}
	else if (velocity < 0.75 && velocity > 0)
	{
		PickSeq("Walk");
		body->setAnimationSpeed(3/velocity);
	}
	else if (combo_time > 0)
	{
		step1 = false;
		step2 = false;
		step3 = false;
		PickSeq("Idle_Combo");
	}
	else
	{
		step1 = false;
		step2 = false;
		step3 = false;
		PickSeq("Idle");
	}
}

void Character::checkInput()
{
	A_Pressed = false;
	A_Held = false;
	B_Pressed = false;
	B_Held = false;
	X_Pressed = false;
	X_Held = false;
	Y_Pressed = false;
	Y_Held = false;

	
	if (!control)
		return;



	if (control->IsKeyDown(KEY_KEY_X))
	{
		A_Pressed = true;
		A_HC++;
		if (A_HC > 10 && !A_Held)
		{
			A_Held = true;
		}
	}
	else
		A_HC = 0;

	if (control->IsKeyDown(KEY_KEY_Z))
	{
		B_Pressed = true;
		B_HC++;
		if (B_HC > 10 && !B_Held)
		{
			B_Held = true;
		}
	}
	else
		B_HC = 0;

	if (control->IsKeyDown(KEY_KEY_A))
	{
		X_Pressed = true;
		X_HC++;
		if (X_HC > 10 && !X_Held)
		{
			X_Held = true;
		}
	}
	else
		X_HC = 0;

   	if (control->IsKeyDown(KEY_KEY_S))
	{
		Y_Pressed = true;
		Y_HC++;
		if (Y_HC > 10 && !Y_Held)
		{
			Y_Held = true;
		}
	}
	else
		Y_HC = 0;

	Up = control->IsKeyDown(KEY_UP);
	Down = control->IsKeyDown(KEY_DOWN);
	Left = control->IsKeyDown(KEY_LEFT);
	Right = control->IsKeyDown(KEY_RIGHT);

	/*if (control->IsKeyDown(KEY_KEY_D))
	{
		recall->setPosition(pos + core::vector3df(0,5,0));
	}

	if (control->IsKeyDown(KEY_KEY_F))
	{
		pos = recall->getAbsolutePosition();
	}

	if (control->IsKeyDown(KEY_KEY_Q))
	{
		
	}  */
}

void Character::setGravity(scene::IMetaTriangleSelector * world)
{
	if (!body)
		return;

	scene::ISceneManager * smgr = device->getSceneManager();
	core::vector3df box = body->getBoundingBox().getExtent();


	box.Z = 3;
	box.X = 3;
	box.Y -= (box.Y / 4);
	core::vector3df grav(0,-5,0);
	core::vector3df trans(0,(0 - box.Y + 0.5f),0);
	
	col = smgr->createCollisionResponseAnimator(world,body,box,grav,trans,0.000001f);
	body->addAnimator(col);

	camcol = smgr->createCollisionResponseAnimator(world,cam,core::vector3df(1,1,1),core::vector3df(0,0,0));
	cam->addAnimator(camcol);

	//scene::IMeshSceneNode * ball = smgr->addSphereSceneNode(1,16);
	//ball->setScale(box);
	//ball->setPosition(trans);

	body->setPosition(origin);
	pos = origin;
}

void Character::LoadSeq(core::stringc path,char * name)
{
	core::stringc anim(name);
	ReadSeq(device,path,&Anims[numAnims],name);

	if (anim == "Attack_A")
		A_End = Anims[numAnims].end;
	if (anim == "Attack_B")
		B_End = Anims[numAnims].end;
	if (anim == "Attack_C")
		C_End = Anims[numAnims].end;
	
	numAnims++;
}

void Character::PickSeq(char * name)
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

void Character::moveTo(irr::core::vector3df dest,f32 speed)
{		
	pos = body->getAbsolutePosition();

	dest.Y = pos.Y;

	if (pos.getDistanceFrom(dest) < 2.5f)
		return;

	core::vector3df mid = (dest - pos);
	direction = mid.getHorizontalAngle().rotationToDirection();

	velocity = speed;
	core::vector3df dir = (body->getRotation()).rotationToDirection();
	direction.interpolate(direction,dir,0.5f);
}
void Character::Attack()
{
	if (!B_Held)
	{
		if (!isFalling)
		{
			if (target_lock)
			{
				core::vector3df dest = target->getAbsolutePosition();
				core::vector3df mid = (dest - pos);
				core::vector3df new_rot = mid.getHorizontalAngle().rotationToDirection();
				direction = new_rot;
			}

			core::stringc path;
			path += "./data/audio/VC/";
			path += char_name;
			path += "/";

			if (combo == 2 && combo_time < 30 && combo_time > 0)
			{
				PickSeq("Attack_C");
				body->setLoopMode(false);
				body->setAnimationSpeed(10);
				Anim_Override = true;
				combo = 0;
				combo_time = 0;
				isAttacking = true;
				audio->play2D("./data/audio/SFX/Slash3.ogg");
				path +=	"Slash_C00.ogg";
				audio->play2D(path.c_str());

				velocity = 2;
			}
			else if (combo == 1 && combo_time < 30 && combo_time > 0)
			{
				PickSeq("Attack_B");
				body->setAnimationSpeed(7.5f);
				body->setLoopMode(false);
				Anim_Override = true;
				combo = 2;
				combo_time = 60;
				isAttacking = true;
				audio->play2D("./data/audio/SFX/Slash2.ogg");

				switch(Random(0,3))
				{
				case 0:
					path +=	"Slash_B00.ogg";
					audio->play2D(path.c_str());
					break;
				case 1:
					path +=	"Slash_B01.ogg";
					audio->play2D(path.c_str());
					break;
				case 2:
					path +=	"Slash_B02.ogg";
					audio->play2D(path.c_str());
					break;
				}

				velocity = 1;
			}
			else if (combo_time == 0)
			{
				PickSeq("Attack_A");
				body->setAnimationSpeed(7.5f);
				body->setLoopMode(false);
				Anim_Override = true;
				combo = 1;
				combo_time = 60;
				isAttacking = true;
				audio->play2D("./data/audio/SFX/Slash1.ogg");
				
				switch(Random(0,3))
				{
				case 0:
					path +=	"Slash_A00.ogg";
					audio->play2D(path.c_str());
					break;
				case 1:
					path +=	"Slash_A01.ogg";
					audio->play2D(path.c_str());
					break;
				case 2:
					path +=	"Slash_A02.ogg";
					audio->play2D(path.c_str());
					break;
				}

				velocity = 1;
			}

		}

	}
}
void Character::Damage(s32 amount,core::vector3df from,bool heavy)
{
	if (Damage_Timeout)
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
	num->addAnimator(smgr->createFlyStraightAnimator(core::vector3df(0,10,0),core::vector3df(0,20,0),1000));
	num->addAnimator(smgr->createDeleteAnimator(2000));

	core::stringc path_2;
	path_2 += "./data/audio/vc/";
	path_2 += char_name;
	path_2 += "/hurt";
	path_2 += Random(0,4);
	path_2 += ".ogg";

	audio->play2D(path_2.c_str());

	Damage_Timeout = true;
	Anim_Override = true;
	body->setLoopMode(false);
	body->setAnimationSpeed(10);
	PickSeq("Hit_Light");

	HP -= amount;
}
void Character::AwardEXP(s32 amount)
{
	EXP += amount;

	if (EXP > Level * 10)
	{
		audio->play2D("./data/audio/VC/level_up.ogg");
		audio->play2D("./data/audio/sfx/levelup.ogg");

		attack += Random(0,Level);
		defense += Random(0,Level);
		speed += Random(0,Level);
		max_HP += Random(10,Level * 10);
		max_MP += Random(5,Level * 5);
		Level++;

		EXP = 0;
	}
}


void Character::step_sound()
{
	// 38 36 34 32 30 

	s32 fr0 = 36;
	s32 fr1 = 34;
	s32 fr2 = 31;
	s32 fr3 = 32;
	s32 fr4 = 29;

	if (body->getFrameNr() > fr4 - 0.5f && body->getFrameNr() < fr4 + 0.5f && !step1)
	{
					core::stringc sfx;
			sfx += "./data/audio/SFX/step/";

			switch (floor_ID)
			{
				case WATER:
					sfx += "water";
					break;
				case GRASS:
					sfx += "grass";
					break;
				default:
					sfx += "default";
					break;
			}

			sfx += Random(0,3);
			sfx += ".ogg";

			audio->play2D(sfx.c_str());

			step1 = true;
			step2 = false;
	}

	if (body->getFrameNr() > fr2 - 0.5f && body->getFrameNr() < fr2 + 0.5f && !step2)
	{
					core::stringc sfx;
			sfx += "./data/audio/SFX/step/";

			switch (floor_ID)
			{
				case WATER:
					sfx += "water";
					break;
				case GRASS:
					sfx += "grass";
					break;
				default:
					sfx += "default";
					break;
			}

			sfx += Random(0,3);
			sfx += ".ogg";

			audio->play2D(sfx.c_str());

			step1 = false;
			step2 = true;
	}

	floor_ID = 0;

}

void Character::jump()
{
	if (!isFalling && !isJumping)
	{
		Jump = maxJump;
		incJump = 2.5f;
	}


}