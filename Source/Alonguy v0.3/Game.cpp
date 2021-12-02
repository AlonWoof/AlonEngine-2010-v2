#include "game.h"



//-4.717764, -71.324249, 334.985260

GameMan::GameMan()
{

	control = new InputReciever();
	// Initialize the Irrlicht device!
	// This is the beginning of a great game!

#ifndef DEBUG_BUILD
	device = irr::createDevice(video::EDT_NULL);
	core::dimension2d<u32> screensize = device->getVideoModeList()->getDesktopResolution();

	device->closeDevice();
	device = 0;

	device = irr::createDevice(video::EDT_DIRECT3D9,screensize,16,true,false,true,control);
#endif

#ifdef DEBUG_BUILD
	device = irr::createDevice(video::EDT_DIRECT3D9,core::dimension2d<u32>(640,480),32,false,true,true,control);
#endif


	// Assign the major pointers to the managers.
	vmgr = device->getVideoDriver();
	smgr = device->getSceneManager();
	gmgr = device->getGUIEnvironment();
	audio = irrklang::createIrrKlangDevice();

	audio->setDefault3DSoundMaxDistance(50);
	audio->setDefault3DSoundMinDistance(1);
	audio->setRolloffFactor(0.0125f);

	BGM = 0;
	BGM_2 = 0;
	num_targets = 0;
	mini_shoop_num = 0;
	npc_num = 0;

	inBattle = false;
	Map_loaded = false;

	COA_enemy_room = false;




	for (s32 i = 0; i < ENDING; i++)
	{
		Progress_Bits[i] = false;
	}

	portal_A = 0;

	device->getFileSystem()->addFileArchive("./data/common/ENV00.sap");
	device->getFileSystem()->addFileArchive("./data/common/GUI00.sap");
	device->getFileSystem()->addFileArchive("./data/common/Numbers.sap");
	device->getFileSystem()->addFileArchive("./data/vehicle/VEHI00.sap");
	device->getFileSystem()->addFileArchive("./data/obj/crate.sap");
	device->getFileSystem()->addFileArchive("./data/obj/pole.sap");
	device->getFileSystem()->addFileArchive("./data/enemy/mini_shoop.sap");
	device->getFileSystem()->addFileArchive("./data/map/maps.sap");
	device->getFileSystem()->addFileArchive("./data/char/NPC00.sap");
	device->getFileSystem()->addFileArchive("./data/common/magic.sap");

	core::array<video::ITexture*> textures;
	for (s32 g=3; g > -1; --g)
	{
		core::stringc tmp;
		tmp = "fire_ball";
		tmp += g;
		tmp += ".png";
		video::ITexture* t = device->getVideoDriver()->getTexture(tmp.c_str());
		textures.push_back(t);
	}
	magic_fireball = smgr->createTextureAnimator(textures, 100);



}


