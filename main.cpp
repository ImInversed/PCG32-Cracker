#include<cstdint>
#include<iostream>
#include<bitset>
#include"pcg32random.h"
#include"pcg32random_cracker.h"
#include"roblox_pcg32random_deobfuscator.h"

void main()
{
    // The range used to generate the math.random value. The larger the range, the easier it is to crack the seed.
    uint64_t l = 0;
    uint64_t u = 2147483647;
    uint64_t ul = u - l;

    // change these!
    uint32_t x0 = 874399921;
    uint32_t x1 = 469057138;
    uint32_t x2 = 153506511;

    Range x0_range = deobfuscate_ul31(x0);

    for (uint32_t i = x0_range.lower; i <= x0_range.upper; i++)
    {
        uint64_t reverse = recover_mt(i, deobfuscate_ul31(x1).lower, deobfuscate_ul31(x2).lower, 12);

        std::cout << reverse << std::endl;

        if (reverse != 0)
        {
            std::cout << "Found seed candidate:" << std::endl;

            uint32_t rand4 = pcg32_random(&reverse);
            uint32_t rand5 = pcg32_random(&reverse);

            std::cout << rand4 << std::endl;
        }
    }
}
