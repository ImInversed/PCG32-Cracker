#pragma once
#include"pcg32random.h"

bool searchStateOutputsForValue(uint64_t state, uint32_t depth = 10000, uint32_t value) {
	for (uint32_t i = 0; i < depth; i++)
	{
		uint32_t random_number = pcg32_random(&state);

		if (random_number == value)
		{
			return true;
		}
	}

	return false;
}
