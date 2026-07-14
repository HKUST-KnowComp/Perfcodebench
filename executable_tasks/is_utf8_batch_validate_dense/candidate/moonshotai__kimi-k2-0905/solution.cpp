#include "interface.h"
#include <cstdint>
#include <cstring>

namespace {

/* ---------- byte-classification table ---------- */
enum class Cls : uint8_t {
    ASC = 0,   // 0x00-0x7F
    C2  = 1,   // 0xC2-0xDF  first of 2-byte
    C3  = 2,   // 0xE0-0xEF  first of 3-byte
    C4  = 3,   // 0xF0-0xF4  first of 4-byte
    CON = 4,   // 0x80-0xBF  continuation
    INV = 5    // everything else (invalid lead)
};

static const uint8_t class_tab[256] = {
// 0x00-0x7F
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
// 0x80-0xFF
5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5, 5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5, 5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4, 4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4, 4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 3,3,3,3,3,3,3,3,5,5,5,5,5,5,5,5
};

/* ---------- DFA transition table ---------- */
/* state encoding:
   0 = accept, 1..4 = need that many continuation bytes,
   5..7 = need 1..3 cont bytes *and* the next lead must be 0x80-0xBF */
static const uint8_t next[8][6] = {
/*        ASC  C2  C3  C4  CON  INV */
/*0*/   { 0,  1,  2,  3,  7,  99 },
/*1*/   { 99, 99, 99, 99, 0,  99 },
/*2*/   { 99, 99, 99, 99, 5,  99 },
/*3*/   { 99, 99, 99, 99, 6,  99 },
/*5*/   { 99, 99, 99, 99, 0,  99 },
/*6*/   { 99, 99, 99, 99, 2,  99 },
/*7*/   { 99, 99, 99, 99, 99, 99 },
/*99*/  { 99, 99, 99, 99, 99, 99 }
};

inline bool validate_chunk(const uint8_t* p, std::size_t n) noexcept {
    uint32_t st = 0;
    const uint8_t* end = p + n;
    while (p + 4 <= end) {
        uint8_t c0 = class_tab[p[0]];
        uint8_t c1 = class_tab[p[1]];
        uint8_t c2 = class_tab[p[2]];
        uint8_t c3 = class_tab[p[3]];
        st = next[st][c0];
        st = next[st][c1];
        st = next[st][c2];
        st = next[st][c3];
        if (st >= 99) return false;
        p += 4;
    }
    while (p < end) {
        st = next[st][class_tab[*p++]];
        if (st >= 99) return false;
    }
    return st == 0;
}

} // anonymous namespace

std::size_t count_valid_strings(const std::vector<std::string>& inputs) {
    std::size_t cnt = 0;
    for (const auto& s : inputs) {
        cnt += validate_chunk(reinterpret_cast<const uint8_t*>(s.data()), s.size());
    }
    return cnt;
}