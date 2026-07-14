#include "interface.h"
#include <yyjson.h>
#include <string>
#include <vector>
#include <cstdio>

std::string build_json(const std::vector<uint32_t>& ids,
                       const std::vector<uint32_t>& values,
                       const std::vector<std::string>& names) {
    const size_t n = ids.size();
    if (n == 0) return "[]";

    // Pre-calculate exact output size to avoid reallocations
    size_t total = 2; // "["
    for (size_t i = 0; i < n; ++i) {
        // {"id":<ids[i]>,"flag":<true/false>,"value":<values[i]>,"name":"<names[i]>"}
        total += 23 + 10 + 10 + 2 + names[i].size(); // conservative upper bounds
        if (i) total += 1; // comma
    }
    total += 1; // "]"

    std::string out;
    out.reserve(total);
    out.push_back('[');

    char num[32];
    for (size_t i = 0; i < n; ++i) {
        if (i) out.push_back(',');
        out.append("{\"id\":");
        int len = std::snprintf(num, sizeof(num), "%u", ids[i]);
        out.append(num, len);
        out.append(\",\"flag\":");
        out.append((ids[i] % 3U) == 0U ? "true" : "false");
        out.append(\",\"value\":");
        len = std::snprintf(num, sizeof(num), "%u", values[i]);
        out.append(num, len);
        out.append(\",\"name\":\"");
        // escape names[i] if needed; task guarantees no control chars
        out.append(names[i]);
        out.push_back('"');
        out.push_back('}');
    }
    out.push_back(']');
    return out;
}