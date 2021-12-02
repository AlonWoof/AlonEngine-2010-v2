
#include "common.h"

s32 Randomizer::seed = 0x0f0f0f0f;

//! generates a pseudo random number
s32 Randomizer::rand()
{
	const s32 m = 2147483399;	// a non-Mersenne prime
	const s32 a = 40692;		// another spectral success story
	const s32 q = m/a;
	const s32 r = m%a;		// again less than q

	seed = a * (seed%q) - r* (seed/q);
	if (seed<0) seed += m;

	return seed;
}

//! resets the randomizer
void Randomizer::reset()
{
	seed = 0x0f0f0f0f;
}

s32 Random(s32 min, s32 max)
{
	max += 1;
	s32 rnd = (max - min);
	s32 rand = rnd ? min + (Randomizer::rand() % rnd) : min;

	return rand;
}


f32 Random(f32 min, f32 max)
{
	max += 1;
	f32 rnd = (max - min);
	f32 rand = rnd ? min + (Randomizer::rand() % (s32)rnd) : min;

	return rand;
}