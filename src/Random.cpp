//=============================================================================
//
// ƒ‰ƒ“ƒ_ƒ€ˆ— [Random.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================
#include "Random.h"
#include <random>
#include <ctime>

namespace
{
	std::mt19937 Random(static_cast<unsigned int>(time(nullptr)));
}

//=============================================================================
// float—”
//=============================================================================
float CRandom::RandF(float min, float max)
{
	std::uniform_real_distribution<float> dist(min, max);

	return dist(Random);
}

//=============================================================================
// int—”
//=============================================================================
int CRandom::RandI(int min, int max)
{
	std::uniform_int_distribution<int> dist(min, max);

	return dist(Random);
}