void GameMan::LoadMap(char *name)
{


   	HP_bar = gmgr->addImage(vmgr->getTexture("HP_bar.png"),core::vector2d<s32>(120,24));
	MP_bar = gmgr->addImage(vmgr->getTexture("MP_bar.png"),core::vector2d<s32>(120,64));

	HP_bar->setScaleImage(true);
	MP_bar->setScaleImage(true);

	core::stringc filename;
	filename += Player->char_name;
	filename += "_face_large.png";

	portrait = gmgr->addImage(vmgr->getTexture(filename),core::vector2d<s32>(16,24));

	UI_overlay = gmgr->addImage(vmgr->getTexture("UI_overlay.png"),core::vector2d<s32>(0,0));


	if (shadow)
	{
		HP_bar_party = gmgr->addImage(vmgr->getTexture("HP_bar.png"),core::vector2d<s32>(526,425));
		MP_bar_party = gmgr->addImage(vmgr->getTexture("MP_bar.png"),core::vector2d<s32>(526,448));

		HP_bar_party->setScaleImage(true);
		MP_bar_party->setScaleImage(true);

		core::stringc filename_2;
		filename_2 += shadow->char_name;
		filename_2 += "_face_small.png";

		portrait_party = gmgr->addImage(vmgr->getTexture(filename_2),core::vector2d<s32>(464,426));

		UI_overlay_party = gmgr->addImage(vmgr->getTexture("UI_overlay_party.png"),core::vector2d<s32>(0,0));

		world = smgr->createMetaTriangleSelector();
	}

	core::stringc meshname;
	meshname += name;
	meshname += ".b3d";

	npc_num = 0;


	env_tex = vmgr->addRenderTargetTexture(core::dimension2du(128,128));

	scene::IAnimatedMesh* levelmesh = smgr->getMesh(meshname);
	smgr->getMeshManipulator()->recalculateNormals(levelmesh->getMesh(0),true,false);
	BSP = 0;
	BSP = smgr->addOctreeSceneNode(levelmesh->getMesh(0),0,OBJ_BSP);
	scene::ITriangleSelector* selector = 0;
	selector = smgr->createOctreeTriangleSelector(
				BSP->getMesh(),BSP, 128);
		BSP->setTriangleSelector(selector);

		BSP->setMaterialType(video::EMT_TRANSPARENT_ALPHA_CHANNEL_REF);


	world->addTriangleSelector(selector);
	selector->drop();

	core::stringc obpath;
	obpath += name;
	obpath += "_obmap.irr";

	smgr->loadScene(obpath);

	core::array<video::ITexture*> textures;
	for (s32 g=4; g > -1; --g)
	{
		core::stringc tmp;
		tmp = "water";
		tmp += g;
		tmp += ".png";
		video::ITexture* t = device->getVideoDriver()->getTexture(tmp.c_str());
		textures.push_back(t);
	}

	water = smgr->createTextureAnimator(textures, 100);

	core::array<video::ITexture*> textures_2;
	for (s32 g=3; g > -1; --g)
	{
		core::stringc tmp;
		tmp = "dark";
		tmp += g;
		tmp += ".png";
		video::ITexture* t = device->getVideoDriver()->getTexture(tmp.c_str());
		textures_2.push_back(t);
	}

	dark =	smgr->createTextureAnimator(textures_2, 100);

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


	dark2 =	smgr->createTextureAnimator(textures_3, 100);

   	core::array<video::ITexture*> textures_4;
	for (s32 g=9; g > -1; --g)
	{
		core::stringc tmp;
		tmp = "waterfall";
		tmp += g;
		tmp += ".png";
		video::ITexture* t = device->getVideoDriver()->getTexture(tmp.c_str());
		textures_4.push_back(t);
	}

	waterfall = smgr->createTextureAnimator(textures_4, 100);

	core::array<scene::ISceneNode *> nodes;
	smgr->getSceneNodesFromType(scene::ESNT_ANY, nodes);

	for (u32 i=0; i < nodes.size(); ++i)
	{
		scene::ISceneNode * node = nodes[i];

		box_count = 0;

		if (node->getID() == OBJ_DOOR && node->getType() == scene::ESNT_MESH)
		{
		 	scene::ITriangleSelector * selector_2 = 0;
			selector_2 = smgr->createTriangleSelector(
				((scene::IMeshSceneNode *)node)->getMesh(),node);
			node->setTriangleSelector(selector_2);

			world->addTriangleSelector(selector_2);
			selector_2->drop();
		}

		if (node->getID() == OBJ_WATERFALL)
		{
			node->addAnimator(waterfall);
			node->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
		}

		if (node->getID() == MAGIC_FIREBALL)
		{
			node->addAnimator(magic_fireball);
			node->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
		}

		if (node->getID() == OBJ_BSP && node->getType() == scene::ESNT_MESH)
		{
			scene::ITriangleSelector * selector_2 = 0;
			selector_2 = smgr->createTriangleSelector(
				((scene::IMeshSceneNode *)node)->getMesh(),node);
			node->setTriangleSelector(selector_2);

			world->addTriangleSelector(selector_2);
			selector_2->drop();
		}


		if (node->getID() == OBJ_WATER)
		{
			node->addAnimator(water);
			node->setMaterialFlag(video::EMF_BACK_FACE_CULLING,false);
			node->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);

			water_block[box_count] = node->getTransformedBoundingBox();
			box_count++;
		}


		if (node->getID() == OBJ_PORTAL_A)
		{
			portal_A = (scene::IAnimatedMeshSceneNode *)node;
			portal_A->addAnimator(smgr->createRotationAnimator(core::vector3df(0,1,0)));
			portal_A->setAnimationSpeed(10);
			portal_A->setFrameLoop(-1,21);
		}

		if (node->getID() == OBJ_TARGET)
		{
			target[num_targets] = node;
			num_targets++;
		}

		if (node->getID() == OBJ_CRATE || node->getID() == OBJ_HEAVY_CRATE)
		{
			core::vector3df grav(0,-5,0);
			
			const core::aabbox3d<f32>& box = node->getBoundingBox();
			core::vector3df radius = box.MaxEdge - box.getCenter();
			core::vector3df trans(0,0,0);
			scene::ISceneNodeAnimatorCollisionResponse * col = smgr->createCollisionResponseAnimator(world,node,radius,grav);
			node->addAnimator(col);
			col->drop();

		}

		if (node->getID() == OBJ_HEAVY_CRATE)
		{


		}

		if (node->getID() == OBJ_DARK1)
		{
			node->addAnimator(dark);
			node->setMaterialType(video::EMT_SPHERE_MAP);
			node->setMaterialFlag(video::EMF_LIGHTING,false);
		}

		if (node->getID() == OBJ_DARK2)
		{
			node->addAnimator(dark2);
			node->setMaterialFlag(video::EMF_LIGHTING,false);
			smgr->addLightSceneNode(node,core::vector3df(0,0,0),video::SColorf(0.125f,0.0f,0.5f),50);
		}

		if (node->getID() == SYS_BGMNAME)
		{
			printf("BGM: ");
			changeBGM((char *)node->getName());
			BGM_loopback = (u32)node->getAbsolutePosition().Z * 10;
			printf((char *)node->getName());
			printf("\n");
		}

		if (node->getID() == RANDOM_NPC && node->getType() == scene::ESNT_ANIMATED_MESH)
		{

		}

		if (node->getID() == NPC_RANDOM)
		{
			if (npc_num < 32)
			{
				npc[npc_num] = new NPC(device,audio);
				npc[npc_num]->Spawn(node->getAbsolutePosition());
				npc_num++;
			}

		}

		if (node->getID() == TRIGGER_ENTER_A)
		{

			if (Exit_Num == 0)
				entrance = node->getAbsolutePosition();
		}

		if (node->getID() == TRIGGER_ENTER_B)
		{

			if (Exit_Num == 1)
				entrance = node->getAbsolutePosition();
		}

		if (node->getID() == TRIGGER_ENTER_C)
		{

			if (Exit_Num == 2)
				entrance = node->getAbsolutePosition();
		}

		if (node->getID() == COA_BOSS_TRIGGER)
		{
			if (node->getAbsolutePosition().getDistanceFrom(Player->pos) < 40)
			{
				COA_BossAppear();
				smgr->addToDeletionQueue(node);
			}
		}

	 }

	if (Player->dormant)
	{
		Player->reactivate(entrance);
	}

	if (shadow->dormant)
	{
		shadow->reactivate(entrance);
	}

	Player->SetControl(control);

	Player->setGravity(world);
	shadow->setGravity(world);

	Player->setPosition(entrance);


	u32 tex_count = vmgr->getTextureCount();

	for (u32 i = 0; i < tex_count; i++)
	{
		io::SNamedPath path = vmgr->getTextureByIndex(i)->getName();
	}


	Map_loaded = true;

}

void GameMan::unLoadMap()
{
	Player->deactivate();
	shadow->deactivate();

	if (BGM)
		BGM->stop();

	BGM = 0;
	num_targets = 0;
	mini_shoop_num = 0;
	world->drop();
	smgr->clear();

	Map_loaded = false;
}

