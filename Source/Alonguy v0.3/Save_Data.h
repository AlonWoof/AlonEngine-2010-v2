#include "common.h"
#include "character.h"

class GameSave
{
public:

	GameSave();

	void record_stats(Character * saved_char);
	bool write_out(char * filename,irr::io::IFileSystem * fs);
	bool read_in(char * filename,irr::io::IFileSystem * fs);

	u32 HP;
	u32 max_HP;
	u32 MP;
	u32 max_MP;
	u32 attack;
	u32 defense;
	u32 speed;

	u32 EXP;
	u32 Level;

	core::vector3df restart_pos;

	stringc write_error;

	char * map_name;

	bool awesome_clear;
	bool found_weegee;
	bool intro_done;
};