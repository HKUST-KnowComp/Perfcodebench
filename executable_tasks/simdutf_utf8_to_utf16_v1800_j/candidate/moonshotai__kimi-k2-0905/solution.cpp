#include "interface.h"
#include <simdutf.h>
#include <vector>

uint64_t transcode_checksum(const std::string& input) {
    // simdutf guarantees that the worst-case UTF-16 output is never larger than UTF-8 input
    std::vector<char16_t> out(input.size() + 1);   // +1 for over-alignment / padding

    // Convert and get actual number of char16_t units written
    size_t utf16_words = simdutf::convert_utf8_to_utf16le(
                            input.data(),
                            input.size(),
                            reinterpret_cast<char16_t*>(out.data()));

    // FNV-1a checksum over the produced UTF-16LE sequence
    const uint64_t prime = 1099511628211ULL;
    const uint64_t offset = 14695981039346656037ULL;
    uint64_t hash = offset;
    const char16_t* p = out.data();
    const char16_t* const end = p + utf16_words;
    while (p != end) {
        hash ^= static_cast<uint64_t>(*p++);
        hash *= prime;
    }
    return hash;
}