bool GameMan::TitleScreen()
{

	smgr->loadScene("Title_screen.irr");

	smgr->addCameraSceneNode(0,core::vector3df(0,40,0),core::vector3df(0,40,40));

	core::array<scene::ISceneNode *> nodes;
	smgr->getSceneNodesFromType(scene::ESNT_ANY, nodes);

	core::vector2d<s32> origin(0,0);

	gui::IGUIImage * BG = gmgr->addImage(vmgr->getTexture("black_bg.png"),origin);
	gui::IGUIImage * splash_A = gmgr->addImage(vmgr->getTexture("splash0.png"),origin);
	gui::IGUIImage * splash_B = gmgr->addImage(vmgr->getTexture("splash1.png"),origin);
	gui::IGUIImage * flash = gmgr->addImage(vmgr->getTexture("flash.png"),origin);

	splash_A->setColor(video::SColor(0,255,255,255));
	splash_B->setColor(video::SColor(0,255,255,255));
	flash->setColor(video::SColor(0,255,255,255));

	core::array<video::ITexture*> textures;
	for (s32 g=7; g > 0; --g)
	{
		core::stringc tmp;
		tmp = "portal";
		tmp += g;
		tmp += ".bmp";
		video::ITexture* t = device->getVideoDriver()->getTexture(tmp.c_str());
		textures.push_back(t);
	}
	scene::ISceneNodeAnimator * portal =	smgr->createTextureAnimator(textures, 100);

	s32 opacity_A = 0;
	s32 opacity_B = 0;
	s32 opacity_C = 0;

	for (u32 i=0; i < nodes.size(); ++i)
	{
		scene::ISceneNode * node = nodes[i];

		if (node->getID() == Title_Letters && node->getType() == scene::ESNT_ANIMATED_MESH)
		{
			title_letters = (scene::IAnimatedMeshSceneNode *)node;
		}

		if (node->getID() == Title_Portal)
		{
			node->addAnimator(portal);	
		}
	}

	bool title_done = false;

	title_letters->setAnimationSpeed(0);
	title_letters->setLoopMode(false);

	u32 systime = device->getTimer()->getRealTime();
	u32 time  = device->getTimer()->getRealTime() - systime;

	changeBGM("Title");

	while (device->run() && !title_done)
	{
		time  = device->getTimer()->getRealTime() - systime;

		vmgr->beginScene();
		smgr->drawAll();
		gmgr->drawAll();
		vmgr->endScene();

		if (time < 2000)
		{
			if (opacity_A < 255)
				opacity_A += 5;

			splash_A->setColor(video::SColor(opacity_A,255,255,255));
		}

		if (time > 3500 && time < 4500)
		{
		   	if (opacity_A > 0)
				opacity_A -= 5;


			splash_A->setColor(video::SColor(opacity_A,255,255,255));
			splash_B->setColor(video::SColor(255 - opacity_A,255,255,255));
		}

		if (time < 14000 && time > 13900)
		{
			opacity_A = 255;

			flash->setColor(video::SColor(255,255,255,255));
			BG->setVisible(false);
			splash_A->setVisible(false);
			splash_B->setVisible(false);
		}

		if (time > 14000)
		{
			if (opacity_A >	0)
				opacity_A -= 5;

			flash->setColor(video::SColor(opacity_A,255,255,255));
		}

		if (time > 15000)
		{
			title_letters->setAnimationSpeed(10);
		}


		if (control->IsKeyDown(KEY_SPACE))
		{
			smgr->clear();
			gmgr->clear();
			title_done = true;
			return true;
		}
	}

	

	return false;

}
void GameMan::run()
{
	// Safety Check!
	if (!device)
		return;


	//if (!TitleScreen())
	//	return;

	// Begin test code!
	device->getFileSystem()->addFileArchive("./data/char/ALON00.sap");

	mini_shoop_num = 0;

	shadow = new Character(device,audio);
	shadow->Spawn(Maria,core::vector3df(0,100,0));
	shadow->body->setVisible(false);

	Player = new Character(device,audio);
	Player->Spawn(Alonguy,core::vector3df(0,100,0));
	Player->SetControl(control);

	LoadMap("awesome_cave");

	audio->setDopplerEffectParameters();

	rand_time = device->getTimer()->getRealTime();

	game_over = gmgr->addImage(vmgr->getTexture("fail.png"),core::vector2d<s32>(0,0));
	game_over->setVisible(false);

	menu_back = gmgr->addImage(vmgr->getTexture("menu_back.png"),core::vector2d<s32>(0,0));
	menu_back->setVisible(false);

	menu_main = gmgr->addImage(vmgr->getTexture("Stats_menu.png"),core::vector2d<s32>(0,0));
	menu_main->setVisible(false);

	bool load_game = false;
	load_game = true;
	dead_game = false;


	if (load_game)
	{
		if (saver.read_in("Auto",device->getFileSystem()))
		{
			Player->setPosition(saver.restart_pos);
			Player->last_safe_pos = saver.restart_pos;
			Player->HP = saver.HP;
			Player->MP = saver.MP;
			Player->max_HP = saver.max_HP;
			Player->max_MP = saver.max_MP;

			Player->attack = saver.attack;
			Player->defense = saver.defense;
			Player->speed = saver.speed;
		}

	}

	frames = 0;
	s32 ref_time = device->getTimer()->getRealTime();
	s32 time = device->getTimer()->getRealTime() - ref_time;
	s32 camref = device->getTimer()->getRealTime();
	s32 cam_time = 0;

	bool reload = false;

	last_time = device->getTimer()->getTime();

	while (device->run())
	{
		rand_reset = device->getTimer()->getRealTime() - rand_time;
		time = device->getTimer()->getRealTime() - ref_time;
		cam_time = device->getTimer()->getRealTime() - camref;

		new_time = device->getTimer()->getTime();
		
		if (Map_loaded)
		{
			update_players();
			check_triggers();
			ref_time = device->getTimer()->getRealTime();
		}

		vmgr->setRenderTarget(0);
		vmgr->beginScene(true,true,video::SColor(255,0,0,0));

		if (Map_loaded)
			smgr->drawAll();

		if (Player->body->isVisible())
			gmgr->drawAll();

		if (BGM)
		{
			if (BGM->getPlayPosition() + 50 > BGM->getPlayLength())
				BGM->setPlayPosition(BGM_loopback);

			if (control->IsKeyDown(KEY_KEY_R))
				BGM->setPlayPosition(BGM->getPlayLength() - 5000);
		}

			vmgr->endScene();
		
			frames++;
		
		//... and finish off this frame!

		if (rand_reset > 10000)
		{
			Randomizer::reset();
			rand_time = device->getTimer()->getRealTime();

		}

		last_time = device->getTimer()->getTime();

	}


}

