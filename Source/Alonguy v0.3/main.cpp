#include "common.h"
#include "game.h"

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#pragma comment(lib, "IrrKlang.lib")
#endif


int main()
{
	GameMan game;

	game.run();

	return 0;
}