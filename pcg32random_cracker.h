#pragma once
#include <cstdint>
#include <cmath>
#include "pcg32random.h"
#include <thread>
#include <vector>
#include <atomic>
#include <iostream>

bool almostEqualUnsigned(uint32_t a, uint32_t b, uint32_t epsilon) {
    return (a - b + (b > a)) <= epsilon;
}

bool checkFutureOutputs(uint64_t state, uint32_t x1, uint32_t x2, int depth = 10) {
    bool found = false;

    for (int i = 0; i < depth; i++) {
        uint32_t random = pcg32_random(&state);

        //std::this_thread::sleep_for(std::chrono::seconds(3));

        //std::cout << random << std::endl;

        if (almostEqualUnsigned(random, x1, 3)) {
            found = true;

            break;
        }
    }

    if (found == true)
    {
        for (int i = 0; i < depth; i++) {
            uint32_t random = pcg32_random(&state);

            if (almostEqualUnsigned(random, x2, 3)) {
                return true;
            }
        }
    }

    return false;
}

static uint64_t recover2(uint32_t x0, uint32_t x1, uint32_t x2)
{
    uint64_t state = 0b0111010111100100010000101010011100111011100111111110100000010001;

    for (uint64_t upper = 0; upper < (UINT64_C(1) << 32); upper++)
    {
        uint32_t rot = upper >> 27;
        // Rot  Middle
        //01110 1011110010001 0000101010011100111011100111111110100000010001

        // The middle 13 bits are directly given to us in PCG32's output.
    }

    for (uint64_t lower = 0; lower < UINT64_C(1) << 27; lower++)
    {

    }
}

static uint64_t recover(uint32_t x0, uint32_t x1)
{
	// Loop through all 2^32 combinations of the upper half of the state
	for (uint64_t upper = 0; upper < (UINT64_C(1) << 32); upper++)
    {
		const int rot = upper >> 27;
		const uint32_t rotx0 = x0 << rot | x0 >> (32 - rot);

		if (((rotx0 ^ upper >> 13) & ~31) == (uint32_t)(upper << 5))
        {
			const uint32_t high_lower_bits = ((rotx0 ^ upper >> 13) & 31) << 27;

			for (uint32_t lower = 0; lower < (UINT64_C(1) << 27); lower++)
            {
				const uint64_t candidate = (uint64_t)upper << 32 | high_lower_bits | lower;

				uint64_t state = candidate;

				const int rand1 = pcg32_random(&state);
				const int rand2 = pcg32_random(&state);

				if (rand1 == x0 && rand2 == x1)
                {
					return candidate;
				}
			}
		}
	}

	return 0;
}

struct WorkerArgs {
    uint32_t x0, x1, x2;
    uint64_t start, end;
    std::atomic<bool>* found;
    uint64_t* result;
};

void _recover_worker(WorkerArgs args) {
    for (uint64_t upper = args.start; upper < args.end && !args.found->load(); upper++)
    {
        const int rot = upper >> 27;
        const uint32_t rotx0 = args.x0 << rot | args.x0 >> (32 - rot);

        if (((rotx0 ^ upper >> 13) & ~31) == (uint32_t)(upper << 5))
        {
            const uint32_t high_lower_bits = ((rotx0 ^ upper >> 13) & 31) << 27;

            for (uint32_t lower = 0; lower < (UINT64_C(1) << 27) && !args.found->load(); lower++)
            {
                const uint64_t candidate = (uint64_t)upper << 32 | high_lower_bits | lower;

                uint64_t state = candidate;
                const uint32_t rand0 = pcg32_random(&state);
                //const uint32_t rand1 = pcg32_random(&state);
                //const uint32_t rand2 = pcg32_random(&state);

                if (rand0 == args.x0 && checkFutureOutputs(state, args.x1, args.x2, 1000))
                {
                    *args.result = candidate;
                    args.found->store(true);
                    return;
                }
            }
        }
    }
}

uint64_t recover_mt(uint32_t x0, uint32_t x1, uint32_t x2, int nthreads = std::thread::hardware_concurrency())
{
    std::atomic<bool> found(false);
    uint64_t result = 0;

    uint64_t chunk = (UINT64_C(1) << 32) / nthreads;
    std::vector<std::thread> threads;

    for (int i = 0; i < nthreads; i++)
    {
        WorkerArgs args{ x0, x1, x2,
                          i * chunk,
                          (i == nthreads - 1) ? (UINT64_C(1) << 32) : (i + 1) * chunk,
                          &found,
                          &result };

        threads.emplace_back(_recover_worker, args);
    }

    for (auto& t : threads) t.join();

    return result;
}