void GameMan::check_triggers()
{
	core::vector3df camdir = smgr->getActiveCamera()->getTarget() - smgr->getActiveCamera()->getAbsolutePosition();

	audio->setListenerPosition(smgr->getActiveCamera()->getAbsolutePosition(),camdir,last_campos);   


	if (shadow->pos.getDistanceFrom(Player->pos) > 10)
	{
		f32 dist = shadow->pos.getDistanceFrom(Player->pos);
		shadow->moveTo(Player->pos,1);
	}

	if (shadow->pos.getDistanceFrom(Player->pos) < 10)
	{
		
	}

	if (control->IsKeyDown(KEY_KEY_L))
	{
		Player->pos.Y++;
		Player->setPosition(Player->pos);
	}

	if (BGM)
	{
		if (inBattle && BGM->getVolume() > 0)
		{
			f32 vol = BGM->getVolume();
			vol -= 0.05f;
			BGM->setVolume(vol);
		}

		if (inBattle && BGM->getVolume() < 0.05f && !BGM_2)
		{
			BGM_2 = audio->play2D("./data/audio/BGM/Battle.ogg",true,false,true);
			BGM_2->setVolume(0.5f);
		}

		if (!inBattle && BGM->getVolume() < 0.5f)
		{
			if (BGM_2)
				BGM_2->stop();

			BGM_2 = 0;

			f32 vol = BGM->getVolume();
			vol += 0.05f;
			BGM->setVolume(vol);
		}
	}

	inBattle = false;

		core::vector3df go_to = Player->pos;

		core::triangle3df tri;

		smgr->getSceneCollisionManager()->getSceneNodeAndCollisionPointFromRay(
			smgr->getSceneCollisionManager()->getRayFromScreenCoordinates(control->mouse_pos),go_to,tri,OBJ_BSP);

		go_to.Y = Player->pos.Y;

		if (control->L_Click && Player->topdown_view)
			Player->moveTo(go_to,1);

	if (control->IsKeyDown(KEY_KEY_T) && BGM->getVolume() > 0)
	{
		Player->topdown_view = true;
	}

	if (control->IsKeyDown(KEY_KEY_Y) && BGM->getVolume() < 1.0f)
	{
		Player->topdown_view = false;
	}

	if (control->IsKeyDown(KEY_KEY_O))
	{
		f32 box_X = Player->col->getEllipsoidRadius().X;
		f32 box_Z = Player->col->getEllipsoidRadius().Z;

		printf("\n X: %f Z: %f \n",box_X,box_Z);		
	}


	f32 bestDist = 50;
	f32 AI_bestdist = 80;

	core::array<scene::ISceneNode *> nodes;
	smgr->getSceneNodesFromType(scene::ESNT_ANY, nodes);

	Player->isSwimming = false;


	for (u32 i=0; i < nodes.size(); ++i)
	{
		scene::ISceneNode * node = nodes[i];

		if (node->getID() == OBJ_HAZARD)
		{
			if (Player->body->getTransformedBoundingBox().intersectsWithBox(node->getTransformedBoundingBox()))
			{

			}
		}

		if (node->getID() == TRIGGER_EXIT)
		{
			if (node->getTransformedBoundingBox().isPointInside(Player->pos))
			{
				//printf((char *)node->getName());

				f32 stg = node->getRotation().Z;
				printf("\n Numbers: %i and %f \n",stg,node->getMaterial(0).Shininess);

				Exit_cutscene(node,return_stage(stg));
				return;
			}
		}

		if (node->getID() == ITEM_DARKORB)
		{			 
			if (Player->pos.getDistanceFrom(node->getAbsolutePosition()) < 7.5f)
			{
				audio->play3D("./data/audio/sfx/dark_orb.ogg",node->getAbsolutePosition());
				Player->MP += 5;
				smgr->addToDeletionQueue(node);
			}
		}

		if (node->getID() < 15 && node->getID() > 11 && node->isVisible())
		{
			if (Player->pos.getDistanceFrom(node->getAbsolutePosition()) < 100)
				inBattle = true;

			if (!Player->target_lock)
			{
				core::vector3df tarpos = node->getAbsolutePosition();

				if (tarpos.getDistanceFrom(Player->pos) < bestDist)
						Player->target = node;
			}

			if (!shadow->target_lock)
			{
				core::vector3df tarpos = node->getAbsolutePosition();

				if (tarpos.getDistanceFrom(shadow->pos) < AI_bestdist)
						shadow->target = node;
			}


		}


		if (node->getID() == OBJ_WATER)
		{

			if (node->getTransformedBoundingBox().isPointInside(Player->pos))
			{	
				Player->HP++;
				Player->floor_ID = WATER;
			}

			if (node->getTransformedBoundingBox().isPointInside(shadow->pos))
			{	
				shadow->HP++;
				shadow->floor_ID = WATER;
			}
				
		}

		if (node->getID() == OBJ_DOOR)
		{
			if (Player->pos.getDistanceFrom(node->getAbsolutePosition()) < 15 && Player->X_Pressed && !Player->X_Held && !COA_enemy_room)
				Door_cutscene(node);
		}

		if (node->getID() == SPAWN_MINISHOOP && Player->pos.getDistanceFrom(node->getAbsolutePosition()) < 50)
		{
			mini_shoop[mini_shoop_num] = new MiniShoop(device,audio);
			mini_shoop[mini_shoop_num]->Spawn(node->getAbsolutePosition());
			mini_shoop[mini_shoop_num]->setWorld(world);
			mini_shoop_num++;

			smgr->addToDeletionQueue(node);
		}

		if (node->getID() == OBJ_HEAVY_CRATE || node->getID() == OBJ_DAMAGED_CRATE)
		{
			if (node->getAbsolutePosition().getDistanceFrom(Player->pos) < node->getScale().Z * 10)
			{
				core::vector3df crate_pos = node->getAbsolutePosition();
				core::vector3df P_pos = Player->pos;

				if (Player->velocity > 0.5f)
					Player->velocity = 0.5f;

				core::vector3df mid = crate_pos - P_pos;

				core::vector3df direction = mid.getHorizontalAngle().rotationToDirection();
				direction.Y = 0;

				P_pos -= direction;

				Player->setPosition(P_pos);

				crate_pos += Player->direction;
				node->setPosition(crate_pos);
				
			}


			if (node->getTransformedBoundingBox().intersectsWithLine(Player->damage) && Player->isAttacking)
			{
				if (node->getID() == OBJ_DAMAGED_CRATE && Player->body->getFrameNr() < 16)
				{
					audio->play3D("./data/audio/sfx/break_crate.ogg",node->getAbsolutePosition());
					smgr->addToDeletionQueue(node);
				}


				if (node->getID() == OBJ_HEAVY_CRATE)
				{

					core::vector3df crate_pos = node->getAbsolutePosition();
					core::vector3df P_pos = Player->pos;

					core::vector3df mid = crate_pos - P_pos;

					core::vector3df direction = mid.getHorizontalAngle().rotationToDirection();
					direction.Y = 0;

					crate_pos += direction * 5;

					node->setPosition(crate_pos);

				}

			}


		}

		if (node->getID() == TRIGGER_ENEMYCLEAR)
		{
			if (node->getTransformedBoundingBox().isPointInside(Player->pos) && !COA_enemy_room && !Progress_Bits[COA_SWITCH1] )
			{
				COA_enemy_room = true;

				s32 start = mini_shoop_num;
				s32 end = mini_shoop_num;

				COA_Start = mini_shoop_num;

				for (s32 i = 0; i < 4; i++)
				{
					mini_shoop[mini_shoop_num] = new MiniShoop(device,audio);
					mini_shoop[mini_shoop_num]->Spawn(core::vector3df(180, 15, (f32)i *10 + 180));
					mini_shoop[mini_shoop_num]->setWorld(world);
					mini_shoop_num++;
				}

				COA_End = mini_shoop_num;

			}

			if (COA_enemy_room)
			{
				COA_kills = 0;

				for (s32 i = COA_Start; i < COA_End; i++)
				{
					if (!mini_shoop[i]->isAlive)
						COA_kills++;
				}

				if (COA_kills == 4)
				{
					Progress_Bits[COA_SWITCH1] = true;
					COA_enemy_room = false;
					node->getParent()->setPosition(core::vector3df(0,-1000,0));
				}
			}

		}

		if (node->getID() == OBJ_VINES)
		{
			core::vector3df point = Player->pos;

			if (node->getTransformedBoundingBox().isPointInside(point))
			{
				Player->pos.Y += 0.5f;
			}

		}

		if (node->getID() == RANDOM_NPC)
		{
			core::stringc hair_tex;
			hair_tex += "hair";
			hair_tex += NPC_hair;
			core::stringc eye_tex;
		   	eye_tex += "eye";
			eye_tex += NPC_eye;

			core::stringc bottom_tex;
			bottom_tex += "bottom";
			bottom_tex += NPC_bottom;
			core::stringc top_tex;
		   	top_tex += "top";
			top_tex += NPC_top;
			core::stringc skin_tex;
			skin_tex += "skin";
			skin_tex += NPC_skin;

			bottom_tex += ".png";
			top_tex += ".png";
			skin_tex += ".png";
			eye_tex += ".png";
			hair_tex += ".png";

			node->getMaterial(0).setTexture(0,vmgr->getTexture(skin_tex));
			node->render();
			node->getMaterial(0).setTexture(0,vmgr->getTexture(top_tex));
			node->render();
			node->getMaterial(0).setTexture(0,vmgr->getTexture(bottom_tex));
			node->render();

			node->getMaterial(1).setTexture(0,vmgr->getTexture(hair_tex));
			node->getMaterial(2).setTexture(0,vmgr->getTexture(eye_tex));
		}

		if (node->getID() == OBJ_DESTRUCT_SWITCH)
		{
			if (node->getTransformedBoundingBox().intersectsWithLine(Player->damage) && Player->isAttacking)
			{
				node->getParent()->setPosition(core::vector3df(0,-1000,0));
				node->setID(OBJ_PRESSED_SWITCH);
			}
		}

		if (node->getID() == OBJ_CRATE)
		{

			if (node->getAbsolutePosition().getDistanceFrom(Player->pos) < 10)
			{
				core::vector3df crate_pos = node->getAbsolutePosition();
				core::vector3df P_pos = Player->pos;

				if (Player->velocity > 0.5f)
					Player->velocity = 0.5f;

				core::vector3df mid = crate_pos - P_pos;

				core::vector3df direction = mid.getHorizontalAngle().rotationToDirection();
				direction.Y = 0;

				if (crate_pos.getDistanceFrom(Player->pos) < 8)
					P_pos -= direction;

				crate_pos += (direction);
				node->setPosition(crate_pos);
				Player->setPosition(P_pos);
				
			}

			if (node->getTransformedBoundingBox().intersectsWithLine(Player->damage) && Player->isAttacking)
			{
				if (Player->target == node)
				{
					Player->target_lock = false;
					Player->target = 0;
				}

				audio->play3D("./data/audio/sfx/break_crate.ogg",node->getAbsolutePosition());

				switch(Random(0,3))
				{
					case 0:
						createDarkOrb(node->getAbsolutePosition());
						break;
					case 1:
						createDarkOrb(node->getAbsolutePosition() + core::vector3df(0,0,3));
						createDarkOrb(node->getAbsolutePosition());
						createDarkOrb(node->getAbsolutePosition() + core::vector3df(3,0,0));
						break;
					case 2:
						createDarkOrb(node->getAbsolutePosition());
						break;
					default:
						break;
				}
				smgr->addToDeletionQueue(node);
				node = 0;
			}
		}
	}

	for (s32 i = 0; i < npc_num; i++)
	{
		npc[i]->update();
	}

	for (s32 i = 0; i < num_targets; i++)
	{
		if (target[i])
		{
			if (!Player->target_lock)
			{
				core::vector3df tarpos = target[i]->getAbsolutePosition();

				if (tarpos.getDistanceFrom(Player->pos) < bestDist)
						Player->target = target[i];
				}

				if (target[i]->getTransformedBoundingBox().intersectsWithLine(Player->damage) && Player->isAttacking)
				{
					if (Player->target == target[i])
					{
						Player->target_lock = false;
						Player->target = 0;
					}

					audio->play3D("./data/audio/sfx/break_crate.ogg",target[i]->getAbsolutePosition());
					target[i]->remove();
					target[i] = 0;
				}
			}
		}


		for (s32 i = 0; i < mini_shoop_num; i++)
		{
			mini_shoop[i]->update();

			f32 best_dist = mini_shoop[i]->vision;

			if (Player->pos.getDistanceFrom(mini_shoop[i]->pos) < best_dist)
			{
				mini_shoop[i]->target_lock = true;
				mini_shoop[i]->target = Player->body;

				best_dist = Player->pos.getDistanceFrom(mini_shoop[i]->pos);
			}

			if (shadow->pos.getDistanceFrom(mini_shoop[i]->pos) < mini_shoop[i]->vision)
			{
				mini_shoop[i]->target_lock = true;
				mini_shoop[i]->target = shadow->body;

				best_dist = shadow->pos.getDistanceFrom(mini_shoop[i]->pos);
			}


			if (mini_shoop[i]->body->getTransformedBoundingBox().intersectsWithLine(Player->damage) && Player->isAttacking)
			{
				mini_shoop[i]->Damage(Player->attack,Player->pos);
			}

			if (mini_shoop[i]->body->getTransformedBoundingBox().intersectsWithLine(shadow->damage) && shadow->isAttacking)
			{
				mini_shoop[i]->Damage(shadow->attack,shadow->pos);
			}

			if (Player->body->getTransformedBoundingBox().intersectsWithLine(mini_shoop[i]->damage) && mini_shoop[i]->Attack_Phase == 2)
			{
				Player->Damage(mini_shoop[i]->attack,mini_shoop[i]->pos,false);
			}

			if (shadow->body->getTransformedBoundingBox().intersectsWithLine(mini_shoop[i]->damage) && mini_shoop[i]->Attack_Phase == 2)
			{
				shadow->Damage(mini_shoop[i]->attack,mini_shoop[i]->pos,false);
			}
																								   
			if (!mini_shoop[i]->give_EXP && !mini_shoop[i]->isAlive)
			{
				s32 exp = mini_shoop[i]->award_exp;

				if (shadow)
					exp = exp/2;

				Player->AwardEXP(exp);

				if (shadow)
					shadow->AwardEXP(exp);

				mini_shoop[i]->give_EXP = true;
			}
		}

		if (Player->HP < 1)
		{
			Death_Cutscene();
		}

		if (shadow->HP < 1)
		{
			shadow->body->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
		}
		else
			shadow->body->setMaterialType(video::EMT_SOLID);

		// Now to be map-specific!

		if (map_number == LVL_FIELD_AA)
			Field_Intro();

		if (map_number == LVL_TRIAL_TWO && !COT_enemies_done)
		{
			COT_enemies();
			COT_enemies_done = true;
		}

}

