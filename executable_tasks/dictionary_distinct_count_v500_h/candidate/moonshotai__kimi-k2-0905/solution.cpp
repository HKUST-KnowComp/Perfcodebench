#include "interface.h"
#include <vector>
#include <unordered_set>
#include <cstdint>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters)
{
    const uint32_t dictSize = static_cast<uint32_t>(dictionary.size());
    // 64-bit hash for every dictionary string
    std::vector<uint64_t> dictHash(dictSize);
    for (uint32_t i = 0; i < dictSize; ++i) {
        const std::string& s = dictionary[i];
        uint64_t h = 0x12345678U;
        for (char c : s) {
            h ^= static_cast<uint8_t>(c);
            h *= 0x100000001b3U;
        }
        dictHash[i] = h;
    }

    const size_t nCodes = codes.size();
    std::vector<uint8_t> seenDict(dictSize, 0);   // 0/1 per dictionary index

    uint64_t count = 0;
    for (int iter = 0; iter < iters; ++iter) {
        std::fill(seenDict.begin(), seenDict.end(), 0);
        uint64_t distinct = 0;

        for (size_t j = 0; j < nCodes; ++j) {
            uint32_t idx = codes[j];
            if (idx >= dictSize) idx = 0;             // defensive
            uint8_t& flag = seenDict[idx];
            if (flag == 0) {
                flag = 1;
                distinct++;
            }
        }
        count = distinct;
    }
    return count;
}