
#include "common.h"
#include "Character.h"
#include "enemy.h"
#include "npc.h"
#include "save_data.h"
#include "Dark_Alonguy.h"

enum level_num
{
	LVL_AW_CAVE_A,
	LVL_AW_CAVE_B,
	LVL_AW_OUTER,
	LVL_FIELD_AA,
	LVL_FIELD_AB,
	LVL_TRIAL_ONE,
	LVL_TRIAL_TWO,
	LVL_TRIAL_THREE,
	LVL_LOST_WORLD,
	LVL_AW_OUTER_B
};

enum Random_Level
{
	field_A
};

enum TITLE_ID
{
	Title_Letters,
	Title_Portal
};


class GameMan
{
public:
	//Constructors/Destructors
	GameMan();
	~GameMan()
	{
		//Empty for now!
	};

	// The main loop!
	void run();

	// Load/Unload the Levels
	void LoadMap(char * name);
	void unLoadMap();

	void update_players();

	void load_FX();

	scene::ISceneNodeAnimator * magic_fireball;
	scene::ISceneNodeAnimator * magic_fire_explosion;

	// Load the title!
	bool TitleScreen();

	scene::IAnimatedMeshSceneNode * title_letters;

	gui::IGUIImage * UI_overlay;
	gui::IGUIImage * HP_bar;
	gui::IGUIImage * MP_bar;
	gui::IGUIImage * portrait;

	gui::IGUIImage * UI_overlay_party;
	gui::IGUIImage * HP_bar_party;
	gui::IGUIImage * MP_bar_party;
	gui::IGUIImage * portrait_party;


	bool Map_loaded;

	char * map_name;

	video::ITexture * env_tex;

	scene::ICameraSceneNode * rendcam;

	u32 last_time;
	u32 new_time;

	// BGM
	void changeBGM(char * newbgm);

	void Weegee_cutscene();
	void Intro_cutscene();
	void Awesome_cutscene();
	void Pit_cutscene();
	void Ganon_cutscene();
	void Warp_Cutscene(core::vector3df exit);
	void WarpIn_Cutscene(core::vector3df exit);
	void Death_Cutscene();
	void Door_cutscene(scene::ISceneNode * door);
	void Exit_cutscene(scene::ISceneNode * exit,char * dest);

	char * return_stage(f32 in);

	void Pause_Game();

	void createDarkOrb(core::vector3df pos);

	void check_triggers();

	core::stringc bgmName;
	core::vector3df last_campos;
	irrklang::ISound * BGM;
	irrklang::ISound * BGM_2;
	Character * Player;
	Character * shadow;

	scene::ISceneNode * sky;
	scene::IAnimatedMeshSceneNode * Awesome;

	scene::ISceneNodeAnimator * water;
	scene::ISceneNodeAnimator * waterfall;
	scene::ISceneNodeAnimator * dark;
	scene::ISceneNodeAnimator * dark2;
	scene::ISceneNodeAnimator * dark3;

	gui::IGUIImage * menu_back;
	gui::IGUIImage * menu_main;

	scene::IAnimatedMeshSceneNode * portal_A;

	s32 rand_reset;
	s32 rand_time;

	u32 BGM_loopback;

	core::vector3df entrance;

	scene::ISceneNode * target[64];
	s32 num_targets;
	s32 Exit_Num;


	bool inBattle;


private:
	// The main managers!
	IrrlichtDevice * device;
	IVideoDriver * vmgr;
	ISceneManager * smgr;
	gui::IGUIEnvironment * gmgr;

	GameSave saver;

	//2D menus or 3D scenes?
	bool is3dmode;

	// Archive Mounting Variables
	u32 Level_SAP;

	// Level BSP
	scene::IMeshSceneNode * BSP;
	
	// Input Manager
	InputReciever * control;

	// World Triangles
	scene::IMetaTriangleSelector * world;

	irrklang::ISoundEngine * audio;

	gui::IGUIImage * game_over;


	s32 frames;

	bool dead_game;

	s32 found_portal;

	s32 box_count;
	core::aabbox3df water_block[32];

	MiniShoop * mini_shoop[32];
	s32 mini_shoop_num;

	NPC * npc[32];
	s32 npc_num;

	scene::IAnimatedMeshSceneNode * portal[5];

	s32 map_number;

	//Caverns of Awesome:
	bool COA_enemy_room;
	s32 COA_Start;
	s32 COA_End;
	s32 COA_kills;
	BOSS_ShoopDaWhoop * COA_BOSS;

	void COA_Intro();
	void COA_EnemyClear();
	void COA_PathClear();
	void COA_BossAppear();
	void COA_BossDie();

	bool COT_intro_done;
	bool COT_enemies_done;
	void COT_Intro();
	void COT_enemies();

	s32 COT_enemy_count;
	s32 COT_active_enemies;

	MiniShoop * COT_shoops[50];


	bool field_intro_done;
	void Field_Intro();

	s32 NPC_top;
	s32 NPC_bottom;
	s32 NPC_skin;
	s32 NPC_hair;
	s32 NPC_eye;

	bool Progress_Bits[ENDING];

};