void GameMan::update_players()
{
	if (Player)
	{
		Player->update();
			
		f32 one_sev =  170.0f;
		f32 px = (one_sev/Player->max_HP);

		f32 bar_size = (px*Player->HP);
		HP_bar->setMinSize(core::dimension2du((u32)bar_size,16));
		HP_bar->updateAbsolutePosition();

		px = (one_sev/Player->max_MP);

		bar_size = (px*Player->MP);
		MP_bar->setMinSize(core::dimension2du((u32)bar_size,16));
		MP_bar->updateAbsolutePosition();
	}

	if (shadow)
	{
		shadow->update();

		f32 one_hund =  100.0f;
		f32 px = (one_hund/shadow->max_HP);

		f32 bar_size = (px*shadow->HP);
		HP_bar_party->setMinSize(core::dimension2du((u32)bar_size,9));
		HP_bar_party->updateAbsolutePosition();

		px = (one_hund/shadow->max_MP);

		bar_size = (px*shadow->MP);
		MP_bar_party->setMinSize(core::dimension2du((u32)bar_size,9));
		MP_bar_party->updateAbsolutePosition();
	}

	if (shadow)
	{
		if (shadow->target)
		{
			if (shadow->target->getID() > 11 && shadow->target->getID() < 16 && shadow->target->isVisible() && shadow->HP > 0 && !Player->cutscene)	
			{
				shadow->target_lock = true;

				s32 hp = shadow->HP;
				s32 max = shadow->max_HP;

				s32 percent = (100/max) * hp;

				if (Random(0,130 - percent) == 0)
					shadow->Attack();
				else
				{
					core::vector3df target_look = shadow->target->getRotation().rotationToDirection();
					target_look.invert();
					core::vector3df goto_pos = shadow->target->getAbsolutePosition() + (target_look * (f32)Random(15,45));
					shadow->moveTo(goto_pos,1);
				}
			}
		}

		if (!inBattle || shadow->HP < 1)
		{
			shadow->target_lock = false;

			if (shadow->pos.getDistanceFrom(Player->pos) > 10)
			{
				f32 dist = shadow->pos.getDistanceFrom(Player->pos);
				shadow->moveTo(Player->pos,1);
			}

			if (shadow->pos.getDistanceFrom(Player->pos) > 50)
			{
				shadow->setPosition(Player->cam->getAbsolutePosition());
			}

		if (Player->pos.Y - shadow->pos.Y > 10 && !shadow->isFalling) 
			shadow->jump();	
		}

	}


}

