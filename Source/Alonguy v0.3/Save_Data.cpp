#include "Save_Data.h"

GameSave::GameSave()
{
	HP = 25;
	MP = 25;
	max_HP = 50;
	max_MP = 50;
	attack = 0;
	defense = 0;
	speed = 0;
	Level = 0;
	EXP = 0;
	restart_pos = core::vector3df(0,0,0);
	awesome_clear = false;
}

void GameSave::record_stats(Character *saved_char)
{
	HP = saved_char->HP;
	MP = saved_char->MP;
	max_HP = saved_char->max_HP;
	max_MP = saved_char->max_MP;
	attack = saved_char->attack;
	defense = saved_char->defense;
	speed = saved_char->speed;
	Level = saved_char->Level;
	EXP = saved_char->EXP;

	restart_pos = saved_char->last_safe_pos;
}

bool GameSave::write_out(char *filename, irr::io::IFileSystem *fs)
{
	core::stringc path;
	path += "./save/";
	path += filename;
	path += ".asv";

	io::IXMLWriter * XML = fs->createXMLWriter(path);
	XML->writeXMLHeader();

	core::stringw tmp1;
	tmp1 += max_HP;
	core::stringw tmp2;
	tmp2 += HP;
	core::stringw tmp3;
	tmp3 += max_MP;
	core::stringw tmp4;
	tmp4 += MP;

	XML->writeElement(L"HP",false,L"Max",tmp1.c_str(),L"Current",tmp2.c_str());
	XML->writeClosingTag(L"HP");
	XML->writeLineBreak();
	XML->writeElement(L"MP",false,L"Max",tmp3.c_str(),L"Current",tmp4.c_str());
	XML->writeClosingTag(L"MP");
	XML->writeLineBreak();
	

	core::stringw tmp5;
	tmp5 += attack;
	core::stringw tmp6;
	tmp6 += defense;
	core::stringw tmp7;
	tmp7 += speed;
	core::stringw tmp8;
	tmp8 += EXP;
	core::stringw tmp9;
	tmp9 += Level;

	XML->writeElement(L"Stats",false,L"Attack",tmp5.c_str(),L"Defense",tmp6.c_str(),
		L"Speed",tmp7.c_str(),L"EXP",tmp8.c_str(),L"Level",tmp9.c_str());
	XML->writeClosingTag(L"Stats");
	
	XML->writeLineBreak();

	core::stringw X_pos;
	core::stringw Y_pos;
	core::stringw Z_pos;

	X_pos += restart_pos.X;
	Y_pos += restart_pos.Y;
	Z_pos += restart_pos.Z;

	XML->writeElement(L"Position",false,L"X",X_pos.c_str(),L"Y",Y_pos.c_str(),L"Z",Z_pos.c_str());
	XML->writeClosingTag(L"Position");
	XML->writeLineBreak();

	core::stringw awesome;
	awesome += awesome_clear;

	core::stringw area;
	area += map_name;
		

	XML->writeElement(L"Progress",false,L"Area",area.c_str());
	XML->writeClosingTag(L"Progress");
	XML->writeLineBreak();
	

	return true;
}

bool GameSave::read_in(char * filename,irr::io::IFileSystem * fs)
{
	core::stringc path;
	path += "./save/";
	path += filename;
	path += ".asv";

	if (!fs->existFile(path))
	{
		printf("\nFile ");
		printf(path.c_str());
		printf(" not found!");
		 return false;
	}

	io::IXMLReader * reader = fs->createXMLReader(path);

	bool done = false;

	bool gotHP = false;
	bool gotMP = false;
	bool gotStats = false;
	bool gotPosition = false;
	bool gotProgress = false;

	core::stringc tmp1;
	tmp1 += "HP";
	core::stringc tmp2;
	tmp2 += "MP";
	core::stringc tmp3;
	tmp3 += "Stats";
	core::stringc tmp4;
	tmp4 += "Position";
	core::stringc tmp5;
	tmp5 += "Progress";

	while(reader->read())
	{

		if	(tmp1 == reader->getNodeName() && !gotHP)
		{
			max_HP = reader->getAttributeValueAsInt(L"Max");
			HP = reader->getAttributeValueAsInt(L"Current");
			printf("\n HP: %i Max HP: %i",HP,max_HP);
			gotHP = true;
		}

		if	(tmp2 == reader->getNodeName() && !gotMP)
		{
			max_MP = reader->getAttributeValueAsInt(L"Max");
			MP = reader->getAttributeValueAsInt(L"Current");
			printf("\n MP: %i Max MP: %i",MP,max_MP);
			gotMP = true;
		}

		if	(tmp3 == reader->getNodeName() && !gotStats)
		{
			attack = reader->getAttributeValueAsInt(L"Attack");
			defense = reader->getAttributeValueAsInt(L"Defense");
			speed = reader->getAttributeValueAsInt(L"Speed");
			EXP = reader->getAttributeValueAsInt(L"EXP");
			Level =	reader->getAttributeValueAsInt(L"Level");

			gotStats = true;
		}

		if	(tmp4 == reader->getNodeName() && !gotPosition)
		{
			core::vector3df pos(0,0,0);
			pos.X = reader->getAttributeValueAsFloat(L"X");
			pos.Y = reader->getAttributeValueAsFloat(L"Y");
			pos.Z = reader->getAttributeValueAsFloat(L"Z");

			restart_pos = pos;

			gotPosition = true;
		}

		if (tmp5 == reader->getNodeName() && !gotProgress)
		{
			core::vector3df pos(0,0,0);
			core::stringc area;
			area += reader->getAttributeValue(L"Area");

			map_name = (char *)area.c_str();

			gotProgress = true;
		}

		if (gotHP && gotMP && gotStats && gotPosition && gotProgress)
			return true;
	}

	return false;
}