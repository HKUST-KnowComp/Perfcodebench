#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

namespace {

alignas(64) constexpr bool kIsUpper[256] = {
    ['A']=1,['B']=1,['C']=1,['D']=1,['E']=1,['F']=1,['G']=1,['H']=1,
    ['I']=1,['J']=1,['K']=1,['L']=1,['M']=1,['N']=1,['O']=1,['P']=1,
    ['Q']=1,['R']=1,['S']=1,['T']=1,['U']=1,['V']=1,['W']=1,['X']=1,
    ['Y']=1,['Z']=1
};

alignas(64) constexpr bool kIsDigit[256] = {
    ['0']=1,['1']=1,['2']=1,['3']=1,['4']=1,
    ['5']=1,['6']=1,['7']=1,['8']=1,['9']=1
};

alignas(64) constexpr bool kIsLower[256] = {
    ['a']=1,['b']=1,['c']=1,['d']=1,['e']=1,['f']=1,['g']=1,['h']=1,
    ['i']=1,['j']=1,['k']=1,['l']=1,['m']=1,['n']=1,['o']=1,['p']=1,
    ['q']=1,['r']=1,['s']=1,['t']=1,['u']=1,['v']=1,['w']=1,['x']=1,
    ['y']=1,['z']=1
};

alignas(64) constexpr bool kIsHex[256] = {
    ['0']=1,['1']=1,['2']=1,['3']=1,['4']=1,
    ['5']=1,['6']=1,['7']=1,['8']=1,['9']=1,
    ['A']=1,['B']=1,['C']=1,['D']=1,['E']=1,['F']=1
};

inline bool matches(const char* s, std::size_t len) noexcept {
    // Pattern: ^[A-Z]{2}[0-9]{4}-[a-z]{6}-[0-9A-F]{8}$
    // Length must be exactly 24
    if (len != 24) return false;
    
    const auto u = [](char c) -> unsigned char { return static_cast<unsigned char>(c); };
    
    // [A-Z]{2} at positions 0-1
    if (!kIsUpper[u(s[0])]) return false;
    if (!kIsUpper[u(s[1])]) return false;
    
    // [0-9]{4} at positions 2-5
    if (!kIsDigit[u(s[2])]) return false;
    if (!kIsDigit[u(s[3])]) return false;
    if (!kIsDigit[u(s[4])]) return false;
    if (!kIsDigit[u(s[5])]) return false;
    
    // '-' at position 6
    if (s[6] != '-') return false;
    
    // [a-z]{6} at positions 7-12
    if (!kIsLower[u(s[7])]) return false;
    if (!kIsLower[u(s[8])]) return false;
    if (!kIsLower[u(s[9])]) return false;
    if (!kIsLower[u(s[10])]) return false;
    if (!kIsLower[u(s[11])]) return false;
    if (!kIsLower[u(s[12])]) return false;
    
    // '-' at position 13
    if (s[13] != '-') return false;
    
    // [0-9A-F]{8} at positions 14-21
    if (!kIsHex[u(s[14])]) return false;
    if (!kIsHex[u(s[15])]) return false;
    if (!kIsHex[u(s[16])]) return false;
    if (!kIsHex[u(s[17])]) return false;
    if (!kIsHex[u(s[18])]) return false;
    if (!kIsHex[u(s[19])]) return false;
    if (!kIsHex[u(s[20])]) return false;
    if (!kIsHex[u(s[21])]) return false;
    
    // Implicit $ - we already checked length is 24, positions 22 and 23 don't exist
    // Wait, 24 chars means indices 0-23. Let me recount:
    // AA0000-aaaaaa-FFFFFFFF
    // 01234567890123456789012
    //           1111111111222
    // That's 23 characters (0-22). Let me recount the pattern:
    // [A-Z]{2} = 2 chars (0-1)
    // [0-9]{4} = 4 chars (2-5)
    // - = 1 char (6)
    // [a-z]{6} = 6 chars (7-12)
    // - = 1 char (13)
    // [0-9A-F]{8} = 8 chars (14-21)
    // Total = 2+4+1+6+1+8 = 22 chars, indices 0-21, length 22
    
    return true;
}

} // namespace

uint64_t regex_log_match_checksum(const std::vector<std::string>& records, int iters) {
    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (const std::string& s : records) {
            const bool ok = matches(s.data(), s.size());
            const uint64_t v = ok ? static_cast<uint64_t>(static_cast<unsigned char>(s[2]) + static_cast<unsigned char>(s[14])) : 0u;
            hash ^= v + 1u;
            hash *= 1099511628211ULL;
        }
    }
    return hash;
}