void GameMan::createDarkOrb(core::vector3df pos)
{
	scene::IBillboardSceneNode * orb = smgr->addBillboardSceneNode(0,core::dimension2df(3,3));
	orb->addAnimator(dark2);
	orb->setMaterialFlag(video::EMF_LIGHTING,false);
	orb->setID(ITEM_DARKORB);
	orb->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);

	orb->addAnimator(smgr->createCollisionResponseAnimator(world,orb,core::vector3df(2,2,2),core::vector3df(0,-0.1f,0)));

	orb->setPosition(pos);
}

void GameMan::Death_Cutscene()
{
	// Sad cutscene...

	core::vector3df P_pos = Player->pos;
	core::vector3df pos_A(-21.77f, 203.63f, -540.33f);
	core::vector3df pos_B(-13.66f, 228.08f, -581.02f);
	core::vector3df pos_C(-38.77f, 212.86f, -530.17f);

	smgr->setAmbientLight(video::SColorf(0.5f,0.5f,0.5f,1.0f));			
	vmgr->setFog(video::SColor(255,255,255,255),video::EFT_FOG_EXP,1000,5000,0.0010f);
	smgr->getActiveCamera()->setFarValue(500);

	Player->cutscene_cam->setPosition(P_pos + (Player->rot.rotationToDirection() * 15));
	Player->cutscene = true;
	u32 start_time = device->getTimer()->getRealTime();

	Player->isAttacking = false;

	Player->Anim_Override = true;
	Player->PickSeq("Death");
	Player->body->setLoopMode(false);
	Player->body->setAnimationSpeed(2.5f);

	s32 opacity = 0;

	bool music = false;
	bool save = false;
	bool fader = false;

	s32 pos_end = 0;

	audio->play2D("./data/audio/vc/die0.ogg");

	while (Player->cutscene)
	{
	
	P_pos = Player->body->getJointNode("head")->getAbsolutePosition();


	if (!device->run())
		break;

	update_players();
	vmgr->beginScene(true,true,video::SColor(255,0,0,0));
	smgr->drawAll();
	gmgr->drawAll();

	u32 time = (device->getTimer()->getRealTime() - start_time);

	

	if (time < 3000)
	{
		Player->cutscene_cam->setTarget(P_pos);
		
		if (BGM)
			BGM->stop();
		if (BGM_2)
			BGM_2->stop();

		if (!music)
		{
			audio->play2D("./data/audio/bgm/Game_Over.ogg");
			music = true;
		}
	}

	if (time > 3000)
	{
		if (pos_end < 100)
		{
			core::vector3df C_pos = Player->cutscene_cam->getAbsolutePosition();

			C_pos.Y += 0.5f;
			pos_end++;

			Player->cutscene_cam->setPosition(C_pos);
		}
		else if	(opacity < 255)
		{
			game_over->setVisible(true);
			game_over->setColor(video::SColor(opacity,255,255,255));
			opacity += 5;
		}

		if (opacity == 255)
		{
			core::rect<s32> screen;
			screen.UpperLeftCorner = core::vector2d<s32>(0,0);
			screen.LowerRightCorner = core::vector2d<s32>(640,480);

			

			if (time > 12000)
			{
				dead_game = true;
				device->closeDevice();
				Player->cutscene = false;
			}
		}
		/*
		if (!save)
		{

			saver.record_stats(Player);
			saver.HP = saver.max_HP/2;
			saver.write_out("Auto",device->getFileSystem());

			save = true;
		}
		*/
	}


	vmgr->endScene();	
	}
}

