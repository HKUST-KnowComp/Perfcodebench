#include "interface.h"
#include <string>
#include <vector>
#include <cstdio>
#include <numeric>

static inline uint64_t checksum_text(const char* const data, const std::size_t len){
    uint64_t h = 1469598103934665603ULL;
    for(std::size_t i=0;i<len;++i){
        h ^= static_cast<uint8_t>(data[i]);
        h *= 1099511628211ULL;
    }
    return h;
}

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters)
{
    const std::size_t N = ids.size();
    const std::size_t newlines = N;
    const std::size_t pipes    = 3*N;
    const std::size_t baseDigits = []{
        // quick upper-bound: digits for 12-digit timestamp + 1-id + 1-level + 3 pipes and a newline
        return 12u + 1u + 1u + 3u + 1u;
    }();
    const std::size_t msgBytesSum = std::accumulate(
        messages.begin(), messages.end(), std::size_t(0),
        [](std::size_t acc, const std::string& s){ return acc + s.size(); });
    const std::size_t totalPerIter = baseDigits*N + msgBytesSum;

    std::string out;
    out.reserve(totalPerIter + (N<<2)); // a bit extra to be safe

    uint64_t finalHash = 0;
    for(int iter = 0; iter < iters; ++iter){
        out.clear();
        for(std::size_t i=0;i<N;++i){
            char tmp[48];
            int len = std::sprintf(tmp, "%u|%u|%u|", timestamps[i], ids[i], levels[i]);
            out.append(tmp, len);
            out.append(messages[i]);
            out.push_back('\n');
        }
        finalHash = checksum_text(out.data(), out.size());
    }
    return finalHash;
}