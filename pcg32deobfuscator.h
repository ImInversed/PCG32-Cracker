// PCG32 generates a large number of 32 bits, which roblox then has to confine to a certain range, e.g., math.random(1, 10000)
// This code undoes that operation and returns a range of possible PCG32 outputs that couldve generated a number within that range.

#pragma once
#include<cstdint>
#include<cmath>

struct Range {
	uint32_t lower;
	uint32_t upper;
};

// if ul is equal to 2^31 - 1, then you can just derive this range:
Range deobfuscate_ul31(uint32_t output)
{
	Range range = { 2 * output, 2 * output + 1 };

	return range;
}

// Generalized version for any u and l value
Range deobfuscate_generalized(uint64_t l, uint64_t ul, uint32_t output)
{
	uint32_t target = output - l;

	uint64_t lower = ((uint64_t)target * (1ULL << 32) + (ul + 1) - 1) / (ul + 1);
	uint64_t upper = ((uint64_t)(target + 1) * (1ULL << 32)) / (ul + 1) - 1;

	Range range = { lower, upper };

	return range;
}

// The old and slow version.
void old_deobfuscate(uint64_t l, uint64_t ul, uint32_t output)
{
	uint64_t finish = pow(2, 32);

	for (uint64_t combination = 0; combination < finish; combination++)
	{
		uint64_t x = (ul + 1) * combination;
		int32_t r0 = l + (x >> 32);

		if (r0 == output)
		{
			// ...
		}
	}
}