void GameMan::Door_cutscene(scene::ISceneNode * door)
{
	core::vector3df P_pos = Player->pos;

	core::vector3df door_pos = door->getAbsolutePosition();
	core::vector3df origin_pos   = door_pos;
	core::vector3df door_dir = door->getRotation().rotationToDirection();

	bool forward = false;

	if (P_pos.getDistanceFrom(door_pos - door_dir) < P_pos.getDistanceFrom(door_pos + door_dir))
		forward = true;

	core::vector3df cam_pos_A;
	core::vector3df cam_pos_B;

	core::vector3df go_pos_A;
	core::vector3df go_pos_B;



	if (forward)
	{
		cam_pos_A = door_pos + core::vector3df(0,20,0) - door_dir * 30;
		cam_pos_B = door_pos + core::vector3df(0,20,0) + door_dir * 30;
		go_pos_A = door_pos - door_dir * 20;
		go_pos_B = door_pos + door_dir * 20;
	}
	else
	{
		cam_pos_A = door_pos + core::vector3df(0,20,0) + door_dir * 30;
		cam_pos_B = door_pos + core::vector3df(0,20,0) - door_dir * 30;
		go_pos_A = door_pos + door_dir * 20;
		go_pos_B = door_pos - door_dir * 20;
	}

	Player->cutscene_cam->setPosition(cam_pos_A);
	//Player->setPosition(go_pos_A + core::vector3df(0,0.5f,0));
	Player->cutscene = true;
	u32 start_time = device->getTimer()->getRealTime();

	while (Player->cutscene)
	{
	
	if (!device->run())
		break;

	update_players();
	vmgr->beginScene(true,true,video::SColor(255,0,0,0));
	smgr->drawAll();
	gmgr->drawAll();

	door_pos = door->getAbsolutePosition();

	u32 time = (device->getTimer()->getRealTime() - start_time);


	if (time < 1000)
	{
		Player->cutscene_cam->setPosition(cam_pos_A);
		Player->cutscene_cam->setTarget(go_pos_B);

		if (door_pos.Y < door_pos.Y + 20)
			door_pos += core::vector3df(0,0.5f,0);
	}

	if (time < 1500 && time > 1000)
	{
		Player->cutscene_cam->setTarget(go_pos_B);
		Player->moveTo(go_pos_B,1.0f);
		Player->cutscene_cam->setPosition(cam_pos_A);
	}

	if (time > 1500 && time < 2000)
	{
		Player->cutscene_cam->setTarget(go_pos_A);
		Player->moveTo(go_pos_B,1.0f);
		Player->cutscene_cam->setPosition(cam_pos_B);

	}

	if (time > 2000)
	{
		Player->cutscene_cam->setPosition(cam_pos_B);
		Player->cutscene_cam->setTarget(go_pos_A);

		if (door_pos.Y > origin_pos.Y)
			door_pos -= core::vector3df(0,0.5f,0);
	}


	if (time > 3000)
	{
		Player->cutscene = false;
		Player->SetControl(control);
	}

	door->setPosition(door_pos);

	vmgr->endScene();	
	}
}


void GameMan::Exit_cutscene(scene::ISceneNode * exit,char * dest)
{
	core::vector3df P_pos = Player->pos;

	core::vector3df exit_pos = exit->getAbsolutePosition();
	core::vector3df exit_dir = exit->getRotation().rotationToDirection();

	core::vector3df go_pos;

	go_pos = exit_pos + exit_dir * 20;

	//Player->setPosition(go_pos_A + core::vector3df(0,0.5f,0));
	Player->cutscene = true;
	u32 start_time = device->getTimer()->getRealTime();

	Player->cutscene_cam->setPosition(Player->cam->getAbsolutePosition());

	while (Player->cutscene)
	{
	
	if (!device->run())
		break;

	update_players();
	vmgr->beginScene(true,true,video::SColor(255,0,0,0));
	smgr->drawAll();
	gmgr->drawAll();


	u32 time = (device->getTimer()->getRealTime() - start_time);


	if (time < 2000)
	{
		Player->cutscene_cam->setTarget(go_pos);
		Player->moveTo(go_pos,1);
	}

	if (time > 2000)
	{
		unLoadMap();
		printf("Loading Map: ");
		printf(dest);
		printf("\n");
		LoadMap(dest);
		Player->cutscene = false;
	}

	vmgr->endScene();	
	}
}


void GameMan::Pause_Game()
{
	bool isPaused = true;

	u32 current_menu = 0;

	menu_back->setVisible(true);
	menu_main->setVisible(true);

	s32 ref_time = device->getTimer()->getRealTime();
	s32 time = 0;

	f32 opacity = 0;

	while(isPaused && device->run())
	{
		time = device->getTimer()->getRealTime() - ref_time;

		vmgr->beginScene();

		smgr->drawAll();
		gmgr->drawAll();


		if (time < 1000)
		{
			opacity += 0.5f;
		}


		vmgr->endScene();

	}

	menu_back->setVisible(false);
	menu_main->setVisible(false);

}


void GameMan::changeBGM(char *newbgm)
{
	core::stringc path;
	path += "./data/audio/BGM/";
	path += newbgm;
	path += ".ogg";

	if (bgmName == path)
		return;

	if (BGM)
		BGM->stop();

	bgmName = path;
	BGM = audio->play2D(path.c_str(),true,false,true);
	BGM->setVolume(0.5f);
}

char * GameMan::return_stage(irr::f32 in)
{
	char * name;

	printf("s32 in: %f",in);

	map_number = (s32)in;

	switch((s32)in)
	{
	case LVL_AW_OUTER:
		Exit_Num = 0;
		name = "Outside_Awesome";
		break;

	case LVL_AW_CAVE_B:
		Exit_Num = 1;
		name = "Awesome_Cave";
		break;

	case LVL_AW_CAVE_A:
		Exit_Num = 0;
		name = "Awesome_Cave";
		break;

	case LVL_FIELD_AA:
		Exit_Num = 0;
		name = "Field_A";
		break;

	case LVL_TRIAL_ONE:
		Exit_Num = 0;
		name = "Trial_one";
		break;

	case LVL_TRIAL_TWO:
		Exit_Num = 0;
		name = "Trial_Two";
		break;

	case LVL_AW_OUTER_B:
		Exit_Num = 1;
		name = "Outside_Awesome";
		break;

	default: 
	   	Exit_Num = 0;
		name = "Outside_Awesome";
		map_number = LVL_AW_OUTER;
		break;
	}

	return name;

}
//20.31f, 0.0f, 2.26f


/* Cutscene Template
void GameMan::Intro_cutscene()
{
	core::vector3df P_pos = Player->pos;

	Player->cutscene_cam->setPosition(core::vector3df(0.0f, 0.0f, 0.0f));
	Player->cutscene = true;
	u32 start_time = device->getTimer()->getRealTime();

	while (Player->cutscene)
	{
	
	if (!device->run())
		break;

	Player->update();
	vmgr->beginScene(true,true,video::SColor(255,0,0,0));
	smgr->drawAll();
	gmgr->drawAll();

	u32 time = (device->getTimer()->getRealTime() - start_time);

	if (time > 11000)
	{
		Player->cutscene = false;
		Player->SetControl(control);
		Player->setPosition(core::vector3df(0,20,0));
	}

	vmgr->endScene();	
	}
}
*/

