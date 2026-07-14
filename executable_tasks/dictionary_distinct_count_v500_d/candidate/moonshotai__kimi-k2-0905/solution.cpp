#include "interface.h"

#include <string>
#include <vector>
#include <unordered_set>
#include <cstdint>
#include <cstring>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters)
{
    // ---------- one-time setup ----------
    static constexpr uint32_t BLOOM_BITS = 1u << 16;   // 64 kbits
    static constexpr uint32_t BLOOM_MASK = BLOOM_BITS - 1;
    alignas(64) static uint64_t bloom[BLOOM_BITS / 64] = {0};

    const uint32_t dict_size = static_cast<uint32_t>(dictionary.size());

    // string_view-like POD: pointer + length
    struct Ref { const char* ptr; uint32_t len; };
    static thread_local Ref dict_ref[65536];           // covers full uint16_t range

    // populate bloom filter and dict_ref once per process
    static bool init_done = false;
    if (!init_done) {
        for (uint32_t i = 0; i < dict_size; ++i) {
            const std::string& s = dictionary[i];
            dict_ref[i] = { s.data(), static_cast<uint32_t>(s.size()) };

            // very cheap hash: mix code with length
            uint32_t h = i * 0x9e3779b9u + static_cast<uint32_t>(s.size());
            h ^= h >> 16;
            bloom[(h >> 6) & BLOOM_MASK] |= 1ull << (h & 63);
        }
        init_done = true;
    }

    // ---------- per-iteration work ----------
    uint64_t count = 0;
    for (int iter = 0; iter < iters; ++iter) {
        // 1. collect unique codes
        uint32_t unique[65536];
        uint32_t unique_cnt = 0;

        for (uint16_t c : codes) {
            // bloom filter fast-path
            uint32_t h = c * 0x9e3779b9u + dict_ref[c].len;
            h ^= h >> 16;
            if (bloom[(h >> 6) & BLOOM_MASK] & (1ull << (h & 63))) {
                // possible duplicate – linear scan tiny unique[]
                bool found = false;
                for (uint32_t j = 0; j < unique_cnt; ++j) {
                    if (unique[j] == c) { found = true; break; }
                }
                if (!found) unique[unique_cnt++] = c;
            } else {
                // definitely new
                unique[unique_cnt++] = c;
                bloom[(h >> 6) & BLOOM_MASK] |= 1ull << (h & 63);
            }
        }

        // 2. build unordered_set<string_view> for these unique codes
        struct SV { const char* p; uint32_t l; };
        struct SVHash {
            size_t operator()(const SV& v) const noexcept {
                // FNV-1a on bytes
                size_t h = 14695981039346656037ull;
                const char* e = v.p + v.l;
                for (const char* c = v.p; c != e; ++c)
                    h = (h ^ static_cast<unsigned char>(*c)) * 1099511628211ull;
                return h;
            }
        };
        struct SVEqual {
            bool operator()(const SV& a, const SV& b) const noexcept {
                return a.l == b.l && std::memcmp(a.p, b.p, a.l) == 0;
            }
        };
        std::unordered_set<SV, SVHash, SVEqual> seen;
        seen.reserve(unique_cnt);

        for (uint32_t i = 0; i < unique_cnt; ++i) {
            uint16_t c = static_cast<uint16_t>(unique[i]);
            seen.insert({ dict_ref[c].ptr, dict_ref[c].len });
        }
        count = static_cast<uint64_t>(seen.size());
    }
    return count;
}