void GameMan::COA_BossAppear()
{
	core::vector3df P_pos = Player->pos;

	Player->cutscene_cam->setPosition(core::vector3df(-38.82f, 6.48f, 278.60f));
	Player->cutscene = true;
	u32 start_time = device->getTimer()->getRealTime();

	while (Player->cutscene)
	{
	
	if (!device->run())
		break;

	update_players();
	vmgr->beginScene(true,true,video::SColor(255,0,0,0));
	smgr->drawAll();
	gmgr->drawAll();

	u32 time = (device->getTimer()->getRealTime() - start_time);

	if (time < 2000)
	{
		Player->cutscene_cam->setTarget(Player->pos);
		Player->moveTo(core::vector3df(-6.14f, 6.48f, 308.41f),0.5f);
	}

	if (time > 2000 && time < 4000)
	{
		Player->cutscene_cam->setTarget(Player->pos);
		Player->cutscene_cam->setPosition(core::vector3df(7.42f, 36.86f, 343.78f));
		Player->moveTo(core::vector3df(-6.14f, 6.48f, 308.41f),0.5f);
	}

	if (time > 4000 && time < 4100)
	{
		Player->cutscene = false;
		Player->SetControl(control);
	}

	vmgr->endScene();	
	}
}

void GameMan::Field_Intro()
{
	if (Progress_Bits[FIELD_INTRO])
		return;

	Player->cutscene = true;
	u32 start_time = device->getTimer()->getRealTime();

	u32 opacity = 0;
	gui::IGUIImage * intro = gmgr->addImage(vmgr->getTexture("intro_field.png"),core::vector2d<s32>(0,0));

	intro->setColor(video::SColor(opacity,255,255,255));

	while (Player->cutscene)
	{
	
		if (!device->run())
			break;

		update_players();
		vmgr->beginScene(true,true,video::SColor(255,0,0,0));
		smgr->drawAll();
		gmgr->drawAll();

		u32 time = (device->getTimer()->getRealTime() - start_time);

		if (time < 4000)
		{
			intro->setColor(video::SColor(opacity,255,255,255));

			if (opacity < 255)
				opacity += 5;
		}


		if (time > 6000)
		{
			intro->setColor(video::SColor(opacity,255,255,255));

			if (opacity > 0)
				opacity -= 5;
		}

		if (time > 8000)
		{
			Player->cutscene = false;
			Player->SetControl(control);
			Progress_Bits[FIELD_INTRO] = true;
		}


		
	vmgr->endScene();	
	}
}
void GameMan::COT_enemies()
{
	COT_enemy_count	= 50;
	COT_active_enemies = 5;

	for (s32 i = 0; i < COT_active_enemies; i++)
	{
		COT_shoops[i] = new MiniShoop(device,audio);
		COT_shoops[i]->Spawn(core::vector3df(Random(-30,30),90,Random(-30,30)));
		COT_shoops[i]->setWorld(world);
	}

	bool one = false;
	bool two = false;
	bool three = false;
	bool four = false;
	bool five = false;


	while (COT_enemy_count > 0)
	{
		if (!device->run())
			break;

		update_players();
		//check_triggers();

		vmgr->beginScene(true,true,video::SColor(255,0,0,0));
		smgr->drawAll();
		gmgr->drawAll();

		f32 bestDist = 30;

		for (s32 i = 0; i < COT_active_enemies; i++)
		{

			if (!Player->target_lock)
			{
				core::vector3df tarpos = COT_shoops[i]->pos;

				if (tarpos.getDistanceFrom(Player->pos) < bestDist)
				{
						Player->target = COT_shoops[i]->body;
						bestDist = tarpos.getDistanceFrom(Player->pos);
				}
			}

			COT_shoops[i]->update();

			if (Player->pos.getDistanceFrom(COT_shoops[i]->pos) < COT_shoops[i]->vision)
			{
				COT_shoops[i]->target_lock = true;
				COT_shoops[i]->target = Player->body;
			}

			if (COT_shoops[i]->body->getTransformedBoundingBox().intersectsWithLine(Player->damage) && Player->isAttacking)
			{
				COT_shoops[i]->Damage(Player->attack,Player->pos);
			}

			if (Player->body->getTransformedBoundingBox().intersectsWithLine(COT_shoops[i]->damage) && COT_shoops[i]->Attack_Phase == 2)
			{
				Player->Damage(COT_shoops[i]->attack,COT_shoops[i]->pos,false);
			}
																								   
			if (!COT_shoops[i]->give_EXP && !COT_shoops[i]->isAlive)
			{
				Player->AwardEXP(COT_shoops[i]->award_exp);
				COT_shoops[i]->give_EXP = true;
				COT_enemy_count--;
			}
		}


		if (COT_enemy_count == 45 && !one)
		{
			for (s32 i = 0; i < COT_active_enemies; i++)
			{
				COT_shoops[i]->body->setVisible(false);
			}

			for (s32 i = 0; i < COT_active_enemies; i++)
			{
				COT_shoops[i] = new MiniShoop(device,audio);
				COT_shoops[i]->Spawn(core::vector3df(Random(-60,60),90,Random(-60,60)));
				COT_shoops[i]->setWorld(world);
			}

			one = true;
		}

		if (COT_enemy_count == 40 && !two)
		{
			for (s32 i = 0; i < COT_active_enemies; i++)
			{
				COT_shoops[i]->body->setVisible(false);
			}

			COT_active_enemies = 10;
			for (s32 i = 0; i < COT_active_enemies; i++)
			{
				COT_shoops[i] = new MiniShoop(device,audio);
				COT_shoops[i]->Spawn(core::vector3df(Random(-60,60),90,Random(-60,60)));
				COT_shoops[i]->setWorld(world);
			}

			two = true;
		}

		if (COT_enemy_count == 30 && !three)
		{

			for (s32 i = 0; i < COT_active_enemies; i++)
			{
				COT_shoops[i]->body->setVisible(false);
			}
			
			for (s32 i = 0; i < COT_active_enemies; i++)
			{
				COT_shoops[i] = new MiniShoop(device,audio);
				COT_shoops[i]->Spawn(core::vector3df(Random(-60,60),90,Random(-60,60)));
				COT_shoops[i]->setWorld(world);
			}

			three	= true;
		}
	
		if (COT_enemy_count == 20 && !four)
		{
			for (s32 i = 0; i < COT_active_enemies; i++)
			{
				COT_shoops[i]->body->setVisible(false);
			}

			for (s32 i = 0; i < COT_active_enemies; i++)
			{
				COT_shoops[i] = new MiniShoop(device,audio);
				COT_shoops[i]->Spawn(core::vector3df(Random(-60,60),90,Random(-60,60)));
				COT_shoops[i]->setWorld(world);
			}

			four	= true;
		}

		if (COT_enemy_count == 10 && !five)
		{
			for (s32 i = 0; i < COT_active_enemies; i++)
			{
				COT_shoops[i]->body->setVisible(false);
			}


			for (s32 i = 0; i < COT_active_enemies; i++)
			{
				COT_shoops[i] = new MiniShoop(device,audio);
				COT_shoops[i]->Spawn(core::vector3df(Random(-60,60),90,Random(-60,60)));
				COT_shoops[i]->setWorld(world);
			}

			five = true;
		}

		if (Player->HP < 1)
			Death_Cutscene();

		vmgr->endScene();
	}

}